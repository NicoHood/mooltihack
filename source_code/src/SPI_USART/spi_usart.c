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
/*! \file   spi.c
 *  \brief  USART SPI functions
 *  Copyright [2014] [Darran Hunt]
 */
#include "spi_usart.h"

/**
 * Initialise the SPI USART interface to the specified data rate
 */
void spiUsartBegin(void)
{
    // Enable pins
    UBRR1 = 0;

    // MOSI & SCK as ouputs (enables master mode)
    // MISO as input
    // Disable pull-up
    SPI_USART_DDR |= (1 << SCK_USART_BIT) | (1 << MOSI_USART_BIT);
    SPI_USART_DDR &= ~(1 << MISO_USART_BIT);
    SPI_USART_PORT &= ~(1 << MISO_USART_BIT);

    // Set MSPI mode of operation and SPI data mode 0.
    // Enable receiver and transmitter
    // Set data rate
    UCSR1C = (1 << UMSEL11) | (1 << UMSEL10) | (0 << UCPOL1) | (0 << UCSZ10);
    UCSR1B = (1<<RXEN1) | (1<<TXEN1);
    UBRR1 = UBRR1_SPI_RATE(SPI_USART_RATE);
}

/**
 * Change the SPI USART interface data rate
 * @param rate - frequency to run the SPI interface at
 */
void spiUsartSetRate(uint16_t rate)
{
    UBRR1 = rate;
}

/**
 * send and receive a byte of data via the SPI USART interface.
 * @param data - the byte to send
 * @returns the received byte
 */
uint8_t spiUsartTransfer(uint8_t data)
{
    /* Wait for empty transmit buffer */
    while (!(UCSR1A & (1<<UDRE1)));
    UDR1 = data;
    /* Wait for data to be received */
    while (!(UCSR1A & (1<<RXC1)));
    return UDR1;
}

/**
 * this function is just meant to raise the RXC bit
 */
void spiUsartDummyWrite(void)
{
    /* Wait for empty transmit buffer */
    while (!(UCSR1A & (1<<UDRE1)));
    UDR1 = 0x00;
    /* Wait for data to be received */
    while (!(UCSR1A & (1<<RXC1)));
}

/**
 * send a byte of data via the SPI USART interface.
 * @param data - the byte to send
 */
void spiUsartSendTransfer(uint8_t data)
{
    /* Wait for data to be received */
    while (!(UCSR1A & (1<<RXC1)));
    UDR1;
    /* Wait for empty transmit buffer */
    while (!(UCSR1A & (1<<UDRE1)));
    UDR1 = data;
}

/**
 * wait for the end of a send transfer
 */
void spiUsartWaitEndSendTransfer(void)
{
    /* Wait for data to be received */
    while (!(UCSR1A & (1<<RXC1)));
    UDR1;
}

/**
 * read a number of bytes from SPI USART interface.
 * @param data - pointer to buffer to store data in
 * @param size - number of bytes to read
 */
void spiUsartRead(uint8_t *data, size_t size)
{
    while (size--)
    {
        /* Wait for empty transmit buffer */
        while (!(UCSR1A & (1<<UDRE1)));
        UDR1 = 0;
        /* Wait for data to be received */
        while (!(UCSR1A & (1<<RXC1)));
        *data++ = UDR1;
    }
}

/**
 * write a number of bytes to SPI USART interface.
 * @param data - pointer to buffer of data to write
 * @param size - number of bytes to write
 */
void spiUsartWrite(uint8_t *data, size_t size)
{
    while (size--)
    {
        /* Wait for empty transmit buffer */
        while (!(UCSR1A & (1<<UDRE1)));
        UDR1 = *data++;
        /* Wait for data to be received */
        while (!(UCSR1A & (1<<RXC1)));
        UDR1;
    }
}
