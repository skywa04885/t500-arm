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
		.timing = {
				.presc = 90
		},
		.auto_enable_disable = true
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
		.min_sps = NEMA17_MIN_SPS,
		.max_sps = NEMA17_MAX_SPS,
		.sps_inc = NEMA17_SPS_INC,
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
STEPPER_ISR(TIM4_IRQHandler, stepper1);

/***********************************
 * Global Variables
 ***********************************/

u8 buffer[2048];
u8 write_buffer[2048];

stepper_t *stepper_motors[] = {
		&stepper0,
		&stepper1,
		&stepper2,
		&stepper3
};
u8 stepper_motors_count = sizeof (stepper_motors) / sizeof (stepper_t *);

/***********************************
 * Functions
 ***********************************/

/**
 * Configures the steppers and their GPIO Ports
 */
void steppers_init(void)
{
	// Enables the clock for the stepper GPIO's, and the timers
	*RCC_APB1ENR |= (_BV(RCC_APB1ENR_TIM2EN) | _BV(RCC_APB1ENR_TIM3EN));
	*RCC_AHB1ENR |= (_BV(RCC_AHB1ENR_GPIOBEN));

	// Initializes the stepper motors
	stepper_init(&stepper0);
	stepper_init(&stepper1);
	stepper_init(&stepper2);
	stepper_init(&stepper3);

	// Enables the timers in the NVIC
	NVIC_ISER->iser0 |= (1 << NVIC_ISER0_TIM3) | (1 << NVIC_ISER0_TIM2) | (1 << NVIC_ISER0_TIM4);
}

/**
 * Gets called by startup code
 */
int main(void)
{
	// Performs the configuration
	clock_init();
	usart2_init(500000);
	delay_init();
	steppers_init();
	manager_init();

	// Prints the MAC address
	for (;;)
	{
		manager_poll();
	}
}
