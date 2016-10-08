/* CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at src/license_cddl-1.0.txt
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at src/license_cddl-1.0.txt
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*!  \file     flash_mem.h
*    \brief    Mooltipass Flash IC Library Header
*/

// Include Guard
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Software version
#define FLASH_VERSION 110

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Configuration included from AppConfig.h, examples below
#include <AppConfig.h>

// Flash chip size
//#define FLASH_CHIP_4M

// Slave Select Flash (MP Mini)
//#define FLASH_BIT_SS    PB4
//#define FLASH_PORT_SS   PORTB
//#define FLASH_DDR_SS    DDRB

// LegacyFunctions
// void sectorZeroErase(uint8_t sectorNumber); TODO correct dev function
// void sectorErase(uint8_t sectorNumber);
// void blockErase(uint16_t blockNumber);
// void pageErase(uint16_t pageNumber);
// void chipErase(void);
// void formatFlash(void);
// void initFlashIOs(void);
// RET_TYPE checkFlashID(void);
void flashWriteBufferToPage(uint16_t page);
void loadPageToInternalBuffer(uint16_t page_number);
void flashRawRead(uint8_t* datap, uint16_t addr, size_t size);
void flashWriteBuffer(uint8_t* datap, uint16_t offset, size_t size);
void writeDataToFlash(uint16_t pageNumber, uint16_t offset, uint16_t dataSize, void *data);
void readDataFromFlash(uint16_t pageNumber, uint16_t offset, uint16_t dataSize, void *data);

// Errorcode enum, zero indicates no error
typedef enum
{
    FLASH_RET_OK = 0x00,
    FLASH_RET_ERR_INPUT_PARAM = 0x01,
    FLASH_RET_ERR_ERASE_PROGRAM = 0x02,
    FLASH_RET_ERR_ID = 0x03,
} flash_ret_t;

// Flash setup functions
void flash_init(void);
flash_ret_t flash_check_device_id(void);

// Flash page read/write operations
flash_ret_t flash_read_page(uint16_t page, uint16_t offset, uint8_t* data, size_t size);
flash_ret_t flash_write_page(uint16_t page, uint16_t offset, uint8_t* data, size_t size);

// Flash page read/write raw operations
flash_ret_t flash_read_raw(uint16_t addr, uint8_t* data, size_t size);
flash_ret_t flash_read_raw_far(uint32_t addr, uint8_t* data, size_t size);
flash_ret_t flash_write_raw(uint16_t addr, uint8_t* data, size_t size);
flash_ret_t flash_write_raw_far(uint32_t addr, uint8_t* data, size_t size);

// Flash low level page modification operations
flash_ret_t flash_read_into_buffer(uint16_t page);
flash_ret_t flash_write_into_buffer(uint16_t offset, uint8_t* data, size_t size);
flash_ret_t flash_write_buffer_to_page(uint16_t page);
static inline flash_ret_t flash_rewrite_page(uint16_t page) __attribute__((always_inline));

// Flash erase functions
flash_ret_t flash_erase_page(uint16_t page);
flash_ret_t flash_erase_pages(uint16_t page, uint16_t count);
flash_ret_t flash_erase_chip(void);

/**
 * Rewrite the contents of a flash page. Refer to the datasheet when a page needs to be rewirtten.
 * @param   page the page to be rewritten
 * @return  error code, zero means no error
 * @note    this function is implemented always inline and just wraps the normal write function with no data.
 */
flash_ret_t flash_rewrite_page(uint16_t page)
{
    return flash_write_page(page, 0, NULL, 0);
}

// Used to identify a 1M Flash Chip (AT45DB011D)
#if defined(FLASH_CHIP_1M)
    #define FLASH_CHIP 1
    #define FLASH_CHIP_STR  "\x01"
    #define FLASH_MANUF_ID 0x1F        // Used for Chip Identity (see datasheet)
    #define FLASH_FAM_DEN_VAL 0x22     // Used for Chip Identity (see datasheet)
    #define FLASH_PAGE_COUNT 512UL     // Number of pages in the chip
    #define FLASH_BYTES_PER_PAGE 264UL // Bytes per page of the chip
    #define FLASH_SIZE (FLASH_PAGE_COUNT * FLASH_BYTES_PER_PAGE)

// Used to identify a 2M Flash Chip (AT45DB021E)
#elif defined(FLASH_CHIP_2M)
    #define FLASH_CHIP 2
    #define FLASH_CHIP_STR  "\x02"
    #define FLASH_MANUF_ID 0x1F        // Used for Chip Identity (see datasheet)
    #define FLASH_FAM_DEN_VAL 0x23     // Used for Chip Identity (see datasheet)
    #define FLASH_PAGE_COUNT 1024UL    // Number of pages in the chip
    #define FLASH_BYTES_PER_PAGE 264UL // Bytes per page of the chip
    #define FLASH_SIZE (FLASH_PAGE_COUNT * FLASH_BYTES_PER_PAGE)

// Used to identify a 4M Flash Chip (AT45DB041E)
#elif defined(FLASH_CHIP_4M)
    #define FLASH_CHIP 4
    #define FLASH_CHIP_STR  "\x04"
    #define FLASH_MANUF_ID 0x1F        // Used for Chip Identity (see datasheet)
    #define FLASH_FAM_DEN_VAL 0x24     // Used for Chip Identity (see datasheet)
    #define FLASH_PAGE_COUNT 2048UL    // Number of pages in the chip
    #define FLASH_BYTES_PER_PAGE 264UL // Bytes per page of the chip
    #define FLASH_SIZE (FLASH_PAGE_COUNT * FLASH_BYTES_PER_PAGE)

// Used to identify a 8M Flash Chip (AT45DB081E)
#elif defined(FLASH_CHIP_8M)
    #define FLASH_CHIP 8
    #define FLASH_CHIP_STR  "\x08"
    #define FLASH_MANUF_ID 0x1F        // Used for Chip Identity (see datasheet)
    #define FLASH_FAM_DEN_VAL 0x25     // Used for Chip Identity (see datasheet)
    #define FLASH_PAGE_COUNT 4096UL    // Number of pages in the chip
    #define FLASH_BYTES_PER_PAGE 264UL // Bytes per page of the chip
    #define FLASH_SIZE (FLASH_PAGE_COUNT * FLASH_BYTES_PER_PAGE)

// Used to identify a 16M Flash Chip (AT45DB161E)
#elif defined(FLASH_CHIP_16M)
    #define FLASH_CHIP 16
    #define FLASH_CHIP_STR  "\x10"
    #define FLASH_MANUF_ID 0x1F        // Used for Chip Identity (see datasheet)
    #define FLASH_FAM_DEN_VAL 0x26     // Used for Chip Identity (see datasheet)
    #define FLASH_PAGE_COUNT 4096UL    // Number of pages in the chip
    #define FLASH_BYTES_PER_PAGE 528UL // Bytes per page of the chip
    #define FLASH_SIZE (FLASH_PAGE_COUNT * FLASH_BYTES_PER_PAGE)

// Used to identify a 32M Flash Chip (AT45DB321E)
#elif defined(FLASH_CHIP_32M)
    #define FLASH_CHIP 32
    #define FLASH_CHIP_STR  "\x20"
    #define FLASH_MANUF_ID 0x1F        // Used for Chip Identity (see datasheet)
    #define FLASH_FAM_DEN_VAL 0x27     // Used for Chip Identity (see datasheet)
    #define FLASH_PAGE_COUNT 8192UL    // Number of pages in the chip
    #define FLASH_BYTES_PER_PAGE 528UL // Bytes per page of the chip
    #define FLASH_SIZE (FLASH_PAGE_COUNT * FLASH_BYTES_PER_PAGE)

#else
    #error "No flash chip size defined"
#endif

// Check SS definition existance
#ifndef FLASH_BIT_SS
#error "FLASH_BIT_SS not defined"
#endif
#ifndef FLASH_PORT_SS
#error "FLASH_PORT_SS not defined"
#endif
#ifndef FLASH_DDR_SS
#error "FLASH_DDR_SS not defined"
#endif

#ifdef __cplusplus
}
#endif
