#include "ip.h"

/*********************************************
 * Prototypes
 *********************************************/

u16 ip_calc_cs(ip_pkt_t *ip_pkt)
{
	return checksum_oc16((u16 *) ip_pkt, (ip_pkt->hdr.ihl * 2));
}
