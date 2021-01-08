#ifndef _SRC_INTERNET_PACKET_BUILDER_H
#define _SRC_INTERNET_PACKET_BUILDER_H

#include <stdbool.h>
#include <string.h>

#include "../types.h"

#include "manager.h"
#include "bswap.h"

#include "ethernet.h"
#include "ip.h"
#include "udp.h"
#include "bootp.h"
#include "dns.h"
#include "arp.h"
#include "icmp.h"
#include "custom/discover.h"
#include "custom/control.h"

ethernet_pkt_t *pkt_builder_ethernet(u8 *buffer, u8 *ha, u16 proto);

ip_pkt_t *pkt_builder_ip(u8 *buffer, u8 *ha, u8 *dest4, u8 ttl, u8 proto);

udp_pkt_t *pkt_builder_udp(u8 *buffer, u8 *ha, u8 *dest4, u8 ttl, u16 port);

bootp_pkt_t *pkt_builder_bootp(u8 *buffer, u8 *ha, u8 *dest4, u8 op, u8 ttl, u16 flags);

dns_pkt_t *pkt_builder_dns(u8 *buffer, u8 *ha, u8 *dest4, u8 ttl, u16 id);

arp_pkt_t *pkt_builder_arp(u8 *buffer, u8 *tha, u8 *tpa, u8 ttl, u16 op);

discover_pkt_t *pkt_builer_discover(u8 *buffer, u8 *ha, u8 *dest4, u8 ttl, const char *vstring);

control_pkt_t *pkt_builder_control_reply(u8 *buffer, u8 *ha, u8 *dest4, u8 ttl, u16 op, u32 sn);

icmp_pkt_t *pkt_builder_icmp(u8 *buffer, u8 *ha, u8 *dest4, u8 ttl, u8 type, u8 code);

void pkt_builder_icmp_finish(ip_pkt_t *ip_pkt);

void pkt_builder_control_reply_finish(ip_pkt_t *ip_pkt, u16 ps);

void pkt_builder_ip_finish(ip_pkt_t *ip_pkt);

void pkt_builder_udp_finish(ip_pkt_t *ip_pkt, udp_pkt_t *udp_pkt);

void pkt_builder_bootp_finish(ip_pkt_t *ip_pkt, bootp_oparam_t *lparam);

void pkt_builer_discover_finish(ip_pkt_t *ip_pkt);

#endif
