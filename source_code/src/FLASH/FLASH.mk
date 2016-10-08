# Include Guard
ifeq ($(findstring FLASH, $(DMBS_BUILD_MODULES)),)

# Sanity check user supplied DMBS path
ifndef DMBS_PATH
$(error Makefile DMBS_PATH option cannot be blank)
endif

# Location of the current module
FLASH_MODULE_PATH := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# Include core module
include $(DMBS_PATH)/core.mk

# Include module dependencies
include SPI_USART/SPI_USART.mk

# Help settings
DMBS_BUILD_MODULES         += FLASH
DMBS_BUILD_TARGETS         +=
DMBS_BUILD_MANDATORY_VARS  += DMBS_PATH
DMBS_BUILD_OPTIONAL_VARS   +=
DMBS_BUILD_PROVIDED_VARS   += FLASH_SRC FLASH_TEST_SRC
DMBS_BUILD_PROVIDED_MACROS +=

# FLASH Library and stream cipher modes
FLASH_SRC := $(FLASH_MODULE_PATH)/flash_mem.c \
             $(FLASH_MODULE_PATH)/flash_mem_legacy.c
FLASH_TEST_SRC := $(FLASH_MODULE_PATH)/flash_test.c

# Compiler flags and sources
C_FLAGS += -I$(FLASH_MODULE_PATH)
SRC += $(FLASH_SRC)

# Phony build targets for this module
.PHONY: $(DMBS_BUILD_TARGETS)

endif
