#ifndef _SPI_H
#define _SPI_H

#include "include.h"
#include "stm32f446.h"

/**
 * Initializes the SPI peripheral, and enables the clocks
 */
void spi_init(void);

/**
 * Reads and writes byte over SPI
 */
uint8_t spi_transceive(uint8_t byte);

#endif
