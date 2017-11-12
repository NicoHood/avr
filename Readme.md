# DMBS AVR Library Collection

This repository is a collection of libraries for AVR microcontrollers.
[DMBS](https://github.com/abcminiuser/dmbs) is used as primary build system for all libraries.
This repository contains new, standalone libraries as well as makefiles for popular external projects like [FastLED](https://github.com/FastLED/FastLED).

The purpose of this library is to use a clean and modular build system with libraries optimized in size and speed while still being able to use popular Arduino libraries like FastLED, but without any Arduino core.

### Benefits
- Clean, modular, extensible, professional makefile build system used (DMBS)
- Standardized folder structures and naming across all libraries
- Smaller code due to build time optimizations and better makefile handling
- Usage of stdio with serial port: putc, puts, printf, etc.
- Small and fast timer0 code from teensy core: micros(), millis(), delay(), delayMicroseconds()
- FastLED library fully compatible
- FastPin from FastLED usable without Led code (for faster pin access)
- Extremely optimized PinChangeInterrupt library with compile-time optimization

## Modules Overview

The following modules are currently included:

 - [BOARD](lib/BOARD/Readme.md) - Boards specific definitions like led pins
 - [TIMER0](lib/TIMER0/Readme.md) - Basic time functions
 - [USART](lib/USART/Readme.md) - Serial i/o operations with stdio functionality (printf etc.)
 - [FASTLED](lib/FASTLED/Readme.md) - [FastLED](https://github.com/FastLED/FastLED) library
 - [FASTPIN](lib/FASTLED/Readme.md) - FastPin class of [FastLED](https://github.com/FastLED/FastLED) library
 - [PCINT](lib/PCINT/Readme.md) - [PinChangeInterrupt](https://github.com/NicoHood/PinChangeInterrupt) library

## Usage

In order to build all libraries you need to initialize the submodules for DMBS and other external libraries. The following command can also be used to update all library dependencies.

```
git submodule update --init --recursive
```

You then can include the libraries in your DMBS makefile. All sources are already properly added to `SRC`. All required compiler switches also automatically get applied by including the library makefile.

```makefile
# Include DMBS build script makefiles
DMBS_PATH    = ../DMBS
LIB_PATH     = ../lib

include $(LIB_PATH)/TIMER0/TIMER0.mk
```

It is recommended to read each libraries makefile first. It it show you how to setup and use the library and give you additional hints and examples.

## Writing Your Own Modules
You can use my libraries as an example or use the [guide](https://github.com/abcminiuser/dmbs/blob/master/DMBS/WritingYourOwnModules.md) and [template](https://github.com/abcminiuser/dmbs/pull/22) from DMBS directly.
