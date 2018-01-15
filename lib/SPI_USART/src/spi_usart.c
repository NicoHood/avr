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

/*! \file   spi_usart.c
 *  \brief  USART SPI functions
 */

#include "spi_usart.h"
#include "spi_usart_private.h"

/**
 * Initialise the SPI USART interface to the specified data rate
 */
void spi_usart_init(void)
{
    // Baud rate must be set to 0 prior to enabling the USART as SPI
    // master, to ensure proper initialization of the XCK line.
    UBRR1 = 0;

    // MOSI & SCK as ouputs (enables master mode)
    // MISO as input
    // Disable pull-up
    SPI_USART_DDR |= (1 << SCK_USART_BIT) | (1 << MOSI_USART_BIT);
    SPI_USART_DDR &= ~(1 << MISO_USART_BIT);
    SPI_USART_PORT &= ~(1 << MISO_USART_BIT);

    // Set Master SPI (MSPI) mode of operation and SPI data mode 0.
    UCSR1C = (1 << UMSEL11) | (1 << UMSEL10) | (0 << UCPOL1) | (0 << UCSZ10);

    // Enable RX and TX.
    UCSR1B = (1 << RXEN1) | (1 << TXEN1);

    // Set baud rate. Must be set _after_ enabling the transmitter.
    UBRR1 = UBRR1_SPI_RATE(SPI_USART_BAUDRATE);
}

/**
 * Send and receive a byte of data via the SPI USART interface.
 * @param data - the byte to send
 * @returns the received byte
 */
uint8_t spi_usart_transfer_8(uint8_t data)
{
    // Wait for empty transmit buffer
    while (!(UCSR1A & (1 << UDRE1)));
    UDR1 = data;

    // Wait for data to be received
    while (!(UCSR1A & (1 << RXC1)));
    return UDR1;
}

/**
 * Send and receive a number of bytes via the SPI USART interface, LSB first
 * @param data - pointer to buffer of data to send
 * @param size - number of bytes to send
 */
void spi_usart_transfer(uint8_t* data, size_t size) __attribute__((alias("spi_usart_transfer_lsb")));

/**
 * Send and receive a number of bytes via the SPI USART interface, LSB first
 * @param data - pointer to buffer of data to send
 * @param size - number of bytes to send
 */
void spi_usart_transfer_lsb(uint8_t* data, size_t size)
{
    while (size--)
    {
        *data = spi_usart_transfer_8(*data);
        data++;
    }
}

/**
 * Send and receive a number of bytes via the SPI USART interface, MSB first
 * @param data - pointer to buffer of data to send
 * @param size - number of bytes to send
 */
void spi_usart_transfer_msb(uint8_t* data, size_t size)
{
    while (size--)
    {
        *(data + size) = spi_usart_transfer_8(*(data + size));
    }
}

/**
 * Write a byte of data via the SPI USART interface.
 * @param data - the byte to write
 */
void spi_usart_write_8(uint8_t data)
{
    spi_usart_transfer_8(data);
}

/**
 * Write a number of bytes to SPI USART interface, LSB first
 * @param data - pointer to buffer of data to write
 * @param size - number of bytes to write
 */
void spi_usart_write(uint8_t* data, size_t size) __attribute__((alias("spi_usart_write_lsb")));

/**
 * Write a number of bytes to SPI USART interface, LSB first
 * @param data - pointer to buffer of data to write
 * @param size - number of bytes to write
 */
void spi_usart_write_lsb(uint8_t* data, size_t size)
{
    while (size--)
    {
        spi_usart_write_8(*data++);
    }
}

/**
 * Write a number of bytes to SPI USART interface, MSB first
 * @param data - pointer to buffer of data to write
 * @param size - number of bytes to write
 */
void spi_usart_write_msb(uint8_t* data, size_t size)
{
    while (size--)
    {
        spi_usart_write_8(*(data + size));
    }
}

/**
 * Reads a byte of data via the SPI USART interface.
 * @returns the received byte
 */
uint8_t spi_usart_read_8(void)
{
    return spi_usart_transfer_8(0x00);
}

/**
 * Read a number of bytes from SPI USART interface, LSB first
 * @param data - pointer to buffer to store data in
 * @param size - number of bytes to read
 */
void spi_usart_read(uint8_t* data, size_t size) __attribute__((alias("spi_usart_read_lsb")));

/**
 * Read a number of bytes from SPI USART interface, LSB first
 * @param data - pointer to buffer to store data in
 * @param size - number of bytes to read
 */
void spi_usart_read_lsb(uint8_t* data, size_t size)
{
    while (size--)
    {
        *data++ = spi_usart_read_8();
    }
}

/**
 * Read a number of bytes from SPI USART interface, MSB first
 * @param data - pointer to buffer to store data in
 * @param size - number of bytes to read
 */
void spi_usart_read_msb(uint8_t* data, size_t size)
{
    while (size--)
    {
        *(data + size) = spi_usart_read_8();
    }
}
