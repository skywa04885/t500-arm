#ifndef _STM32F446_H
#define _STM32F446_H

/***********************************
 * RCC
 ***********************************/

#define RCC_BASE					0x40023800

/* RCC Offsets */

#define RCC_AHB1ENR				((uint32_t *) (RCC_BASE + 0x30))

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

#endif
