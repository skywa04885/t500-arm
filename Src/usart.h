#ifndef _SRC_USART_H
#define _SRC_USART_H

#include "include.h"
#include "stm32f446.h"
#include "device.h"

#define USART_TXRX_BASE				GPIOC_BASE
#define USART_TX_LED					10
#define USART_RX_LED					12

void usart_init(void);
void usart_write_char(char c);
char usart_read_char(void);

void usart_write_string(const char *str);
uint32_t usart_read_string(char *buffer, uint32_t size);

#endif
