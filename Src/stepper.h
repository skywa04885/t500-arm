#ifndef _STEPPER_H
#define _STEPPER_H

#include "include.h"
#include "stm32f446.h"

typedef struct {
	uint8_t mf, pu, dir;
	uint32_t base;
} stepper_t;

typedef enum {
	STEPPER_FORWARD,
	STEPPER_BACKWARD
} stepper_dir_t;

void stepper_init(const stepper_t *stepper);
void stepper_set_dir(const stepper_t *stepper, stepper_dir_t dir);
void stepper_disable(const stepper_t *stepper);
void stepper_enable(const stepper_t *stepper);
void stepper_move_simple(const stepper_t *stepper, uint32_t pulses);

#endif
