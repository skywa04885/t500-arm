/*
	STM32 ProjectA source code, made by Skywa04885... All Rights Reserved !

	contact: sw04885@fannst.nl
*/

#ifndef _SRC_HARDWARE_EXTI_H
#define _SRC_HARDWARE_EXTI_H

#define EXTI_BASE					0x40013C00

#define EXTI_IMR						((volatile u32 *) (EXTI_BASE + 0x00))
#define EXTI_EMR						((volatile u32 *) (EXTI_BASE + 0x04))
#define EXTI_RTSR					((volatile u32 *) (EXTI_BASE + 0x08))
#define EXTI_FTSR					((volatile u32 *) (EXTI_BASE + 0x0C))
#define EXTI_SWIER					((volatile u32 *) (EXTI_BASE + 0x10))
#define EXTI_PR						((volatile u32 *) (EXTI_BASE + 0x14))

#endif
