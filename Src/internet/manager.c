#include "manager.h"

extern stepper_t *stepper_motors[];
extern u8 stepper_motors_count;

extern u8 buffer[];
extern u8 write_buffer[];

const char *label = "NetMan";

manager_config_t config = {
		/* Vendor Info */
		.vstring = "MCU-ProjectA",
		/* State */
		.dhcp_state = MANAGER_DHCP_STATE_NOT_READY,
		/* Hardware Configuration */
		.max_frame_length = 512,
		.rx_buff_end = 4096,
		.tx_buff_start = 4098,
		/* Configuration */
		.mac = { 0xAA, 0xAA, 0xA4, 0x52, 0x37, 0x3C },
		.ipv4_address = { 10, 0, 0, 188  },
		.ipv4_broadcast = { 255, 255, 255, 255 },
		.mac_broadcast = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
		/* Servers */
		.ipv4_dns_server = { 8, 8, 8, 8 },
		/* Options */
		.full_duplex = 1
};

void manager_handle_control_packet(enc28j60_pkt_t *pkt)
{
	ip_pkt_t *ip_pkt = (ip_pkt_t *) pkt->eth_pkt.payload;
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
			control_pkt_t *resp_control_pkt = pkt_builder_control_reply((u8 *) &resp_pkt->eth_pkt, pkt->eth_pkt.hdr.sa, ip_ipv4->sa, 60, CONTROL_PKT_OP_STEPPER_STATUS, control_pkt->sn);
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
			control_pkt_t *resp_control_pkt = pkt_builder_control_reply((u8 *) &resp_pkt->eth_pkt, pkt->eth_pkt.hdr.sa, ip_ipv4->sa, 60, CONTROL_PKT_OP_MOTOR_INFO, control_pkt->sn);
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

extern void udp_packet_callback(enc28j60_pkt_t *pkt)
{
	ip_pkt_t *ip_pkt = (ip_pkt_t *) pkt->eth_pkt.payload;
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
					pkt->eth_pkt.hdr.sa, ip_ipv4->sa, 60, config.vstring);

			// Finishes the discover packet, and writes it back
			pkt_builer_discover_finish(resp_ip_pkt);
			enc28j60_write(resp_pkt, BSWAP16(resp_ip_pkt->hdr.tl));
			LOGGER_INFO(label, "Responded to Discover\n");

			break;
		}
		case CONTROL_PORT:
		{
			control_pkt_t *control_pkt = (control_pkt_t *) udp_pkt->payload;
			if (!control_pkt_check_prefix(control_pkt)) return;
			else if (control_pkt->type != CONTROL_PKT_TYPE_REQUEST) return;
			manager_handle_control_packet(pkt);
		}
		default: break;
	}
}

void manager_init(void)
{
	// Initializes the ENC28J60
	enc28j60_spi_init();
	enc28j60_init();

	// Sets ENC28J60 LED modes
	enc28j60_ledb_mode(ENC28J60_PHLCON_LCFG_DISPLAY_TX_ACTIVITY);
	enc28j60_leda_mode(ENC28J60_PHLCON_LCFG_DISPLAY_LINK_STATUS_AND_RX_ACTIVITY);

	// Waits for the link
	LOGGER_INFO(label, "Awaiting LINK\n");
	while (!enc28j60_is_link_up());
	LOGGER_INFO(label, "LINK Active\n");
}

void manager_poll(void)
{
	static bool link_up = true;

	// Checks the link status, if it is down, we want to stop polling
	//  since it is useless
	if (!enc28j60_is_link_up())
	{
		if (link_up)
		{
			LOGGER_INFO(label, "LINK down\n");
			link_up = false;
		}

		for (u16 i = 0; i < 0xFFFF; ++i);

		return;
	} else if (!link_up)
	{
		LOGGER_INFO(label, "LINK UP\n");
		link_up = true;
	}

	// Polls the ENC28J60 for any events
	enc28j60_poll(buffer);
}
