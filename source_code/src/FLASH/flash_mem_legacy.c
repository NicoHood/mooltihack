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

/* Copyright (c) 2014, Michael Neiderhauser. All rights reserved. */

/*!  \file     flash_mem.c
*    \brief    Mooltipass Flash IC Library
*    Created:  31/3/2014
*    Author:   Michael Neiderhauser
*/
#include "flash_mem.h"
#include "defines.h"
#include "usb.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <spi_usart.h>
#if SPI_FLASH != SPI_USART
    #error "SPI not implemented"
#endif

/*! \fn     memoryBoundaryErrorCallback(void)
*   \brief  Function called when a memory boundary issue occurs
*/
void memoryBoundaryErrorCallback(void)
{
    // We'll add more debug later if needed
    usbPutstr("#MBE");
    while(1);
}

//------------------------------------------------------------------------------
//---------------------------wrapper functions---------------------------
//------------------------------------------------------------------------------

void pageErase(uint16_t pageNumber)
{
    #ifdef MEMORY_BOUNDARY_CHECKS
    flash_ret_t ret =
    #endif
    flash_erase_page(pageNumber);

    #ifdef MEMORY_BOUNDARY_CHECKS
    // Error check the parameter pageNumber and offset
    if(ret == FLASH_RET_ERR_INPUT_PARAM)
    {
        memoryBoundaryErrorCallback();
    }
    #endif
}

void loadPageToInternalBuffer(uint16_t pageNumber)
{
    #ifdef MEMORY_BOUNDARY_CHECKS
    flash_ret_t ret =
    #endif
    flash_read_into_buffer(pageNumber);

    #ifdef MEMORY_BOUNDARY_CHECKS
    // Error check the parameter pageNumber and offset
    if(ret == FLASH_RET_ERR_INPUT_PARAM)
    {
        memoryBoundaryErrorCallback();
    }
    #endif
}

void writeDataToFlash(uint16_t pageNumber, uint16_t offset, uint16_t dataSize, void *data)
{
    #ifdef MEMORY_BOUNDARY_CHECKS
    flash_ret_t ret =
    #endif
    flash_write_page(pageNumber, offset, (uint8_t*) data, dataSize);

    #ifdef MEMORY_BOUNDARY_CHECKS
    // Error check the parameter pageNumber and offset
    if(ret == FLASH_RET_ERR_INPUT_PARAM)
    {
        memoryBoundaryErrorCallback();
    }
    #endif
} // End writeDataToFlash


void readDataFromFlash(uint16_t pageNumber, uint16_t offset, uint16_t dataSize, void *data)
{
    #ifdef MEMORY_BOUNDARY_CHECKS
    flash_ret_t ret =
    #endif
    flash_read_page(pageNumber, offset, (uint8_t*) data, dataSize);

    #ifdef MEMORY_BOUNDARY_CHECKS
    // Error check the parameter pageNumber and offset
    if(ret == FLASH_RET_ERR_INPUT_PARAM)
    {
        memoryBoundaryErrorCallback();
    }
    #endif
} // End readDataFromFlash

void flashRawRead(uint8_t* datap, uint16_t addr, size_t size)
{
    #ifdef MEMORY_BOUNDARY_CHECKS
    flash_ret_t ret =
    #endif
    flash_read_raw(addr, datap, size);

    #ifdef MEMORY_BOUNDARY_CHECKS
    // Error check the parameter pageNumber and offset
    if(ret == FLASH_RET_ERR_INPUT_PARAM)
    {
        memoryBoundaryErrorCallback();
    }
    #endif
}

void flashWriteBuffer(uint8_t* datap, uint16_t offset, size_t size)
{
    #ifdef MEMORY_BOUNDARY_CHECKS
    flash_ret_t ret =
    #endif
    flash_write_into_buffer(offset, datap, size);

    #ifdef MEMORY_BOUNDARY_CHECKS
    // Error check the parameter pageNumber and offset
    if(ret == FLASH_RET_ERR_INPUT_PARAM)
    {
        memoryBoundaryErrorCallback();
    }
    #endif
}

void flashWriteBufferToPage(uint16_t page)
{
    #ifdef MEMORY_BOUNDARY_CHECKS
    flash_ret_t ret =
    #endif
    flash_write_buffer_to_page(page);

    #ifdef MEMORY_BOUNDARY_CHECKS
    // Error check the parameter pageNumber and offset
    if(ret == FLASH_RET_ERR_INPUT_PARAM)
    {
        memoryBoundaryErrorCallback();
    }
    #endif
}
