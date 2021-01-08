#include "manager.h"

extern stepper_t *stepper_motors[];
extern u8 stepper_motors_count;

extern u8 read_buffer[];
extern u8 write_buffer[];

extern enc28j60_fifo_t __ip_fifo;
extern enc28j60_fifo_t __system_fifo;
extern enc28j60_fifo_t __transmit_fifo;

const char *label = "NetMan";

manager_config_t config = {
		/* Vendor Info */
		.vstring = "MCU-ProjectA",
		/* State */
		.dhcp_state = MANAGER_DHCP_STATE_NOT_READY,
		/* Hardware Configuration */
		.max_frame_length = 512,
		.rx_buff_end = (8 * 1024) - 1500 - 4,
		.tx_buff_start = (8 * 1024) - 1500,
		/* Configuration */
		.mac = { 0xAA, 0xAA, 0xA4, 0x52, 0x37, 0x3C },
		.ipv4_address = { 192, 168, 2, 188  },
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

void udp_packet_callback(enc28j60_pkt_t *pkt)
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

	// Prints the EREVID
	LOGGER_INFO(label, "EREVID: B%u\n", enc28j60_get_erevid());
}

void manager_poll(void)
{
	ethernet_pkt_t *eth_pkt = (ethernet_pkt_t *) read_buffer;
	u16 len;
	u8 i;

	//
	// Handles PKTIF issues
	//

	static u32 pkti = 0;
	static u8 prev_pkt_count = 0;
	if (++pkti > 1000000)
	{
		pkti = 0;
		u8 pkt_count = enc28j60_packet_count();
		if (pkt_count > 0)
		{
			if (pkt_count == prev_pkt_count)
			{
				LOGGER_INFO(label, "Crash, re-init\n");
				enc28j60_init();
			}
			else prev_pkt_count = pkt_count;
		}

	}

	//
	// Handles the system packets ( max 10 per poll )
	//


	i = 0;
	len = 0;
	do
	{
		len = enc28j60_fifo_read_packet(&__system_fifo, eth_pkt);
		if (len > 0)
		{
			switch (BSWAP16(eth_pkt->hdr.type))
			{

			case ETHERNET_PKT_TYPE_ARP:
			{
				arp_pkt_t *arp_pkt = (arp_pkt_t *) eth_pkt->payload;
				if (BSWAP16(arp_pkt->hdr.op) == ARP_PKT_OP_REQUEST) manager_handle_arp_request(eth_pkt);
				else if (BSWAP16(arp_pkt->hdr.op) == ARP_PKT_OP_REPLY) manager_handle_arp_reply(eth_pkt);
			}

			default: break;

			}
		}
	} while (i++ < 10 && len > 0);

	//
	// Handles the IP packets
	//

	i = 0;
	len = 0;
	do
	{
		len = enc28j60_fifo_read_packet(&__ip_fifo, eth_pkt);
		if (len > 0) manager_handle_ip(eth_pkt);
	} while (i++ < 10 && len > 0);
}

/*********************************************
 * Network Layer ( IP )
 *********************************************/

void manager_handle_ip(ethernet_pkt_t *eth_pkt)
{
	ip_pkt_t *ip_pkt = (ip_pkt_t *) eth_pkt->payload;
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;

	// Validates the checksum, and throws the packet away
	//  if it does not match
	if (ip_calc_cs(ip_pkt) != 0xFFFF) return;

	// Handles the specific packets
	switch (ip_pkt->hdr.proto)
	{

	case IP_HDR_PROTO_ICMP:
		manager_handle_icmp(eth_pkt);
		break;

	case IP_HDR_PROTO_UDP:
		break;

	case IP_HDR_PROTO_TCP:
		break;

	default:
		break;

	}
}

/*********************************************
 * Network Layer ( ICMP )
 *********************************************/

void manager_handle_icmp(ethernet_pkt_t *eth_pkt)
{
	ip_pkt_t *ip_pkt = (ip_pkt_t *) eth_pkt->payload;
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;
	icmp_pkt_t *icmp_pkt = (icmp_pkt_t *) ip_ipv4->payload;

	// Validates the checksum
	if (icmp_calc_cs(icmp_pkt, BSWAP16(ip_pkt->hdr.tl) - (ip_pkt->hdr.ihl * 4)) != 0xFFFF)
		return;

	// Checks the ICMP packet type, and calls the handler
	switch (icmp_pkt->hdr.type)
	{

	case ICMP_TYPE_ECHO:
		manager_handle_icmp_echo_request(eth_pkt);
		break;

	default:
		break;

	}
}

void manager_handle_icmp_echo_request(ethernet_pkt_t *eth_pkt)
{
	//
	// Prepares the pointer
	//

	ip_pkt_t *ip_pkt = (ip_pkt_t *) eth_pkt->payload;
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;
	icmp_pkt_t *icmp_pkt = (icmp_pkt_t *) ip_ipv4->payload;
	icmp_erm_t *icmp_erm = (icmp_erm_t *) icmp_pkt->payload;

	enc28j60_pkt_t *reply_pkt = (enc28j60_pkt_t *) write_buffer;
	ethernet_pkt_t *reply_eth_pkt = (ethernet_pkt_t *) &reply_pkt->eth_pkt;
	ip_pkt_t *reply_ip_pkt = (ip_pkt_t *) reply_eth_pkt->payload;
	ip_ipv4_body_t *reply_ip_ipv4 = (ip_ipv4_body_t *) reply_ip_pkt->payload;
	icmp_pkt_t *reply_icmp_pkt = (icmp_pkt_t *) reply_ip_ipv4->payload;
	icmp_erm_t *reply_icmp_erm = (icmp_erm_t *) reply_icmp_pkt->payload;

	//
	// Builds and writes the ICMP Packet
	//

	// Builds the ICMP reply base
	pkt_builder_icmp((u8 *) reply_eth_pkt, eth_pkt->hdr.sa, ip_ipv4->sa,
			60, ICMP_TYPE_ECHO_REPLY, ICMP_ERM_CODE);

	// Copies the id and sequence number
	reply_icmp_erm->sn = icmp_erm->sn;
	reply_icmp_erm->id = icmp_erm->id;

	// Copies the ICMP body
	u16 body_size = BSWAP16(ip_pkt->hdr.tl) - ((ip_pkt->hdr.ihl * 4) + sizeof (ip_ipv4_body_t));
	reply_ip_pkt->hdr.tl = ip_pkt->hdr.tl;
	memcpy(reply_icmp_erm->data, icmp_erm->data, body_size);

	// Finishes the packet, and writes
	pkt_builder_icmp_finish(reply_ip_pkt);
	enc28j60_write(reply_pkt, BSWAP16(reply_ip_pkt->hdr.tl));

	// Prints status update
	LOGGER_INFO(label, "ICMP Echo Reply to %u.%u.%u.%u\n", ip_ipv4->sa[0], ip_ipv4->sa[1],
			ip_ipv4->sa[2], ip_ipv4->sa[3]);
}

/*********************************************
 * Network Layer ( ARP )
 *********************************************/

void manager_handle_arp_request(ethernet_pkt_t *eth_pkt)
{
	// Reads the Request ARP Packet
	arp_pkt_t *arp_pkt = (arp_pkt_t *) eth_pkt->payload;
	arp_payload_ipv4_t *arp_ipv4 = (arp_payload_ipv4_t *) arp_pkt->payload;

	// Creates the Reply ARP Packet
	enc28j60_pkt_t *reply_pkt = (enc28j60_pkt_t *) write_buffer;
	ethernet_pkt_t *reply_eth_pkt = (ethernet_pkt_t *) &reply_pkt->eth_pkt;
	pkt_builder_arp((u8 *) reply_eth_pkt, arp_ipv4->sha, arp_ipv4->spa,
			60, ARP_PKT_OP_REPLY);

	// Sends the reply packet
	#define ARP_REQUEST_WRITE_SIZE (sizeof (enc28j60_pkt_t) + sizeof (arp_pkt_t) + sizeof (arp_payload_ipv4_t))
	enc28j60_write(reply_pkt, ARP_REQUEST_WRITE_SIZE);

	// Prints status update
	LOGGER_INFO(label, "ARP Reply to %u.%u.%u.%u\n", arp_ipv4->spa[0], arp_ipv4->spa[1],
			arp_ipv4->spa[2], arp_ipv4->spa[3]);
}

void manager_handle_arp_reply(ethernet_pkt_t *pkt)
{

}
