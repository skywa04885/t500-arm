#ifndef _DEVICE_H
#define _DEVICE_H

#include "stm32f446.h"

/* Timing */

#define APB1_SPEED					45000000

/* A1602 */

#define A1602_BASE					GPIOC_BASE
#define A1602_E						15
#define A1602_RW						14
#define A1602_RS						13
#define A1602_D4						0
#define A1602_D5						1
#define A1602_D6						2
#define A1602_D7						3
#define A1602_DSHIFT					0

/* Stepper0 */

#define STEPPER0_BASE				GPIOB_BASE
#define STEPPER0_DIR					15
#define STEPPER0_PU					14
#define STEPPER0_MF					13


/* Stepper1 */

#define STEPPER1_BASE				GPIOB_BASE
#define STEPPER1_DIR					12
#define STEPPER1_PU					2
#define STEPPER1_MF					1


/* Stepper2 */

#define STEPPER2_BASE				GPIOB_BASE
#define STEPPER2_DIR					4
#define STEPPER2_PU					5
#define STEPPER2_MF					3


/* Stepper3 */

#define STEPPER3_BASE				GPIOB_BASE
#define STEPPER3_DIR					8
#define STEPPER3_PU					9
#define STEPPER3_MF					6

#endif
