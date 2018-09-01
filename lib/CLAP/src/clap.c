/*
Copyright (c) 2018 NicoHood
See the readme for credit to other people.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "clap.h"
#include "clap_private.h"
#include "timer0.h"
#include <util/atomic.h>

// How this code works:
// ADC value is measured frequently via interrupts, usually every 104uS for most
// F_CPU value, in 8 bit resolution. Every timing is calculated in counts of
// 104uS between each interrupt call.
// 1.) Wait for input to reach the upper treshold (MAX_TRESHOLD)
// 2.) Wait for input to reach lower treshold (MIN_TRESHOLD)
// 3.) Also check if the difference between max and min is high enough (PEAK_TRESHOLD)
// 4.) If 2. and 3. were not fulfilled within PEAK_TIMEOUT_US restart a new reading
// 5.) On successful clapping detection increase clap counter and safe the current time via micros()
// 6.) Set a (~2byte) timeout countdown to add a decounce (smart combination of the max and timeout value)
// 7.) Start a new reading and possibly increment the clap counter of the current series on new valid inputs.
// 8.) In the main loop check if the clap counter is greater null.
// 9.) Safe the last clap time (via atomic read), afterwards the current time (order is important)
// 10.) If the last clap was greater CLAP_SERIES_TIMEOUT_MS flag a new input.

#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
#error "32u4 series not tested yet."

#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega128__) || defined(__AVR_ATmega328P__)
/**
 *  Initializes ADC in interrupt free-running mode
 *  This function assumes the used ADC registers are in their default state.
 *  A lot of code was taken from:
 *  http://www.glennsweeney.com/tutorials/interrupt-driven-analog-conversion-with-an-atmega328p
 *
 *  @param adc_pin [description]
 */
void clap_init(void)
{
    // Left adjust measureing to only get an 8bit precise value
    ADMUX |= (1 << ADLAR);

    // Set REFS1..0 in ADMUX to change reference voltage to the proper source
    //ADMUX &= ~((1 << REFS1) | (1 << REFS0))
    ADMUX |= (1 << REFS0);

    // Clear MUX3..0 in ADMUX in preparation for setting the analog input
    //ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));
    ADMUX |= CLAP_ADC_PIN & ((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));

    // Set ADEN in ADCSRA to enable the ADC.
    // Note, this instruction takes 12 ADC clocks to execute
    ADCSRA |= (1 << ADEN);

    // Set ADATE in ADCSRA to enable auto-triggering.
    ADCSRA |= (1 << ADATE);

    // Clear ADTS2..0 in ADCSRB to set trigger mode to free running.
    // This means that as soon as an ADC has finished, the next will be
    // immediately started.
    ADCSRB &= ~((1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0));

    // Set the Prescaler to 128 (16000KHz/128 = 125KHz)
    // Above 200KHz 10-bit results are not reliable.
    ADCSRA |= CLAP_ADPS_SETTING;
}

/**
 *  Starts the ADC conversion.
 *  Run this function when interrupts are enabled
 */
void clap_enable(void)
{
    // Set ADSC in ADCSRA to start the (or a new) ADC conversion
    ADCSRA |= (1 << ADSC);

    // Set ADIE in ADCSRA to enable the ADC interrupt.
    // Without this, the internal interrupt will not trigger.
    ADCSRA |= (1 << ADIE);
}

void clap_disable(void)
{
    // Disable interrupts
    ADCSRA &= ~(1 << ADIE);
}

#else
#error "Unsupported MCU"
#endif

// Personal notes:
// The analog reading/conversion takes about 116 uS
// http://www.glennsweeney.com/tutorials/interrupt-driven-analog-conversion-with-an-atmega328p
// A normal conversion takes 13 ADC clock cycles.
// The first conversion after the ADC is switched on (i.e., ADCSRA.ADEN is written to '1')
// takes 25 ADC clock cycles in order to initialize the analog circuitry.
// So, on an Arduino running at 16MHz (16,000KHz), the ADC clock is at (16,000/128)KHz, or 125KHz.
// At 13 clock cycles, that's about 104us (microseconds) to perform an ADC.
// -> The interrupt triggers every 104us, which is 9.6khz
// The clapping sound is at around 3khz, so the recognition should be faster than that.
// Using slower F_CPU results in slower interrupt triggers, so the divier should be smaller.

// 255
// 130      ^        max
// 110     / \       MAX_TRESHOLD
//  70 ---/   \  --- BASE
//  30         \/    max - input = peak -> must be greater PEAK_TRESHOLD
//   0

static volatile uint32_t last_clap = 0;
static volatile uint8_t clap_count = 0;

/**
 *  Interrupt service routine for the ADC completion.
 *  Takes below 4us to execute without any debug code.
 *
 *  @param ADC_vect [description]
 */
ISR(ADC_vect)
{
    static uint8_t max = CLAP_MAX_TRESHOLD;
    static uint8_t timeout = 0;

    // Must read low register first
    uint8_t input = ADCH;

    // Debounce after a successfull clap detection.
    // Use a combination of max and timeout variables
    // to accieve counting down from more than 255 counts
    // (255 counts -> 26.52mS which is too short)
    // TODO try to reorder to safe flash?
    if (max < CLAP_MAX_TRESHOLD)
    {
        // Decrease timeout counter
        timeout--;
        if (!timeout)
        {
            // Reset maximum value if timed out.
            // This is important to ignore if a high max value is not followed by a low peak.
            // If we do not reset, the next peak must be even louder than the current one.
            max++;
        }
    }
    // Check if new peak was reached
    else if (input >= max)
    {
        // Safe new value and start a new counting
        max = input;
        timeout = CLAP_PEAK_TIMEOUT_COUNT;
    }
    // If there is a currently running series, check for the lower peak
    else if (timeout)
    {
        // New valid clap input
        if (input <= CLAP_MIN_TRESHOLD && (uint8_t)(max - input) >= CLAP_PEAK_TRESHOLD)
        {
            // Set maximum value temporary to zero in order to force some debounce.
            // The timeout value must count down to zero completely.
            // 104uS * 255 = 26.52mS debounce.
            max = CLAP_MAX_TRESHOLD - (CLAP_DEBOUNCE_COUNT >> 8) - 1;
            timeout = (CLAP_DEBOUNCE_COUNT + 1) & 0xFF;

            clap_count++;
            last_clap = millis();
        }
        else {
            // Decrease timeout counter
            timeout--;
            if (!timeout)
            {
                // Reset maximum value if timed out.
                // This is important to ignore if a high max value is not followed by a low peak.
                // If we do not reset, the next peak must be even louder than the current one.
                max = CLAP_MAX_TRESHOLD;
            }
        }
    }

    // Not needed because free-running mode is enabled.
    // Set ADSC in ADCSRA to start another ADC conversion
    //ADCSRA |= (1 << ADSC);
}

// Returns 0 on no new clap
// -1, -2, ... on a new clap series
// 1, 2, ... on a final clap series with timeout
int8_t clap_read(void)
{
    static uint8_t processed_claps = 0;
    if (clap_count > processed_claps)
    {
        processed_claps++;
        // Add special case when int limits are exceeded
        if(processed_claps >= 127)
        {
            processed_claps = 0;
            clap_count = 0;
            return 127;
        }
        return -1 * processed_claps;
    }
    if (clap_count)
    {
        // Important: First get the clap time, then measure the current time!
        uint32_t prev;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            prev = last_clap;
        }
        uint32_t current_time = millis();
        uint32_t diff = current_time - prev;
        if (diff > CLAP_SERIES_TIMEOUT_MS)
        {
            uint8_t claps = clap_count;
            clap_count -= processed_claps;
            processed_claps = 0;
            return claps;
        }
    }
    return 0;
}
