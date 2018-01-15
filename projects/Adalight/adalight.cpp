/*
Copyright (c) 2017 NicoHood
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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "timer0.h"
#include "fastled.h"
#include "fastpin.h"
#include "usart.h"
#include "board_leds.h"

// How many leds are in your strip?
#define NUM_LEDS 25

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN. For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 3
#define CLOCK_PIN 13

// Define the array of leds
CRGB leds[NUM_LEDS];

// Return values:
//  1 Data written, update required
//  0 Active
// -1 Error
// -2 Inactive/Timeout
template <FILE* const stream, CRGB* myleds, const int numLeds, const uint32_t timeout = 15000>
int adalight(void)
{
    static uint32_t previousTime = 0;
    static uint8_t magicPos = 0;
    static uint16_t numPixel = 0;
    static uint8_t pixelCache[2] = { 0, 0 };

    // Process a maximum of 64 bytes.
    // This is required to not wait too long between each update
    // but also to not block forever on fast input rates.
    uint8_t bytesAvailable = 64;

    // Mark adalight as active from here (leds will be overwritten soon!)
    bool updateLeds = false;
    bool newData = false;
    bool error = false;
    while (bytesAvailable--)
    {
        // Write leds via Adalight
        // Check if any errors occured while reading the new data
        int input = fgetc(stream);
        if (input < 0) {
            break;
        }
        newData = true;

        // Get the next magic word letter.
        // Always check for a new magic word serious, even while processing LED data.
        uint8_t magicWord;
        if (magicPos == 0) {
            magicWord = 'A';
        }
        else if (magicPos == 1) {
            magicWord = 'd';
        }
        else if (magicPos == 2) {
            magicWord = 'a';
        }
        else if (magicPos == 3) {
            magicWord = ((numLeds - 1) >> 8);
        }
        else if (magicPos == 4) {
            magicWord = ((numLeds - 1) & 0xFF);
        }
        else { // (magicPos == 5)
            magicWord = ((numLeds - 1) >> 8) ^ ((numLeds - 1) & 0xFF) ^ 0x55;
        }

        // Check if input matches magic word
        if (input == magicWord)
        {
            magicPos++;
        }
        // Check if input matches the first magic word letter
        else if (input == 'A') {
            // Do not flag this case as error, as we are just peeking for a new magic word.
            // It does not mean, the 'A' received is an error, it can be LED data.
            magicPos = 1;
        }
        // No magic word matched
        else {
            magicPos = 0;

            // Error if we are waiting for a new magic word which is wrong
            if (!numPixel) {
                error = true;
            }
        }

        // Found the magic word! Read leds in the followin iterations.
        if (magicPos >= 6) {
            // Error if we got a new magic word when we were still reading led data
            if (numPixel) {
                error = true;
            }

            // Reset magic word position to detect magic words again.
            // Optimatically this only happens directly after we received all LED data.
            magicPos = 0;
            numPixel = numLeds * 3;
            continue;
        }

        // Proceed Led signal
        if (numPixel)
        {
            // Proceed next pixel
            numPixel--;

            // Cache a full led pixel to avoid corrupted pixel data when frequently updating.
            auto pixel = numPixel % 3;
            if (pixel) {
                pixelCache[pixel - 1] = input;
            }
            else {
                auto led = (numLeds - 1) - (numPixel / 3);
                myleds[led].raw[0] = pixelCache[1];
                myleds[led].raw[1] = pixelCache[0];
                myleds[led].raw[2] = input;
            }

            // Update Leds if this is the last pixel
            if (!numPixel) {
                updateLeds = true;
                break;
            }
        }
    }

    // On any input reset the timeout
    auto currentTime = millis();
    if (newData) {
        previousTime = currentTime;
    }
    // Check if it has timed out before
    else if (!previousTime)
    {
        return -2;
    }
    // On no input and a timeout reset temporary variables
    else if ((currentTime - previousTime) > timeout)
    {
        // Clear leds and variables for a clean start
        memset(myleds, 0x00, numLeds);
        numPixel = 0;
        magicPos = 0;
        previousTime = 0;
        updateLeds = true;
    }

    // Flag that the whole led array requires an update
    if (updateLeds) {
        return numLeds;
    }

    // Only flag errors if no valid update happened to not block too often
    if (error) {
        return -1;
    }

    // No error yet, effect is still running fine.
    return 0;
}

// File Stream
static FILE UsartSerialStream;

int main(void)
{
    // Blink Led
    // LED_BUILTIN might be a data/clock pin for the LED strip on some AVRs.
    // Make sure to select a different pin for error indicator then.
    FastPin<LED_BUILTIN> pin;
    pin.setOutput();

    // Initialize libraries and enable interrupts
    usart_init();
    timer0_init();
    sei();

    // Setup usart stream
    usart_init_stream(&UsartSerialStream);

    // Uncomment/edit one of the following lines for your leds arrangement.
    // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    // FastLED.addLeds<APA104, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1903B, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GW6205, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GW6205_400, DATA_PIN, RGB>(leds, NUM_LEDS);

    FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SM16716, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<LPD8806, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<P9813, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<APA102, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<DOTSTAR, RGB>(leds, NUM_LEDS);

    // FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);

    FastLED.showColor(CRGB::Red);
    delay(500);
    FastLED.showColor(CRGB::Green);
    delay(500);
    FastLED.showColor(CRGB::Blue);
    delay(500);
    FastLED.showColor(CRGB::Black);

    // Run adalight main loop. Use hyperion on the PC side.
    while(true)
    {
        static uint32_t previousTime = 0;
        auto currentTime = millis();
        auto ret = adalight<&UsartSerialStream, leds, NUM_LEDS>();
        if(ret > 0) {
            FastLED.show();
        }
        // Temporary error
        else if (ret == -1)
        {
            // Turn error led on
            pin.hi();
            previousTime = currentTime;
        }
        // Timeout
        else if (ret <= -2)
        {
            // Do other effects or keep updating LEDs to ensure they are off.
            FastLED.show();
        }

        // Turn error led off after one second
        if ((currentTime - previousTime) > 1000UL)
        {
            pin.lo();
        }
    }
}
