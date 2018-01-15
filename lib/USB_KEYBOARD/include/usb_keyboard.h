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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Software version
#define USB_KEYBOARD_VERSION 100

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <LUFA/Drivers/USB/Class/Common/HIDClassCommon.h>

// Select keyboard layout
#ifdef USB_KEYBOARD_LAYOUT_US
#define USB_keyboardAsciiMap USB_keyboardAsciiMapUs
#else
#warning "No valid keyboard layout selected. Falling back to USB_KEYBOARD_LAYOUT_US."
#define USB_keyboardAsciiMap USB_keyboardAsciiMapUs
#endif

/*
Common return values for keyboard API:
    -2 = Full report buffer or invalid keycode (_FDEV_EOF)
    -1 = USB error (_FDEV_ERR)
     0 = OK, key already pressed/released nothing to send/released
     1 = OK, 1 key sent/released
     n = OK, n keys sent/released
*/

// Function for sending the keyreport
int8_t usb_keyboard_send(void);

// Functions to modify the keyreport
int8_t usb_keyboard_add_keycode(uint8_t k);
int8_t usb_keyboard_add(char c);
int8_t usb_keyboard_remove_keycode(uint8_t k);
int8_t usb_keyboard_remove(char c);
int8_t usb_keyboard_clear(void);

// Function to debug print the keyboard report to stdout
void usb_keybord_dump_report(void);

// Functions for modifying and sending the keyreport
int8_t usb_keyboard_press_keycode(uint8_t k);
int8_t usb_keyboard_press(char c);
int8_t usb_keyboard_release_keycode(uint8_t k);
int8_t usb_keyboard_release(char c);
int8_t usb_keyboard_release_all(void);

// Press and release combined
int8_t usb_keyboard_write_keycode(uint8_t k);
int8_t usb_keyboard_write(char c);

// Stream functions
void usb_keyboard_init_stream(FILE* const stream);

// Get led state
uint8_t usb_keyboard_read_leds(void);

#ifdef __cplusplus
}
#endif
