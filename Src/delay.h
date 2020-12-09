#ifndef _SRC_DELAY_H
#define _SRC_DELAY_H

#include "include.h"
#include "stm32f446.h"

void delay_init(void);
void delay_ns(uint16_t ns);
void delay_us(uint16_t us);
void delay_ms(uint32_t ms);
void delay_s(uint32_t s);

#endif
