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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Software version
#define USART_VERSION 100

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

// Initialize
void usart_init(void);
void usart_init_stdout(void);
void usart_init_stdin(void);
void usart_init_stderr(void);
void usart_init_stdio(void);

// Transmit
void usart_putchar(const char c);
void usart_flush(void);
void usart_write(const uint8_t* buff, size_t len);
void usart_write_P(const uint8_t* buff, size_t len);
void usart_puts(const char *s);
void usart_puts_P(const char *s);

// Receive
int usart_getchar(void);
int usart_peek(void);
uint8_t usart_avail_read(void);

#ifdef __cplusplus
}
#endif
