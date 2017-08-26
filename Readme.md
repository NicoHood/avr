# DMBS AVR Library Collection

This repository is a collection of libraries for AVR microcontrollers.
[DMBS](https://github.com/abcminiuser/dmbs) is used as primary build system for all libraries.
This repository contains new, standalone libraries as well as makefiles for popular external projects like [FastLED](https://github.com/FastLED/FastLED).

The purpose of this library is to use a clean and modular build system with libraries optimized in size and speed while still being able to use popular Arduino libraries like FastLED, but without any Arduino core.

## Modules Overview

The following modules are currently included:

 - [BOARD](lib/BOARD/Readme.md) - Boards specific definitions like led pins
 - [TIMER0](lib/TIMER0/Readme.md) - Basic time functions
 - [FASTLED](lib/FASTLED/Readme.md) - [FastLED](https://github.com/FastLED/FastLED) library
 - [FASTPIN](lib/FASTLED/Readme.md) - FastPin class of [FastLED](https://github.com/FastLED/FastLED) library

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

## Known issues

### DMBS
It is recommended to use my [patched version of DMBS](https://github.com/NicoHood/dmbs/tree/compiler_option) to get all functionalities. Those patches will get applied upstream hopefully soon.

### FastLED
In order to build FastLED [a few patches](https://github.com/NicoHood/FastLED/tree/makefile) need to get applied to the current master. Fixes upstream are tracked within [this issue](https://github.com/FastLED/FastLED/issues/485).
