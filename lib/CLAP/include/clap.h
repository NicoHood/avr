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

// Include guard
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Software version
#define CLAP_VERSION 100

#include <stdint.h>

// The minimum and maximum treshold the input signal MUST reach.
// Everything that is not exceeding those limits is not considered as clap.
#ifndef CLAP_MAX_TRESHOLD
#define CLAP_MAX_TRESHOLD 110
#endif
#ifndef CLAP_MIN_TRESHOLD
#define CLAP_MIN_TRESHOLD 30
#endif

// The treshold jitter defined how much additional volume to the min and max must be added to
// recognize a successful clap. It can be either above or below the extremes. Or a mix of it.
#ifndef CLAP_JITTER_TRESHOLD
#define CLAP_JITTER_TRESHOLD 30
#endif

// The time how long a peak can take.
// It is the timespan between the upper and lower extreme.
// Usually a lower peak happens 2ms or less after an upper peak.
#ifndef CLAP_PEAK_TIMEOUT_US
#define CLAP_PEAK_TIMEOUT_US 2000
#endif

// Set debounce time
#ifndef CLAP_DEBOUNCE_MS
#define CLAP_DEBOUNCE_MS 50UL
#endif

// Defines the timeout of a series of multiple claps.
#ifndef CLAP_SERIES_TIMEOUT_MS
#define CLAP_SERIES_TIMEOUT_MS 1000UL
#endif

// Function prototypes
void clap_init(void);
void clap_enable(void);
void clap_disable(void);
int8_t clap_read(void);

#ifdef __cplusplus
}
#endif
