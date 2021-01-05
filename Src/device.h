#ifndef _DEVICE_H
#define _DEVICE_H

#include "stm32f446.h"

/* Max Speeds */

#define NEMA17_MIN_SPS				300
#define NEMA17_MAX_SPS				12000
#define NEMA17_SPS_INC				2

#define NEMA23_MIN_SPS				300
#define NEMA23_MAX_SPS				8000
#define NEMA23_SPS_INC				4

/* Timing */

#define APB1_SPEED					45000000

/* Stepper0 */

#define STEPPER0_BASE				GPIOB_BASE
#define STEPPER0_DIR				15
#define STEPPER0_PU					14
#define STEPPER0_MF					13


/* Stepper1 */

#define STEPPER1_BASE				GPIOB_BASE
#define STEPPER1_DIR				12
#define STEPPER1_PU					2
#define STEPPER1_MF					1


/* Stepper2 */

#define STEPPER2_BASE				GPIOB_BASE
#define STEPPER2_DIR				4
#define STEPPER2_PU					5
#define STEPPER2_MF					3


/* Stepper3 */

#define STEPPER3_BASE				GPIOB_BASE
#define STEPPER3_DIR				8
#define STEPPER3_PU					9
#define STEPPER3_MF					6

#endif
