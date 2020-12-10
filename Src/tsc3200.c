#include "tsc3200.h"

void tsc3200_init(tsc3200_t *sensor)
{
	// Resets all pins
	*GPIO_MODER(sensor->gpio_base) &= ~(GPIO_MODE(0b11, sensor->gpio_oe)
			| GPIO_MODE(0b11, sensor->gpio_out)
			| GPIO_MODE(0b11, sensor->gpio_s0)
			| GPIO_MODE(0b11, sensor->gpio_s1)
			| GPIO_MODE(0b11, sensor->gpio_s2)
			| GPIO_MODE(0b11, sensor->gpio_s3));

	// Makes the desired pins output
	*GPIO_MODER(sensor->gpio_base) |= GPIO_MODE(GPIO_OUTPUT, sensor->gpio_oe)
			| GPIO_MODE(GPIO_OUTPUT, sensor->gpio_s0)
			| GPIO_MODE(GPIO_OUTPUT, sensor->gpio_s1)
			| GPIO_MODE(GPIO_OUTPUT, sensor->gpio_s2)
			| GPIO_MODE(GPIO_OUTPUT, sensor->gpio_s3);

	// Sets 100
	*GPIO_ODR(sensor->gpio_base) |= (1 << sensor->gpio_s0) | (1 << sensor->gpio_s1);
	// Configures the timer
	*TIM_CCMR1(sensor->timer_base) = TIM_CCMR1_CC1S(0b01) | TIM_CCMR1_CC2S(0b10);
	*TIM_CCER(sensor->timer_base) = (1 << TIM_CCER_CC2P) | (1 << TIM_CCER_CC2NP);
	*TIM_SMCR(sensor->timer_base) = TIM_SMCR_TS(TIM_SMCR_TS_TI1FP1) | TIM_SMCR_SMS(TIM_SMCR_SMS_RESET_MODE);
	*TIM_CCER(sensor->timer_base) |= (1 << TIM_CCER_CC1E) | (1 << TIM_CCER_CC2E);
}

void tsc3200_select(tsc3200_t *sensor, tsc3200_diode_t diode)
{
	*GPIO_ODR(sensor->gpio_base) &= ~(1 << sensor->gpio_s2)
			| ( 1 << sensor->gpio_s3);

	switch (diode)
	{
		case TSC3200_DIODE_RED: break;
		case TSC3200_DIODE_BLUE:
			*GPIO_ODR(sensor->gpio_base) |= (1 << sensor->gpio_s3);
			break;
		case TSC3200_DIODE_CLEAR:
			*GPIO_ODR(sensor->gpio_base) |= (1 << sensor->gpio_s2);
			break;
		case TSC3200_DIODE_GREEN:
			*GPIO_ODR(sensor->gpio_base) |= (1 << sensor->gpio_s2)
				| (1 << sensor->gpio_s3);
			break;
		default: break;
	}
}

uint16_t tsc3200_read_freq(tsc3200_t *sensor)
{
	*GPIO_ODR(sensor->gpio_base) &= ~(1 << sensor->gpio_oe);

	// Waits an short amount, and reads the width
	delay_ms(1);
	uint16_t res = *TIM_CCR1(sensor->timer_base);\

	//
	*GPIO_ODR(sensor->gpio_base) |= (1 << sensor->gpio_oe);

	return res;
}

tsc3200_ret_t tsc3200_read(tsc3200_t *sensor)
{
	tsc3200_ret_t res;

	tsc3200_select(sensor, TSC3200_DIODE_RED);
	res.r = tsc3200_read_freq(sensor);

	tsc3200_select(sensor, TSC3200_DIODE_BLUE);
	res.b = tsc3200_read_freq(sensor);

	tsc3200_select(sensor, TSC3200_DIODE_CLEAR);
	res.a = tsc3200_read_freq(sensor);

	tsc3200_select(sensor, TSC3200_DIODE_GREEN);
	res.g = tsc3200_read_freq(sensor);

	return res;
}
