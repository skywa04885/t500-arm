#include "control.h"

control_pkt_arg_t *control_pkt_arg_add_motor(control_pkt_arg_t *arg, control_pkt_arg_motor_t *motor)
{
	arg->t = CONTROL_PKT_ARG_TYPE_MOTOR_STATUS;
	arg->l = sizeof (control_pkt_arg_motor_t);

	memcpy((void *) arg->payload, (void *) motor, sizeof (control_pkt_arg_motor_t));
	control_pkt_arg_motor_t *arg_motor = (control_pkt_arg_motor_t *) arg->payload;

	return (control_pkt_arg_t *) arg_motor->next;
}

control_pkt_arg_t *control_pkt_arg_end(control_pkt_arg_t *arg)
{
	arg->t = CONTROL_PKT_ARG_TYPE_END;
	arg->l = 0;

	return (control_pkt_arg_t *) arg->payload;
}


control_pkt_arg_t *control_pkt_arg_parse_next(control_pkt_arg_t *arg)
{
	arg = (control_pkt_arg_t *) &arg->payload[arg->l];

	if (arg->t == CONTROL_PKT_ARG_TYPE_END) return NULL;
	return arg;
}
