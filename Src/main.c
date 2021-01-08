#include "main.h"

/***********************************
 * Global variables
 ***********************************/

stepper_t stepper0 = {
		.mf = STEPPER0_MF,
		.pu = STEPPER0_PU,
		.dir = STEPPER0_DIR,
		.gpio_base = STEPPER0_BASE,
		.position = 0,
		.timer_base = TIM2_BASE,
		.min_sps = NEMA23_MIN_SPS,
		.max_sps = NEMA23_MAX_SPS,
		.sps_inc = NEMA23_SPS_INC,
		.cop = {
			.target_pos = 0
		},
		.timing = {
			.presc = 90
		},
		.auto_enable_disable = false
};

stepper_t stepper1 = {
		.mf = STEPPER1_MF,
		.pu = STEPPER1_PU,
		.dir = STEPPER1_DIR,
		.gpio_base = STEPPER1_BASE,
		.timer_base = TIM3_BASE,
		.position = 0,
		.min_sps = NEMA23_MIN_SPS,
		.max_sps = NEMA23_MAX_SPS,
		.sps_inc = NEMA23_SPS_INC,
		.cop = {
			.target_pos = 0
		},
		.timing = {
			.presc = 90
		},
		.auto_enable_disable = true
};

stepper_t stepper2 = {
		.mf = STEPPER2_MF,
		.pu = STEPPER2_PU,
		.dir = STEPPER2_DIR,
		.gpio_base = STEPPER2_BASE,
		.timer_base = TIM4_BASE,
		.position = 0,
		.min_sps = NEMA23_FAST_MIN_SPS,
		.max_sps = NEMA23_FAST_MAX_SPS,
		.sps_inc = NEMA23_FAST_SPS_INC,
		.cop = {
			.target_pos = 0
		},
		.timing = {
			.presc = 90
		},
		.auto_enable_disable = false
};

stepper_t stepper3 = {
		.mf = STEPPER3_MF,
		.pu = STEPPER3_PU,
		.dir = STEPPER3_DIR,
		.gpio_base = STEPPER3_BASE,
		.timer_base = TIM5_BASE,
		.min_sps = NEMA17_MIN_SPS,
		.max_sps = NEMA17_MAX_SPS,
		.sps_inc = NEMA17_SPS_INC,
};

const char *device_id = "LUKESTE0";

/***********************************
 * Stepper ISR's
 ***********************************/

STEPPER_ISR(TIM2_IRQHandler, stepper0);
STEPPER_ISR(TIM3_IRQHandler, stepper1);
STEPPER_ISR(TIM4_IRQHandler, stepper2);

/***********************************
 * Global Variables
 ***********************************/

u8 write_buffer[1500];
u8 read_buffer[1500];

stepper_t *stepper_motors[] = {
		&stepper0,
		&stepper1,
		&stepper2,
		&stepper3
};
u8 stepper_motors_count = sizeof (stepper_motors) / sizeof (stepper_t *);

extern enc28j60_fifo_t __system_fifo;

/***********************************
 * Functions
 ***********************************/

/**
 * Configures the steppers and their GPIO Ports
 */
void steppers_init(void)
{
	// Enables the clock for the stepper GPIO's, and the timers
	*RCC_APB1ENR |= (_BV(RCC_APB1ENR_TIM2EN) | _BV(RCC_APB1ENR_TIM3EN) | _BV(RCC_APB1ENR_TIM4EN));
	*RCC_AHB1ENR |= (_BV(RCC_AHB1ENR_GPIOBEN));

	// Initializes the stepper motors
	stepper_init(&stepper0);
	stepper_init(&stepper1);
	stepper_init(&stepper2);
	stepper_init(&stepper3);

	// Enables the timers in the NVIC
	NVIC_ISER->iser0 |= (_BV(NVIC_ISER0_TIM3) | _BV(NVIC_ISER0_TIM2) | _BV(NVIC_ISER0_TIM4));
}

/**
 * Gets called by startup code
 */
int main(void)
{
	// Performs the configuration
	clock_init();
	delay_init();
	usart2_init(500000);
	manager_init();

//	steppers_init();
//	stepper_enable(&stepper0);

	/*
	// Servo Test

	// Enables TIM10 in RCC
	*RCC_APB2ENR |= (_BV(RCC_APB2ENR_TIM10EN));

	// Pulse Width Modulation mode allows to generate a signal with a frequency determined by
	//  the value of the TIMx_ARR register and a duty cycle determined by the value of the
	//  TIMx_CCRx register.

	*TIM_ARR(TIM10_BASE) = 180;
	*TIM_CCR1(TIM10_BASE) = 0;

	// The PWM mode can be selected independently on each channel (one PWM per OCx
	//  output) by writing ‘110’ (PWM mode 1) or ‘111’ (PWM mode 2) in the OCxM bits in the
	//  TIMx_CCMRx register. The corresponding preload register must be enabled by setting the
	//  OCxPE bit in the TIMx_CCMRx register, and eventually the auto-reload preload register (in
	//  upcounting or center-aligned modes) by setting the ARPE bit in the TIMx_CR1 register.

	*TIM_CCMR1(TIM10_BASE) = (TIM_CCMR1_OC1M(0b001)		// CH1 = Active on match
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

	*TIM_CCER &= ~(_BV(TIM_CCER_CC1P)); 		// OC1 Active High

	for (;;);
	*/


	// Prints the MAC address
	for (;;)
	{
		manager_poll();
	}
}
