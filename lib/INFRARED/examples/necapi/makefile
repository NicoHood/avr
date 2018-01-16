#
#            DMBS Build System
#     Released into the public domain.
#
#  dean [at] fourwalledcubicle [dot] com
#        www.fourwalledcubicle.com
#

# Run "make help" for target help.
MCU          = atmega32u4
BOARD		 = ARDUINO_LEONARDO
ARCH         = AVR8
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = necapi
SRC          = $(TARGET).cpp
CC_FLAGS     = -Werror
LD_FLAGS     = -Werror

# Only enable a selection of PCINTs to safe flash
#PCINT_ENABLE += 6

# USART speed
USART_BAUDRATE = 2000000

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
include $(LIB_PATH)/INFRARED/INFRARED.mk
include $(LIB_PATH)/USB_CDC_SERIAL/USB_CDC_SERIAL.mk
include $(LIB_PATH)/USB_KEYBOARD/USB_KEYBOARD.mk

# DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/gcc.mk
include $(DMBS_PATH)/cppcheck.mk
include $(DMBS_PATH)/doxygen.mk
include $(DMBS_PATH)/dfu.mk
include $(DMBS_PATH)/hid.mk
include $(DMBS_PATH)/avrdude.mk
include $(DMBS_PATH)/atprogram.mk
