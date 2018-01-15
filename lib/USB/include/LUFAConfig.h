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

/** \file
 *  \brief LUFA Library Configuration Header File
 *
 *  This header file is used to configure LUFA's compile time options,
 *  as an alternative to the compile time constants supplied through
 *  a makefile.
 *
 *  For information on what each token does, refer to the LUFA
 *  manual section "Summary of Compile Tokens".
 */

#ifndef _LUFA_CONFIG_H_
#define _LUFA_CONFIG_H_

	#if (ARCH == ARCH_AVR8)

		/* Non-USB Related Configuration Tokens: */
//		#define DISABLE_TERMINAL_CODES

		/* General USB Driver Related Tokens: */
		#define ORDERED_EP_CONFIG
		#define USE_STATIC_OPTIONS               (USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)
		#define USB_DEVICE_ONLY
//		#define USB_HOST_ONLY
//		#define USB_STREAM_TIMEOUT_MS            {Insert Value Here}
//		#define NO_LIMITED_CONTROLLER_CONNECT
//		#define NO_SOF_EVENTS

		/* USB Device Mode Driver Related Tokens: */
//		#define USE_RAM_DESCRIPTORS
		#define USE_FLASH_DESCRIPTORS
//		#define USE_EEPROM_DESCRIPTORS
//		#define NO_INTERNAL_SERIAL
		#define FIXED_CONTROL_ENDPOINT_SIZE      8
		#define DEVICE_STATE_AS_GPIOR            2
		#define FIXED_NUM_CONFIGURATIONS         1
//		#define CONTROL_ONLY_DEVICE
		#define INTERRUPT_CONTROL_ENDPOINT
//		#define NO_DEVICE_REMOTE_WAKEUP
		#define NO_DEVICE_SELF_POWER

	#else

		#error Unsupported architecture for this LUFA configuration file.

	#endif
#endif
