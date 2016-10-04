# Include Guard
ifeq ($(findstring AES, $(DMBS_BUILD_MODULES)),)

# Sanity check user supplied DMBS path
ifndef DMBS_PATH
$(error Makefile DMBS_PATH option cannot be blank)
endif

# Location of the current module
AES_MODULE_PATH := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# Include core module
include $(DMBS_PATH)/core.mk

# Help settings
DMBS_BUILD_MODULES         += AES
DMBS_BUILD_TARGETS         += aes256-nessie-test aes256_ctr_test
DMBS_BUILD_MANDATORY_VARS  += DMBS_PATH
DMBS_BUILD_OPTIONAL_VARS   +=
DMBS_BUILD_PROVIDED_VARS   += AES_SRC_ECB AES_SRC_CTR AES_SRC_ALL
DMBS_BUILD_PROVIDED_MACROS +=

# AES Library and stream cipher modes
AES_SRC_ECB := $(AES_MODULE_PATH)/aes.c
AES_SRC_CTR := $(AES_MODULE_PATH)/aes256_ctr.c
AES_SRC_ALL := $(sort $(AES_SRC_ECB) $(AES_SRC_CTR))

# Test programs
AES_SRC_CTR_TEST := $(AES_MODULE_PATH)/aes256_ctr_test.c
AES_SRC_NESSIE_TEST := $(AES_MODULE_PATH)/aes256_nessie_test.c

# Compiler flags and sources
C_FLAGS += -I$(AES_MODULE_PATH)
SRC += $(AES_SRC_ALL)

# TODO tests
aes256-nessie-test:
	echo "Nessie test not complete"

aes256_ctr_test:
	echo "CTR test not complete"

# Phony build targets for this module
.PHONY: $(DMBS_BUILD_TARGETS)

endif
