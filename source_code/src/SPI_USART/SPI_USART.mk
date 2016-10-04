# Include Guard
ifeq ($(findstring SPI_USART, $(DMBS_BUILD_MODULES)),)

# Sanity check user supplied DMBS path
ifndef DMBS_PATH
$(error Makefile DMBS_PATH option cannot be blank)
endif

# Location of the current module
SPI_USART_MODULE_PATH := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# Include core module
include $(DMBS_PATH)/core.mk

# Help settings
DMBS_BUILD_MODULES         += SPI_USART
DMBS_BUILD_TARGETS         +=
DMBS_BUILD_MANDATORY_VARS  += DMBS_PATH
DMBS_BUILD_OPTIONAL_VARS   +=
DMBS_BUILD_PROVIDED_VARS   += SPI_USART_SRC
DMBS_BUILD_PROVIDED_MACROS +=

# SPI_USART Library and stream cipher modes
SPI_USART_SRC := $(SPI_USART_MODULE_PATH)/spi_usart.c

# Compiler flags and sources
C_FLAGS += -I$(SPI_USART_MODULE_PATH)
SRC += $(SPI_USART_SRC)

# Phony build targets for this module
.PHONY: $(DMBS_BUILD_TARGETS)

endif
