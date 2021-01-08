#ifndef _SRC_INTERNET_MICROCHIP_ENC28J60_H
#define _SRC_INTERNET_MICROCHIP_ENC28J60_H

/*********************************************
 * C Standard Library
 *********************************************/

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/*********************************************
 * Project Headers
 *********************************************/

#include "../../hardware/spi.h"
#include "../../hardware/gpio.h"
#include "../../hardware/rcc.h"
#include "../../hardware/exti.h"
#include "../../hardware/syscfg.h"
#include "../../delay.h"
#include "../../stm32f446.h"
#include "../../types.h"

#include "../checksum.h"
#include "../bswap.h"
#include "../../logger.h"

#include "enc28j60_registers.h"

#include "../ethernet.h"
#include "../arp.h"
#include "../udp.h"
#include "../ip.h"
#include "../icmp.h"

/*********************************************
 * Data Types
 *********************************************/

typedef enum
{
	ENC28J60_SPI_CMD_RCR = 0,
	ENC28J60_SPI_CMD_RBM,
	ENC28J60_SPI_CMD_WCR,
	ENC28J60_SPI_CMD_WBM,
	ENC28J60_SPI_CMD_BFS,
	ENC28J60_SPI_CMD_BFC,
	ENC28J60_SPI_CMD_SRC = 7
} enc28j60_spi_command_t;

typedef enum
{
	ENC28J60_BANK_0 = 0,
	ENC28J60_BANK_1,
	ENC28J60_BANK_2,
	ENC28J60_BANK_3
} enc28j60_bank_t;

typedef struct __attribute__ (( packed ))
{
	u16 rbc;
	u16 flags;
} enc28j60_status_vector_t;

typedef struct __attribute__ (( packed ))
{
	u8 cb;
	ethernet_pkt_t eth_pkt;
} enc28j60_pkt_t;

typedef struct
{
	u16 write, read, size;
	u8 *data;
} enc28j60_fifo_t;

typedef struct
{
	u8 		pn;		/* Part Number */
	u8		rl;		/* Revision Level */
	u32		id;		/* PHY ID */
} enc28j60_phid_t;

/*********************************************
 * ENC28J60 FIFO Operations
 *********************************************/

void 		__enc28j60_fifo_add_byte(enc28j60_fifo_t *fifo, u8 byte);
bool 		enc28j60_fifo_write_packet(enc28j60_fifo_t *fifo, const enc28j60_pkt_t *pkt, u16 len);

u8 			__enc28j60_fifo_read_byte(enc28j60_fifo_t *fifo);
u16 		enc28j60_fifo_read_packet(enc28j60_fifo_t *fifo, ethernet_pkt_t *pkt);

/*********************************************
 * ENC28J60 SPI
 *********************************************/

void 		__enc28j60_clocks_init(void);
void 		__enc28j60_gpio_init(void);
void		__enc28j60_spi_init(void);
void 		__enc29j60_external_interrupt_init(void);

void 		enc28j60_spi_init(void);
void		enc28j60_spi_select(void);
void		enc28j60_spi_deselect(void);
u8 			enc28j60_spi_transceive(u8 byte);

/*********************************************
 * ENC28J60 Communication
 *********************************************/

void		enc28j60_wbm(const u8 *data, u16 len);
void		enc28j60_rbm(u8 *data, u16 len);

void		enc28j60_wcr(u8 reg, u8 val);

void		enc28j60_bfs(u8 reg, u8 mask);
void		enc28j60_bfc(u8 reg, u8 mask);

u8			enc28j60_eth_rcr(u8 reg);
u8			enc28j60_mac_rcr(u8 reg);

void		enc28j60_eth_wait_until_set(u8 reg, u8 bit);
void		enc28j60_mac_wait_until_set(u8 reg, u8 bit);
void 		enc28j60_eth_wait_until_clear(u8 reg, u8 bit);
void 		enc28j60_mac_wait_until_clear(u8 reg, u8 bit);

void		enc28j60_src(void);

void		enc28j60_set_wbm_address(u16 address);
void		enc28j60_set_rbm_address(u16 address);

/*********************************************
 * ENC28J60 Default Functions
 *********************************************/

void		enc28j60_phy_write(u8 reg, u16 val);
u16			enc28j60_phy_read(u8 reg);

void		enc28j60_bank_select(enc28j60_bank_t bank);

void		enc28j60_write_mac(u8 *mac);
void		enc28j60_mac_read(u8 *mac);

void		enc28j60_leda_mode(enc28j60_phlcon_lcfg_t mode);
void		enc28j60_ledb_mode(enc28j60_phlcon_lcfg_t mode);

void		enc28j60_rx_enable(void);
void		enc28j60_rx_disable(void);

bool		enc28j60_is_link_up(void);
u8			enc28j60_get_erevid(void);

enc28j60_phid_t		enc28j60_get_phid(void);

void		enc28j60_manual_trigger_interrupt(void);

/*********************************************
 * ENC28J60 Initialization
 *********************************************/

void		__enc28j60_interrupt_init(void);
void		__enc28j60_filters_init(void);

void 		enc28j60_init(void);
void 		enc28j60_phy_init(void);
void 		enc28j60_filter_init(void);
void		enc28j60_rx_init(void);
void 		enc28j60_mac_init(void);

/*********************************************
 * ENC28J60 Networking
 *********************************************/

u8			enc28j60_packet_count(void);
void		enc28j60_pkt_prepare(enc28j60_pkt_t *pkt, uint8_t *da, u16 type);
void		enc28j60_write(const enc28j60_pkt_t *pkt, u16 len);
u16			enc28j60_read(enc28j60_pkt_t *pkt);

#endif
