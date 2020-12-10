#include "main.h"

/***********************************
 * Global variables
 ***********************************/

static tsc3200_t csensor0 = {
		.gpio_base = GPIOA_BASE, .timer_base = TIM1_BASE,
		.gpio_s3 = 13, .gpio_s2 = 14, .gpio_s1 = 5, .gpio_s0 = 6,
		.gpio_oe = 7, .gpio_out = 8
};

static stepper_t stepper0 = {
		.mf = STEPPER0_MF,
		.pu = STEPPER0_PU,
		.dir = STEPPER0_DIR,
		.gpio_base = STEPPER0_BASE,
		.position = 0,
		.timer_base = TIM2_BASE,
		.min_sps = NEMA17_MIN_SPS,
		.max_sps = 10000,
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
		.max_sps = 15000,
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

static const char *device_id = "LUKESTE0";

static stepper_t *motors[] = {
		&stepper0,
		&stepper1,
		&stepper2,
		&stepper3
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
	NVIC_ISER->iser0 |= (1 << NVIC_ISER0_TIM3) | (1 << NVIC_ISER0_TIM2) | (1 << NVIC_ISER0_TIM4);
}

/**
 * Gets called by startup code
 */
int main(void)
{
	*GPIO_MODER(GPIOA_BASE) |= GPIO_MODE(GPIO_ALTERNATIVE_FUNC, 8);
	*GPIO_AFRH(GPIOA_BASE) |= GPIO_AF(GPIO_AF1, 0);

	// RCC: Source clock for GPIOB
	*RCC_AHB1ENR |= (1 << GPIOAEN) | (1 << GPIOBEN) | (1 << GPIOCEN);

	// RCC: Enable power to TIM2, TIM3, TIM1
	*RCC_APB2ENR |= (1 << RCC_APB2ENR_TIM1EN);
	*RCC_APB1ENR |= (1 << RCC_APB1ENR_TIM2EN) | (1 << RCC_APB1ENR_TIM3EN) | (1 << RCC_APB1ENR_TIM4EN);

	// Performs the configuration
	setup_clock();
	delay_init();
	usart_init();
	steppers_init();
	tsc3200_init(&csensor0);

	for (;;)
	{
		tsc3200_ret_t res = tsc3200_read(&csensor0);
		printf("R: %u, G: %u, B: %u, A: %u\r\n", res.r, res.g, res.b, res.a);
	}

    /* Loop forever */
	stepper_simple_move(&stepper2, 100000);
	stepper_simple_move(&stepper0, 0);

	char buffer[256];
	for(;;)
	{
		command_read(buffer, sizeof (buffer));
		command_packet_t *packet = (command_packet_t *) buffer;

		switch (packet->hdr.type)
		{
			case COMMAND_READ_MOTOR_STATUS:
			{
				uint8_t i = packet->body.payload[0];
				if (i > sizeof (motors) / sizeof (stepper_t *))
				{
					packet->hdr.flags.error = 1;
					packet->body.payload[0] = COMMAND_ERROR_MOTOR_NOT_KNOWN;
					packet->body.size = sizeof (command_arg_error_t);
					command_write(packet);
					break;
				}

				packet->hdr.flags.error = 0;
				packet->body.size = 123;
				command_response_motor_status_t *status = (command_response_motor_status_t *) packet->body.payload;

				status->moving = motors[i]->stepper_moving;
				status->enabled = motors[i]->stepper_enabled;
				status->current_sps = motors[i]->cop.current_sps;
				status->pos = motors[i]->position;

				command_write(packet);
				break;
			}
			case COMMAND_READ_INFO:
			{
				// Sets error to zero, and sets the new size
				packet->hdr.flags.error = 0;
				packet->body.size = sizeof (command_arg_info_t);

				// Creates the info, and adds the motor count, and device id
				command_arg_info_t *info = (command_arg_info_t *) packet->body.payload;
				info->motor_count = sizeof (motors) / sizeof (stepper_t *);
				for (uint8_t i = 0; i < 8; ++i)
					info->device_id[i] = device_id[i];

				command_write(packet);
				break;
			}
			case COMMAND_SET_MOTOR_POS:
			{
				command_arg_set_motor_pos_t *command = (command_arg_set_motor_pos_t *) packet->body.payload;
				if (command->motor > sizeof (motors) / sizeof (stepper_t *))
				{

					packet->hdr.flags.error = 1;
					packet->body.payload[0] = COMMAND_ERROR_MOTOR_NOT_KNOWN;
					packet->body.size = sizeof (command_arg_error_t);
					command_write(packet);
					break;
				}

				// Sets the new stepper position
				stepper_simple_move(motors[command->motor], command->pos);

				// Sends the OK response
				packet->hdr.flags.error = 0;
				packet->body.size = 0;
				command_write(packet);

				// Breaks from setpos
				break;
			}
			default:
			{
				packet->hdr.flags.error = 1;
				packet->body.payload[0] = COMMAND_ERROR_NOT_KNOWN;
				packet->body.size = sizeof (command_arg_error_t);
				command_write(packet);
			}
		}
	}
}
