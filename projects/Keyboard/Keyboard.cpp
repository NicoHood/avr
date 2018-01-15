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
#include "fastpin.h"
#include "board_leds.h"
#include "usb.h"
#include "usb_keyboard.h"

// Filestreams for stdio functions
static FILE UsbKeyboardStream;

int main(void)
{
    // Initialize libraries
    USB_Init();

    // Setup stdio functionallity
    usb_keyboard_init_stream(&UsbKeyboardStream);

    LED_INIT();
    FastPin<7> pin;
    pin.setInputPullup();

    // Startup delay, required for pullup to activate
    _delay_ms(10);

    // Enable interrupts
    sei();

    while(true)
    {
        // Test keyboard leds
        // uint8_t leds = usb_keyboard_read_leds();
        // if(leds & HID_KEYBOARD_LED_CAPSLOCK) {
        //     LED_ON();
        // }
        // else {
        //     LED_OFF();
        // }

        if (!pin.get()) {
            LED_ON();

            // Press a key/s on the keyboard
            if(USB_DeviceState == DEVICE_STATE_Configured){
                // Media/Consumer keys
                //usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_VOLUME_UP);
                //usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_MUTE);
                //usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_PLAY);
                //usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_NEXT_TRACK);

                // Power functionallity
                //usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_LOCK);
                //usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_SLEEP);
                //usb_keyboard_write_keycode(HID_KEYBOARD_SC_POWER);

                // Combine keycode with ascii API and press a key combination
                //usb_keyboard_press_keycode(HID_KEYBOARD_SC_LEFT_CONTROL);
                //usb_keyboard_press('a');
                //usb_keyboard_release_all();

                // Stdio functionallity
                fprintf(&UsbKeyboardStream, "Hello Keyboard!\n");
            }

            // Wakeup PC from suspend
            if (USB_DeviceState == DEVICE_STATE_Suspended && USB_Device_RemoteWakeupEnabled)
            {
                USB_Device_SendRemoteWakeup();
            }

            // Debounce
            while(!pin.get());
            _delay_ms(50);
            LED_OFF();
        }
        else {
            LED_OFF();
        }
    }
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
