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

/*! \file   spi_usart.h
 *  \brief  USART SPI functions
 */

// Include Guard
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Software version
#define SPI_USART_VERSION 112

#include <stdint.h>
#include <stddef.h>

// Setup
void spi_usart_init(void);

// Transfer: write & read data
uint8_t spi_usart_transfer_8(uint8_t data);
void spi_usart_transfer(uint8_t* data, size_t size);
void spi_usart_transfer_lsb(uint8_t* data, size_t size);
void spi_usart_transfer_msb(uint8_t* data, size_t size);

// Write data
void spi_usart_write_8(uint8_t data);
void spi_usart_write(uint8_t* data, size_t size);
void spi_usart_write_lsb(uint8_t* data, size_t size);
void spi_usart_write_msb(uint8_t* data, size_t size);

// Read data
uint8_t spi_usart_read_8(void);
void spi_usart_read(uint8_t* data, size_t size);
void spi_usart_read_lsb(uint8_t* data, size_t size);
void spi_usart_read_msb(uint8_t* data, size_t size);

#ifdef __cplusplus
}
#endif
