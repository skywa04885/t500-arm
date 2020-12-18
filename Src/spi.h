#ifndef _SPI_H
#define _SPI_H

#include "include.h"
#include "stm32f446.h"

/**
 * Initializes the SPI peripheral, and enables the clocks
 */
void spi_init(void);

/**
 * Writes an byte over the SPI bus
 */
void spi_write_byte(uint8_t byte);

/**
 * Writes an half word over the SPI bus
 */
void spi_write_halfword(uint16_t halfword);

/**
 * Writes an word over the SPI bus
 */
void spi_write_word(uint32_t word);

#endif
