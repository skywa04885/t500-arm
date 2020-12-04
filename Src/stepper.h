#ifndef _STEPPER_H
#define _STEPPER_H

#include "include.h"
#include "stm32f446.h"

/***********************************
 * Definitions
 ***********************************/

#define STEPPER_SPEEDUP_RATE				2 /* Increase speed by N steps each speedup pulse */

/***********************************
 * ISR's
 ***********************************/

#define STEPPER_TIMER_ISR(TIMER, MOTOR) \
	void TIMER() \
	{ \
		/* Checks if we've completed operation */ \
		if (MOTOR.cop.done_steps >= MOTOR.cop.total_steps) \
		{ \
			/* Sets the timer active to false, and disables timer */ \
			MOTOR.timer_active = false; \
			*TIM_CR1(MOTOR.timer_base) &= ~(1 << TIM_CR1_CEN); \
			return; \
		} \
		/* Else just send pulse, and increment */ \
		*GPIO_ODR(MOTOR.base) &= ~(1 << MOTOR.pu); \
		*GPIO_ODR(MOTOR.base) |=  (1 << MOTOR.pu); \
		++MOTOR.position; \
		++MOTOR.cop.done_steps; \
	}

/***********************************
 * Types
 ***********************************/

typedef struct {
	uint32_t presc;
} stepper_timing_t;

typedef struct {
	uint32_t start_slowdown_at,
		stop_speedup_at,
		total_steps,
		done_steps;
} stepper_operation_t;

typedef struct {
	uint8_t mf,					/* Enable */
		pu,						/* Pulse */
		dir;					/* Direction */
	uint32_t base;				/* GPIO Base Address */
	uint32_t timer_osc;			/* Speed of the timer */
	uint32_t timer_base;		/* TIM Base Address ( General Purpose ) */
	bool timer_active;			/* If the timer is active */
	uint16_t min_sps, max_sps;	/* Minimum and Max speed of the motor */
	int32_t position;			/* Current stepper position */
	stepper_operation_t cop;	/* Current stepper operation */
	stepper_timing_t timing;	/* The timer timing info */
} stepper_t;

typedef enum {
	STEPPER_FORWARD,
	STEPPER_BACKWARD
} stepper_dir_t;

/***********************************
 * Prototypes
 ***********************************/

void stepper_init(stepper_t *stepper);
void stepper_set_dir(const stepper_t *stepper, stepper_dir_t dir);
void stepper_disable(const stepper_t *stepper);
void stepper_enable(const stepper_t *stepper);
void stepper_simple_move(stepper_t *stepper, int32_t new_position);

#endif
