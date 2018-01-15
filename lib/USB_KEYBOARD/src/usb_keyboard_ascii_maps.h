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

#include <avr/pgmspace.h>
#include <LUFA/Drivers/USB/Class/Common/HIDClassCommon.h>

#define SHIFT 0x80
static const uint8_t USB_keyboardAsciiMapUs[] PROGMEM =
{
    HID_KEYBOARD_SC_RESERVED,                                   // NUL
    HID_KEYBOARD_SC_RESERVED,                                   // SOH
    HID_KEYBOARD_SC_RESERVED,                                   // STX
    HID_KEYBOARD_SC_RESERVED,                                   // ETX
    HID_KEYBOARD_SC_RESERVED,                                   // EOT
    HID_KEYBOARD_SC_RESERVED,                                   // ENQ
    HID_KEYBOARD_SC_RESERVED,                                   // ACK
    HID_KEYBOARD_SC_RESERVED,                                   // BEL
    HID_KEYBOARD_SC_BACKSPACE,                                  // BS Backspace
    HID_KEYBOARD_SC_TAB,                                        // TAB    Tab
    HID_KEYBOARD_SC_ENTER,                                      // LF Enter
    HID_KEYBOARD_SC_RESERVED,                                   // VT
    HID_KEYBOARD_SC_RESERVED,                                   // FF
    HID_KEYBOARD_SC_RESERVED,                                   // CR
    HID_KEYBOARD_SC_RESERVED,                                   // SO
    HID_KEYBOARD_SC_RESERVED,                                   // SI
    HID_KEYBOARD_SC_RESERVED,                                   // DEL
    HID_KEYBOARD_SC_RESERVED,                                   // DC1
    HID_KEYBOARD_SC_RESERVED,                                   // DC2
    HID_KEYBOARD_SC_RESERVED,                                   // DC3
    HID_KEYBOARD_SC_RESERVED,                                   // DC4
    HID_KEYBOARD_SC_RESERVED,                                   // NAK
    HID_KEYBOARD_SC_RESERVED,                                   // SYN
    HID_KEYBOARD_SC_RESERVED,                                   // ETB
    HID_KEYBOARD_SC_RESERVED,                                   // CAN
    HID_KEYBOARD_SC_RESERVED,                                   // EM
    HID_KEYBOARD_SC_RESERVED,                                   // SUB
    HID_KEYBOARD_SC_RESERVED,                                   // ESC
    HID_KEYBOARD_SC_RESERVED,                                   // FS
    HID_KEYBOARD_SC_RESERVED,                                   // GS
    HID_KEYBOARD_SC_RESERVED,                                   // RS
    HID_KEYBOARD_SC_RESERVED,                                   // US

    HID_KEYBOARD_SC_SPACE,                                      // ' ' Space
    HID_KEYBOARD_SC_1_AND_EXCLAMATION|SHIFT,                    // !
    HID_KEYBOARD_SC_APOSTROPHE_AND_QUOTE|SHIFT,                 // "
    HID_KEYBOARD_SC_3_AND_HASHMARK|SHIFT,                       // #
    HID_KEYBOARD_SC_4_AND_DOLLAR|SHIFT,                         // $
    HID_KEYBOARD_SC_5_AND_PERCENTAGE|SHIFT,                     // %
    HID_KEYBOARD_SC_7_AND_AMPERSAND|SHIFT,                      // &
    HID_KEYBOARD_SC_APOSTROPHE_AND_QUOTE,                       // '
    HID_KEYBOARD_SC_9_AND_OPENING_PARENTHESIS|SHIFT,            // (
    HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS|SHIFT,            // )
    HID_KEYBOARD_SC_8_AND_ASTERISK|SHIFT,                       // *
    HID_KEYBOARD_SC_EQUAL_AND_PLUS|SHIFT,                       // +
    HID_KEYBOARD_SC_COMMA_AND_LESS_THAN_SIGN,                   // ,
    HID_KEYBOARD_SC_MINUS_AND_UNDERSCORE,                       // -
    HID_KEYBOARD_SC_DOT_AND_GREATER_THAN_SIGN,                  // .
    HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK,                    // /
    HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS,                  // 0
    HID_KEYBOARD_SC_1_AND_EXCLAMATION,                          // 1
    HID_KEYBOARD_SC_2_AND_AT,                                   // 2
    HID_KEYBOARD_SC_3_AND_HASHMARK,                             // 3
    HID_KEYBOARD_SC_4_AND_DOLLAR,                               // 4
    HID_KEYBOARD_SC_5_AND_PERCENTAGE,                           // 5
    HID_KEYBOARD_SC_6_AND_CARET,                                // 6
    HID_KEYBOARD_SC_7_AND_AMPERSAND,                            // 7
    HID_KEYBOARD_SC_8_AND_ASTERISK,                             // 8
    HID_KEYBOARD_SC_9_AND_OPENING_PARENTHESIS,                  // 9
    HID_KEYBOARD_SC_SEMICOLON_AND_COLON|SHIFT,                  // :
    HID_KEYBOARD_SC_SEMICOLON_AND_COLON,                        // ;
    HID_KEYBOARD_SC_COMMA_AND_LESS_THAN_SIGN|SHIFT,             // <
    HID_KEYBOARD_SC_EQUAL_AND_PLUS,                             // =
    HID_KEYBOARD_SC_DOT_AND_GREATER_THAN_SIGN|SHIFT,            // >
    HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK|SHIFT,              // ?
    HID_KEYBOARD_SC_2_AND_AT|SHIFT,                             // @
    HID_KEYBOARD_SC_A|SHIFT,                                    // A
    HID_KEYBOARD_SC_B|SHIFT,                                    // B
    HID_KEYBOARD_SC_C|SHIFT,                                    // C
    HID_KEYBOARD_SC_D|SHIFT,                                    // D
    HID_KEYBOARD_SC_E|SHIFT,                                    // E
    HID_KEYBOARD_SC_F|SHIFT,                                    // F
    HID_KEYBOARD_SC_G|SHIFT,                                    // G
    HID_KEYBOARD_SC_H|SHIFT,                                    // H
    HID_KEYBOARD_SC_I|SHIFT,                                    // I
    HID_KEYBOARD_SC_J|SHIFT,                                    // J
    HID_KEYBOARD_SC_K|SHIFT,                                    // K
    HID_KEYBOARD_SC_L|SHIFT,                                    // L
    HID_KEYBOARD_SC_M|SHIFT,                                    // M
    HID_KEYBOARD_SC_N|SHIFT,                                    // N
    HID_KEYBOARD_SC_O|SHIFT,                                    // O
    HID_KEYBOARD_SC_P|SHIFT,                                    // P
    HID_KEYBOARD_SC_Q|SHIFT,                                    // Q
    HID_KEYBOARD_SC_R|SHIFT,                                    // R
    HID_KEYBOARD_SC_S|SHIFT,                                    // S
    HID_KEYBOARD_SC_T|SHIFT,                                    // T
    HID_KEYBOARD_SC_U|SHIFT,                                    // U
    HID_KEYBOARD_SC_V|SHIFT,                                    // V
    HID_KEYBOARD_SC_W|SHIFT,                                    // W
    HID_KEYBOARD_SC_X|SHIFT,                                    // X
    HID_KEYBOARD_SC_Y|SHIFT,                                    // Y
    HID_KEYBOARD_SC_Z|SHIFT,                                    // Z
    HID_KEYBOARD_SC_OPENING_BRACKET_AND_OPENING_BRACE,          // [
    HID_KEYBOARD_SC_BACKSLASH_AND_PIPE,                         // bslash
    HID_KEYBOARD_SC_CLOSING_BRACKET_AND_CLOSING_BRACE,          // ]
    HID_KEYBOARD_SC_6_AND_CARET|SHIFT,                          // ^
    HID_KEYBOARD_SC_MINUS_AND_UNDERSCORE|SHIFT,                 // _
    HID_KEYBOARD_SC_NON_US_HASHMARK_AND_TILDE,                  // `
    HID_KEYBOARD_SC_A,                                          // a
    HID_KEYBOARD_SC_B,                                          // b
    HID_KEYBOARD_SC_C,                                          // c
    HID_KEYBOARD_SC_D,                                          // d
    HID_KEYBOARD_SC_E,                                          // e
    HID_KEYBOARD_SC_F,                                          // f
    HID_KEYBOARD_SC_G,                                          // g
    HID_KEYBOARD_SC_H,                                          // h
    HID_KEYBOARD_SC_I,                                          // i
    HID_KEYBOARD_SC_J,                                          // j
    HID_KEYBOARD_SC_K,                                          // k
    HID_KEYBOARD_SC_L,                                          // l
    HID_KEYBOARD_SC_M,                                          // m
    HID_KEYBOARD_SC_N,                                          // n
    HID_KEYBOARD_SC_O,                                          // o
    HID_KEYBOARD_SC_P,                                          // p
    HID_KEYBOARD_SC_Q,                                          // q
    HID_KEYBOARD_SC_R,                                          // r
    HID_KEYBOARD_SC_S,                                          // s
    HID_KEYBOARD_SC_T,                                          // t
    HID_KEYBOARD_SC_U,                                          // u
    HID_KEYBOARD_SC_V,                                          // v
    HID_KEYBOARD_SC_W,                                          // w
    HID_KEYBOARD_SC_X,                                          // x
    HID_KEYBOARD_SC_Y,                                          // y
    HID_KEYBOARD_SC_Z,                                          // z
    HID_KEYBOARD_SC_OPENING_BRACKET_AND_OPENING_BRACE|SHIFT,    // {
    HID_KEYBOARD_SC_BACKSLASH_AND_PIPE|SHIFT,                   // |
    HID_KEYBOARD_SC_CLOSING_BRACKET_AND_CLOSING_BRACE|SHIFT,    // }
    HID_KEYBOARD_SC_NON_US_HASHMARK_AND_TILDE|SHIFT,            // ~
    HID_KEYBOARD_SC_RESERVED                                    // DEL
};
