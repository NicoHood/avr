#
#            DMBS Build System
#     Released into the public domain.
#
#  dean [at] fourwalledcubicle [dot] com
#        www.fourwalledcubicle.com
#

# Run "make help" for target help.
MCU          = atmega328p
BOARD		 = ARDUINO_UNO
ARCH         = AVR8
F_CPU        = 16000000
OPTIMIZATION = s
TARGET       = blink
SRC          = $(TARGET).cpp
CC_FLAGS     = -Werror
LD_FLAGS     = -Werror

# Only enable a selection of PCINTs to safe flash
#PCINT_ENABLE += 23

# Include DMBS build script makefiles
ROOT_PATH 	?= ../../../../
DMBS_PATH   ?= $(ROOT_PATH)/DMBS
LIB_PATH    ?= $(ROOT_PATH)/lib

# Link time optimization
LTO = Y

# Default target
all:

# Include library build script makefile and sources
include $(LIB_PATH)/BOARD/BOARD.mk
include $(LIB_PATH)/PCINT/PCINT.mk
include $(LIB_PATH)/FASTLED/FASTPIN.mk

# DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/gcc.mk
include $(DMBS_PATH)/cppcheck.mk
include $(DMBS_PATH)/doxygen.mk
include $(DMBS_PATH)/dfu.mk
include $(DMBS_PATH)/hid.mk
include $(DMBS_PATH)/avrdude.mk
include $(DMBS_PATH)/atprogram.mk
