#ifndef _STM32F446_H
#define _STM32F446_H

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

#define NVIC_ISER						((volatile nvic_iser_t *) 0xE000E100)

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

#define NVIC_ICPR						((volatile nvic_icpr_t *) 0xE000E280)

#define NVIC_ISER0_TIM2					28
#define NVIC_ISER0_TIM3					29
#define NVIC_ISER0_TIM4					30

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

#define TIM_CR1(A)				((volatile uint16_t *) (A + 0x00))
#define TIM_CR2(A)				((volatile uint16_t *) (A + 0x04))
#define TIM_SMCR(A)				((volatile uint16_t *) (A + 0x08))
#define TIM_DIER(A)				((volatile uint16_t *) (A + 0x0C))
#define TIM_SR(A)				((volatile uint16_t *) (A + 0x10))
#define TIM_EGR(A)				((volatile uint16_t *) (A + 0x14))
#define TIM_CCMR1(A)				((volatile uint16_t *) (A + 0x18))
#define TIM_CCMR2(A)				((volatile uint16_t *) (A + 0x1C))
#define TIM_CCER(A)				((volatile uint16_t *) (A + 0x20))
#define TIM_CNT(A)				((volatile uint32_t *) (A + 0x24))
#define TIM_PSC(A)				((volatile uint16_t *) (A + 0x28))
#define TIM_ARR(A)				((volatile uint32_t *) (A + 0x2C))
#define TIM_RCR(A)				((volatile uint16_t *) (A + 0x30))
#define TIM_CCR1(A)				((volatile uint32_t *) (A + 0x34))
#define TIM_CCR2(A)				((volatile uint32_t *) (A + 0x38))
#define TIM_CCR3(A)				((volatile uint32_t *) (A + 0x3C))
#define TIM_CCR4(A)				((volatile uint32_t *) (A + 0x40))
#define TIM_BDTR(A)				((volatile uint16_t *) (A + 0x44))
#define TIM_DCR(A)				((volatile uint16_t *) (A + 0x48))
#define TIM_DMAR(A)				((volatile uint16_t *) (A + 0x4C))

/* TIM_EGR */
#define TIM_EGR_UG				0
#define TIM_EGR_CC1G			1

/* TIM_SMCR */
#define TIM_SMCR_SMS(A)			(A << 0)
#define TIM_SMCR_SMS_RESET_MODE	0b100

#define TIM_SMCR_TS(A)			(A << 8)
#define TIM_SMCR_TS_TI1FP1		0b101

/* TIM_CCER */

#define TIM_CCER_CC1NP			3
#define TIM_CCER_CC1P			1
#define TIM_CCER_CC1E			0

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

#define TIM_CCMR1_OC1M(A)		((A & 0x7) << 4)
#define TIM_CCMR1_OC1M_MASK		TIM_CCMR1_OC1M(0b111)

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
