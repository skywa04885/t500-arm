#define LABEL "NetMan"
#include "manager.h"

extern stepper_t *stepper_motors[];
extern u8 stepper_motors_count;

extern u8 read_buffer[];
extern u8 write_buffer[];

extern enc28j60_fifo_t __ip_fifo;
extern enc28j60_fifo_t __system_fifo;
extern enc28j60_fifo_t __transmit_fifo;

extern void udp_packet_callback(ethernet_pkt_t *eth_pkt);

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

void manager_init(void)
{
	// Initializes the ENC28J60
	enc28j60_spi_init();
	enc28j60_init();

	// Sets ENC28J60 LED modes
	enc28j60_ledb_mode(ENC28J60_PHLCON_LCFG_DISPLAY_TX_ACTIVITY);
	enc28j60_leda_mode(ENC28J60_PHLCON_LCFG_DISPLAY_LINK_STATUS_AND_RX_ACTIVITY);

	// Prints the EREVID
	LOGGER_INFO(LABEL, "EREVID: B%u\n", enc28j60_get_erevid());
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
				LOGGER_INFO(LABEL, "Crash, re-init\n");
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
		manager_handle_udp(eth_pkt);
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
	// Builds the ICMP packet
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

	//
	// Finishes and writes the packet
	//

	// Finishes the packet, and writes
	pkt_builder_icmp_finish(reply_ip_pkt);
	enc28j60_write(reply_pkt, BSWAP16(reply_ip_pkt->hdr.tl));

	// Prints status update
	LOGGER_INFO(LABEL, "ICMP Echo Reply to %u.%u.%u.%u\n", ip_ipv4->sa[0], ip_ipv4->sa[1],
			ip_ipv4->sa[2], ip_ipv4->sa[3]);
}

/*********************************************
 * Network Layer ( ARP )
 *********************************************/

void manager_handle_arp_request(ethernet_pkt_t *eth_pkt)
{

	//
	// Reads the packet
	//

	arp_pkt_t *arp_pkt = (arp_pkt_t *) eth_pkt->payload;
	arp_payload_ipv4_t *arp_ipv4 = (arp_payload_ipv4_t *) arp_pkt->payload;

	//
	// Creates the reply
	//

	enc28j60_pkt_t *reply_pkt = (enc28j60_pkt_t *) write_buffer;
	ethernet_pkt_t *reply_eth_pkt = (ethernet_pkt_t *) &reply_pkt->eth_pkt;

	// Builds the ARP reply
	pkt_builder_arp((u8 *) reply_eth_pkt, arp_ipv4->sha, arp_ipv4->spa,
			60, ARP_PKT_OP_REPLY);

	//
	// Writes the reply
	//

	// Calculates the total length and writes it to the ENC28J60
	#define ARP_REQUEST_WRITE_SIZE (sizeof (enc28j60_pkt_t) + sizeof (arp_pkt_t) + sizeof (arp_payload_ipv4_t))
	enc28j60_write(reply_pkt, ARP_REQUEST_WRITE_SIZE);

	// Prints status update
	LOGGER_INFO(LABEL, "ARP Reply to %u.%u.%u.%u\n", arp_ipv4->spa[0], arp_ipv4->spa[1],
			arp_ipv4->spa[2], arp_ipv4->spa[3]);
}

void manager_handle_arp_reply(ethernet_pkt_t *pkt)
{

}

void manager_ipv4s_to_macs(manager_arp_resolve_t *resolve, u8 total)
{
	enc28j60_pkt_t *pkt = (enc28j60_pkt_t *) write_buffer;
	ethernet_pkt_t *eth_pkt = (ethernet_pkt_t *) &pkt->eth_pkt;

	arp_pkt_t *arp_pkt = (arp_pkt_t *) eth_pkt->payload;
	arp_payload_ipv4_t *arp_ipv4 = (arp_payload_ipv4_t *) arp_pkt->payload;

	for (u8 i = 0; i < total; ++i)
	{

	}
}

/*********************************************
 * Transport Layer ( UDP )
 *********************************************/

void manager_handle_udp(ethernet_pkt_t *eth_pkt)
{
	//
	// Reads the packet
	//

	ip_pkt_t *ip_pkt = (ip_pkt_t *) eth_pkt->payload;
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;
	udp_pkt_t *udp_pkt = (udp_pkt_t *) ip_ipv4->payload;

	//
	// Validate Checksum
	//

//	if (udp_calc_cs(udp_pkt, ip_ipv4->da, ip_ipv4->sa, 4, 4, IP_HDR_PROTO_UDP) != 0xFFFF)
//		return;

	//
	// Calls the callback
	//

	udp_packet_callback(eth_pkt);
}

/*********************************************
 * Application Layer ( DNS )
 *********************************************/

void manager_send_dns(const char *domain)
{
	enc28j60_pkt_t *pkt = (enc28j60_pkt_t *) write_buffer;
	ethernet_pkt_t *eth_pkt = (ethernet_pkt_t *) &pkt->eth_pkt;
//	dns_pkt_t *dns_pkt = pkt_builder_dns((u8 *) & eth_pkt, , config.ipv4_dns_server);
}
