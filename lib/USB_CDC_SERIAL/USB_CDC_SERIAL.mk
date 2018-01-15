# Copyright (c) 2017-2018 NicoHood
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
ifeq ($(filter USB_CDC_SERIAL, $(DMBS_BUILD_MODULES)),)

# Sanity check user supplied DMBS path
ifndef DMBS_PATH
$(error Makefile DMBS_PATH option cannot be blank)
endif

# Location of the current module
USB_CDC_SERIAL_MODULE_PATH := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# Import the CORE module of DMBS
include $(DMBS_PATH)/core.mk

# Library dependencies
USB_MODULE_PATH        ?= $(USB_CDC_SERIAL_MODULE_PATH)/../USB/
$(call ERROR_IF_EMPTY, USB_MODULE_PATH)
include $(USB_MODULE_PATH)/USB.mk
BOARD_MODULE_PATH        ?= $(USB_CDC_SERIAL_MODULE_PATH)/../BOARD/
$(call ERROR_IF_EMPTY, BOARD_MODULE_PATH)
include $(BOARD_MODULE_PATH)/BOARD.mk

# This module needs to be included before gcc.mk
ifneq ($(filter GCC, $(DMBS_BUILD_MODULES)),)
$(error Include this module before gcc.mk)
endif

# Help settings
DMBS_BUILD_MODULES         += USB_CDC_SERIAL
DMBS_BUILD_TARGETS         +=
DMBS_BUILD_MANDATORY_VARS  += DMBS_PATH
DMBS_BUILD_OPTIONAL_VARS   +=
DMBS_BUILD_PROVIDED_VARS   += USB_CDC_SERIAL_SRC
DMBS_BUILD_PROVIDED_MACROS +=

# Sanity check user supplied values
$(foreach MANDATORY_VAR, $(DMBS_BUILD_MANDATORY_VARS), $(call ERROR_IF_UNSET, $(MANDATORY_VAR)))

# USB CDC Serial Library
USB_CDC_SERIAL_SRC = $(USB_CDC_SERIAL_MODULE_PATH)/src/usb_cdc_serial.c

# Compiler flags and sources
SRC                += $(USB_CDC_SERIAL_SRC)
CC_FLAGS           += -DDMBS_MODULE_USB_CDC_SERIAL
CC_FLAGS           += -I$(USB_CDC_SERIAL_MODULE_PATH)/include


# Phony build targets for this module
.PHONY: $(DMBS_BUILD_TARGETS)

endif
