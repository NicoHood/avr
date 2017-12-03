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

#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include <assert.h>
#include "usart.h"

// Settings sample
//#define USART_BAUDRATE 2000000
//#define USART_BAUDRATE 115200
//#define USART_BAUDRATE 9600
//#define USART_THREAD_SAFE

// setbaud.h will pull in UBRRH_VALUE, UBRRL_VALUE and USE_2X
#define BAUD USART_BAUDRATE
#include <util/setbaud.h>

// Check buffer size limits
// TODO rename to static_assert() when avr-libc >2.0.0 gets released
// https://savannah.nongnu.org/bugs/?41689
_Static_assert(USART_BUFFER_RX < (1 << 8) && USART_BUFFER_RX >= 0,
    "USART_BUFFER_RX is an unsigned 8bit value. Please choose 8, 16, 32, 64, 128 or 255");
_Static_assert(USART_BUFFER_TX < (1 << 8) && USART_BUFFER_TX >= 0,
    "USART_BUFFER_TX is an unsigned 8bit value. Please choose 8, 16, 32, 64, 128 or 255");

// Interrupts are used with buffers
#if (USART_BUFFER_RX) || (USART_BUFFER_TX)
#include <avr/interrupt.h>
#endif

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
#ifndef USART_DATA_BITS
#define USART_DATA_BITS USART_DATA_BITS_8
#endif
#ifndef USART_STOP_BITS
#define USART_STOP_BITS USART_STOP_BITS_1
#endif
#ifndef USART_PARITY
#define USART_PARITY    USART_PARITY_NO
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

#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)

// Register mapping
#define USART_UBRRH         UBRR1H
#define USART_UBRRL         UBRR1L
#define USART_UCSRA         UCSR1A
#define USART_UCSRB         UCSR1B
#define USART_UCSRC         UCSR1C
#define USART_UDR           UDR1

// Bit mapping UCSRnA
#define USART_RXC           RXC1
#define USART_TXC           TXC1
#define USART_UDRE          UDRE1
#define USART_UPE           UPE1
#define USART_U2X           U2X1

// Bit mapping UCSRnB
#ifndef URSEL
#undef USART_URSEL
#else
#define USART_URSEL         URSEL
#endif
#define USART_RXCIE         RXCIE1
#define USART_UDRIE         UDRIE1
#define USART_RXEN          RXEN1
#define USART_TXEN          TXEN1

// Bit mapping UCSRnC
#define USART_UPM0          UPM10
#define USART_UPM1          UPM11
#define USART_USBS          USBS1
#define USART_UCSZ0         UCSZ10
#define USART_UCSZ1         UCSZ11
#define USART_UCSZ2         UCSZ12

// Interrupt vectors
#define USART_RX_VECT       USART1_RX_vect
#define USART_UDRE_VECT     USART1_UDRE_vect

// TX/RX pin functions
#define USART_TX_HIGH()     PORTD |= (1 << PD3)
#define USART_TX_LOW()      PORTD &= ~(1 << PD3)

#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega128__) || defined(__AVR_ATmega328P__)

// Register mapping
#define USART_UBRRH         UBRR0H
#define USART_UBRRL         UBRR0L
#define USART_UCSRA         UCSR0A
#define USART_UCSRB         UCSR0B
#define USART_UCSRC         UCSR0C
#define USART_UDR           UDR0

// Bit mapping UCSRnA
#define USART_RXC           RXC0
#define USART_TXC           TXC0
#define USART_UDRE          UDRE0
#define USART_UPE           UPE0
#define USART_U2X           U2X0

// Bit mapping UCSRnB
#ifndef URSEL
#undef USART_URSEL
#else
#define USART_URSEL         URSEL
#endif
#define USART_RXCIE         RXCIE0
#define USART_UDRIE         UDRIE0
#define USART_RXEN          RXEN0
#define USART_TXEN          TXEN0

// Bit mapping UCSRnC
#define USART_UPM0          UPM00
#define USART_UPM1          UPM01
#define USART_USBS          USBS0
#define USART_UCSZ0         UCSZ00
#define USART_UCSZ1         UCSZ01
#define USART_UCSZ2         UCSZ02

// Interrupt vectors
#define USART_RX_VECT       USART_RX_vect
#define USART_UDRE_VECT     USART_UDRE_vect

// TX/RX pin functions
#define USART_TX_HIGH()     PORTD |= (1 << PD1)
#define USART_TX_LOW()      PORTD &= ~(1 << PD1)

#else
#error "Unsupported MCU"
#endif
