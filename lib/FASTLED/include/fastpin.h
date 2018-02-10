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

// Include guard
#pragma once

// Software version
#define FASTPIN_VERSION 110

// Disable inclusion of FastLED led drivers
#ifdef __INC_FASTSPI_LED2_H
#define FASTPIN_FASTLED
#else
#define __INC_FASTSPI_LED2_H
#endif

#include "../src/fastpin.h"
#include "../src/platforms/avr/fastpin_avr.h"

// Arduino compatiblity layer.
// Only works with libraries which use compile time pin options.
#ifndef ARDUINO

#ifndef INPUT
#define INPUT 0x0
#endif
#ifndef OUTPUT
#define OUTPUT 0x1
#endif
#ifndef INPUT_PULLUP
#define INPUT_PULLUP 0x2
#endif
#ifndef LOW
#define LOW 0x0
#endif
#ifndef HIGH
#define HIGH 0x1
#endif
#ifndef TOGGLE
#define TOGGLE 0x2
#endif

template <uint8_t pin, uint8_t mode>
static inline void pinMode(void)
{
    FastPin<pin> p;
    if (mode == OUTPUT){
        p.setOutput();
    }
    else if (mode == INPUT){
        p.setInput();
    }
    else if (mode == INPUT_PULLUP){
        p.setInput();
        p.hi();
    }
}
#define pinMode(pin, mode) pinMode<pin, mode>()

template <uint8_t pin, uint8_t mode>
static inline void digitalWrite(void)
{
    FastPin<pin> p;
    if (mode == LOW){
        p.lo();
    }
    else if (mode == HIGH){
        p.hi();
    }
    else if (mode == TOGGLE){
        p.toggle();
    }
}
#define digitalWrite(pin, mode) digitalWrite<pin, mode>()

#endif // ifndef ARDUINO

#ifndef FASTPIN_FASTLED
#undef __INC_FASTSPI_LED2_H
#endif
