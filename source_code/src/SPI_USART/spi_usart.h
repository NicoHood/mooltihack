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

#include <stdint.h>
#include <stddef.h>

// Function prototypes
void spiUsartBegin(void);
void spiUsartSetRate(uint32_t rate);
uint8_t spiUsartTransfer(uint8_t data);
void spiUsartRead(uint8_t *data, size_t size);
void spiUsartWrite(uint8_t *data, size_t size);
