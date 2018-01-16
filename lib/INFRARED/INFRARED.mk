# Copyright (c) 2018 NicoHood
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
ifeq ($(filter INFRARED, $(DMBS_BUILD_MODULES)),)

# Sanity check user supplied DMBS path
ifndef DMBS_PATH
$(error Makefile DMBS_PATH option cannot be blank)
endif

# Location of the current module
INFRARED_MODULE_PATH := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# Import the CORE module of DMBS
include $(DMBS_PATH)/core.mk

# Library dependencies
TIMER0_PATH        ?= $(INFRARED_MODULE_PATH)/../TIMER0
BOARD_PATH         ?= $(INFRARED_MODULE_PATH)/../BOARD
FASTPIN_PATH       ?= $(INFRARED_MODULE_PATH)/../FASTLED
PCINT_PATH         ?= $(INFRARED_MODULE_PATH)/../PCINT
$(call ERROR_IF_EMPTY, TIMER0_PATH)
$(call ERROR_IF_EMPTY, BOARD_PATH)
$(call ERROR_IF_EMPTY, FASTPIN_PATH)
$(call ERROR_IF_EMPTY, PCINT_PATH)
include $(TIMER0_PATH)/TIMER0.mk
include $(BOARD_PATH)/BOARD.mk
include $(FASTPIN_PATH)/FASTPIN.mk
include $(PCINT_PATH)/PCINT.mk

# This module needs to be included before gcc.mk
ifneq ($(filter GCC, $(DMBS_BUILD_MODULES)),)
$(error Include this module before gcc.mk)
endif

# Help settings
DMBS_BUILD_MODULES         += INFRARED
DMBS_BUILD_TARGETS         +=
DMBS_BUILD_MANDATORY_VARS  += DMBS_PATH
DMBS_BUILD_OPTIONAL_VARS   +=
DMBS_BUILD_PROVIDED_VARS   += INFRARED_SRC
DMBS_BUILD_PROVIDED_MACROS +=

# Sanity check user supplied values
$(foreach MANDATORY_VAR, $(DMBS_BUILD_MANDATORY_VARS), $(call ERROR_IF_UNSET, $(MANDATORY_VAR)))

# INFRARED Library
INFRARED_SRC := $(INFRARED_MODULE_PATH)/src/IRLremote.cpp

# Compiler flags and sources
SRC                += $(INFRARED_SRC)
CC_FLAGS           += -DDMBS_MODULE_INFRARED
CC_FLAGS           += -I$(INFRARED_MODULE_PATH)/include
CC_FLAGS           += -include $(TIMER0_MODULE_PATH)/include/timer0.h

# Phony build targets for this module
.PHONY: $(DMBS_BUILD_TARGETS)

endif
