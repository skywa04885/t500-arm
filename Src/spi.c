#include "spi.h"

// PA7: SPI1_MOSI
// PA6: SPI1_MISO
// PA5: SPI1_SCK
// PA4: NSS

/**
 * Initializes the SPI peripheral, and enables the clocks
 */
void spi_init(void)
{
	// Enables the SPI1 Clock
	*RCC_APB2ENR |= (1 << RCC_APB2ENR_SPI1EN);

	// Configures GPIO
	*GPIO_MODER(GPIOA_BASE) |= GPIO_MODE(GPIO_ALTERNATIVE_FUNC, 7)
			| GPIO_MODE(GPIO_ALTERNATIVE_FUNC, 6)
			| GPIO_MODE(GPIO_ALTERNATIVE_FUNC, 5);
	*GPIO_AFRL(GPIOA_BASE) |= GPIO_AF(GPIO_AF5, 7)
			| GPIO_AF(GPIO_AF5, 6)
			| GPIO_AF(GPIO_AF5, 5);

	// Sets the GPIO speeds
	*GPIO_OSPEEDR(GPIOA_BASE) |= GPIO_SPEED(GPIO_HIGH_SPEED, 7)
			| GPIO_SPEED(GPIO_HIGH_SPEED, 6)
			| GPIO_SPEED(GPIO_HIGH_SPEED, 5);

	// Configures SPI1
	*SPI_CR1(SPI1_BASE) = (1 << SPI_CR1_MSTR)
			| SPI_CR1_BR(SPI_CR1_BR_DIV64)
			| (1 << SPI_CR1_SSM)
			| (1 << SPI_CR1_SSI);
	*SPI_CR1(SPI1_BASE) |= (1 << SPI_CR1_SPE);
}

/**
 * Reads and writes byte over SPI
 */
uint8_t spi_transceive(uint8_t byte)
{
	while (!(*SPI_SR(SPI1_BASE) & (1 << SPI_SR_TXE)));
	*SPI_DR(SPI1_BASE) = byte;
	while (!(*SPI_SR(SPI1_BASE) & (1 << SPI_SR_RXNE)));
	return *SPI_DR(SPI1_BASE);
}
