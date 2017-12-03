# Copyright (c) 2017 NicoHood
# See the readme for credit to other people.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# Include Guard
ifeq ($(filter BOARD, $(DMBS_BUILD_MODULES)),)

# Sanity check user supplied DMBS path
ifndef DMBS_PATH
$(error Makefile DMBS_PATH option cannot be blank)
endif

# Location of the current module
BOARD_MODULE_PATH := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# Import the CORE module of DMBS
include $(DMBS_PATH)/core.mk

# This module needs to be included before gcc.mk
ifneq ($(filter GCC, $(DMBS_BUILD_MODULES)),)
$(error Include this module before gcc.mk)
endif

# Help settings
DMBS_BUILD_MODULES         += BOARD
DMBS_BUILD_TARGETS         += list_boards
DMBS_BUILD_MANDATORY_VARS  += DMBS_PATH BOARD
DMBS_BUILD_OPTIONAL_VARS   +=
DMBS_BUILD_PROVIDED_VARS   += AVRDUDE_PORT AVRDUDE_PROGRAMMER AVRDUDE_BAUD
DMBS_BUILD_PROVIDED_VARS   += AVRDUDE_LFUSE AVRDUDE_HFUSE AVRDUDE_EFUSE AVRDUDE_LOCK
DMBS_BUILD_PROVIDED_MACROS +=

# Sanity check user supplied values
$(foreach MANDATORY_VAR, $(DMBS_BUILD_MANDATORY_VARS), $(call ERROR_IF_UNSET, $(MANDATORY_VAR)))

# Available boards list
BOARD_LIST := CUSTOM_BOARD ARDUINO_LEONARDO ARDUINO_MICRO ARDUINO_UNO ARDUINO_NANO ARDUINO_MINI
SORTED_BOARD_LIST    = $(sort $(BOARD_LIST))
PRINTABLE_BOARD_LIST = $(call CONVERT_TO_PRINTABLE, $(SORTED_BOARD_LIST))

# Compiler flags and sources
CC_FLAGS           += -DDMBS_MODULE_BOARD
CC_FLAGS           += -I $(BOARD_MODULE_PATH)/include
CC_FLAGS           += -D$(BOARD) -DBOARD=\"$(BOARD)\"

# Board variants
ifeq ($(BOARD), ARDUINO_LEONARDO)

# Atmega settings
MCU                ?= atmega32u4
ARCH               ?= AVR8
F_CPU              ?= 16000000

# Avrdude settings Arduino USB Bootloader
AVRDUDE_PORT       ?= /dev/ttyACM0
AVRDUDE_PROGRAMMER ?= avr109
AVRDUDE_BAUD       ?= 57600

# Fuses
AVRDUDE_LFUSE      ?= 0xFF
AVRDUDE_HFUSE      ?= 0xD8
AVRDUDE_EFUSE      ?= 0xCB
AVRDUDE_LOCK       ?= 0x2F

# Additional Arduino specific board definition
CC_FLAGS           += -DAVR_LEONARDO

else ifeq ($(BOARD), ARDUINO_MICRO)

# Atmega settings
MCU                ?= atmega32u4
ARCH               ?= AVR8
F_CPU              ?= 16000000

# Avrdude settings Arduino USB Bootloader
AVRDUDE_PORT       ?= /dev/ttyACM0
AVRDUDE_PROGRAMMER ?= avr109
AVRDUDE_BAUD       ?= 57600

# Fuses
AVRDUDE_LFUSE      ?= 0xFF
AVRDUDE_HFUSE      ?= 0xD8
AVRDUDE_EFUSE      ?= 0xCB
AVRDUDE_LOCK       ?= 0x2F

# Additional Arduino specific board definition
CC_FLAGS           += -DAVR_MICRO

else ifeq ($(BOARD), ARDUINO_UNO)

# Atmega settings
MCU                ?= atmega328p
ARCH               ?= AVR8
F_CPU              ?= 16000000

# Avrdude settings Arduino USB Bootloader
AVRDUDE_PORT       ?= /dev/ttyACM0
AVRDUDE_PROGRAMMER ?= arduino
AVRDUDE_BAUD       ?= 115200

# Fuses
AVRDUDE_LFUSE      ?= 0xFF
AVRDUDE_HFUSE      ?= 0xDE
AVRDUDE_EFUSE      ?= 0xFD
AVRDUDE_LOCK       ?= 0x0F

# Additional Arduino specific board definition
CC_FLAGS           += -DAVR_UNO

else ifeq ($(BOARD), ARDUINO_NANO)

# Atmega settings
MCU                ?= atmega328p
ARCH               ?= AVR8
F_CPU              ?= 16000000

# Avrdude settings Arduino USB Bootloader
AVRDUDE_PORT       ?= /dev/ttyUSB0
AVRDUDE_PROGRAMMER ?= arduino
AVRDUDE_BAUD       ?= 57600

# Fuses
AVRDUDE_LFUSE      ?= 0xFF
AVRDUDE_HFUSE      ?= 0xDA
AVRDUDE_EFUSE      ?= 0xFD
AVRDUDE_LOCK       ?= 0x0F

# Additional Arduino specific board definition
CC_FLAGS           += -DAVR_NANO

else ifeq ($(BOARD), ARDUINO_MINI)

# Atmega settings
MCU                ?= atmega328p
ARCH               ?= AVR8
F_CPU              ?= 16000000

# Avrdude settings Arduino USB Bootloader
AVRDUDE_PORT       ?= /dev/ttyUSB0
AVRDUDE_PROGRAMMER ?= arduino
AVRDUDE_BAUD       ?= 115200

# Fuses
AVRDUDE_LFUSE      ?= 0xFF
AVRDUDE_HFUSE      ?= 0xD8
AVRDUDE_EFUSE      ?= 0xFD
AVRDUDE_LOCK       ?= 0x0F

# Additional Arduino specific board definition
CC_FLAGS           += -DAVR_MINI

else ifeq ($(BOARD), CUSTOM_BOARD)
# User needs to create a file CustomBoard.h and add it to the include path with CC_FLAGS += -I path
# This way you can faster switch between options of a regular and a custom board.
else
$(error Selected board $(BOARD) not available. Run "make list_boards" for a board list)
endif

# Lists board provided by the BOARD module, in alphabetical order
list_boards:
	@echo Boards Provided by the BOARD Module:
	@printf " %b" "$(PRINTABLE_BOARD_LIST:%=   - %\n)"

# Phony build targets for this module
.PHONY: $(DMBS_BUILD_TARGETS)

endif
