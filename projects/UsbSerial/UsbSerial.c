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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "board_leds.h"
#include "usart.h"
#include "usb_cdc_serial.h"

// Filestreams for stdio functions
static FILE USBSerialStream;
static FILE UsartSerialStream;

int main(void)
{
    // Initialize libraries
    usart_init();
    USB_Init();

    // Create a regular character stream for the interface so that it can be used with the stdio.h functions
    usart_init_stream(&UsartSerialStream);
    usb_cdc_serial_init_stream(&USBSerialStream);

    // Setup stdio functionallity
    //stdin = &USBSerialStream;
    //stdout = &USBSerialStream;

    // Enable interrupts
    sei();

    // Print startup message
    //fprintf(&UsartSerialStream, "Hello HW Serial\n");
    //fprintf(&USBSerialStream, "Hello USB Serial\n");

    while(true)
    {
        // Usb -> Serial
		int c = fgetc(&USBSerialStream);
        if(c >= 0){
            fputc(c, &UsartSerialStream);
            //fputc(c, &USBSerialStream);
        }

        // Serial -> Usb
        c = fgetc(&UsartSerialStream);
        if(c >= 0){
            fputc(c, &USBSerialStream);
            //fputc(c, &UsartSerialStream);
        }
    }
}

/** Event handler for the CDC Class driver Line Encoding Changed event.
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
    // Reconfigure usart
    usart_set_baud(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS,
                   CDCInterfaceInfo->State.LineEncoding.ParityType,
                   CDCInterfaceInfo->State.LineEncoding.CharFormat,
                   CDCInterfaceInfo->State.LineEncoding.DataBits);
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	return;
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	return;
}
