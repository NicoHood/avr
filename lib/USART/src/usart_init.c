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

#include "usart.h"
#include "usart_private.h"

void usart_init(void)
{
    // Initialize baud rate. Can be changed later.
    USART_UBRRH = UBRRH_VALUE;
    USART_UBRRL = UBRRL_VALUE;

#if (USE_2X)
    USART_UCSRA |= (1 << USART_U2X);
#endif

    USART_UCSRC = (USART_PARITY | USART_STOP_BITS | USART_DATA_BITS);

    // Determine UCSRB value based on selected compile-time options
    uint8_t USART_UCSRB_VAL = ((1 << USART_RXEN) | (1 << USART_TXEN));
#if defined(USART_URSEL)
    USART_UCSRB_VAL |= (1 << USART_URSEL);
#endif
#if (USART_BUFFER_RX)
    USART_UCSRB_VAL |= (1 << USART_RXCIE);
#endif
    USART_UCSRB = USART_UCSRB_VAL;
}

void usart_init_baud(uint32_t baud, uint8_t parity, uint8_t stop_bits, uint8_t data_bits)
{
    // Calculate ubrr value for selected baudrate
    uint16_t ubrr = SERIAL_2X_UBBRVAL(baud);
    USART_UBRRH = (ubrr >> 8);
    USART_UBRRL = (ubrr & 0xFF);

    // Always use 2X
    USART_UCSRA |= (1 << USART_U2X);

    // Get ucsrc config data
    uint8_t ucsrc = 0;

    // Default: No parity
    switch(parity)
    {
        // Odd
        case 1:
            ucsrc |= USART_PARITY_ODD;
            break;
        // Even
        case 2:
            ucsrc |= USART_PARITY_EVEN;
            break;
        default:
            ucsrc |= USART_PARITY_NO;
            break;
    }

    // Default: 1 stop bit
	if (stop_bits == 2){
        ucsrc |= USART_STOP_BITS_2;
    }
    else{
        ucsrc |= USART_STOP_BITS_1;
    }

    // Default: 8 data bits
	switch (data_bits)
	{
        case 5:
			ucsrc |= USART_DATA_BITS_5;
			break;
        case 6:
			ucsrc |= USART_DATA_BITS_6;
			break;
		case 7:
			ucsrc |= USART_DATA_BITS_7;
			break;
		case 8:
        default:
			ucsrc |= USART_DATA_BITS_8;
			break;
	}

    // Set config data
    USART_UCSRC = ucsrc;

    // Determine UCSRB value based on selected compile-time options
    uint8_t USART_UCSRB_VAL = ((1 << USART_RXEN) | (1 << USART_TXEN));
#if defined(USART_URSEL)
    USART_UCSRB_VAL |= (1 << USART_URSEL);
#endif
#if (USART_BUFFER_RX)
    USART_UCSRB_VAL |= (1 << USART_RXCIE);
#endif
    USART_UCSRB = USART_UCSRB_VAL;
}

void usart_detach(void)
{
    // Flush remaining bytes
    usart_flush();

    // Disable all usart funtions
    USART_UCSRB = 0;
    USART_UCSRA = 0;
    USART_UCSRC = 0;
}

void usart_set_baud(uint32_t baud, uint8_t parity, uint8_t stop_bits, uint8_t data_bits)
{
    // Keep the TX line held high (idle) while the USART is reconfigured
	USART_TX_HIGH();

    // Must turn off USART before reconfiguring it, otherwise incorrect operation may occur
    usart_detach();

    // Only change baudrate if it was specified
    if(baud){
        usart_init_baud(baud, parity, stop_bits, data_bits);
    }

    // Release the TX line after the USART has been reconfigured
	USART_TX_LOW();
}

void usart_init_stream(FILE* const stream)
{
	*stream = (FILE)FDEV_SETUP_STREAM(usart_fputc, usart_fgetc, _FDEV_SETUP_RW);
}
