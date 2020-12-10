#ifndef _TSC3200_H
#define _TSC3200_H

#include "include.h"
#include "device.h"
#include "stm32f446.h"
#include "delay.h"

typedef enum
{
	TSC3200_DIODE_RED,
	TSC3200_DIODE_BLUE,
	TSC3200_DIODE_CLEAR,
	TSC3200_DIODE_GREEN
} tsc3200_diode_t;

typedef struct
{
	uint32_t gpio_base, timer_base;
	uint32_t gpio_s0, gpio_s1; 			/* Output frequency scaling */
	uint32_t gpio_s2, gpio_s3; 			/* Photo-diode selection */
	uint32_t gpio_oe, gpio_out;
} tsc3200_t;

typedef struct
{
	uint16_t r, g, b, a;
} tsc3200_ret_t;

void tsc3200_init(tsc3200_t *sensor);
void tsc3200_select(tsc3200_t *sensor, tsc3200_diode_t diode);
uint16_t tsc3200_read_freq(tsc3200_t *sensor);
tsc3200_ret_t tsc3200_read(tsc3200_t *sensor);

#endif
