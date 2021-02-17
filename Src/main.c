#include "main.h"

#define LABEL "main"

/***********************************
 * Global variables
 ***********************************/

stepper_t stepper0 = {
		.mf = STEPPER0_MF,
		.pu = STEPPER0_PU,
		.dir = STEPPER0_DIR,
		.gpio_base = STEPPER0_BASE,
		.position = 0,
		.timer_base = TIM2_BASE,
		.min_sps = NEMA23_MIN_SPS,
		.max_sps = NEMA23_MAX_SPS,
		.sps_inc = NEMA23_SPS_INC,
		.cop = {
			.target_pos = 0
		},
		.timing = {
			.presc = 90
		},
		.auto_enable_disable = false
};

stepper_t stepper1 = {
		.mf = STEPPER1_MF,
		.pu = STEPPER1_PU,
		.dir = STEPPER1_DIR,
		.gpio_base = STEPPER1_BASE,
		.timer_base = TIM3_BASE,
		.position = 0,
		.min_sps = NEMA23_MIN_SPS,
		.max_sps = NEMA23_MAX_SPS,
		.sps_inc = NEMA23_SPS_INC,
		.cop = {
			.target_pos = 0
		},
		.timing = {
			.presc = 90
		},
		.auto_enable_disable = true
};

stepper_t stepper2 = {
		.mf = STEPPER2_MF,
		.pu = STEPPER2_PU,
		.dir = STEPPER2_DIR,
		.gpio_base = STEPPER2_BASE,
		.timer_base = TIM4_BASE,
		.position = 0,
		.min_sps = NEMA23_FAST_MIN_SPS,
		.max_sps = NEMA23_FAST_MAX_SPS,
		.sps_inc = NEMA23_FAST_SPS_INC,
		.cop = {
			.target_pos = 0
		},
		.timing = {
			.presc = 90
		},
		.auto_enable_disable = false
};

stepper_t stepper3 = {
		.mf = STEPPER3_MF,
		.pu = STEPPER3_PU,
		.dir = STEPPER3_DIR,
		.gpio_base = STEPPER3_BASE,
		.timer_base = TIM5_BASE,
		.min_sps = NEMA17_MIN_SPS,
		.max_sps = NEMA17_MAX_SPS,
		.sps_inc = NEMA17_SPS_INC,
};

const char *device_id = "LUKESTE0";

/***********************************
 * Stepper ISR's
 ***********************************/

STEPPER_ISR(TIM2_IRQHandler, stepper0);
STEPPER_ISR(TIM3_IRQHandler, stepper1);
STEPPER_ISR(TIM4_IRQHandler, stepper2);

/***********************************
 * Global Variables
 ***********************************/

u8 write_buffer[1500];
u8 read_buffer[1500];

stepper_t *stepper_motors[] = {
		&stepper0,
		&stepper1,
		&stepper2,
		&stepper3
};
u8 stepper_motors_count = sizeof (stepper_motors) / sizeof (stepper_t *);

extern enc28j60_fifo_t __system_fifo;
extern manager_config_t config;

/***********************************
 * Functions
 ***********************************/

void manager_handle_control_packet(ethernet_pkt_t *eth_pkt)
{
	ip_pkt_t *ip_pkt = (ip_pkt_t *) eth_pkt->payload;
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;
	udp_pkt_t *udp_pkt = (udp_pkt_t *) ip_ipv4->payload;
	control_pkt_t *control_pkt = (control_pkt_t *) udp_pkt->payload;

	// Checks the control packet opcode, which will tell us what functions we should
	//  call, and what response to send.
	switch (control_pkt->opcode)
	{
		case CONTROL_PKT_OP_STEPPER_STATUS:
		{
			//
			// Parses the arguments
			//

			u8 motor = 0;

			// Parses the arguments
			control_pkt_arg_t *arg = (control_pkt_arg_t *) control_pkt->payload;
			do
			{
				if (arg->t == CONTROL_PKT_ARG_TYPE_U8)
					motor = ((control_pkt_arg_u8_t *) arg->payload)->val;
			} while ((arg = control_pkt_arg_parse_next(arg)) != NULL);

			// Checks if the motor number is actually available
			if (motor >= stepper_motors_count) return;

			//
			// Builds the status
			//

			stepper_t *stepper = stepper_motors[motor];

			u8 flags = 0x00;
			if (stepper->stepper_enabled)
				flags |= (_BV(CONTROL_PKT_ARG_MOTOR_FLAG_ENABLED));
			if (stepper->stepper_moving)
				flags |= (_BV(CONTROL_PKT_ARG_MOTOR_FLAG_MOVING));

			control_pkt_arg_stepper_status_t status = {
					.id = motor,
					.flags = flags,
					.sps = stepper->cop.current_sps,
					.cpos = stepper->position,
					.tpos = stepper->cop.target_pos,
			};

			//
			// Sends the status
			//

			// Builds the response packet
			memset(write_buffer, 0, 1000);
			enc28j60_pkt_t *resp_pkt = (enc28j60_pkt_t *) write_buffer;
			ip_pkt_t *resp_ip_pkt = (ip_pkt_t *) resp_pkt->eth_pkt.payload;
			control_pkt_t *resp_control_pkt = pkt_builder_control_reply((u8 *) &resp_pkt->eth_pkt, eth_pkt->hdr.sa, ip_ipv4->sa, 60, CONTROL_PKT_OP_STEPPER_STATUS, control_pkt->sn);
			control_pkt_arg_t *resp_arg = (control_pkt_arg_t *) resp_control_pkt->payload;

			// Builds the arguments
			u16 resp_payload_size = 0;

			resp_arg = control_pkt_arg_add_stepper_status(resp_arg, &status);
			resp_payload_size += sizeof (control_pkt_arg_t) + sizeof (control_pkt_arg_stepper_status_t);

			resp_arg = control_pkt_arg_end(resp_arg);
			resp_payload_size += sizeof (control_pkt_arg_t);

			// Finishes and writes the response packet
			pkt_builder_control_reply_finish(resp_ip_pkt, resp_payload_size);
			enc28j60_write(resp_pkt, BSWAP16(resp_ip_pkt->hdr.tl));

			break;
		}
		case CONTROL_PKT_OP_STEPPER_DISABLE:
		case CONTROL_PKT_OP_STEPPER_ENABLE:
		{
			//
			// Parses the arguments
			//

			u8 motor = 0;

			// Parses the arguments
			control_pkt_arg_t *arg = (control_pkt_arg_t *) control_pkt->payload;
			do
			{
				if (arg->t == CONTROL_PKT_ARG_TYPE_U8)
					motor = ((control_pkt_arg_u8_t *) arg->payload)->val;
			} while ((arg = control_pkt_arg_parse_next(arg)) != NULL);

			// Checks if the motor number is actually available
			if (motor >= stepper_motors_count) return;

			//
			// Enables / Disables the stepper the stepper
			//

			if (control_pkt->opcode == CONTROL_PKT_OP_STEPPER_DISABLE)
				stepper_disable(stepper_motors[motor]);
			else
				stepper_enable(stepper_motors[motor]);

			break;
		}
		case CONTROL_PKT_OP_STEPPER_MOVE_TO:
		{
			//
			// Parses the arguments
			//

			u8 motor = 0;
			i32 pos = 0;

			// Parses the arguments
			control_pkt_arg_t *arg = (control_pkt_arg_t *) control_pkt->payload;
			do
			{
				switch (arg->t)
				{
					case CONTROL_PKT_ARG_TYPE_U8:
						motor = ((control_pkt_arg_u8_t *) arg->payload)->val;
						break;
					case CONTROL_PKT_ARG_TYPE_I32:
						pos = ((control_pkt_arg_i32_t *) arg->payload)->val;
						break;
					default:
						break;
				}
			} while ((arg = control_pkt_arg_parse_next(arg)) != NULL);

			// Checks if the motor number is actually available
			if (motor >= stepper_motors_count) return;

			//
			// Moves the stepper
			//

			// Moves the motor
			stepper_simple_move(stepper_motors[motor], pos);

			break;
		}
		case CONTROL_PKT_OP_MOTOR_INFO:
		{

			//
			// Prepares the response packet
			//

			// Builds the motor info base packet, this will get the motor-info
			//  later on.
			memset(write_buffer, 0, 1000);
			enc28j60_pkt_t *resp_pkt = (enc28j60_pkt_t *) write_buffer;
			ip_pkt_t *resp_ip_pkt = (ip_pkt_t *) resp_pkt->eth_pkt.payload;
			control_pkt_t *resp_control_pkt = pkt_builder_control_reply((u8 *) &resp_pkt->eth_pkt, eth_pkt->hdr.sa, ip_ipv4->sa, 60, CONTROL_PKT_OP_MOTOR_INFO, control_pkt->sn);
			control_pkt_arg_t *resp_arg = (control_pkt_arg_t *) resp_control_pkt->payload;

			// Adds the stepper motors to the packet
			u16 resp_payload_size = 0;
			for(u8 i = 0; i < stepper_motors_count; ++i)
			{
				stepper_t *motor = stepper_motors[i];

				control_pkt_arg_motor_t motor_arg = {
						.id = i,
						.t = CONTROL_PKT_MOTOR_TYPE_STEPPER,
						.m = (motor->auto_enable_disable ==true ? CONTROL_PKT_MOTOR_MODE_AUTO : CONTROL_PKT_MOTOR_MODE_MANUAL),
						.mnsps = motor->min_sps,
						.mxsps = motor->max_sps
				};

				resp_payload_size += sizeof(control_pkt_arg_t) + sizeof (control_pkt_arg_motor_t);
				resp_arg = control_pkt_arg_add_motor(resp_arg, &motor_arg);
			}

			resp_arg = control_pkt_arg_end(resp_arg);
			resp_payload_size += sizeof (control_pkt_arg_t);

			//
			// Finishes the response packet and transmits
			//

			pkt_builder_control_reply_finish(resp_ip_pkt, resp_payload_size);
			enc28j60_write(resp_pkt, BSWAP16(resp_ip_pkt->hdr.tl));

			break;
		}
	}
}

void udp_packet_callback(ethernet_pkt_t *eth_pkt)
{
	ip_pkt_t *ip_pkt = (ip_pkt_t *) eth_pkt->payload;
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;
	udp_pkt_t *udp_pkt = (udp_pkt_t *) ip_ipv4->payload;

	switch (BSWAP16(udp_pkt->hdr.dp))
	{
		case DISCOVER_PKT_PORT:
		{

			discover_pkt_t *discover_pkt = (discover_pkt_t *) udp_pkt->payload;

			// Checks if the packet is meant for us
			if (discover_pkt->op != DISCOVER_OPCODE_REQUEST) break;
			else if (memcmp(discover_pkt->p, discover_pkt_prefix(), 3) != 0) break;


			// Builds the response
			memset(write_buffer, 0, 1000);
			enc28j60_pkt_t *resp_pkt = (enc28j60_pkt_t *) write_buffer;
			ip_pkt_t *resp_ip_pkt = (ip_pkt_t *) resp_pkt->eth_pkt.payload;

			// Prepares the discover packet
			pkt_builer_discover((u8 *) &resp_pkt->eth_pkt,
					eth_pkt->hdr.sa, ip_ipv4->sa, 60, config.vstring);

			// Finishes the discover packet, and writes it back
			pkt_builer_discover_finish(resp_ip_pkt);
			enc28j60_write(resp_pkt, BSWAP16(resp_ip_pkt->hdr.tl));
			LOGGER_INFO(LABEL, "Responded to Discover\n");

			break;
		}
		case CONTROL_PORT:
		{
			control_pkt_t *control_pkt = (control_pkt_t *) udp_pkt->payload;
			if (!control_pkt_check_prefix(control_pkt)) return;
			else if (control_pkt->type != CONTROL_PKT_TYPE_REQUEST) return;
			manager_handle_control_packet(eth_pkt);
		}
		default: break;
	}
}

/**
 * Configures the steppers and their GPIO Ports
 */
void steppers_init(void)
{
	// Enables the clock for the stepper GPIO's, and the timers
	*RCC_APB1ENR |= (_BV(RCC_APB1ENR_TIM2EN) | _BV(RCC_APB1ENR_TIM3EN) | _BV(RCC_APB1ENR_TIM4EN));
	*RCC_AHB1ENR |= (_BV(RCC_AHB1ENR_GPIOBEN));

	// Initializes the stepper motors
	stepper_init(&stepper0);
	stepper_init(&stepper1);
	stepper_init(&stepper2);
	stepper_init(&stepper3);

	// Enables the timers in the NVIC
	NVIC_ISER->iser0 |= (_BV(NVIC_ISER0_TIM3) | _BV(NVIC_ISER0_TIM2) | _BV(NVIC_ISER0_TIM4));
}

/**
 * Gets called by startup code
 */
int main(void)
{
	// Performs the configuration
	clock_init();
	delay_init();
	usart2_init(500000);
	manager_init();

	steppers_init();
	stepper_enable(&stepper0);

	servo_init();

	// Prints the MAC address
	for (;;)
	{
		manager_poll();
	}
}
