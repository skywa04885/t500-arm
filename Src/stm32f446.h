#ifndef _STM32F446_H
#define _STM32F446_H

/***********************************
 * USART
 ***********************************/

#define USART2_BASE						0x40004400

/* USART Offsets */

#define USART_SR(A)						((uint32_t *) (A + 0x00))
#define USART_DR(A)						((uint32_t *) (A + 0x04))
#define USART_BRR(A)						((uint32_t *) (A + 0x08))
#define USART_CR1(A)						((uint32_t *) (A + 0x0C))
#define USART_CR2(A)						((uint32_t *) (A + 0x10))
#define USART_CR3(A)						((uint32_t *) (A + 0x14))
#define USART_GTPR(A)					((uint32_t *) (A + 0x18))

/* USART_SR */

#define USART_SR_TXE						7
#define USART_SR_TC						6
#define USART_SR_RXNE					5

/* USART_CR1 */

#define USART_CR1_RE						2
#define USART_CR1_TE						3
#define USART_CR1_UE						13

/***********************************
 * NVIC
 ***********************************/

typedef struct __attribute__ (( packed ))
{
	uint32_t iser0;
	uint32_t iser1;
	uint32_t iser2;
	uint32_t iser3;
	uint32_t iser4;
	uint32_t iser5;
	uint32_t iser6;
	uint32_t iser7;
} nvic_iser_t;

#define NVIC_ISER						((nvic_iser_t *) 0xE000E100)

typedef struct __attribute__ (( packed ))
{
	uint32_t icpr0;
	uint32_t icpr1;
	uint32_t icpr2;
	uint32_t icpr3;
	uint32_t icpr4;
	uint32_t icpr5;
	uint32_t icpr6;
	uint32_t icpr7;
} nvic_icpr_t;

#define NVIC_ICPR						((nvic_icpr_t *) 0xE000E280)

#define NVIC_ISER0_TIM2					28
#define NVIC_ISER0_TIM3					29
#define NVIC_ISER0_TIM4					30

/***********************************
 * Flash
 ***********************************/

#define FLASH_BASE				0x40023C00
#define FLASH_ACR				((uint32_t *) (FLASH_BASE + 0x00))

/* Flash ACR */
#define FLASH_ACR_DCEN			10
#define FLASH_ACR_ICEN			9
#define FLASH_ACR_PRFTEN			8

#define FLASH_LATENCY(A)			(A << 0)

/***********************************
 * PWR
 ***********************************/

#define PWR_BASE					0x40007000

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
#define RCC_APB2ENR				((uint32_t *) (RCC_BASE + 0x44))

/* RCC_PLLCFGR */

#define RCC_PLLCFGR_PLLR(A)		(A << 28)
#define RCC_PLLCFGR_PLLR_MASK	RCC_PLLCFGR_PLLR(0b111)

#define RCC_PLLCFGR_PLLQ(A)		(A << 24)
#define RCC_PLLCFGR_PLLQ_MASK	RCC_PLLCFGR_PLLQ(0b1111)

#define RCC_PLLCFGR_PLLSRC		22

#define RCC_PLLCFGR_PLLP(A)		(A << 16)
#define RCC_PLLCFGR_PLLP_MASK	RCC_PLLCFGR_PLLP(0b11)
#define RCC_PLLCFGR_PLLP_DIV2	0b00
#define RCC_PLLCFGR_PLLP_DIV4	0b01
#define RCC_PLLCFGR_PLLP_DIV6	0b10
#define RCC_PLLCFGR_PLLP_DIV8	0b11

#define RCC_PLLCFGR_PLLN(A)		(A << 6)
#define RCC_PLLCFGR_PLLN_MASK	RCC_PLLCFGR_PLLN(0b111111111)

#define RCC_PLLCFGR_PLLM(A)		(A << 0)
#define RCC_PLLCFGR_PLLM_MASK	RCC_PLLCFGR_PLLM(0b111111)

/* RCC_CR */

#define RCC_CR_PLLRDY			25
#define RCC_CR_PLLON				24
#define RCC_CR_HSEON				16
#define RCC_CR_HSERDY			17

/* RCC_CFGR */

#define RCC_CFGR_HPRE(A)			(A << 4)
#define RCC_CFGR_HPRE_MASK		RCC_CFGR_HPRE(0b1111)
#define RCC_CFGR_HPRE_NDIV		0b0000
#define RCC_CFGR_HPRE_DIV2		0b1000
#define RCC_CFGR_HPRE_DIV4		0b1001
#define RCC_CFGR_HPRE_DIV8		0b1010
#define RCC_CFGR_HPRE_DIV16		0b1011
#define RCC_CFGR_HPRE_DIV64		0b1100
#define RCC_CFGR_HPRE_DIV128		0b1101
#define RCC_CFGR_HPRE_DIV256		0b1110
#define RCC_CFGR_HPRE_DIV512		0b1111

#define RCC_CFGR_PPRE2(A)		(A << 13)
#define RCC_CFGR_PPRE2_MASK		RCC_CFGR_PPRE2(0b111)
#define RCC_CFGR_PPRE2_NDIV		0b000
#define RCC_CFGR_PPRE2_DIV2		0b100
#define RCC_CFGR_PPRE2_DIV4		0b101
#define RCC_CFGR_PPRE2_DIV8		0b110
#define RCC_CFGR_PPRE2_DIV16		0b111

#define RCC_CFGR_PPRE1(A)		(A << 10)
#define RCC_CFGR_PPRE1_MASK		RCC_CFGR_PPRE1(0b111)
#define RCC_CFGR_PPRE1_NDIV		0b000
#define RCC_CFGR_PPRE1_DIV2		0b100
#define RCC_CFGR_PPRE1_DIV4		0b101
#define RCC_CFGR_PPRE1_DIV8		0b110
#define RCC_CFGR_PPRE1_DIV16		0b111

#define RCC_CFGR_SW(A)			(A << 0)
#define RCC_CFGR_SW_MASK			RCC_CFGR_SW(0b11)
#define RCC_CFGR_SW_HSI			0b00
#define RCC_CFGR_SW_HSE			0b01
#define RCC_CFGR_SW_PLL_P		0b10
#define RCC_CFGR_SW_PLL_R		0b11

#define RCC_CFGR_SWS(A)			(A << 2)
#define RCC_CFGR_SWS_MASK		RCC_CFGR_SWS(0b11)
#define RCC_CFGR_SWS_HSI			0b00
#define RCC_CFGR_SWS_HSE			0b01
#define RCC_CFGR_SWS_PLL			0b10
#define RCC_CFGR_SWS_PLL_R		0b11

/* RCC_APB1ENR */

#define RCC_APB1ENR_TIM14EN		8
#define RCC_APB1ENR_TIM13EN		7
#define RCC_APB1ENR_TIM12EN		6
#define RCC_APB1ENR_TIM7EN		5
#define RCC_APB1ENR_TIM6EN		4
#define RCC_APB1ENR_TIM5EN		3
#define RCC_APB1ENR_TIM4EN		2
#define RCC_APB1ENR_TIM3EN		1
#define RCC_APB1ENR_TIM2EN		0
#define RCC_APB1ENR_PWREN		28
#define RCC_APB1END_USART2EN		17

/* RCC_AHB1ENR Bits */

#define GPIOAEN					0
#define GPIOBEN					1
#define GPIOCEN					2
#define GPIODEN					3
#define GPIOEEN					4
#define GPIOFEN					5
#define GPIOGEN					6
#define GPIOHEN					7

/* RCC_APB2ENR Bits */

#define RCC_APB2ENR_TIM1EN		0
#define RCC_APB2ENR_TIM8EN		1

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
#define GPIO_AF(A, B)			(A << (4 * B))

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

#define TIM_CR1(A)				((uint16_t *) (A + 0x00))
#define TIM_CR2(A)				((uint16_t *) (A + 0x04))
#define TIM_SMCR(A)				((uint16_t *) (A + 0x08))
#define TIM_DIER(A)				((uint16_t *) (A + 0x0C))
#define TIM_SR(A)				((uint16_t *) (A + 0x10))
#define TIM_EGR(A)				((uint16_t *) (A + 0x14))
#define TIM_CCMR1(A)				((uint16_t *) (A + 0x18))
#define TIM_CCMR2(A)				((uint16_t *) (A + 0x1C))
#define TIM_CCER(A)				((uint16_t *) (A + 0x20))
#define TIM_CNT(A)				((uint32_t *) (A + 0x24))
#define TIM_PSC(A)				((uint16_t *) (A + 0x28))
#define TIM_ARR(A)				((uint32_t *) (A + 0x2C))
#define TIM_RCR(A)				((uint16_t *) (A + 0x30))
#define TIM_CCR1(A)				((uint32_t *) (A + 0x34))
#define TIM_CCR2(A)				((uint32_t *) (A + 0x38))
#define TIM_CCR3(A)				((uint32_t *) (A + 0x3C))
#define TIM_CCR4(A)				((uint32_t *) (A + 0x40))
#define TIM_BDTR(A)				((uint16_t *) (A + 0x44))
#define TIM_DCR(A)				((uint16_t *) (A + 0x48))
#define TIM_DMAR(A)				((uint16_t *) (A + 0x4C))

/* TIM_SMCR */
#define TIM_SMCR_SMS(A)			(A << 0)
#define TIM_SMCR_SMS_RESET_MODE	0b100

#define TIM_SMCR_TS(A)			(A << 8)
#define TIM_SMCR_TS_TI1FP1		0b101

/* TIM_CCER */

#define TIM_CCER_CC2P			5
#define TIM_CCER_CC2NP			7
#define TIM_CCER_CC1E			0
#define TIM_CCER_CC2E			4

/* TIM_SR */

#define TIM_SR_CC1IF				1
#define TIM_SR_UIF				0

/* TIM_CCMR1 */

#define TIM_CCMR1_OC1FE			2
#define TIM_CCMR1_OC1PE			3
#define TIM_CCMR1_OC1CE			7

#define TIM_CCMR1_OC2FE			10
#define TIM_CCMR1_OC2PE			11
#define TIM_CCMR1_OC2CE			15

#define TIM_CCMR1_OC1M(A)		(A << 4)
#define TIM_CCMR1_OC1M_MASK		TIM_CCMR1_OC1M(0b111)
#define TIM_CCMR1_OC1M_FROZEN	0b000
#define TIM_CCMR1_OC1M_ACTIVE	0b101

#define TIM_CCMR1_CC1S(A)		(A << 0)
#define TIM_CCMR1_CC2S(A)		(A << 8)

#define TIM_CCMR1_CC1S_OUTPUT	0b00
#define TIM_CCMR1_CC1S_TI1		0b01
#define TIM_CCMR1_CC1S_TI2		0b10

#define TIM_CCMR1_IC1F(A)		(A << 0)

/* TIM1_CR1 */

#define TIM_CR1_ARPE				7
#define TIM_CR1_CEN				0
#define TIM_CR1_UDIS				1
#define TIM_CR1_DIR				4
#define TIM_CR1_OPM				3

/* TIM_DIER */

#define TIM_DIER_CC1IE			1
#define TIM_DIER_UIE				0

#endif
