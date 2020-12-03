#include "stepper.h"

void stepper_init(const stepper_t *stepper)
{
	*GPIO_MODER(stepper->base) &= GPIO_MODE_RESET(stepper->mf);
	*GPIO_MODER(stepper->base) &= GPIO_MODE_RESET(stepper->pu);
	*GPIO_MODER(stepper->base) &= GPIO_MODE_RESET(stepper->dir);

	*GPIO_MODER(stepper->base) |= GPIO_MODE(GPIO_OUTPUT, stepper->mf);
	*GPIO_MODER(stepper->base) |= GPIO_MODE(GPIO_OUTPUT, stepper->pu);
	*GPIO_MODER(stepper->base) |= GPIO_MODE(GPIO_OUTPUT, stepper->dir);

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

void stepper_move_simple(const stepper_t *stepper, uint32_t pulses)
{
	for (uint32_t i = 0; i < pulses; ++i)
	{
		*GPIO_ODR(stepper->base) &= ~(1 << stepper->pu);
		for (uint32_t i = 0; i < 100; ++i);
		*GPIO_ODR(stepper->base) |= (1 << stepper->pu);
		for (uint32_t i = 0; i < 700; ++i);
	}
}
