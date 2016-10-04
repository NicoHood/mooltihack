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

/*! \file   spi_usart_private.h
 *  \brief  USART SPI functions
 */

// Include Guard
#pragma once

#include <AppConfig.h>
#include <stdint.h>
#include <stddef.h>
#include <avr/io.h>

#ifndef F_CPU
    #error "F_CPU not defined"
#endif

/**
 * SPI Data rates:
 *
 * RATE = F_CPU / 2*(URR1 + 1)
 *      = 16000000 / (2*URR1 + 2)
 *
 * URR1 = (F_CPU / (2*RATE)) - 1
 */
#define UBRR1_SPI_RATE(rate) ((F_CPU / (2UL * rate)) - 1UL)

// Configuration examples
//#define SPI_USART_RATE 8000000UL //   8MHz ->  0
//#define SPI_USART_RATE 4000000UL //   4MHz ->  1
//#define SPI_USART_RATE 2000000UL //   2MHz ->  3
//#define SPI_USART_RATE 1000000UL //   1MHz ->  7
//#define SPI_USART_RATE 800000UL  // 800KHz ->  9
//#define SPI_USART_RATE 500000UL  // 500KHz -> 15
//#define SPI_USART_RATE 400000UL  // 400KHz -> 19
//#define SPI_USART_RATE 100000UL  // 100KHz -> 79

// SPI_USART_RATE needs to be defined above or via make flag -D or -include
#ifndef SPI_USART_RATE
    #error "SPI_USART_RATE not defined"
#endif

// SPI USART pin definitions
#if defined(__AVR_ATmega32U4__)
    // Name schema similar to normal SPI definitions in avr/io.h
    #define SPI_USART_DDR   DDRD
    #define SPI_USART_PORT  PORTD
    #define SCK_USART_BIT   PORTD5
    #define MOSI_USART_BIT  PORTD3
    #define MISO_USART_BIT  PORTD2
#else
    #error "USART SPI pins not defined for this MCU"
#endif
