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

// Include Guard
#pragma once

#include <stdbool.h> // TODO rem
#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#ifndef F_CPU
    #error "F_CPU not defined"
#endif

// Function prototypes
int usart_fputc(char c, FILE *stream);
int usart_fgetc(FILE *stream);

// Check for baudrate setting
#ifndef USART_BAUDRATE
#error "USART_BAUDRATE not defined"
#endif

// Default settings
#ifndef USART_USE_2X
#define USART_USE_2X    1
#endif
#ifndef USART_DATA_BITS
#define USART_DATA_BITS USART_DATA_BITS_8
#endif
#ifndef USART_STOP_BITS
#define USART_STOP_BITS USART_STOP_BITS_1
#endif
#ifndef USART_PARITY
#define USART_PARITY    USART_PARITY_NO
#endif

// Usart prescale speed, dependant on U2X mode
#if defined(USART_USE_2X)
#define USART_BAUD_PRESCALE (((( F_CPU / 8) + (USART_BAUDRATE / 2)) / (USART_BAUDRATE)) - 1)
#else
#define USART_BAUD_PRESCALE (((( F_CPU / 16) + (USART_BAUDRATE / 2)) / (USART_BAUDRATE)) - 1)
#endif

// Data bits
#define USART_DATA_BITS_5   0
#define USART_DATA_BITS_6   (1 << USART_UCSZ0)
#define USART_DATA_BITS_7   (1 << USART_UCSZ1)
#define USART_DATA_BITS_8   ((1 << USART_UCSZ1) | (1 << USART_UCSZ0))
#define USART_DATA_BITS_9   ((1 << USART_UCSZ2) | (1 << USART_UCSZ1) | (1 << USART_UCSZ0))

// Stop bits
#define USART_STOP_BITS_1   0
#define USART_STOP_BITS_2   (1 << USART_USBS)

// Parity
#define USART_PARITY_NO     0
#define USART_PARITY_EVEN   (1 << USART_UPM1)
#define USART_PARITY_ODD    ((1 << USART_UPM1) | (1 << USART_UPM0))

#if defined(__AVR_ATmega32U4__)

// Register mapping
#define USART_UBRRH         UBRR1H
#define USART_UBRRL         UBRR1L
#define USART_UCSRA         UCSR1A
#define USART_UCSRB         UCSR1B
#define USART_UCSRC         UCSR1C
#define USART_UDR           UDR1

// Bit mapping
#define USART_RXEN          RXEN1
#define USART_TXEN          TXEN1
#define USART_UDRE          UDRE1
#define USART_RXC           RXC1
#define USART_U2X           U2X1
#define USART_UCSZ0         UCSZ10
#define USART_UCSZ1         UCSZ11
#define USART_UCSZ2         UCSZ12
#define USART_USBS          USBS
#define USART_UPM0          UPM0
#define USART_UPM1          UPM1
#undef USART_URSEL

#else
#error "Unsupported MCU"
#endif
