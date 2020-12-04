#include "stepper.h"

void stepper_init(stepper_t *stepper)
{
	/***********************************
	 * Base structure initialization
	 ***********************************/

	stepper->timer_active = false;

	/***********************************
	 * Configures GPIO
	 ***********************************/

	// Makes all the pins input ( masks away all the ones )
	*GPIO_MODER(stepper->base) &= GPIO_MODE_RESET(stepper->mf);
	*GPIO_MODER(stepper->base) &= GPIO_MODE_RESET(stepper->pu);
	*GPIO_MODER(stepper->base) &= GPIO_MODE_RESET(stepper->dir);

	// Makes all the pins output
	*GPIO_MODER(stepper->base) |= GPIO_MODE(GPIO_OUTPUT, stepper->mf);
	*GPIO_MODER(stepper->base) |= GPIO_MODE(GPIO_OUTPUT, stepper->pu);
	*GPIO_MODER(stepper->base) |= GPIO_MODE(GPIO_OUTPUT, stepper->dir);

	// Sets the default values ( disabled, no pulse, move forward )
	*GPIO_ODR(stepper->base) &= ~(1 << stepper->mf);
	*GPIO_ODR(stepper->base) |= (1 << stepper->pu);
	*GPIO_ODR(stepper->base) |= (1 << stepper->dir);

	*GPIO_OSPEEDR(stepper->base) |= GPIO_SPEED(GPIO_HIGH_SPEED, stepper->pu);
}

void stepper_set_dir(const stepper_t *stepper, stepper_dir_t dir)
{
	switch (dir)
	{
	case STEPPER_BACKWARD:
		*GPIO_ODR(stepper->base) |= (1 << stepper->dir);
		break;
	case STEPPER_FORWARD:
		*GPIO_ODR(stepper->base) &= ~(1 << stepper->dir);
		break;
	}
}

void stepper_enable(const stepper_t *stepper)
{
	*GPIO_ODR(stepper->base) |= (1 << stepper->mf);
}

void stepper_disable(const stepper_t *stepper)
{
	*GPIO_ODR(stepper->base) &= ~(1 << stepper->mf);
}

void stepper_simple_move(stepper_t *stepper, int32_t new_position)
{
	stepper_dir_t moving_direction;

	/***********************************
	 * Performs the calculations
	 ***********************************/

	// Calculates the number of steps to be performed, and sets the stepper
	//  direction ( also required for position rounding )
	if (new_position > stepper->position)
	{
		stepper->cop.total_steps = new_position - stepper->position;
		moving_direction = STEPPER_FORWARD;
	}
	else
	{
		stepper->cop.total_steps = stepper->position - new_position;
		moving_direction = STEPPER_BACKWARD;
	}

	// Rounds the number if not even ( will mess with calculations )
	if ((stepper->cop.total_steps % 2) != 0)
	{
		stepper->cop.total_steps++;
		if (moving_direction == STEPPER_FORWARD) ++new_position;
		else --new_position;
	}

	// Calculates the auto-reload register

	/***********************************
	 * Starts moving
	 ***********************************/

	// Waits until timer ready for usage
	while (stepper->timer_active);

	// Sets the stepper direction
	stepper_set_dir(stepper, moving_direction);

	// Configures the timer
	*TIM_ARR(stepper->timer_base) 	= 100;
	*TIM_PSC(stepper->timer_base)	= stepper->timing.presc;

	/***********************************
	 * Finalizes
	 ***********************************/

	// Updates the position of the stepper ( required for future movements )
	stepper->position = new_position;
}

