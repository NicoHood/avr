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

#include <util/atomic.h>
#include "usb_keyboard.h"
#include "usb_keyboard_ascii_maps.h"

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Keyboard_HID_Interface =
    {
        .Config =
            {
                .InterfaceNumber                = INTERFACE_ID_Keyboard,
                .ReportINEndpoint               =
                    {
                        .Address                = KEYBOARD_EPADDR,
                        .Size                   = KEYBOARD_EPSIZE,
                        .Banks                  = 1,
                    },
                .PrevReportINBuffer             = NULL,
                .PrevReportINBufferSize         = sizeof(USB_KeyboardReport_Data_t),
            },
    };

// Keyboard report data and flag for sending it to the USB host
// Two buffers are required to not interrupt the API while modifying the report.
// Otherwise interrupts need to be disabled in every modification of the report.
static USB_KeyboardReport_Data_t USB_KeyboardReport_Data = { 0 };
static USB_KeyboardReport_Data_t USB_KeyboardReport_Data_Buffer = { 0 };
static uint8_t USB_KeyboardReport_Leds = 0x00;
static bool sendnow = false;

bool CALLBACK_HID_Keyboard_CreateHIDReport(USB_KeyboardReport_Data_t* ReportData)
{
    // Copy report data
    memcpy(ReportData, &USB_KeyboardReport_Data_Buffer, sizeof(USB_KeyboardReport_Data_Buffer));

    // Make sure to send the report now if we are waiting for it
    if(sendnow)
    {
        sendnow = false;
        return true;
    }
    return false;
}

void CALLBACK_HID_Keyboard_ProcessHIDReport(uint8_t* leds)
{
    USB_KeyboardReport_Leds = *leds;
}

uint8_t usb_keyboard_read_leds(void)
{
    return USB_KeyboardReport_Leds;
}

void usb_keybord_dump_report(void)
{
    // Print modifiers first, then all keycodes. Ignore reserved byte.
    printf("Modifiers: %X\n", USB_KeyboardReport_Data.Modifier);
    for (uint8_t i = 0; i < sizeof(USB_KeyboardReport_Data.KeyCode); i++) {
        printf("Key %d: %X\n", i, USB_KeyboardReport_Data.KeyCode[i]);
    }
}

int8_t usb_keyboard_send(void)
{
    // Copy report data
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        memcpy(&USB_KeyboardReport_Data_Buffer, &USB_KeyboardReport_Data, sizeof(USB_KeyboardReport_Data));
    }

    // Send the keyreport now. This function is blocking.
    sendnow = true;
    do{
        // Do not loop forever if the UB device disconnected
        if (USB_DeviceState != DEVICE_STATE_Configured){
            return _FDEV_ERR;
        }

        // The HID USBTask will call CALLBACK_HID_Keyboard_CreateHIDReport
        // which will unset 'sendnow' if the report was sent.
        HID_Device_USBTask(&Keyboard_HID_Interface);
    }
    while(sendnow);

    // No error
    return 0;
}

int8_t usb_keyboard_add_keycode(uint8_t k)
{
    // It's a modifier key
    if(k >= HID_KEYBOARD_SC_LEFT_CONTROL && k <= HID_KEYBOARD_SC_RIGHT_GUI)
    {
        // Convert key into bitfield (0 - 7)
        k -= HID_KEYBOARD_SC_LEFT_CONTROL;

        // Check if key is already present
        if (USB_KeyboardReport_Data.Modifier & (1 << k)){
            return 0;
        }

        // Add key
        USB_KeyboardReport_Data.Modifier |= (1 << k);
        return 1;
    }

    // Search for the first empty keyslot and add keycode
    for(uint8_t i = 0; i < sizeof(USB_KeyboardReport_Data.KeyCode); i++)
    {
        // Check for an empty slot
        if (!USB_KeyboardReport_Data.KeyCode[i]){
            USB_KeyboardReport_Data.KeyCode[i] = k;
            return 1;
        }

        // Check if key is already present
        if(USB_KeyboardReport_Data.KeyCode[i] == k){
            return 0;
        }
    }

    // No free slot found
    return _FDEV_EOF;
}

int8_t usb_keyboard_add(char c)
{
    // Ignore invalid input
    if (c >= sizeof(USB_keyboardAsciiMap)) {
        return _FDEV_EOF;
    }

    // Read keycode from AsciiMap. Might contain a shift modifier
    uint8_t k = pgm_read_byte(USB_keyboardAsciiMap + c);

    // Check for valid keycode
    if (!k) {
        return _FDEV_EOF;
    }

    // Add normal keycode first. Stop if keyreport is full.
    int8_t ret = usb_keyboard_add_keycode(k & ~SHIFT);
    if (ret < 0) {
        return ret;
    }

    // Add shift to modifier keys
    // The shift key is always available, the function can only return 0 or 1.
    if (k & SHIFT) {
        ret += usb_keyboard_add_keycode(HID_KEYBOARD_SC_LEFT_SHIFT);
    }

    // Return number of newly pressed keys
    return ret;
}

int8_t usb_keyboard_press_keycode(uint8_t k)
{
    // Add key(s) and remember how many were newly added
    int8_t ret = usb_keyboard_add_keycode(k);
    if (ret < 0) {
        return ret;
    }

    // Try to send report now
    if (ret) {
        int8_t ret2 = usb_keyboard_send();
        if (ret2 < 0){
            return ret2;
        }
    }

    // Return the number of pressed keys on success
    return ret;
}

int8_t usb_keyboard_press(char c)
{
    // Add key(s) and remember how many were newly added
    int8_t ret = usb_keyboard_add(c);
    if (ret < 0) {
        return ret;
    }

    // Try to send report now
    if (ret) {
        int8_t ret2 = usb_keyboard_send();
        if (ret2 < 0){
            return ret2;
        }
    }

    // Return the number of pressed keys on success
    return ret;
}

int8_t usb_keyboard_remove_keycode(uint8_t k)
{
    // It's a modifier key
    int8_t ret = 0;
    if(k >= HID_KEYBOARD_SC_LEFT_CONTROL && k <= HID_KEYBOARD_SC_RIGHT_GUI)
    {
        // Convert key into bitfield (0 - 7)
        k -= HID_KEYBOARD_SC_LEFT_CONTROL;

        // Check if key is already present
        ret = (bool)(USB_KeyboardReport_Data.Modifier & (1 << k));

        // Remove key
        USB_KeyboardReport_Data.Modifier &= ~(1 << k);
        return ret;
    }

    // Search for the key and remove keycode
    for(uint8_t i = 0; i < sizeof(USB_KeyboardReport_Data.KeyCode); i++)
    {
        // Check for an empty slot and abort
        if (!USB_KeyboardReport_Data.KeyCode[i]){
            break;
        }

        // Reorder all other keys to make add() function properly
        if (ret) {
            USB_KeyboardReport_Data.KeyCode[i - 1] = USB_KeyboardReport_Data.KeyCode[i];
            continue;
        }

        // Check if key is already present
        if(USB_KeyboardReport_Data.KeyCode[i] == k)
        {
            // Clear key (in case it is the last one in the report)
            USB_KeyboardReport_Data.KeyCode[i] = HID_KEYBOARD_SC_RESERVED;
            ret = 1;
        }
    }

    // Return if keycode got released
    return ret;
}

int8_t usb_keyboard_remove(char c)
{
    // Ignore invalid input
    if (c >= sizeof(USB_keyboardAsciiMap)) {
        return _FDEV_EOF;
    }

    // Read keycode from AsciiMap. Might contain a shift modifier
    uint8_t k = pgm_read_byte(USB_keyboardAsciiMap + c);

    // Check for valid keycode
    if (!k) {
        return _FDEV_EOF;
    }

    // Remove normal keycode first
    int8_t ret = usb_keyboard_remove_keycode(k & ~SHIFT);

    // Remove shift modifier
    if (k & SHIFT) {
        ret += usb_keyboard_remove_keycode(HID_KEYBOARD_SC_LEFT_SHIFT);
    }

    // Return number of removed keys
    return ret;
}

int8_t usb_keyboard_release_keycode(uint8_t k)
{
    // Add key(s) and remember how many were released
    int8_t ret = usb_keyboard_remove_keycode(k);
    if (ret < 0) {
        return ret;
    }

    // Try to send report now
    if (ret) {
        int8_t ret2 = usb_keyboard_send();
        if (ret2 < 0){
            return ret2;
        }
    }

    // Return the number of released keys on success
    return ret;
}

int8_t usb_keyboard_release(char c)
{
    // Add key(s) and remember how many were released
    int8_t ret = usb_keyboard_remove(c);
    if (ret < 0) {
        return ret;
    }

    // Try to send report now
    if (ret) {
        int8_t ret2 = usb_keyboard_send();
        if (ret2 < 0){
            return ret2;
        }
    }

    // Return the number of released keys on success
    return ret;
}

int8_t usb_keyboard_write_keycode(uint8_t k)
{
    // First press key(s) then ALWAYS release key(s)
    // Otherwise keys will stay in report an produce weird errors.
    int8_t ret = usb_keyboard_press_keycode(k);
    int8_t ret2 = usb_keyboard_release_keycode(k);
    if(ret2 < 0){
        return ret2;
    }

    // Return number of newly added keys
    return ret;
}

int8_t usb_keyboard_write(char c)
{
    // First press key(s) then ALWAYS release key(s)
    // Otherwise keys will stay in report an produce weird errors.
    int8_t ret = usb_keyboard_press(c);
    int8_t ret2 = usb_keyboard_release(c);
    if(ret2 < 0){
        return ret2;
    }

    // Return number of newly added keys
    return ret;
}

int8_t usb_keyboard_clear(void)
{
    // Clear all report data. Used on device (re)connect.
    int8_t ret = 0;

    // Count all pressed modifier keys
    while(USB_KeyboardReport_Data.Modifier)
    {
        if (USB_KeyboardReport_Data.Modifier & 0x01) {
            ret++;
        }
        USB_KeyboardReport_Data.Modifier >>= 1;
    }

    // Count all released keycodes
    for (uint8_t i = 0; i < sizeof(USB_KeyboardReport_Data.KeyCode); i++)
    {
        if (USB_KeyboardReport_Data.KeyCode[i]) {
            ret++;
        }
        USB_KeyboardReport_Data.KeyCode[i] = 0x00;
    }
    return ret;
}

int8_t usb_keyboard_release_all(void)
{
    // First clear report
    int ret = usb_keyboard_clear();
    if (ret)
    {
        // Then try to send the empty report
        int8_t ret2 = usb_keyboard_send();
        if (ret2 < 0) {
            return ret2;
        }
    }
    return ret;
}

static int usb_keyboard_fputc(char c, FILE *stream)
{
    int8_t ret = usb_keyboard_write(c);
    if (ret < 0) {
        return _FDEV_EOF;
    }
    return c;
}

void usb_keyboard_init_stream(FILE* const stream)
{
	*stream = (FILE)FDEV_SETUP_STREAM(usb_keyboard_fputc, NULL, _FDEV_SETUP_WRITE);
}
