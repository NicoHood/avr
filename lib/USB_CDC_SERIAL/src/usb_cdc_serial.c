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

#include "usb_descriptors.h"
#include "board_leds.h"

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber         = INTERFACE_ID_CDC_CCI,
				.DataINEndpoint                 =
					{
						.Address                = CDC_TX_EPADDR,
						.Size                   = CDC_TXRX_EPSIZE,
						.Banks                  = 2,
					},
				.DataOUTEndpoint                =
					{
						.Address                = CDC_RX_EPADDR,
						.Size                   = CDC_TXRX_EPSIZE,
						.Banks                  = 2,
					},
				.NotificationEndpoint           =
					{
						.Address                = CDC_NOTIFICATION_EPADDR,
						.Size                   = CDC_NOTIFICATION_EPSIZE,
						.Banks                  = 1,
					},
			},
	};

static bool HostReady = false;

/** CDC class driver callback function the processing of changes to the virtual
 *  control lines sent from the host..
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo)
{
	/* You can get changes to the virtual CDC lines in this callback; a common
	   use-case is to use the Data Terminal Ready (DTR) flag to enable and
	   disable CDC communications in your application when set to avoid the
	   application blocking while waiting for a host to become ready and read
	   in the pending data from the USB endpoints.
	*/
	HostReady = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR) != 0;
}

#define TX_RX_LED_PULSE_MS 3
static uint8_t rx_led_count = 0;
static uint8_t tx_led_count = 0;

void usb_cdc_serial_init(void)
{
    // Init Leds
    RX_LED_INIT();
    TX_LED_INIT();
    RX_LED_OFF();
    TX_LED_OFF();
    rx_led_count = 0;
    tx_led_count = 0;
}

void CDC_Device_MillisecondElapsed(void)
{
    // Turn off leds after a few milliseconds
    if (tx_led_count) {
        tx_led_count--;
        if (!tx_led_count) {
            TX_LED_OFF();
        }
    }
    if (rx_led_count) {
        rx_led_count--;
        if (!rx_led_count) {
            RX_LED_OFF();
        }
    }
}

static int usb_cdc_serial_fputc(char c, FILE* stream)
{
    TX_LED_ON();
    tx_led_count = TX_RX_LED_PULSE_MS;
	if (!HostReady) {
		return _FDEV_ERR;
	}
	return CDC_Device_SendByte(&VirtualSerial_CDC_Interface, c) ? _FDEV_ERR : c;
}

static int usb_cdc_serial_fgetc(FILE* stream)
{
	int16_t ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);

	if (ReceivedByte < 0){
        return _FDEV_EOF;
    }

    RX_LED_ON();
    rx_led_count = TX_RX_LED_PULSE_MS;

	return ReceivedByte;
}

void usb_cdc_serial_init_stream(FILE* const stream)
{
    *stream = (FILE)FDEV_SETUP_STREAM(usb_cdc_serial_fputc, usb_cdc_serial_fgetc, _FDEV_SETUP_RW);
}
