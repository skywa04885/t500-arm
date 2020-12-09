#include "usart.h"

void usart_init(void)
{
	// Enables the clock to USART2
	*RCC_APB1ENR |= (1 << RCC_APB1END_USART2EN);

	// Sets PA2 and PA3 to alternative function mode, and selects PA7 (RX/TX)
	*GPIO_MODER(GPIOA_BASE) |= GPIO_MODE(GPIO_ALTERNATIVE_FUNC, 2)
			| GPIO_MODE(GPIO_ALTERNATIVE_FUNC, 3);
	*GPIO_AFRL(GPIOA_BASE) |= GPIO_AF(GPIO_AF7, 2)
			| GPIO_AF(GPIO_AF7, 3);

	// Sets PA2 and PA3 high speed bus
	*GPIO_OSPEEDR(GPIOA_BASE) |= GPIO_SPEED(GPIO_HIGH_SPEED, 2) | GPIO_SPEED(GPIO_HIGH_SPEED, 3);

	// Sets the USART2 DIV value, to get the baud of 115200, with clock of 45Mhz
	*USART_BRR(USART2_BASE) = ((2 * 45000000) + 115200) / (2 * 115200);


	// Enables USART2 RX, TX and UART
	*USART_CR1(USART2_BASE) = (1 << USART_CR1_TE)
			| (1 << USART_CR1_RE)
			| (1 << USART_CR1_UE);

	// Makes the RX/TX LED's output
	*GPIO_MODER(USART_TXRX_BASE) |= GPIO_MODE(GPIO_OUTPUT, USART_TX_LED);
	*GPIO_MODER(USART_TXRX_BASE) |= GPIO_MODE(GPIO_OUTPUT, USART_RX_LED);
}

void usart_write_char(char c)
{
	*GPIO_ODR(USART_TXRX_BASE) |= (1 << USART_TX_LED);

	while (!(*USART_SR(USART2_BASE) & (1 << USART_SR_TXE)));
	*USART_DR(USART2_BASE) = c;

	*GPIO_ODR(USART_TXRX_BASE) &= ~(1 << USART_TX_LED);
}

char usart_read_char(void)
{
	*GPIO_ODR(USART_TXRX_BASE) |= (1 << USART_RX_LED);
	while (!(*USART_SR(USART2_BASE) & (1 << USART_SR_RXNE)));
	*GPIO_ODR(USART_TXRX_BASE) &= ~(1 << USART_RX_LED);
	return *USART_DR(USART2_BASE);
}

void usart_write_string(const char *str)
{
	while (*str != '\0')
		usart_write_char(*str++);
}

uint32_t usart_read_string(char *buffer, uint32_t size)
{
	uint32_t current_size = 0;

	--size; // Make space for null-term
	while (current_size < size)
	{
		buffer[current_size] = usart_read_char();
		if (buffer[current_size++] == '\n') break;
	}

	buffer[current_size] = '\0';
	return current_size;
}

int __io_putchar(int ch)
{
	usart_write_char(ch);
	return ch;
}

int __io_getchar(void)
{
	return usart_read_char();
}
