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
ifeq ($(filter FASTLED, $(DMBS_BUILD_MODULES)),)

# Sanity check user supplied DMBS path
ifndef DMBS_PATH
$(error Makefile DMBS_PATH option cannot be blank)
endif

# Location of the current module
FASTLED_MODULE_PATH := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# Import the CORE module of DMBS
include $(DMBS_PATH)/core.mk

# Library dependencies
TIMER0_PATH        ?= $(FASTLED_MODULE_PATH)/../TIMER0/
$(call ERROR_IF_EMPTY, TIMER0_PATH)
include $(TIMER0_PATH)/TIMER0.mk

# This module needs to be included before gcc.mk
ifneq ($(filter GCC, $(DMBS_BUILD_MODULES)),)
$(error Include this module before gcc.mk)
endif

# Help settings
DMBS_BUILD_MODULES         += FASTLED
DMBS_BUILD_TARGETS         +=
DMBS_BUILD_MANDATORY_VARS  += DMBS_PATH LIB_PATH
DMBS_BUILD_OPTIONAL_VARS   += TIMER0_PATH
DMBS_BUILD_PROVIDED_VARS   += FASTLED_SRC
DMBS_BUILD_PROVIDED_MACROS +=

# Sanity check user supplied values
$(foreach MANDATORY_VAR, $(DMBS_BUILD_MANDATORY_VARS), $(call ERROR_IF_UNSET, $(MANDATORY_VAR)))

# FASTLED Library
FASTLED_SRC := $(FASTLED_MODULE_PATH)/src/FastLED/FastLED.cpp
FASTLED_SRC += $(FASTLED_MODULE_PATH)/src/FastLED/power_mgt.cpp
FASTLED_SRC += $(FASTLED_MODULE_PATH)/src/FastLED/noise.cpp
FASTLED_SRC += $(FASTLED_MODULE_PATH)/src/FastLED/wiring.cpp
FASTLED_SRC += $(FASTLED_MODULE_PATH)/src/FastLED/lib8tion.cpp
FASTLED_SRC += $(FASTLED_MODULE_PATH)/src/FastLED/hsv2rgb.cpp
FASTLED_SRC += $(FASTLED_MODULE_PATH)/src/FastLED/colorutils.cpp
FASTLED_SRC += $(FASTLED_MODULE_PATH)/src/FastLED/bitswap.cpp

# Compiler flags and sources
SRC                += $(FASTLED_SRC)
CC_FLAGS           += -DDMBS_MODULE_FASTLED
CC_FLAGS           += -I$(FASTLED_MODULE_PATH)/src/FastLED/
CC_FLAGS           += -I$(FASTLED_MODULE_PATH)/include
CC_FLAGS           += -include $(TIMER0_MODULE_PATH)/include/timer0.h
CC_FLAGS           += -DFASTLED_NO_PINMAP -DFASTLED_NEED_YIELD -Dtimer0_millis=timer0_millis_count
# TODO remove this workaround when PR is merged:
# https://github.com/abcminiuser/dmbs/pull/27
CC_FLAGS           += -DNEED_CXX_BITS
#FastLED.cpp_FLAGS   = -DNEED_CXX_BITS

# Use c++11 standard
CPP_STANDARD        = gnu++11

# Phony build targets for this module
.PHONY: $(DMBS_BUILD_TARGETS)

endif
