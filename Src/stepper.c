#include "stepper.h"

void stepper_init(stepper_t *stepper)
{
	/***********************************
	 * Base structure initialization
	 ***********************************/

	stepper->stepper_moving = false;

	/***********************************
	 * Configures GPIO
	 ***********************************/

	// Makes all the pins input ( masks away all the ones )
	*GPIO_MODER(stepper->gpio_base) &= ~GPIO_MODE(stepper->mf, 0b11);
	*GPIO_MODER(stepper->gpio_base) &= ~GPIO_MODE(stepper->pu, 0b11);
	*GPIO_MODER(stepper->gpio_base) &= ~GPIO_MODE(stepper->dir, 0b11);

	// Makes all the pins output
	*GPIO_MODER(stepper->gpio_base) |= GPIO_MODE(stepper->mf, GPIO_MODE_OUTPUT);
	*GPIO_MODER(stepper->gpio_base) |= GPIO_MODE(stepper->pu, GPIO_MODE_OUTPUT);
	*GPIO_MODER(stepper->gpio_base) |= GPIO_MODE(stepper->dir, GPIO_MODE_OUTPUT);

	// Pull down
	*GPIO_PUPDR(stepper->gpio_base) |= GPIO_PUPD(stepper->mf, GPIO_PUPD_PULL_DOWN);
	*GPIO_PUPDR(stepper->gpio_base) |= GPIO_PUPD(stepper->dir, GPIO_PUPD_PULL_DOWN);
	*GPIO_PUPDR(stepper->gpio_base) |= GPIO_PUPD(stepper->pu, GPIO_PUPD_PULL_DOWN);

	// Sets the default values ( disabled, no pulse, move forward )
	*GPIO_ODR(stepper->gpio_base) &= ~(1 << stepper->mf);
	*GPIO_ODR(stepper->gpio_base) |= (1 << stepper->pu);
	*GPIO_ODR(stepper->gpio_base) |= (1 << stepper->dir);

	// Makes PU high speed
	*GPIO_OSPEEDR(stepper->gpio_base) |= GPIO_OSPEED(stepper->pu, GPIO_HIGH_SPEED);

	/***********************************
	 * Timer
	 ***********************************/

	*TIM_PSC(stepper->timer_base) = stepper->timing.presc;
	*TIM_DIER(stepper->timer_base) = (1 << TIM_DIER_UIE);
}

void stepper_set_dir(stepper_t *stepper, stepper_dir_t dir)
{
	stepper->current_dir = dir;
	switch (dir)
	{
	case STEPPER_BACKWARD:
		*GPIO_ODR(stepper->gpio_base) |= (1 << stepper->dir);
		break;
	case STEPPER_FORWARD:
		*GPIO_ODR(stepper->gpio_base) &= ~(1 << stepper->dir);
		break;
	}
}

void stepper_enable(stepper_t *stepper)
{
	stepper->stepper_enabled = true;
	*GPIO_ODR(stepper->gpio_base) |= (1 << stepper->mf);
}

void stepper_disable(stepper_t *stepper)
{
	stepper->stepper_enabled = false;
	*GPIO_ODR(stepper->gpio_base) &= ~(1 << stepper->mf);
}

void stepper_simple_move(stepper_t *stepper, int32_t new_position)
{
	// Waits until stepper ready for usage
	if (stepper->stepper_moving) return;
	stepper->stepper_moving = true;

	/***********************************
	 * Performs position calculations
	 ***********************************/

	// Calculates the number of steps to be performed, and sets the stepper
	//  direction ( also required for position rounding )
	if (new_position > stepper->position)
	{
		stepper->cop.total_steps = new_position - stepper->position;
		stepper_set_dir(stepper, STEPPER_FORWARD);
	}
	else
	{
		stepper->cop.total_steps = stepper->position - new_position;
		stepper_set_dir(stepper, STEPPER_BACKWARD);
	}

	// Rounds the number if not even ( will mess with calculations )
	if ((stepper->cop.total_steps % 2) != 0)
	{
		++stepper->cop.total_steps;
		if (stepper->current_dir == STEPPER_FORWARD) ++new_position;
		else --new_position;
	}

	/***********************************
	 * Speedup and slow-down calculate
	 ***********************************/

	uint32_t ramp_step_size = (stepper->max_sps - stepper->min_sps) / stepper->sps_inc;

	if (ramp_step_size > (stepper->cop.total_steps / 2))
		stepper->cop.stop_speedup_at = stepper->cop.start_slowdown_at = stepper->cop.total_steps / 2;
	else
	{
		stepper->cop.stop_speedup_at = ramp_step_size;
		stepper->cop.start_slowdown_at = stepper->cop.total_steps - ramp_step_size;
	}

	/***********************************
	 * Starts moving
	 ***********************************/

	// Sets the done steps to zero, and the current steps per second
	stepper->cop.done_steps = 0;
	stepper->cop.current_sps = stepper->min_sps;

	// Enables stepper
	if (stepper->auto_enable_disable) stepper_enable(stepper);

	// Configures the timer
	*TIM_CNT(stepper->timer_base) = 0;
	*TIM_ARR(stepper->timer_base) = 1000000 / stepper->cop.current_sps;
	*TIM_CR1(stepper->timer_base) = (1 << TIM_CR1_CEN);
}

