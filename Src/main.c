#include "include.h"
#include "stm32f446.h"
#include "device.h"
#include "stepper.h"

static stepper_t stepper0 = {
		.mf = STEPPER0_MF,
		.pu = STEPPER0_PU,
		.dir = STEPPER0_DIR,
		.base = STEPPER0_BASE
};

static stepper_t stepper1 = {
		.mf = STEPPER1_MF,
		.pu = STEPPER1_PU,
		.dir = STEPPER1_DIR,
		.base = STEPPER1_BASE
};

static stepper_t stepper2 = {
		.mf = STEPPER2_MF,
		.pu = STEPPER2_PU,
		.dir = STEPPER2_DIR,
		.base = STEPPER2_BASE
};

static stepper_t stepper3 = {
		.mf = STEPPER3_MF,
		.pu = STEPPER3_PU,
		.dir = STEPPER3_DIR,
		.base = STEPPER3_BASE
};

void steppers_init(void)
{
	// RCC: Source clock for GPIOB
	*RCC_AHB1ENR |= (1 << GPIOBEN);

	// Initializes the steppers
	stepper_init(&stepper0);
	stepper_init(&stepper1);
	stepper_init(&stepper2);
	stepper_init(&stepper3);
}

int main(void)
{
	steppers_init();

//	stepper_enable(&stepper0);

//	stepper_set_dir(&stepper0, STEPPER_FORWARD);
//	stepper_move_simple(&stepper0, 7000);

//	stepper_set_dir(&stepper0, STEPPER_BACKWARD);
//	stepper_move_simple(&stepper0, 7000);

//	stepper_disable(&stepper0);

	stepper_enable(&stepper1);

	stepper_set_dir(&stepper1, STEPPER_FORWARD);
	stepper_move_simple(&stepper1, 7000);

	stepper_set_dir(&stepper1, STEPPER_BACKWARD);
	stepper_move_simple(&stepper1, 7000);

	stepper_disable(&stepper1);

    /* Loop forever */
	for(;;);
}
