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

// Include Guard
#pragma once

#include "clap.h"
#include <avr/io.h>
#include <assert.h>

#ifndef F_CPU
    #error "F_CPU not defined."
#endif

#ifndef CLAP_ADC_PIN
#error "CLAP_ADC_PIN not defined."
#endif

// For 20MHz we use the highest available devider. The usual 104uS interrupt
// frame will not apply here, but all calculations are done flexible so
// everything should just work fine.
#if (F_CPU == 16000000) || (F_CPU == 20000000)
#define CLAP_ADPS_SETTING ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))
#elif (F_CPU == 8000000)
#define CLAP_ADPS_SETTING ((1 << ADPS2) | (1 << ADPS1))
#elif (F_CPU == 4000000)
#define CLAP_ADPS_SETTING ((1 << ADPS2) | (1 << ADPS0))
#elif (F_CPU == 2000000)
#define CLAP_ADPS_SETTING (1 << ADPS2)
#elif (F_CPU == 1000000)
#define CLAP_ADPS_SETTING ((1 << ADPS1) | (1 << ADPS0))
#else
#error "Clock speed not supported. Please choose a different F_CPU"
#endif

// 128 for 16MHz, 64 for 8MHz, 32 for 4MHz, 8 for 1MHz
#define CLAP_DIVISION_FACTOR (1 << CLAP_ADPS_SETTING)

// Each ADC measuring takes 13 clock cycles (after clock division)
// 104uS for 16, 8, 4, 1 MHz which is 9.6kHz
// TODO rename to _Static_assert() when avr-libc >2.0.0 gets released
// https://savannah.nongnu.org/bugs/?41689
#define CLAP_ADC_MEASURE_TIME_US (((CLAP_DIVISION_FACTOR * 1000000UL) / F_CPU) * 13UL)
_Static_assert((1000000UL / CLAP_ADC_MEASURE_TIME_US) > 3000UL,
    "Samples must be higher than 3kHz to detect clapping properly.");

_Static_assert(CLAP_MIN_TRESHOLD < CLAP_MAX_TRESHOLD,
    "Upper treshold must be higher than lower treshold.");

#define CLAP_PEAK_TRESHOLD ((CLAP_MAX_TRESHOLD - CLAP_MIN_TRESHOLD) + CLAP_JITTER_TRESHOLD)
_Static_assert(CLAP_PEAK_TRESHOLD <= 255,
    "The JITTER_TRESHOLD must not exeed the pysical limits of measuring.");

// The lowest measured timeout with a valid signal was 255-238=17
// 17 * 104uS = 1.768mS timeout, so a value of 20 aka 2uS is a good start.
#define CLAP_PEAK_TIMEOUT_COUNT (CLAP_PEAK_TIMEOUT_US / CLAP_ADC_MEASURE_TIME_US)
_Static_assert(CLAP_PEAK_TIMEOUT_COUNT > 0 && CLAP_PEAK_TIMEOUT_COUNT <= 255,
    "PEAK_TIMEOUT_US is set either too high or too low.");

// A 100mS timeout requires 961 counts with adc measure times of 104uS
// 961 = 100 * 1000 / 104
#define CLAP_DEBOUNCE_COUNT ((CLAP_DEBOUNCE_MS * 1000UL) / CLAP_ADC_MEASURE_TIME_US)
_Static_assert((CLAP_DEBOUNCE_COUNT >> 8) < CLAP_MAX_TRESHOLD,
    "Either DEBOUNCE_MS must be a lower value or MAX_TRESHOLD");
