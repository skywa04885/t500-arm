#include "main.h"

/***********************************
 * Global variables
 ***********************************/

static stepper_t stepper0 = {
		.mf = STEPPER0_MF,
		.pu = STEPPER0_PU,
		.dir = STEPPER0_DIR,
		.gpio_base = STEPPER0_BASE,
		.position = 0,
		.timer_base = TIM2_BASE,
		.min_sps = NEMA17_MIN_SPS,
		.max_sps = NEMA17_MAX_SPS,
		.sps_inc = NEMA17_SPS_INC,
		.timing = {
				.presc = 90
		},
		.auto_enable_disable = true
};

static stepper_t stepper1 = {
		.mf = STEPPER1_MF,
		.pu = STEPPER1_PU,
		.dir = STEPPER1_DIR,
		.gpio_base = STEPPER1_BASE,
		.timer_base = TIM3_BASE,
		.position = 0,
		.min_sps = NEMA17_MIN_SPS,
		.max_sps = NEMA17_MAX_SPS,
		.sps_inc = NEMA17_SPS_INC,
		.timing = {
				.presc = 90
		},
		.auto_enable_disable = true
};

static stepper_t stepper2 = {
		.mf = STEPPER2_MF,
		.pu = STEPPER2_PU,
		.dir = STEPPER2_DIR,
		.gpio_base = STEPPER2_BASE,
		.timer_base = TIM4_BASE,
		.min_sps = NEMA17_MIN_SPS,
		.max_sps = NEMA17_MAX_SPS,
		.sps_inc = NEMA17_SPS_INC,
};

static stepper_t stepper3 = {
		.mf = STEPPER3_MF,
		.pu = STEPPER3_PU,
		.dir = STEPPER3_DIR,
		.gpio_base = STEPPER3_BASE,
		.timer_base = TIM5_BASE,
		.min_sps = NEMA17_MIN_SPS,
		.max_sps = NEMA17_MAX_SPS,
		.sps_inc = NEMA17_SPS_INC,
};

/***********************************
 * Stepper ISR's
 ***********************************/

STEPPER_ISR(TIM2_IRQHandler, stepper0);
STEPPER_ISR(TIM3_IRQHandler, stepper1);

/***********************************
 * Functions
 ***********************************/

/**
 * Configures the clock inside the STM32
 */
void setup_clock(void)
{
	   // Enables HSE and waits until it is ready
	    *RCC_CR |= (1 << RCC_CR_HSEON);
	    while (!(*RCC_CR & (1 << RCC_CR_HSERDY)));


	    // Configure flash
	    *FLASH_ACR = (1 << FLASH_ACR_DCEN) | (1 << FLASH_ACR_ICEN) | (1 << FLASH_ACR_PRFTEN);
	    *FLASH_ACR |= FLASH_LATENCY(5);

	    // Configures HCLK, PCLK1, PCLK2
	    *RCC_CFGR &= ~RCC_CFGR_HPRE_MASK;
	    *RCC_CFGR |= RCC_CFGR_HPRE(RCC_CFGR_HPRE_NDIV);    // HCLK 180Mhz

	    *RCC_CFGR &= ~RCC_CFGR_PPRE1_MASK;
	    *RCC_CFGR |= RCC_CFGR_PPRE1(RCC_CFGR_PPRE1_DIV4);   // PCLK1 45Mhz

	    *RCC_CFGR &= ~RCC_CFGR_PPRE2_MASK;
	    *RCC_CFGR |= RCC_CFGR_PPRE2(RCC_CFGR_PPRE2_DIV2);   // PCLK2 90Mhz

	    // Configures the main PLL
	    *RCC_PLLCFGR = RCC_PLLCFGR_PLLN(180) |
	            RCC_PLLCFGR_PLLP(RCC_PLLCFGR_PLLP_DIV2) |
	            RCC_PLLCFGR_PLLR(2) |
	            RCC_PLLCFGR_PLLM(4) |
	            (1 << RCC_PLLCFGR_PLLSRC);

	    // Enable PLL
	    *RCC_CR |= (1 << RCC_CR_PLLON);
	    while (!(*RCC_CR & (1 << RCC_CR_PLLRDY)));

	    // Use PLL as clock source
	    *RCC_CFGR &= ~RCC_CFGR_SW_MASK;
	    *RCC_CFGR |= RCC_CFGR_SW(RCC_CFGR_SW_PLL_P);
	    while ((*RCC_CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS(RCC_CFGR_SWS_PLL));
}

/**
 * Configures the steppers and their GPIO Ports
 */
void steppers_init(void)
{
	// Initializes the stepper motors
	stepper_init(&stepper0);
	stepper_init(&stepper1);
	stepper_init(&stepper2);
	stepper_init(&stepper3);

	// Enables the timers in the NVIC
	NVIC_ISER->iser0 |= (1 << NVIC_ISER0_TIM3) | (1 << NVIC_ISER0_TIM2);
}

stepper_t* get_stepper_from_command_motor(command_motor_t motor)
{
	stepper_t *stepper = NULL;
	switch (motor)
	{
		case COMMAND_MOTOR0: stepper = &stepper0; break;
		case COMMAND_MOTOR1: stepper = &stepper1; break;
		case COMMAND_MOTOR2: stepper = &stepper2; break;
		case COMMAND_MOTOR3: stepper = &stepper3; break;
		default: break;
	}
	return stepper;
}

/**
 * Gets called by startup code
 */
int main(void)
{
	// RCC: Source clock for GPIOB
	*RCC_AHB1ENR |= (1 << GPIOAEN) | (1 << GPIOBEN) | (1 << GPIOCEN);

	// RCC: Enable power to TIM2, TIM3, TIM1
	*RCC_APB2ENR |= (1 << RCC_APB2ENR_TIM1EN);
	*RCC_APB1ENR |= (1 << RCC_APB1ENR_TIM2EN) | (1 << RCC_APB1ENR_TIM3EN);

	// Performs the configuration
	setup_clock();
	delay_init();
	usart_init();
	steppers_init();

    /* Loop forever */
//	stepper_simple_move(&stepper0, 100000);
//	stepper_simple_move(&stepper0, 100000);
//	stepper_simple_move(&stepper0, 0);

	char buffer[128];
	for(;;)
	{
		command_read(buffer, sizeof (buffer));
		command_packet_t *packet = (command_packet_t *) buffer;

		switch (packet->hdr.type)
		{
			case COMMAND_SET_MOTOR_POS:
			{
				command_arg_set_motor_pos_t *command = (command_arg_set_motor_pos_t *) packet->body.payload;

				// Gets the stepper pointer, and sends an error, if the motor is null
				//  so not found
				stepper_t *stepper = get_stepper_from_command_motor(command->motor);
				if (stepper == NULL)
				{
					packet->hdr.flags.error = 1;
					packet->body.payload[0] = COMMAND_ERROR_MOTOR_NOT_KNOWN;
					packet->body.size = sizeof (command_arg_error_t);
					command_write(packet);
					break;
				}

				// Sets the new stepper position
				stepper_simple_move(stepper, command->pos);

				// Sends the OK response
				packet->hdr.flags.error = 0;
				packet->body.size = 0;
				command_write(packet);

				// Breaks from setpos
				break;
			}
		}
	}
}
