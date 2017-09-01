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
ifeq ($(findstring USART, $(DMBS_BUILD_MODULES)),)

# Sanity check user supplied DMBS path
ifndef DMBS_PATH
$(error Makefile DMBS_PATH option cannot be blank)
endif

# Location of the current module
USART_MODULE_PATH := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# Import the CORE module of DMBS
include $(DMBS_PATH)/core.mk

# This module needs to be included before gcc.mk
ifneq ($(findstring GCC, $(DMBS_BUILD_MODULES)),)
$(error Include this module before gcc.mk)
endif

# Default values of optionally user-supplied variables
USART_DATA_BITS     ?=
USART_STOP_BITS     ?=
USART_PARITY        ?=
USART_BUFFER_RX     ?=
USART_BUFFER_TX     ?=
USART_THREAD_SAFE   ?=

# Help settings
DMBS_BUILD_MODULES         += USART
DMBS_BUILD_TARGETS         +=
DMBS_BUILD_MANDATORY_VARS  += DMBS_PATH USART_BAUDRATE
DMBS_BUILD_OPTIONAL_VARS   += USART_DATA_BITS USART_STOP_BITS USART_PARITY
DMBS_BUILD_OPTIONAL_VARS   += USART_BUFFER_RX USART_BUFFER_TX USART_THREAD_SAFE
DMBS_BUILD_PROVIDED_VARS   += USART_SRC
DMBS_BUILD_PROVIDED_MACROS +=

# Sanity check user supplied values
$(foreach MANDATORY_VAR, $(DMBS_BUILD_MANDATORY_VARS), $(call ERROR_IF_UNSET, $(MANDATORY_VAR)))

# USART Library
USART_SRC := $(USART_MODULE_PATH)/src/usart_init.c
USART_SRC += $(USART_MODULE_PATH)/src/usart_tx.c
USART_SRC += $(USART_MODULE_PATH)/src/usart_rx.c

# Compiler flags and sources
SRC                += $(USART_SRC)
CC_FLAGS           += -DDMBS_MODULE_USART
CC_FLAGS           += -I$(USART_MODULE_PATH)/include
CC_FLAGS           += -DUSART_BAUDRATE=$(USART_BAUDRATE)

# Optional settings
ifneq ($(USART_DATA_BITS), )
CC_FLAGS           += -DUSART_DATA_BITS=$(USART_DATA_BITS)
endif
ifneq ($(USART_STOP_BITS), )
CC_FLAGS           += -DUSART_STOP_BITS=$(USART_STOP_BITS)
endif
ifneq ($(USART_PARITY), )
CC_FLAGS           += -DUSART_PARITY=$(USART_PARITY)
endif
ifneq ($(USART_BUFFER_RX), )
CC_FLAGS           += -DUSART_BUFFER_RX=$(USART_BUFFER_RX)
endif
ifneq ($(USART_BUFFER_TX), )
CC_FLAGS           += -DUSART_BUFFER_TX=$(USART_BUFFER_TX)
endif
ifneq ($(USART_THREAD_SAFE), )
CC_FLAGS           += -DUSART_THREAD_SAFE=$(USART_THREAD_SAFE)
endif

# Phony build targets for this module
.PHONY: $(DMBS_BUILD_TARGETS)

endif
