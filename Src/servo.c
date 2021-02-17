#include "servo.h"

void servo_init(void)
{

	//
	// Configures the GPIO for TIM10
	//

	// Enables clock for GPIOB
	*RCC_AHB1ENR |= (_BV(RCC_AHB1ENR_GPIOBEN));

	// Sets the PB8 mode to output
	*GPIO_MODER(GPIOB_BASE) &= ~GPIO_MODE(8, GPIO_MODE_RESET);
	*GPIO_MODER(GPIOB_BASE) |= GPIO_MODE(8, GPIO_MODE_OUTPUT);

	//
	// Configures the PWM for the Servo
	//

	// Enables TIM10 in RCC
	*RCC_APB2ENR |= (_BV(RCC_APB2ENR_TIM10EN));

	// Pulse Width Modulation mode allows to generate a signal with a frequency determined by
	//  the value of the TIMx_ARR register and a duty cycle determined by the value of the
	//  TIMx_CCRx register.

	// (90000000 / ((45000 + 1) * (39 + 1))) = 49.99 Hz
	*TIM_PSC(TIM10_BASE) = 45000;
	*TIM_ARR(TIM10_BASE) = 39;
	*TIM_CCR1(TIM10_BASE) = 3;

	// The PWM mode can be selected independently on each channel (one PWM per OCx
	//  output) by writing ‘110’ (PWM mode 1) or ‘111’ (PWM mode 2) in the OCxM bits in the
	//  TIMx_CCMRx register. The corresponding preload register must be enabled by setting the
	//  OCxPE bit in the TIMx_CCMRx register, and eventually the auto-reload preload register (in
	//  upcounting or center-aligned modes) by setting the ARPE bit in the TIMx_CR1 register.

	*TIM_CCMR1(TIM10_BASE) = (TIM_CCMR1_OC1M(0b110)		// CH1 = Active on match
			| _BV(TIM_CCMR1_OC1PE));
	*TIM_CR1(TIM10_BASE) = (_BV(TIM_CR1_ARPE));

	// As the preload registers are transferred to the shadow registers only when an update event
	//  occurs, before starting the counter, all registers must be initialized by setting the UG bit in
	//  the TIMx_EGR register.

	*TIM_EGR(TIM10_BASE) |= (_BV(TIM_EGR_UG));

	// The OCx polarity is software programmable using the CCxP bit in the TIMx_CCER register.
	//  It can be programmed as active high or active low. The OCx output is enabled by the CCxE
	//  bit in the TIMx_CCER register. Refer to the TIMx_CCERx register description for more
	//  details.

	*TIM_CCER(TIM10_BASE) &= ~(_BV(TIM_CCER_CC1P) | _BV(TIM_CCER_CC1E));

	// Enables the timer
	*TIM_CR1(TIM10_BASE) = (_BV(TIM_CR1_CEN));
}
