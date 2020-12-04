#include "main.h"

/***********************************
 * Global variables
 ***********************************/

static stepper_t stepper0 = {
		.mf = STEPPER0_MF,
		.pu = STEPPER0_PU,
		.dir = STEPPER0_DIR,
		.base = STEPPER0_BASE,
		.timer_base = TIM2_BASE,
		.min_sps = NEMA17_MIN_SPS,
		.max_sps = NEMA17_MAX_SPS,
		.timing = {
				.presc = (APB1_SPEED / 100000)
		}
};

static stepper_t stepper1 = {
		.mf = STEPPER1_MF,
		.pu = STEPPER1_PU,
		.dir = STEPPER1_DIR,
		.base = STEPPER1_BASE,
		.timer_base = TIM3_BASE,
		.min_sps = NEMA17_MIN_SPS,
		.max_sps = NEMA17_MAX_SPS,
		.timing = {
				.presc = (APB1_SPEED / 100000)
		}
};

static stepper_t stepper2 = {
		.mf = STEPPER2_MF,
		.pu = STEPPER2_PU,
		.dir = STEPPER2_DIR,
		.base = STEPPER2_BASE,
		.timer_base = TIM4_BASE,
		.min_sps = NEMA17_MIN_SPS,
		.max_sps = NEMA17_MAX_SPS,
		.timing = {
				.presc = (APB1_SPEED / 100000)
		}
};

static stepper_t stepper3 = {
		.mf = STEPPER3_MF,
		.pu = STEPPER3_PU,
		.dir = STEPPER3_DIR,
		.base = STEPPER3_BASE,
		.timer_base = TIM5_BASE,
		.min_sps = NEMA17_MIN_SPS,
		.max_sps = NEMA17_MAX_SPS,
		.timing = {
				.presc = (APB1_SPEED / 100000)
		}
};

/***********************************
 * Stepper ISR's
 ***********************************/

STEPPER_TIMER_ISR(TIM2_IRQHandler, stepper0);
STEPPER_TIMER_ISR(TIM3_IRQHandler, stepper1);
STEPPER_TIMER_ISR(TIM4_IRQHandler, stepper2);
STEPPER_TIMER_ISR(TIM5_IRQHandler, stepper3);

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

	// Set the power enable clock and voltage regulator
	*RCC_APB1ENR |= (1 << RCC_APB1ENR_PWREN);
	*PWR_CR |= PWR_CR_VOS(PWR_CR_VOS_SCALEM1);

	// Configure flash
	*FLASH_ACR = (1 << FLASH_ACR_DCEN) | (1 << FLASH_ACR_ICEN) | (1 << FLASH_ACR_PRFTEN);
	*FLASH_ACR |= FLASH_ACR_LATENCY(5);

	// Configures HCLK, PCLK1, PCLK2
	*RCC_CFGR |= RCC_CFGR_HPRE(RCC_CFGR_HPRE_NODIV);	// HCLK 180Mhz
	*RCC_CFGR |= RCC_CFGR_PPRE1(RCC_CFGR_PPRE1_DIV4);	// PCLK1 45Mhz
	*RCC_CFGR |= RCC_CFGR_PPRE2(RCC_CFGR_PPRE2_DIV2);	// PCLK2 90Mhz

	// Configures the main PLL
	*RCC_PLLCFGR = RCC_PLLCFGR_PLLN(180) |
			RCC_PLLCFGR_PLLP(RCC_PLLCFGR_PLLP_2) |
			RCC_PLLCFGR_PLLR(2) |
			RCC_PLLCFGR_PLLM(4) |
			(1 << RCC_PLLCFGR_PLLSRC);

	// Enable PLL
	*RCC_CR	|= (1 << RCC_CR_PLLON);
	while (!(*RCC_CR & (1 << RCC_CR_PLLRDY)));

	// Use PLL as clock source
	*RCC_CFGR |= RCC_CFGR_SW(RCC_CFGR_SW_PLL_P);
	while ((*RCC_CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS(RCC_CFGR_SWS_PLL));

	// Sets the CLOCK Ready status LED
	*GPIO_ODR(STATUS_BASE) |= (1 << STATUS_CLKREADY);
}

/**
 * Initializes the GPIO for status
 */
void status_init(void)
{
	*GPIO_MODER(STATUS_BASE) &= ~GPIO_MODE_RESET(STATUS_CLKREADY);
	*GPIO_MODER(STATUS_BASE) |= GPIO_MODE(GPIO_OUTPUT, STATUS_CLKREADY);
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
}

/**
 * Gets called by startup code
 */
int main(void)
{
	// RCC: Source clock for GPIOB
	*RCC_AHB1ENR |= (1 << GPIOBEN) | (1 << GPIOCEN);

	/* Performs the clock setup and enables the stepper motors */
	status_init();
	setup_clock();
	steppers_init();

    /* Loop forever */
	for(;;);
}
