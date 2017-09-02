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
ifeq ($(findstring PCINT, $(DMBS_BUILD_MODULES)),)

# Sanity check user supplied DMBS path
ifndef DMBS_PATH
$(error Makefile DMBS_PATH option cannot be blank)
endif

# Location of the current module
PCINT_MODULE_PATH := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# Import the CORE module of DMBS
include $(DMBS_PATH)/core.mk

# This module needs to be included before gcc.mk
ifneq ($(findstring GCC, $(DMBS_BUILD_MODULES)),)
$(error Include this module before gcc.mk)
endif

# Default values of optionally user-supplied variables
PCINT_API    ?= Y
PCINT_ENABLE ?=

# Help settings
DMBS_BUILD_MODULES         += PCINT
DMBS_BUILD_TARGETS         +=
DMBS_BUILD_MANDATORY_VARS  += DMBS_PATH
DMBS_BUILD_OPTIONAL_VARS   += PCINT_API PCINT_ENABLE
DMBS_BUILD_PROVIDED_VARS   += PCINT_SRC
DMBS_BUILD_PROVIDED_MACROS +=

# Sanity check user supplied values
$(foreach MANDATORY_VAR, $(DMBS_BUILD_MANDATORY_VARS), $(call ERROR_IF_UNSET, $(MANDATORY_VAR)))
$(call ERROR_IF_NONBOOL, PCINT_API)

# PCINT Library
PCINT_SRC := $(PCINT_MODULE_PATH)/src/PinChangeInterrupt.cpp
PCINT_SRC += $(PCINT_MODULE_PATH)/src/PinChangeInterrupt0.cpp
PCINT_SRC += $(PCINT_MODULE_PATH)/src/PinChangeInterrupt1.cpp
PCINT_SRC += $(PCINT_MODULE_PATH)/src/PinChangeInterrupt2.cpp
PCINT_SRC += $(PCINT_MODULE_PATH)/src/PinChangeInterrupt3.cpp

# Compiler flags and sources
SRC                += $(PCINT_SRC)
CC_FLAGS           += -DDMBS_MODULE_PCINT
CC_FLAGS           += -I$(PCINT_MODULE_PATH)/include
ifeq ($(PCINT_API), Y)
   CC_FLAGS        += -DPCINT_API
endif

# Optional optimization: Only enable the desired PCINTs to safe a lot of flash
ifneq ($(PCINT_ENABLE),)
    # Generate defines to enable PCINT and ports
    SORTED_PCINT_ENABLE = $(sort $(PCINT_ENABLE))
    PCINT_ENABLE_PORT0 = $(sort $(foreach pcint,0 1 2 3 4 5 6 7,$(if $(findstring $(pcint),$(SORTED_PCINT_ENABLE)),-DPCINT_ENABLE_PORT0)))
    PCINT_ENABLE_PORT1 = $(sort $(foreach pcint,8 9 10 11 12 13 14 15,$(if $(findstring $(pcint),$(SORTED_PCINT_ENABLE)),-DPCINT_ENABLE_PORT1)))
    PCINT_ENABLE_PORT2 = $(sort $(foreach pcint,16 17 18 19 20 21 22 23,$(if $(findstring $(pcint),$(SORTED_PCINT_ENABLE)),-DPCINT_ENABLE_PORT2)))
    PCINT_ENABLE_PORT3 = $(sort $(foreach pcint,24 25 26 27 28 29 30 31,$(if $(findstring $(pcint),$(SORTED_PCINT_ENABLE)),-DPCINT_ENABLE_PORT3)))
    PCINT_ENABLE_PCINT = $(foreach pcint, $(SORTED_PCINT_ENABLE), -DPCINT_ENABLE_PCINT$(pcint))

    # Add definitions to CC_FLAGS
    CC_FLAGS += -DPCINT_ENABLE_MANUAL
    CC_FLAGS += $(PCINT_ENABLE_PORT0)
    CC_FLAGS += $(PCINT_ENABLE_PORT1)
    CC_FLAGS += $(PCINT_ENABLE_PORT2)
    CC_FLAGS += $(PCINT_ENABLE_PORT3)
    CC_FLAGS += $(PCINT_ENABLE_PCINT)
endif

# Phony build targets for this module
.PHONY: $(DMBS_BUILD_TARGETS)

endif
