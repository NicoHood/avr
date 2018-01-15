/*
Copyright (c) 2017-2018 NicoHood
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

/*! \file   spi_usart_private.h
 *  \brief  USART SPI functions
 */

// Include Guard
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <avr/io.h>

#ifndef F_CPU
    #error "F_CPU not defined"
#endif

/**
 * SPI Data rates:
 *
 * RATE = F_CPU / 2*(URR1 + 1)
 *      = 16000000 / (2*URR1 + 2)
 *
 * URR1 = (F_CPU / (2*RATE)) - 1
 */
#define UBRR1_SPI_RATE(rate) ((F_CPU / (2UL * rate)) - 1UL)

// Configuration examples
//#define SPI_USART_BAUDRATE 8000000UL //   8MHz ->  0
//#define SPI_USART_BAUDRATE 4000000UL //   4MHz ->  1
//#define SPI_USART_BAUDRATE 2000000UL //   2MHz ->  3
//#define SPI_USART_BAUDRATE 1000000UL //   1MHz ->  7
//#define SPI_USART_BAUDRATE 800000UL  // 800KHz ->  9
//#define SPI_USART_BAUDRATE 500000UL  // 500KHz -> 15
//#define SPI_USART_BAUDRATE 400000UL  // 400KHz -> 19
//#define SPI_USART_BAUDRATE 100000UL  // 100KHz -> 79

// SPI_USART_BAUDRATE needs to be defined above or via make flag -D or -include
#ifndef SPI_USART_BAUDRATE
    #error "SPI_USART_BAUDRATE not defined"
#endif

// SPI USART pin definitions
#if defined(__AVR_ATmega32U4__)
    // Name schema similar to normal SPI definitions in avr/io.h
    #define SPI_USART_DDR   DDRD
    #define SPI_USART_PORT  PORTD
    #define SCK_USART_BIT   PORTD5
    #define MOSI_USART_BIT  PORTD3
    #define MISO_USART_BIT  PORTD2
#else
    #error "USART SPI pins not defined for this MCU"
#endif
