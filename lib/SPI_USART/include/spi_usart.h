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

/*! \file   spi_usart.h
 *  \brief  USART SPI functions
 */

// Include Guard
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Software version
#define SPI_USART_VERSION 112

#include <stdint.h>
#include <stddef.h>

// Setup
void spi_usart_init(void);

// Transfer: write & read data
uint8_t spi_usart_transfer_8(uint8_t data);
void spi_usart_transfer(uint8_t* data, size_t size);
void spi_usart_transfer_lsb(uint8_t* data, size_t size);
void spi_usart_transfer_msb(uint8_t* data, size_t size);

// Write data
void spi_usart_write_8(uint8_t data);
void spi_usart_write(uint8_t* data, size_t size);
void spi_usart_write_lsb(uint8_t* data, size_t size);
void spi_usart_write_msb(uint8_t* data, size_t size);

// Read data
uint8_t spi_usart_read_8(void);
void spi_usart_read(uint8_t* data, size_t size);
void spi_usart_read_lsb(uint8_t* data, size_t size);
void spi_usart_read_msb(uint8_t* data, size_t size);

#ifdef __cplusplus
}
#endif
