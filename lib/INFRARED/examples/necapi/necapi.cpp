/*
Copyright (c) 2018 NicoHood
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
#include <avr/interrupt.h> // sei()
#include <stdio.h> // printf()
#include <avr/pgmspace.h> // Progmem
#include "board.h"
#include "usb.h"
#include "usb_keyboard.h"
#include "usb_cdc_serial.h"
#include "infrared.h"

#if defined(CUSTOM_BOARD)
    // Pin 13 Arduino Uno
    #define LED_BUILTIN     13
    #define LED_INIT()      DDRB  |=  (1 << PB5)
    #define LED_ON()        PORTB |=  (1 << PB5)
    #define LED_OFF()       PORTB &= ~(1 << PB5)
    #define LED_TOGGLE()    PORTB ^=  (1 << PB5)
#endif

// Choose the IR protocol of your remote. See the other example for this.
using namespace IRL_Protek_Remote;
void NecEvent(void);
CNecAPI<NecEvent, IRL_ADDRESS> IRLremote;

// Settings
// Attention: Check if you pin is a valid PCINT and if it is enabled in the makefile!
#define PIN_IR 10

// Filestreams for stdio functions
static FILE SerialStream;

int main(void)
{
    // Init peripherals
    LED_INIT();
    timer0_init();

    // Setup serial input stream
#if defined(DMBS_MODULE_USART)
    usart_init();
    usart_init_stream(&SerialStream);
#else
    USB_Init();
    usb_cdc_serial_init_stream(&SerialStream);
#endif

    stdin = &SerialStream;
    stdout = &SerialStream;

    // Start reading the remote. PinInterrupt or PinChangeInterrupt* will automatically be selected
    if (!IRLremote.begin<PIN_IR>())
    {
        puts_P(PSTR("You did not choose a valid pin."));
    }
    puts_P(PSTR("Startup"));

    // Enable global Interrupts and loop
    sei();

    while(true){
        // Check if we are currently receiving data
        //if (!IRLremote.receiving()) {
            // Run code that disables interrupts, such as some led strips
        //}

        // Get the new data from the remote
        // Always call this to check for timeouts
        IRLremote.read();
    }
}

void NecEvent(void)
{
    // Get the actual command that we are processing
    auto command = IRLremote.command();

    // How many times was the button pressed in a row?
    uint8_t count = IRLremote.count();

    // How long was the current button press held down?
    uint8_t duration = IRLremote.duration();

    // Did the button press series end and how long was the last duration?
    uint8_t released = IRLremote.released();

    // Light Led
    if (duration) {
        LED_ON();
    }
    else if (released) {
        LED_OFF();
    }

    // Debug print
    //printf("-------------------\n");
    //printf("command: %d\n", command);
    //printf("count: %d\n", count);
    //printf("duration: %d\n", duration);
    //printf("released: %d\n", released);

    // Press HID keys with a protek remote
    switch (command)
    {
        // Shutdown menu
        case IRL_KEYCODE_POWER:
            // Only recognize first button press to abort on multiple presses
            if (count == 1){
                // Wakeup PC from suspend
                if (USB_DeviceState == DEVICE_STATE_Suspended && USB_Device_RemoteWakeupEnabled)
                {
                    USB_Device_SendRemoteWakeup();
                }
                else {
                    // Hold the button some time (around 3 sec) to trigger shutdown button
                    if (duration == 16) {
                        usb_keyboard_write_keycode(HID_KEYBOARD_SC_POWER);
                    }
                    else if (released && released < 16) {
                        usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_SLEEP);
                    }
                }
            }
        break;

        // Global system mute
        case IRL_KEYCODE_MUTE:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_MUTE);
            }
        break;

        // Fullscreen/Window mode
        case IRL_KEYCODE_SCREEN:
            // Trigger on every keydown event
            if (duration == 1) {
                // TODO not implemented
            }
        break;

        // Live TV channels window
        case IRL_KEYCODE_SATELLITE:
            // Trigger on every keydown event
            if (duration == 1) {
                // Kodi: Open TV
                usb_keyboard_write('h');
            }
        break;

        // Open videos
        case IRL_KEYCODE_TV_RADIO:
            // Trigger on every keydown event
            if (duration == 1) {
                // Kodi: Open videos
                usb_keyboard_press_keycode(HID_KEYBOARD_SC_LEFT_CONTROL);
                usb_keyboard_press('e');
                usb_keyboard_release_all();
            }
        break;

        // Open music
        case IRL_KEYCODE_TV_MUSIC:
            // Trigger on every keydown event
            if (duration == 1) {
                // Kodi: Open music
                usb_keyboard_press_keycode(HID_KEYBOARD_SC_LEFT_CONTROL);
                usb_keyboard_press('m');
                usb_keyboard_release_all();
            }
        break;

        // 1
        case IRL_KEYCODE_1:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write('1');
            }
        break;

        // Letters are used to qickly jump between movies in the library
        // A, B, C, 2
        case IRL_KEYCODE_2:
            // Trigger depending on how many times the key was pressed
            if (released) {
                if (count == 1){
                    usb_keyboard_write('A');
                }
                else if (count == 2){
                    usb_keyboard_write('B');
                }
                else if (count == 3){
                    usb_keyboard_write('C');
                }
                else if (count == 4){
                    usb_keyboard_write('2');
                }
            }
        break;



        // D, E, F, 3
        case IRL_KEYCODE_3:
            // Trigger depending on how many times the key was pressed
            if (released) {
                if (count == 1){
                    usb_keyboard_write('D');
                }
                else if (count == 2){
                    usb_keyboard_write('E');
                }
                else if (count == 3){
                    usb_keyboard_write('F');
                }
                else if (count == 4){
                    usb_keyboard_write('3');
                }
            }
        break;

        // G, H, I, 4
        case IRL_KEYCODE_4:
            // Trigger depending on how many times the key was pressed
            if (released) {
                if (count == 1){
                    usb_keyboard_write('G');
                }
                else if (count == 2){
                    usb_keyboard_write('H');
                }
                else if (count == 3){
                    usb_keyboard_write('I');
                }
                else if (count == 4){
                    usb_keyboard_write('4');
                }
            }
        break;

        // J, K, L, 5
        case IRL_KEYCODE_5:
            // Trigger depending on how many times the key was pressed
            if (released) {
                if (count == 1){
                    usb_keyboard_write('J');
                }
                else if (count == 2){
                    usb_keyboard_write('K');
                }
                else if (count == 3){
                    usb_keyboard_write('L');
                }
                else if (count == 4){
                    usb_keyboard_write('5');
                }
            }
        break;

        // M, N, O, 6
        case IRL_KEYCODE_6:
            // Trigger depending on how many times the key was pressed
            if (released) {
                if (count == 1){
                    usb_keyboard_write('M');
                }
                else if (count == 2){
                    usb_keyboard_write('N');
                }
                else if (count == 3){
                    usb_keyboard_write('O');
                }
                else if (count == 4){
                    usb_keyboard_write('6');
                }
            }
        break;

        // P, Q, R, S, 7
        case IRL_KEYCODE_7:
            // Trigger depending on how many times the key was pressed
            if (released) {
                if (count == 1){
                    usb_keyboard_write('P');
                }
                else if (count == 2){
                    usb_keyboard_write('Q');
                }
                else if (count == 3){
                    usb_keyboard_write('R');
                }
                else if (count == 4){
                    usb_keyboard_write('S');
                }
                else if (count == 5){
                    usb_keyboard_write('7');
                }
            }
        break;

        // T, U, V, 8
        case IRL_KEYCODE_8:
            // Trigger depending on how many times the key was pressed
            if (released) {
                if (count == 1){
                    usb_keyboard_write('T');
                }
                else if (count == 2){
                    usb_keyboard_write('U');
                }
                else if (count == 3){
                    usb_keyboard_write('V');
                }
                else if (count == 4){
                    usb_keyboard_write('8');
                }
            }
        break;

        // W, X, Y, Z, 9
        case IRL_KEYCODE_9:
            // Trigger depending on how many times the key was pressed
            if (released) {
                if (count == 1){
                    usb_keyboard_write('W');
                }
                else if (count == 2){
                    usb_keyboard_write('X');
                }
                else if (count == 3){
                    usb_keyboard_write('Y');
                }
                else if (count == 4){
                    usb_keyboard_write('Z');
                }
                else if (count == 5){
                    usb_keyboard_write('9');
                }
            }
        break;

        // Space, 0
        case IRL_KEYCODE_0:
            // Trigger depending on how many times the key was pressed
            if (released) {
                if (count == 1){
                    usb_keyboard_write(' ');
                }
                else if (count == 2){
                    usb_keyboard_write('0');
                }
            }
        break;

        case IRL_KEYCODE_BACK:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_BACKSPACE);
            }
        break;

        case IRL_KEYCODE_FAVORITE:
            // Trigger on every keydown event
            if (duration == 1) {
                // Kodi: Mark as favorite movie
                usb_keyboard_write('w');
            }
        break;

        // Volume up
        case IRL_KEYCODE_VOL_UP:
            // Trigger on every holding duration unit
            if (duration) {
                // Kodi: Mark as favorite movie
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_VOLUME_UP);
            }
        break;

        // Volume down
        case IRL_KEYCODE_VOL_DOWN:
            // Trigger on every holding duration unit
            if (duration) {
                // Kodi: Mark as favorite movie
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_VOLUME_DOWN);
            }
        break;

        // Live TV EPG/TV guide
        case IRL_KEYCODE_EPG:
            // Trigger on every keydown event
            if (duration == 1) {
                // Kodi: Open EPG
                usb_keyboard_write('e');
            }
        break;

        // Info
        case IRL_KEYCODE_INFO:
            // Trigger on every keydown event
            if (duration == 1) {
                // Kodi: Open info
                usb_keyboard_write('i');
            }
        break;

        // Increase rating/Scroll up
        case IRL_KEYCODE_CHANNEL_UP:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_PAGE_UP);
            }
        break;

        // Decrease rating/Scroll down
        case IRL_KEYCODE_CHANNEL_DOWN:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_PAGE_DOWN);
            }
        break;

        // Navigation
        case IRL_KEYCODE_RIGHT:
            // Trigger on every 2nd holding duration
            if (duration % 2) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_RIGHT_ARROW);
            }
        break;
        case IRL_KEYCODE_LEFT:
            // Trigger on every 2nd holding duration
            if (duration % 2) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_LEFT_ARROW);
            }
        break;
        case IRL_KEYCODE_UP:
            // Trigger on every 2nd holding duration
            if (duration % 2) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_UP_ARROW);
            }
        break;
        case IRL_KEYCODE_DOWN:
            // Trigger on every 2nd holding duration
            if (duration % 2) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_DOWN_ARROW);
            }
        break;

        // Enter menu, play/pause, general okay button
        case IRL_KEYCODE_OK:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_ENTER);
            }
        break;

        // Exit full screeen
        case IRL_KEYCODE_EXIT:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_ESCAPE);
            }
        break;

        // Context menu/Playlist
        case IRL_KEYCODE_MENU:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_MENU);
            }
        break;

        // I-II
        case IRL_KEYCODE_I_II:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_LEFT_GUI);
            }
        break;

        // Teletext/Visualization settings
        case IRL_KEYCODE_TELETEXT:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_press_keycode(HID_KEYBOARD_SC_LEFT_SHIFT);
                usb_keyboard_press_keycode(HID_KEYBOARD_SC_TAB);
                usb_keyboard_release_all();
            }
        break;

        // Next subtitle
        case IRL_KEYCODE_SUBTITLE:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_TAB);
            }
        break;

        // Switch applications
        case IRL_KEYCODE_ADD:
            // Hold ALT key unless button was not pressed anymore
            if (duration == 1) {
                if (count == 1) {
                    usb_keyboard_press_keycode(HID_KEYBOARD_SC_LEFT_ALT);
                }
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_TAB);
            }
            else if (released) {
                usb_keyboard_release_all();
            }
        break;

        // Red
        case IRL_KEYCODE_RED:
            // Trigger on every keydown event
            if (duration == 1) {
                // TODO Not implemented
            }
        break;

        // Green
        case IRL_KEYCODE_GREEN:
            // Trigger on every keydown event
            if (duration == 1) {
                // TODO Not implemented
            }
        break;

        // Yellow
        case IRL_KEYCODE_YELLOW:
            // Trigger on every keydown event
            if (duration == 1) {
                // TODO Not implemented
            }
        break;

        // Blue
        case IRL_KEYCODE_BLUE:
            // Trigger on every keydown event
            if (duration == 1) {
                // TODO Not implemented
            }
        break;

        // Play/Pause
        case IRL_KEYCODE_PLAY:
        case IRL_KEYCODE_PAUSE:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_PLAY);
            }
        break;

        // Skip forward
        case IRL_KEYCODE_NEXT:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_NEXT_TRACK);
            }
        break;

        // Skip backward
        case IRL_KEYCODE_PREV:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_PREVIOUS_TRACK);
            }
        break;

        // Stop
        case IRL_KEYCODE_STOP:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_MEDIA_STOP);
            }
        break;

        // Change keyboard layout
        case IRL_KEYCODE_USB:
            // Trigger on every keydown event
            if (duration == 1) {
                // TODO not implemented
            }
        break;

        // Screenshot
        case IRL_KEYCODE_REC:
            // Trigger on every keydown event
            if (duration == 1) {
                usb_keyboard_write_keycode(HID_KEYBOARD_SC_PRINT_SCREEN);
            }
        break;

        // LIVE
        case IRL_KEYCODE_LIVE:
            // Trigger on every keydown event
            if (duration == 1) {
                // TODO not implemented
            }
        break;
    }
}
