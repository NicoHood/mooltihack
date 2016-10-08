# USART SPI Library 1.1.1 for Mooltipass

## Overview
##### Files:
- spi_usart.c
- spi_usart.h
- spi_usart_private.h
- SPI_USART.mk

##### Module dependencies:
- None

##### Modules that depend on USART SPI:
- FLASH
- OLEDMINI

## Description
The USART SPI Library was written as basic SPI access layer for the Flash and
OLED library. It does not handle the SS pins of the different peripherals.

## Implementation
##### Functions implemented by USART SPI:
- Initialize the USART in MSPI mode 0
- Transfer (TX + RX) a single byte
- Write a byte array
- Read into a byte array

### Verion 1.1.0 Change Notes
The library once had an optimized faster write option split into 3 separate
functions which is now removed as the OLED works fast enough with 4MHz.

`spiUsartTransfer()` was renamed to `spi_usart_transfer_8()` in version 1.1.x!

### Usage
`SPI_USART_RATE` needs to be defined at compile time in `AppConfig.h` or via
`-D` as compiler flag. Configuration examples can be found below:

```c
// Definition in AppConfig.h, 8MHz
#define SPI_USART_RATE 8000000UL
```

```makefile
# Compiler flag, 4 MHz
C_FLAGS += -DSPI_USART_RATE=4000000UL
```

### API
The USART SPI functions are mostly self explaining. You first need to setup
the USART SPI interface and then can do transfer/write/read operations.

#### Setup
Initializes the USART SPI in MSPI mode. SS pin is not changed!
`spiUsartSetRate()` is optional to change the speed at runtime.

```c
void spi_usart_init(void);
void spiUsartSetRate(uint32_t rate);
```

#### Transfer
Transfer sends the data and overwrites the input buffer with the reading.

```c
uint8_t spi_usart_transfer_8(uint8_t data);
void spi_usart_transfer(uint8_t* data, size_t size);
void spi_usart_transfer_lsb(uint8_t* data, size_t size);
void spi_usart_transfer_msb(uint8_t* data, size_t size);
```

#### Write
Write only sends the data and discards the reading.
```c
void spi_usart_write_8(uint8_t data);
void spi_usart_write(uint8_t* data, size_t size);
void spi_usart_write_lsb(uint8_t* data, size_t size);
void spi_usart_write_msb(uint8_t* data, size_t size);
```

#### Read
Read sends empty data (zero) and reads the data into the input buffer.
```c
uint8_t spi_usart_read_8(void);
void spi_usart_read(uint8_t* data, size_t size);
void spi_usart_read_lsb(uint8_t* data, size_t size);
void spi_usart_read_msb(uint8_t* data, size_t size);
```

## Library TODO's
- None

## Changelog
- 2016/10/05 NicoHood - V 1.1.2
  - Renamed function API
  - Removed not used spiUsartSetRate()
- 2016/10/05 NicoHood - V 1.1.1
  - Renamed `spiUsartTransfer()` to `spi_usart_transfer_8()`
  - Added version number
  - Added API documentation
  - Added LSB/MSB functions
  - Added 8bit and buffer functions
- 2016/10/04 NicoHood - V 1.1.0
  - Cleanup, Improvements, Documentation
- 2014 Darran Hunt - V1.0
  - Initial commit
