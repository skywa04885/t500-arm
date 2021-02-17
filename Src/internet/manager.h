#ifndef _SRC_INTERNET_MANAGER_H
#define _SRC_INTERNET_MANAGER_H

/*********************************************
 * C Standard Library
 *********************************************/

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/*********************************************
 * Project Headers
 *********************************************/

#include "../types.h"
#include "../logger.h"
#include "../stepper.h"
#include "bootp.h"
#include "custom/control.h"
#include "packet-builder.h"
#include "../stepper.h"

#include "microchip/enc28j60.h"


/*********************************************
 * Macros
 *********************************************/

#define MANAGER_MAX_TCP_SOCKETS			4
#define MANAGER_MAX_UDP_SOCKETS			4

/*********************************************
 * Data Types
 *********************************************/

typedef enum
{
	MANAGER_DHCP_STATE_NOT_READY,
	MANAGER_DHCP_STATE_DISCOVER,
	MANAGER_DHCP_STATE_REQUEST,
	MANAGER_DHCP_STATE_READY
} manager_dhcp_state_t;

typedef struct
{
	/* Vendor Info */
	const char *vstring;
	/* State */
	manager_dhcp_state_t dhcp_state;
	/* Hardware Configuration */
	u16 max_frame_length;
	u16 tx_buff_start, rx_buff_end;
	/* Configuration */
	u8 mac[6];
	u8 ipv4_address[4], ipv4_subnet_mask[4];
	u8 ipv4_broadcast[4];
	u8 mac_broadcast[6];
	/* Servers */
	u8 ipv4_dns_server[4], ipv4_dhcp_server[4], ipv4_router[4];
	u8 mac_dns_server[6];
	/* Options */
	unsigned full_duplex : 1;
} manager_config_t;

typedef struct
{
	u8	*mac, *ipv4;
} manager_arp_resolve_t;

/*********************************************
 * Prototypes
 *********************************************/

void 	manager_init(void);
void	manager_poll(void);

/*********************************************
 * Network Layer ( IP )
 *********************************************/

void	manager_handle_ip(ethernet_pkt_t *eth_pkt);

/*********************************************
 * Network Layer ( ICMP )
 *********************************************/

void	manager_handle_icmp(ethernet_pkt_t *eth_pkt);
void	manager_handle_icmp_echo_request(ethernet_pkt_t *eth_pkt);

/*********************************************
 * Network Layer ( ARP )
 *********************************************/

void	manager_handle_arp_request(ethernet_pkt_t *eth_pkt);
void 	manager_handle_arp_reply(ethernet_pkt_t *eth_pkt);

void	manager_ipv4s_to_macs(manager_arp_resolve_t *resolve, u8 total);

/*********************************************
 * Transport Layer ( UDP )
 *********************************************/

void	manager_handle_udp(ethernet_pkt_t *eth_pkt);

/*********************************************
 * Application Layer ( DNS )
 *********************************************/

void	manager_send_dns(const char *domain);

#endif
