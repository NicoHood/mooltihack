# USART SPI Library for Mooltipass

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

The library once had an optimized faster write option split into 3 separate
functions which is now removed as the OLED works fast enough with 4MHz.

`SPI_USART_RATE` needs to be defined at compile time in `AppConfig.h` or via
`-D` as compiler flag. Configuration examples can be found below:

```c
// Definition in AppConfig.h, 8MHz
#define SPI_USART_RATE 8000000UL
```

```makefile
# Compiler flag, 4 MHz
C_FLAGS += -D SPI_USART_RATE=4000000UL
```

## Library TODO's
- None

## Changelog
- 10/2016 NicoHood - Cleanup, Improvements, Documentation
- 2014 Darran Hunt - Initial commit
