#ifndef _STM32F446_H
#define _STM32F446_H

/***********************************
 * Flash
 ***********************************/

#define FLASH_BASE				0x40023C00

/* Flash Offsets */

#define FLASH_ACR				((uint32_t *) (FLASH_BASE + 0x00))

/* FLASH_ACR */

#define FLASH_ACR_DCEN			10
#define FLASH_ACR_ICEN			9
#define FLASH_ACR_PRFTEN			8
#define FLASH_ACR_LATENCY(A)		(A << 0)

/***********************************
 * PWR
 ***********************************/

#define PWR_BASE					0x40007000

/* PWR Offsets */

#define PWR_CR					((uint32_t *) (RCC_BASE + 0x00))

/* PWR_CR */

#define PWR_CR_VOS(A)			(A << 14)
#define PWR_CR_VOS_SCALEM3		0b01
#define PWR_CR_VOS_SCALEM2		0b10
#define PWR_CR_VOS_SCALEM1		0b11

/***********************************
 * RCC
 ***********************************/

#define RCC_BASE					0x40023800

/* RCC Offsets */

#define RCC_CR					((uint32_t *) (RCC_BASE + 0x00))
#define RCC_PLLCFGR				((uint32_t *) (RCC_BASE + 0x04))
#define RCC_CFGR					((uint32_t *) (RCC_BASE + 0x08))
#define RCC_AHB1ENR				((uint32_t *) (RCC_BASE + 0x30))
#define RCC_APB1ENR				((uint32_t *) (RCC_BASE + 0x40))

/* RCC_PLLCFGR */

#define RCC_PLLCFGR_PLLQ(A)		(A << 24)

#define RCC_PLLCFGR_PLLP(A)		(A << 16)
#define RCC_PLLCFGR_PLLP_2		0b00
#define RCC_PLLCFGR_PLLP_4		0b01
#define RCC_PLLCFGR_PLLP_6		0b10
#define RCC_PLLCFGR_PLLP_8		0b11

#define RCC_PLLCFGR_PLLN(A)		(A << 6)

#define RCC_PLLCFGR_PLLR(A)		(A << 28)

#define RCC_PLLCFGR_PLLM(A)		(A << 0)

#define RCC_PLLCFGR_PLLSRC		22

/* RCC_CFGR */

#define RCC_CFGR_SW(A)			(A << 0)
#define RCC_CFGR_SW_PLL_P		0b10

#define RCC_CFGR_SWS(A)			(A << 2)
#define RCC_CFGR_SWS_MASK		(0b11 << 2)
#define RCC_CFGR_SWS_PLL			0b10

#define RCC_CFGR_PPRE1(A)		(A << 10)
#define RCC_CFGR_PPRE1_NODIV		0b000
#define RCC_CFGR_PPRE1_DIV2		0b100
#define RCC_CFGR_PPRE1_DIV4		0b101
#define RCC_CFGR_PPRE1_DIV8		0b110
#define RCC_CFGR_PPRE1_DIV16		0b111

#define RCC_CFGR_PPRE2(A)		(A << 10)
#define RCC_CFGR_PPRE2_NODIV		0b000
#define RCC_CFGR_PPRE2_DIV2		0b100
#define RCC_CFGR_PPRE2_DIV4		0b101
#define RCC_CFGR_PPRE2_DIV8		0b110
#define RCC_CFGR_PPRE2_DIV16		0b111

#define RCC_CFGR_HPRE(A)			(A << 4)
#define RCC_CFGR_HPRE_NODIV		0b0000
#define RCC_CFGR_HPRE_DIV2		0b1000
#define RCC_CFGR_HPRE_DIV4		0b1001
#define RCC_CFGR_HPRE_DIV8		0b1010
#define RCC_CFGR_HPRE_DIV16		0b1011
#define RCC_CFGR_HPRE_DIV64		0b1100
#define RCC_CFGR_HPRE_DIV128		0b1101
#define RCC_CFGR_HPRE_DIV256		0b1110
#define RCC_CFGR_HPRE_DIV512		0b1111

/* RCC_RC */

#define RCC_CR_HSEON				16
#define RCC_CR_HSERDY			17
#define RCC_CR_PLLON				24
#define RCC_CR_PLLRDY			25

/* RCC_APB1ENR */

#define RCC_APB1ENR_PWREN		28

/* RCC_AHB1ENR Bits */

#define GPIOAEN					0
#define GPIOBEN					1
#define GPIOCEN					2
#define GPIODEN					3
#define GPIOEEN					4
#define GPIOFEN					5
#define GPIOGEN					6
#define GPIOHEN					7

/***********************************
 * GPIO
 ***********************************/

/* Base Addresses */

#define GPIOA_BASE				0x40020000
#define GPIOB_BASE				0x40020400
#define GPIOC_BASE				0x40020800
#define GPIOD_BASE				0X40020C00

/* GPIO Offsets */

#define GPIO_MODER(A)			((uint32_t *) (A + 0x00))
#define GPIO_OTYPER(A)			((uint32_t *) (A + 0x04))
#define GPIO_OSPEEDR(A)			((uint32_t *) (A + 0x08))
#define GPIO_PUPDR(A)			((uint32_t *) (A + 0x0C))
#define GPIO_IDR(A)				((uint32_t *) (A + 0x10))
#define GPIO_ODR(A)				((uint32_t *) (A + 0x14))
#define GPIO_BSRR(A)				((uint32_t *) (A + 0x18))
#define GPIO_LCKR(A)				((uint32_t *) (A + 0x1C))
#define GPIO_AFRL(A)				((uint32_t *) (A + 0x20))
#define GPIO_AFRH(A)				((uint32_t *) (A + 0x24))

/* GPIO Modes */

#define GPIO_INPUT				0b00
#define GPIO_OUTPUT				0b01
#define GPIO_ALTERNATIVE_FUNC	0b10
#define GPIO_ANALOG_MODE			0b11
#define GPIO_MODE(A, B)			(A << (2 * B))
#define GPIO_MODE_RESET(A)		(~(0b11 << (2 * A)))

/* GPIO Speeds */

#define GPIO_LOW_SPEED			0b00
#define GPIO_MED_SPEED			0b01
#define GPIO_FAST_SPEED			0b10
#define GPIO_HIGH_SPEED			0b11
#define GPIO_SPEED(A, B)			(A << (2 * B))

/* GPIO Pull up / Pull down */

#define GPIO_NO_PUP_NO_PDOWN		0b00
#define GPIO_PULL_UP				0b01
#define GPIO_PULL_DOWN			0b10
#define GPIO_PULL(A, B)			(A << (2 * B))

/* GPIO Alternative Functions */

#define GPIO_AF0					0b0000
#define GPIO_AF1					0b0001
#define GPIO_AF2					0b0010
#define GPIO_AF3					0b0011
#define GPIO_AF4					0b0100
#define GPIO_AF5					0b0101
#define GPIO_AF6					0b0110
#define GPIO_AF7					0b0111
#define GPIO_AF8					0b1000
#define GPIO_AF9					0b1001
#define GPIO_AF10				0b1010
#define GPIO_AF11				0b1011
#define GPIO_AF12				0b1100
#define GPIO_AF13				0b1101
#define GPIO_AF14				0b1110
#define GPIO_AF15				0b1111
#define GPIO_AF(A)				(A << (4 * B))

/***********************************
 * Timers
 ***********************************/

#define TIM1_BASE				0x40010000
#define TIM2_BASE				0x40000000
#define TIM3_BASE				0x40000400
#define TIM4_BASE				0x40000800
#define TIM5_BASE				0x40000C00
#define TIM6_BASE				0x40001000
#define TIM7_BASE				0x40001400
#define TIM8_BASE				0x40010400

#define TIM9_BASE				0x40014000
#define TIM10_BASE				0x40014400
#define TIM11_BASE				0x40014800
#define TIM12_BASE				0x40001800
#define TIM13_BASE				0x40001C00
#define TIM14_BASE				0x40002000

/* Timer offsets */

#define TIM_CR1(A)				((uint32_t *) (A + 0x00))
#define TIM_CR2(A)				((uint32_t *) (A + 0x04))
#define TIM_SMCR(A)				((uint32_t *) (A + 0x08))
#define TIM_DIER(A)				((uint32_t *) (A + 0x0C))
#define TIM_SR(A)				((uint32_t *) (A + 0x10))
#define TIM_EGR(A)				((uint32_t *) (A + 0x14))
#define TIM_CCMR1(A)				((uint32_t *) (A + 0x18))
#define TIM_CCMR2(A)				((uint32_t *) (A + 0x1C))
#define TIM_CCER(A)				((uint32_t *) (A + 0x20))
#define TIM_CNT(A)				((uint32_t *) (A + 0x24))
#define TIM_PSC(A)				((uint32_t *) (A + 0x28))
#define TIM_ARR(A)				((uint32_t *) (A + 0x2C))
#define TIM_RCR(A)				((uint32_t *) (A + 0x30))
#define TIM_CCR1(A)				((uint32_t *) (A + 0x34))
#define TIM_CCR2(A)				((uint32_t *) (A + 0x38))
#define TIM_CCR3(A)				((uint32_t *) (A + 0x3C))
#define TIM_CCR4(A)				((uint32_t *) (A + 0x40))
#define TIM_BDTR(A)				((uint32_t *) (A + 0x44))
#define TIM_DCR(A)				((uint32_t *) (A + 0x48))
#define TIM_DMAR(A)				((uint32_t *) (A + 0x4C))

/* TIM1_CR1 */

#define TIM_CR1_ARPE				7
#define TIM_CR1_CEN				0

#endif
