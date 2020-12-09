#include "delay.h"

void delay_init(void)
{
	*TIM_CR1(TIM1_BASE) = 0;			// Do nothing
	*TIM_PSC(TIM1_BASE) = 180;			// 180Mhz -> 1Mhz, so every microsecond
}

void delay_ns(uint16_t ns)
{
	ns /= 5;
	for (uint16_t i = 0; i < ns; ++i);
}

void delay_us(uint16_t us)
{
	*TIM_CNT(TIM1_BASE) = 0;
	*TIM_SR(TIM1_BASE) = 0;
	*TIM_ARR(TIM1_BASE) = us;
	*TIM_CR1(TIM1_BASE) = (1 << TIM_CR1_CEN) | (1 << TIM_CR1_OPM);

	while (!(*TIM_SR(TIM1_BASE) & (1 << TIM_SR_UIF)));
}

void delay_ms(uint32_t ms)
{
	for (uint32_t i = 0; i < ms; ++i)
		delay_us(1000);
}

void delay_s(uint32_t s)
{
	for (uint32_t i = 0; i < s; ++i)
		delay_ms(1000);
}
