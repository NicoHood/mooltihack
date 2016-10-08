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

/*!  \file     flash_mem_private.h
*    \brief    Mooltipass Flash IC Library Header (private)
*/

// Include Guard
#pragma once

#include "flash_mem.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <avr/io.h>
#include <spi_usart.h>
#include <AppConfig.h>

// Opcodes sent to the flash are 8bit and thats why packed
// Not used opcodes are commented/not listed. See Section 15 in datasheet
#if (FLASH_BYTES_PER_PAGE == 264)
typedef enum __attribute__((packed))
{
    FLASH_OPCODE_READ_STATUS_REG = 0xD7,
    FLASH_OPCODE_READ_MANUFACTURER_DEVICE_ID = 0x9F,

    FLASH_OPCODE_READ_LOW_POWER = 0x01,
    //FLASH_OPCODE_READ_LOW_FREQUENCY = 0x03,

    FLASH_OPCODE_READ_MODIFY_WRITE_BUF1 = 0x58,
    //FLASH_OPCODE_READ_MODIFY_WRITE_BUF2 = 0x59,

    //FLASH_OPCODE_READ_INTO_BUF1 = 0x53,
    //FLASH_OPCODE_WRITE_INTO_BUF1 = 0x84,
    //FLASH_OPCODE_WRITE_BUF1_TO_PAGE = 0x83,
    FLASH_OPCODE_READ_INTO_BUF2 = 0x55,
    FLASH_OPCODE_WRITE_INTO_BUF2 = 0x87,
    FLASH_OPCODE_WRITE_BUF2_TO_PAGE = 0x86,

    FLASH_OPCODE_ERASE_PAGE = 0x81,
} flash_opcode_t;
#else
    // READ_MODIFY_WRITE_BUF is not available for flash chips with page size 528
    #error "opcodes not defined for this page size"
#endif

// Status Register Section 9.4 in datasheet
typedef union
{
    // Data is sent with LSB first!
    uint8_t raw [2];
    uint16_t raw16;
    struct {
        // Status Register Byte 1, Table 9-1 in datasheet
        struct{
            uint8_t page_size : 1;
            uint8_t protect : 1;
            uint8_t density : 4;
            uint8_t compare_result : 1;
            uint8_t ready1 : 1;
        };
        // Status Register Byte 2, Table 9-2 in datasheet
        struct{
            uint8_t erase_suspend : 1;
            uint8_t program_suspend_buffer1 : 1;
            uint8_t program_suspend_buffer2 : 1;
            uint8_t sector_lockdown : 1;
            uint8_t reserved_4 : 1;
            uint8_t erase_program_error : 1;
            uint8_t reserved_6 : 1;
            uint8_t ready0 : 1;
        };
    };
} flash_status_reg_t;

// Manufacturer and Device ID (short version), Section 12 in datasheet
typedef union
{
    // Data needs is sent with LSB first!
    uint8_t raw [3];

    // The first 3 bytes are mandatory to read
    struct {
        uint8_t manufacturer;
        union{
            uint8_t device_id_1;
            struct{
                uint8_t density_code : 5;
                uint8_t family_code : 3;
            };
        };
        union{
            uint8_t device_id_2;
            struct{
                uint8_t product_variant : 5;
                uint8_t sub_code : 3;
            };
        };
        // Extended device information not used (2 bytes, optional)
    };
} flash_man_dev_id_t;

// Opcode + Address schema, Figure 21-2 and 21-2 in datasheet
typedef union
{
    // Data needs to be sent with MSB first!
    uint8_t raw [4];
    struct
    {
        // Address schema 264 bytes per page, Figure 21-3 in datasheet
        #if (FLASH_BYTES_PER_PAGE == 264)
        union
        {
            struct {
                uint8_t raw8;
                uint16_t raw16;
            };
            struct {
                uint16_t mem_byte_addr : 9; // BA8 - BA0
                uint16_t mem_page_addr : 11; // PA10 - PA0
                uint16_t dummy : 4;
            };
            uint16_t buffer_byte_addr : 9; // BFA8 - BFA0
            struct {
                uint16_t dummy0 : 12;
                uint16_t sector0 : 8; // Sector 0a/0b
                uint16_t dummy1 : 4;
            };
            struct {
                uint32_t dummy2 : 17;
                uint16_t sector : 3; // Sector 1-7
                uint16_t dummy3 : 4;
            };
        };

        // Address schema 256 bytes per page, Figure 21-2 in datasheet
        #elif (FLASH_BYTES_PER_PAGE == 254)
        union
        {
            struct {
                uint8_t raw8;
                uint16_t raw16;
            };
            struct {
                // A18-A0
                uint16_t mem_byte_addr : 8; // A7 - A0
                uint16_t mem_page_addr : 11; // A18 - A8
                uint16_t dummy : 5;
            };
            uint16_t buffer_byte_addr : 9; // BFA7 - BFA0
            struct {
                uint16_t dummy0 : 11;
                uint16_t sector0 : 8; // Sector 0a/0b
                uint16_t dummy1 : 5;
            };
            struct {
                uint16_t dummy2 : 16;
                uint16_t sector : 3; // Sector 1-7
                uint16_t dummy3 : 5;
            };
        };
        #else
            #error "FLASH_BYTES_PER_PAGE not defined or not implemented"
        #endif

        flash_opcode_t opcode;
    };
} flash_opcode_addr_t;
