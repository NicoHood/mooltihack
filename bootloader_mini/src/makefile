#
#            DMBS Build System
#     Released into the public domain.
#
#  dean [at] fourwalledcubicle [dot] com
#        www.fourwalledcubicle.com
#

# Run "make help" for target help.

MCU          = atmega32u4
ARCH         = AVR8
F_CPU        = 16000000
OPTIMIZATION = s
TARGET       = mooltipass
SRC          = $(LIBOBJS) $(OBJECTS) BootloaderAPITable.S
CC_FLAGS     = -Werror $(addprefix -I, $(LIBDIRS)) $(addprefix -D, $(DEFINES))
LD_FLAGS     = -Werror -Wl,--section-start=.text=$(BOOT_START_OFFSET) $(BOOT_API_LD_FLAGS)

# Flash size and bootloader section sizes of the target, in KB. These must
# match the target's total FLASH size and the bootloader size set in the
# device's fuses.
FLASH_SIZE_KB        := 32
BOOT_SECTION_SIZE_KB := 4

# Defines
DEFINES += MOOLTIPASS
DEFINES += BOOT_START_ADDR=$(BOOT_START_OFFSET)

# Avrdude settings Arduino as ISP
AVRDUDE_PORT       = /dev/ttyACM0
AVRDUDE_PROGRAMMER = stk500v1
AVRDUDE_FLAGS      = -b 19200 -e -Ulock:w:0x3F:m -Uefuse:w:0xFB:m -Uhfuse:w:0xD9:m -Ulfuse:w:0xFF:m

# Special optimization (most important at the top)
COMPILER_PATH =../AVR-Development-Environment-Script/bin/bin/
CC_FLAGS  += -flto -fuse-linker-plugin
LD_FLAGS  += -flto -fuse-linker-plugin
#CC_FLAGS  += -mcall-prologues
CC_FLAGS  += -fno-split-wide-types
CC_FLAGS  += -fno-tree-scev-cprop

# Bootloader address calculation formulas
# Do not modify these macros, but rather modify the dependent values above.
CALC_ADDRESS_IN_HEX   = $(shell printf "0x%X" $$(( $(1) )) )
BOOT_START_OFFSET     = $(call CALC_ADDRESS_IN_HEX, ($(FLASH_SIZE_KB) - $(BOOT_SECTION_SIZE_KB)) * 1024 )
BOOT_SEC_OFFSET       = $(call CALC_ADDRESS_IN_HEX, ($(FLASH_SIZE_KB) * 1024) - ($(strip $(1))) )

# Bootloader linker section flags for relocating the API table sections to
# known FLASH addresses - these should not normally be user-edited.
BOOT_SECTION_LD_FLAG  = -Wl,--section-start=$(strip $(1))=$(call BOOT_SEC_OFFSET, $(3)) -Wl,--undefined=$(strip $(2))
BOOT_API_LD_FLAGS     = $(call BOOT_SECTION_LD_FLAG, .apitable_trampolines, BootloaderAPI_Trampolines, 96)
BOOT_API_LD_FLAGS    += $(call BOOT_SECTION_LD_FLAG, .apitable_jumptable,   BootloaderAPI_JumpTable,   32)
BOOT_API_LD_FLAGS    += $(call BOOT_SECTION_LD_FLAG, .apitable_signatures,  BootloaderAPI_Signatures,  8)


# Some helpers to get all the .c and .h files.
# $(TARGET) is included inside $(OBJECTS).
LIBDIRS := $(addprefix ./, USB AES)
LIBOBJS := $(foreach lib, $(LIBDIRS), $(wildcard $(lib)/*.c))
OBJECTS := $(wildcard ./*.c)

# Default target
all:

# Include DMBS build script makefiles
DMBS_PATH   ?= ../dmbs/DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/gcc.mk
include $(DMBS_PATH)/cppcheck.mk
include $(DMBS_PATH)/doxygen.mk
include $(DMBS_PATH)/dfu.mk
include $(DMBS_PATH)/hid.mk
include $(DMBS_PATH)/avrdude.mk
include $(DMBS_PATH)/atprogram.mk
