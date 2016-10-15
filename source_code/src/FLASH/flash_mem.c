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

/*!  \file     flash_mem.c
*    \brief    Mooltipass Flash IC Library
*/
#include "flash_mem.h"
#include "flash_mem_private.h"

/**
 * Reads the flash chip status register
 * @return  flash chip status register
 */
static inline flash_status_reg_t flash_read_status_reg(void)
{
    flash_status_reg_t status_reg;

    // Assert chip select
    FLASH_PORT_SS &= ~(1 << FLASH_BIT_SS);

    // Read status register, Section 9.4, Table 9-1, Figure 25-10 in datasheet
    spi_usart_write_8(FLASH_OPCODE_READ_STATUS_REG);
    spi_usart_read_lsb(status_reg.raw, sizeof(status_reg));

    // Deassert chip select
    FLASH_PORT_SS |= (1 << FLASH_BIT_SS);

    return status_reg;
}

/**
 * Read the Manufacturers and Device ID Register.
 * @return  Manufacturers and Device ID Register
 */
static inline flash_man_dev_id_t flash_read_manufacturer_device_id(void)
{
    flash_man_dev_id_t man_dev_id;

    // Assert chip select
    FLASH_PORT_SS &= ~(1 << FLASH_BIT_SS);

    // Read manufacturer and device id, Section 12 in datasheet
    spi_usart_write_8(FLASH_OPCODE_READ_MANUFACTURER_DEVICE_ID);
    spi_usart_read_lsb(man_dev_id.raw, sizeof(man_dev_id));

    // Deassert chip select
    FLASH_PORT_SS |= (1 << FLASH_BIT_SS);

    return man_dev_id;
}

/**
 * Private internal library function to send an opcode along with data.
 * Not all parameters need to be used.
 * @param   page    The target page number of flash memory
 * @param   offset  The starting byte offset to begin reading in pageNumber
 * @param   data    The buffer used to store the data read from flash
 * @param   size    The number of bytes to read from the flash memory into the data buffer
 * @param   opcode  The opcode of the flash chip function to execute
 * @param   write   Boolean to determine if a write or read operation should be executed
 * @return  error code, zero means no error
 * @note    Function DOES allow crossing page boundaries but prevents invalid page/offset inputs
 */
static inline flash_ret_t flash_transfer_opcode_data
(uint16_t page, uint16_t offset, uint8_t* data, size_t size, flash_opcode_t opcode, bool write)
{
    // Check page and offset limits
    if((page >= FLASH_PAGE_COUNT) || (offset >= FLASH_BYTES_PER_PAGE))
    {
        return FLASH_RET_ERR_INPUT_PARAM;
    }

    // Construct opcode
    flash_opcode_addr_t op;
    op.opcode = opcode;

    // Construct address data manually to save some compiler overhead
#if (FLASH_BYTES_PER_PAGE == 264)
    op.raw16 = (page << (9 - 8)) | (offset >> 8);
    op.raw8 = offset;
#elif (FLASH_BYTES_PER_PAGE == 528)
    op.raw16 = (page << (10 - 8)) | (offset >> 8);
    op.raw8 = offset;
#else
    #warning "Bitshift for this page size not defined. Defaulting to struct."
    op.mem_page_addr = page;
    op.mem_byte_addr = offset;
#endif

    // Assert chip select
    FLASH_PORT_SS &= ~(1 << FLASH_BIT_SS);

    // Send opcode with MSB first
    spi_usart_write_msb(op.raw, sizeof(flash_opcode_addr_t));

    // Retrieve data
    if(write)
    {
        spi_usart_write(data, size);
    }
    else
    {
        spi_usart_read(data, size);
    }

    // Deassert chip select
    FLASH_PORT_SS |= (1 << FLASH_BIT_SS);

    // Wait until memory is ready
    // TODO move for read?
    if(write){
    flash_status_reg_t status_reg;
    do
    {
        status_reg = flash_read_status_reg();
    } while(!status_reg.ready0);

    // Check for erase or programming errors
    if (status_reg.erase_program_error)
    {
        return FLASH_RET_ERR_ERASE_PROGRAM;
    }
}

    // No error
    return FLASH_RET_OK;
}

/**
 * Initializes SS IO for the Flash Chip
 */
void flash_init(void)
{
    // Setup chip select signal
    FLASH_DDR_SS |= (1 << FLASH_BIT_SS);
    FLASH_PORT_SS |= (1 << FLASH_BIT_SS);
}

/**
 * Attempts to read the Manufacturers Information Register.
 * @note    Performs a comparison to verify the size of the flash chip
 * @return  error code, zero means no error
 */
flash_ret_t flash_check_device_id(void)
{
    // Read flash identification
    flash_man_dev_id_t id = flash_read_manufacturer_device_id();

    // Check ID
    if((id.manufacturer != FLASH_MANUF_ID) || (id.device_id_1 != FLASH_FAM_DEN_VAL))
    {
        return FLASH_RET_ERR_ID;
    }

    // No error
    return FLASH_RET_OK;
}

/**
 * Reads a data buffer of flash memory. The data is read starting at offset of a page.
 * @param   page    The target page number of flash memory
 * @param   offset  The starting byte offset to begin reading in pageNumber
 * @param   data    The buffer used to store the data read from flash
 * @param   size    The number of bytes to read from the flash memory into the data buffer
 * @return  error code, zero means no error
 * @note    Function does NOT allow crossing page boundaries.
 */
flash_ret_t flash_read_page(uint16_t page, uint16_t offset, uint8_t* data, size_t size)
{
    // Extra error to not cross page boundaries
    if((offset + size - 1) >= FLASH_BYTES_PER_PAGE)
    {
        return FLASH_RET_ERR_INPUT_PARAM;
    }
    return flash_transfer_opcode_data(page, offset, data, size, FLASH_OPCODE_READ_LOW_POWER, false);
}

/**
 * Writes a data buffer directly to flash memory. The data is written starting at offset of a page.
 * @param   page       The target page number of flash memory
 * @param   offset     The starting byte offset to begin writing in pageNumber
 * @param   data       The buffer containing the data to write to flash memory
 * @param   size   The number of bytes to write from the data buffer (assuming the data buffer is sufficiently large)
 * @return  error code, zero means no error
 * @note    Function does not allow crossing page boundaries.
 */
 flash_ret_t flash_write_page(uint16_t page, uint16_t offset, uint8_t* data, size_t size)
 {
    // Check if offset crossed page boundaries
    if((offset + size - 1) >= FLASH_BYTES_PER_PAGE)
    {
        return FLASH_RET_ERR_INPUT_PARAM;
    }

    // Read-Modify-Write with internal low level functions of the chip
    return flash_transfer_opcode_data(page, offset, data, size, FLASH_OPCODE_READ_MODIFY_WRITE_BUF1, true);
}

/**
 * Contiguous data read across flash page boundaries with a max 65k bytes addressing space
 * @param   addr            byte offset in the flash
 * @param   data            pointer to the buffer to store the read data
 * @param   size            the number of bytes to read
 * @return  error code, zero means no error
 */
flash_ret_t flash_read_raw(uint16_t addr, uint8_t* data, size_t size)
{
    // Check flash boundary
    if(((uint32_t)addr + (uint32_t)size - 1UL) >= FLASH_SIZE)
    {
        return FLASH_RET_ERR_INPUT_PARAM;
    }

    // Map address to page and byte offset
    uint16_t page = addr / FLASH_BYTES_PER_PAGE;
    uint16_t offset = addr % FLASH_BYTES_PER_PAGE;
    return flash_transfer_opcode_data(page, offset, data, size, FLASH_OPCODE_READ_LOW_POWER, false);
}

/**
 * Contiguous data read across flash page boundaries with full addressing space (>65kb)
 * @param   addr            byte offset in the flash
 * @param   data            pointer to the buffer to store the read data
 * @param   size            the number of bytes to read
 * @return  error code, zero means no error
 */
flash_ret_t flash_read_raw_far(uint32_t addr, uint8_t* data, size_t size)
{
    // Check flash boundary
    if(((uint32_t)addr + (uint32_t)size - 1UL) >= FLASH_SIZE)
    {
        return FLASH_RET_ERR_INPUT_PARAM;
    }

    // Map address to page and byte offset
    uint16_t page = addr / FLASH_BYTES_PER_PAGE;
    uint16_t offset = addr % FLASH_BYTES_PER_PAGE;
    return flash_transfer_opcode_data(page, offset, data, size, FLASH_OPCODE_READ_LOW_POWER, false);
}

/**
 * Contiguous data write across flash page boundaries with a max 65k bytes addressing space
 * @param   addr            byte offset in the flash
 * @param   data            The buffer containing the data to write to flash memory
 * @param   size            the number of bytes to write
 * @return  error code, zero means no error
 */
flash_ret_t flash_write_raw(uint16_t addr, uint8_t* data, size_t size)
{
    // Just use the full address space implementation to avoid code dumplication
    return flash_write_raw_far(addr, data, size);
}

/**
 * Contiguous data write across flash page boundaries with full addressing space (>65kb)
 * @param   addr            byte offset in the flash
 * @param   data            The buffer containing the data to write to flash memory
 * @param   size            the number of bytes to write
 * @return  error code, zero means no error
 */
flash_ret_t flash_write_raw_far(uint32_t addr, uint8_t* data, size_t size)
{
    // Check flash boundary
    if(((uint32_t)addr + (uint32_t)size - 1UL) >= FLASH_SIZE)
    {
        return FLASH_RET_ERR_INPUT_PARAM;
    }

    // Map address to page and byte offset
    uint16_t page = addr / FLASH_BYTES_PER_PAGE;
    uint16_t offset = addr % FLASH_BYTES_PER_PAGE;

    // Write page per page
    size_t bytes_to_write;
    flash_ret_t ret;
    do
    {
        // Calculate how many bytes to write
        bytes_to_write = size;
        if(bytes_to_write > (FLASH_BYTES_PER_PAGE - offset)){
            bytes_to_write = (FLASH_BYTES_PER_PAGE - offset);
        }
        size -= bytes_to_write;

        // Write the page
        ret = flash_write_page(page, offset, data, bytes_to_write);
        if(ret != FLASH_RET_OK){
           return ret;
        }

        // Only use the offset for the first write
        offset = 0;
        page++;
    }
    while(size);

    return ret;
}

/**
 * Load a given page in the flash internal buffer
 * @param   page      The target page number of flash memory
 * @return  error code, zero means no error
 */
flash_ret_t flash_read_into_buffer(uint16_t page)
{
    return flash_transfer_opcode_data(page, 0, NULL, 0, FLASH_OPCODE_READ_INTO_BUF2, true);
}

/**
 * Write data into the internal memory buffer. Used for writing multiple
 * chunks of a flashpage, for example via the usb buffer in 62B chunks.
 * Use flash_write_buffer_to_page() afterwards to save the buffer.
 * @param offset offset to start writing to in the internal memory buffer
 * @param data   pointer to data to write
 * @param size   the number of bytes to write
 * @return  error code, zero means no error
 * @note    Function does not allow crossing page boundaries.
 */
flash_ret_t flash_write_into_buffer(uint16_t offset, uint8_t* data, size_t size)
{
    // Check if offset crossed page boundaries
    if((offset + size - 1) >= FLASH_BYTES_PER_PAGE)
    {
        return FLASH_RET_ERR_INPUT_PARAM;
    }

    return flash_transfer_opcode_data(0, offset, data, size, FLASH_OPCODE_WRITE_INTO_BUF2, true);
}

/**
 * Write the contents of the internal memory buffer to a page in flash
 * Use it after flash_write_into_buffer()
 * @param   page the page to store the buffer in
 * @return  error code, zero means no error
 */
flash_ret_t flash_write_buffer_to_page(uint16_t page)
{
    return flash_transfer_opcode_data(page, 0 , NULL, 0, FLASH_OPCODE_WRITE_BUF2_TO_PAGE, true);
}

/**
 * Erases page pageNumber (0 up to FLASH_PAGE_COUNT valid).
 * @param   page      The page to erase
 * @return  error code, zero means no error
 * @note    Sets all bits in page to logic one (High, 0xFF)
 */
flash_ret_t flash_erase_page(uint16_t page)
{
    return flash_transfer_opcode_data(page, 0, NULL, 0, FLASH_OPCODE_ERASE_PAGE, true);
}

/**
 * Erases multiple flash pages
 */
flash_ret_t flash_erase_pages(uint16_t page, uint16_t count)
{
    // TODO count boundary check -> start from the end?

    // Erase all flash pages with the library page erase function
    for (uint16_t i = page; i < (page + count); i++)
    {
        flash_ret_t ret = flash_erase_page(i);
        if(ret)
        {
            return ret;
        }
    }

    // No error
    return FLASH_RET_OK;
}

/**
 * Erase the complete memory (filled with logic 1 (0xFF, HIGH))
 */
flash_ret_t flash_erase_chip(void)
{
    return flash_erase_pages(0, FLASH_PAGE_COUNT);
}
