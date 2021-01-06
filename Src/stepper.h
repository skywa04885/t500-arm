#ifndef _STEPPER_H
#define _STEPPER_H

#include "include.h"
#include "stm32f446.h"
#include "device.h"
#include "hardware/gpio.h"
#include "types.h"

/***********************************
 * Types
 ***********************************/

typedef enum {
	STEPPER_FORWARD,
	STEPPER_BACKWARD
} stepper_dir_t;

typedef struct {
	uint16_t presc;
} stepper_timing_t;

typedef struct {
	uint32_t stop_speedup_at, start_slowdown_at;
	uint32_t total_steps, done_steps;
	i32 	target_pos;
	uint16_t current_sps;
} stepper_operation_t;

typedef struct {
	uint8_t mf, pu, dir;
	uint32_t gpio_base;
	uint32_t timer_osc, timer_base;
	/* =================== */
	int32_t position;
	uint16_t min_sps, max_sps;
	uint16_t sps_inc;
	stepper_dir_t current_dir;
	/* =================== */
	stepper_operation_t cop;
	stepper_timing_t timing;
	/* =================== */
	unsigned stepper_moving: 1;
	unsigned stepper_enabled : 1;
	unsigned auto_enable_disable : 1;
} stepper_t;


/***********************************
 * ISR's
 ***********************************/

#define STEPPER_ISR(NAME, STEPPER) \
	void NAME(void) \
	{ \
		/* Checks if the event is update ( overflow ) */ \
		if (*TIM_SR(STEPPER.timer_base) & (1 << TIM_SR_UIF)) \
		{ \
			/* Clears the update vent flag */ \
			*TIM_SR(STEPPER.timer_base) &= ~(1 << TIM_SR_UIF); \
			\
			/* Checks if we should stop */ \
			if (STEPPER.cop.done_steps >= STEPPER.cop.total_steps) \
			{ \
				if (STEPPER.auto_enable_disable) stepper_disable(&STEPPER); \
				STEPPER.stepper_moving = false; \
				STEPPER.cop.current_sps = 0; \
				*TIM_CR1(STEPPER.timer_base ) = 0; \
				return; \
			} \
			\
			/* Sends the stepper pulse */ \
			*GPIO_ODR(STEPPER.gpio_base) &= ~(1 << STEPPER.pu); \
			for (uint16_t i = 0; i < 100; ++i); \
			*GPIO_ODR(STEPPER.gpio_base) |= (1 << STEPPER.pu); \
			\
			/* Checks if speed up required */ \
			if (STEPPER.cop.done_steps < STEPPER.cop.stop_speedup_at) \
			{ \
				STEPPER.cop.current_sps += STEPPER.sps_inc; \
				*TIM_ARR(STEPPER.timer_base) = 1000000 / STEPPER.cop.current_sps; \
			} else if (STEPPER.cop.done_steps >= STEPPER.cop.start_slowdown_at) \
			{ \
				STEPPER.cop.current_sps -= STEPPER.sps_inc; \
				*TIM_ARR(STEPPER.timer_base) = 1000000 / STEPPER.cop.current_sps; \
			} \
			\
			/* Increments step, and updates the position */ \
			++STEPPER.cop.done_steps; \
			if (STEPPER.current_dir == STEPPER_FORWARD) ++STEPPER.position; \
			else --STEPPER.position; \
		} \
	}

/***********************************
 * Prototypes
 ***********************************/

void stepper_init(stepper_t *stepper);
void stepper_set_dir(stepper_t *stepper, stepper_dir_t dir);
void stepper_disable(stepper_t *stepper);
void stepper_enable(stepper_t *stepper);
void stepper_simple_move(stepper_t *stepper, int32_t new_position);

#endif
