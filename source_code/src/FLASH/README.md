# Flash Memory Library 1.1.0 for Mooltipass

## Overview
##### Files:
- flash_mem.c
- flash_mem_legacy.c
- flash_mem.h
- flash_mem_private.h
- FLASH.mk

##### Module dependencies:
- SPI_USART

##### Modules that depend on USART SPI:
- LOGIC
- OLEDMINI
- OLEDMP
- USB
- Main Application & Bootloader

## Description
The Flash Library was written to add an access layer to write to the SPI flash.
The library was designed to allow per page read/write/erase access but also
raw byte read/write operations with version 1.1.0 and later. The internal flash
structure with sectors blocks and page limits is abstracted with the API.

### Supported Chips
The Flash Library allows usage of a subset of the Adesto family of SPI flash
chips. The Flash Library supports the following chips:
-  1M  (AT45DB011D) - (512  Pages at 264 Bytes Per Page)
-  2M  (AT45DB021E) - (1024 Pages at 264 Bytes Per Page)
-  4M  (AT45DB041E) - (2048 Pages at 264 Bytes Per Page)
-  8M  (AT45DB081E) - (4096 Pages at 264 Bytes Per Page)
- 16M  (AT45DB161E) - (4096 Pages at 528 Bytes Per Page)
- 32M  (AT45DB321E) - (8192 Pages at 528 Bytes Per Page)

## Implementation
##### Functions implemented by FLASH:
- Initialize the flash SS pin
- Status + manufacturer information read
- Flash page read/write operations
- Flash page read/write raw operations
- Flash low level page modification operations
- Flash erase functions

### Verion 1.1.0 Change Notes
With version 1.1.0 a few major changes were made. The library got a whole API
cleanup and only a few old functions are still supported as wrapper to the new.
The will be removed in newer versions when the main application will be updated.

Unnecessary definitions got removed, while the important were kept and all got a
"flash" prefix to not mix them with other libraries. Most definitions got
replaced by enums or structs to have a cleaner code and less possible bit shift
errors. Version 1.1.0 was mainly developed for 4M flash memory and a few
definitions are still not finished yet.

All function operations are now on a page level only so that the user does not
have to deal with internal flash sectors or blocks. Raw byte access was added
as equivalent to the read function. The raw read/write access also supports the
full memory size now with the `_far` function postfix.

The library also (still) provides low level flash access functions to read/write
into the internal sram buffer of the flash chip. This is useful when you want to
write a page, but you have the data in smaller chunks (used in the usb code).
The low level API allows you to write the chunks first into the buffer and then
write the page. This safes erase/write cycles. The low level API now uses the
sram buffer 2 of the IC chip which allows to you do normal write operations
while waiting for new data (via usb).

The read mode changed from low frequency to low power. The maximum speed for low
power mode is 15MHz. The AtMega32u4 can read SPI with a maximum frequency of
8MHz with F_CPU 16MHz so we are far below that limit but can safe some energy.

No function destroys the input buffers anymore. This means you can still use the
data passed to the read/write/erase functions. The functions were also improved
to return an error code enum to notice boundary errors or erase/program errors
of which the last ones are also new. The write into buffer function does not
wait for a ready signal anymore, as it is not required for this function.

### Usage
`FLASH_CHIP_XM` and the Slave Select (SS) pin needs to be defined at compile
time in `AppConfig.h` or via `-D` as compiler flag. Configuration examples can
be found below:
```c
// Flash chip size
#define FLASH_CHIP_4M

// Slave Select Flash (MP Mini)
#define FLASH_BIT_SS    PB4
#define FLASH_PORT_SS   PORTB
#define FLASH_DDR_SS    DDRB
```

```makefile
# Compiler flag, 4M flash chip, SS on PB7 (original MP)
C_FLAGS += -DFLASH_CHIP_4M
C_FLAGS += -DFLASH_BIT_SS=PB7 -DFLASH_PORT_SS=PORTB -DFLASH_DDR_SS=DDRB
```

# API
The FLASH function are mostly self explaining. You first need to setup
the USART SPI interface +SS and then can do read/write/erase operations.

### Definitions
A few definitions are provided to help your code to work better with the page
size and the number of pages. It also defines the total size of Mbit for the
selected IC. An example of the `FLASH_CHIP_4M` setting can be found below:
```c
#define FLASH_CHIP 4
#define FLASH_PAGE_COUNT 2048UL
#define FLASH_BYTES_PER_PAGE 264UL
#define FLASH_SIZE (FLASH_PAGE_COUNT * FLASH_BYTES_PER_PAGE)
```

#### Error Codes
Most functions return an error enum `flash_ret_t` to determine if the function
call was successful. If it was not successful the enum gives you an error code
that gives you more details about the problem. Zero means no error occurred.
```c
typedef enum
{
    FLASH_RET_OK = 0x00,
    FLASH_RET_ERR_INPUT_PARAM = 0x01,
    FLASH_RET_ERR_ERASE_PROGRAM = 0x02,
    FLASH_RET_ERR_ID = 0x03,
} flash_ret_t;
```

#### Setup
The FLASH library requires the setup of the SPI_USART first. Please initialize
the usart spi first and then setup the flash SS pin by calling the init
function. Afterwards the `flash_check_device_id()` function can be used to check
the flash chip presence with the correct size. It will return `FLASH_RET_ERR_ID`
if an error occurred reading the manufacturer and device id.

```c
void flash_init(void);
flash_ret_t flash_check_device_id(void);
```

#### Page Read/Write
Read and write operations on a page basis do now allow to read/write across page
boundaries. You can use the definitions above to check the maximum page size
and number of pages in your code. An offset can be used to read a part of a page.
```c
flash_ret_t flash_read_page(uint16_t page, uint16_t offset, uint8_t* data, size_t size);
flash_ret_t flash_write_page(uint16_t page, uint16_t offset, uint8_t* data, size_t size);
```

#### Raw Read/Write
Raw read and write operations give you the option to access the flash on a byte
address basis. You have the option to only address the first 65kb of flash with
a 16 bit input address or the full address space with the `_far` postfix and a
32 bit input address. Page boundaries can be crossed but not the flash size.
```c
flash_ret_t flash_read_raw(uint16_t addr, uint8_t* data, size_t size);
flash_ret_t flash_read_raw_far(uint32_t addr, uint8_t* data, size_t size);
flash_ret_t flash_write_raw(uint16_t addr, uint8_t* data, size_t size);
flash_ret_t flash_write_raw_far(uint32_t addr, uint8_t* data, size_t size);
```

#### Low Level Write Operations
The low level functions expose a bit of the internal flash IC structure. You can
use this if you need to write a flash page in smaller chunks but do not want to
use the MCU SRAM to store the flash page first. This is useful for USB transfer
as 64 byte chunks which occurs infrequently and you cannot reserve a full flash
page in the MCU SRAM.

You then can read the current flash page into the buffer, modify it (multiple
times) with smaller chunks and then write it back to the flash page. This saves
you additional write operations which makes the flash last longer.

For the low level functions another flash IC SRAM buffer is used than the normal
read/write operations. This means you can modify the low level buffer while
still doing normal write operations.

The rewrite page command wraps the normal write page command with no data as
parameters. This is useful once every 50,000 erase/program operations for a
flash sector. Please refer to the datasheet section "9.3 Auto Page Rewrite".
```c
flash_ret_t flash_read_into_buffer(uint16_t page);
flash_ret_t flash_write_into_buffer(uint16_t offset, uint8_t* data, size_t size);
flash_ret_t flash_write_buffer_to_page(uint16_t page);
flash_ret_t flash_rewrite_page(uint16_t page);
```

#### Erase
Erase functions are only exposed on a page basis and not on block/sector level
which the flash IC internally also uses. This is to make the API simple and
compact. You can erase a single or multiple pages up to the whole flash chip.
```c
flash_ret_t flash_erase_page(uint16_t page);
flash_ret_t flash_erase_pages(uint16_t page, uint16_t count);
flash_ret_t flash_erase_chip(void);
```

## Flash Memory Testing
##### Files:
- flash_test.c
- flash_test.h
- FLASH.mk

##### Warning: The Flash test tool was not updated yet.

Flash Testing attempts to exercise the library and obtain status on the flash chip.
To run Flash Testing:
Include the flash_test.h file and call the flashTest() Function.

To run Flash Testing on the Mooltipass:
In the file tests.c uncomment the (pound)define TEST_FLASH in the beforeFlashInitTests() function.

Note:  The flash_test.c file does use oled and usb libs of the Mooltipass however this should be easy to remove if needed.

## Library TODO's
- Test and fix other chips beside 4M
- Implement support for the 64M Flash Chip
- Remove legacy Files/API

## Changelog
- 2016/10/08 NicoHood - V 1.1.0
  - Cleanup, Improvements, Documentation
  - Added version number
  - Added API documentation
  - Changed user API handling for simpler/clearer usage
  - added raw write function
  - removed block/sector erase functions
  - clearer enums/struct definitions
  - "flash" prefix for every library function/definition
  - Low level functions use their own SRAM buffer
  - No input buffer gets destroyed anymore
  - old flashWriteBuffer() does not wait a ready state anymore
  - erase/program error bit check
  - error code enums
  - uses low power read now
  - added makefile module
- 2014/03/31 Michael Neiderhauser - V1.0
  - Initial commit
