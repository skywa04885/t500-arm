/*
 * MicroChip ENC28J60 Peripheral Driver
 *
 * Datasheet: http://ww1.microchip.com/downloads/en/devicedoc/39662c.pdf
 * Author: Luke A.C.A. Rieff
 */

#ifndef _ENC28J60_H
#define _ENC28J60_H

#include "../../include.h"
#include "../../stm32f446.h"

/****************************************************
 * Memory Mapping
 ****************************************************/

/* Bank 0 */

#define ENC28J60_MEM_BK0_ERDPTL			0x00
#define ENC28J60_MEM_BK0_ERDPTH			0x01
#define ENC28J60_MEM_BK0_EWRPTL			0x02
#define ENC28J60_MEM_BK0_EWRPTH			0x03
#define ENC28J60_MEM_BK0_ETXSTL			0x04
#define ENC28J60_MEM_BK0_ETXSTH			0x05
#define ENC28J60_MEM_BK0_ETXNDL			0x06
#define ENC28J60_MEM_BK0_ETXNDH			0x07
#define ENC28J60_MEM_BK0_ERXSTL			0x08
#define ENC28J60_MEM_BK0_ERXSTH			0x09
#define ENC28J60_MEM_BK0_ERXNDL			0x0A
#define ENC28J60_MEM_BK0_ERXNDH			0x0B
#define ENC28J60_MEM_BK0_ERXRDPTL		0x0C
#define ENC28J60_MEM_BK0_ERXRDPTH		0x0D
#define ENC28J60_MEM_BK0_ERXWRPTL		0x0E
#define ENC28J60_MEM_BK0_ERXWRPTH		0x0F

#define ENC28J60_MEM_BK0_EDMASTL			0x10
#define ENC28J60_MEM_BK0_EDMASTH			0x11
#define ENC28J60_MEM_BK0_EDMANDL			0x12
#define ENC28J60_MEM_BK0_EDMANDH			0x13
#define ENC28J60_MEM_BK0_EDMADSTL		0x14
#define ENC28J60_MEM_BK0_EDMADSTH		0x15
#define ENC28J60_MEM_BK0_EDMACSL			0x16
#define ENC28J60_MEM_BK0_EDMACSH			0x17

#define ENC28J60_MEM_BK0_EIE				0x1B
#define ENC28J60_MEM_BK0_EIR				0x1C
#define ENC28J60_MEM_BK0_ESTAT			0x1D
#define ENC28J60_MEM_BK0_ECON2			0x1E
#define ENC28J60_MEM_BK0_ECON1			0x1F

/* Bank 1 */

#define ENC28J60_MEM_BK1_EHT0			0x00
#define ENC28J60_MEM_BK1_EHT1			0x01
#define ENC28J60_MEM_BK1_EHT2			0x02
#define ENC28J60_MEM_BK1_EHT3			0x03
#define ENC28J60_MEM_BK1_EHT4			0x04
#define ENC28J60_MEM_BK1_EHT5			0x05
#define ENC28J60_MEM_BK1_EHT6			0x06
#define ENC28J60_MEM_BK1_EHT7			0x07
#define ENC28J60_MEM_BK1_EPMM0			0x08
#define ENC28J60_MEM_BK1_EPMM1			0x09
#define ENC28J60_MEM_BK1_EPMM2			0x0A
#define ENC28J60_MEM_BK1_EPMM3			0x0B
#define ENC28J60_MEM_BK1_EPMM4			0x0C
#define ENC28J60_MEM_BK1_EPMM5			0x0D
#define ENC28J60_MEM_BK1_EPMM6			0x0E
#define ENC28J60_MEM_BK1_EPMM7			0x0F

#define ENC28J60_MEM_BK1_EPMCSL			0x10
#define ENC28J60_MEM_BK1_EPMCSH			0x11

#define ENC28J60_MEM_BK1_EPMOL			0x14
#define ENC28J60_MEM_BK1_EPMOH			0x15
#define ENC28J60_MEM_BK1_ERXFCON			0x18
#define ENC28J60_MEM_BK1_EPKTCNT			0x19
#define ENC28J60_MEM_BK1_EIE				0x1B
#define ENC28J60_MEM_BK1_EIR				0x1C
#define ENC28J60_MEM_BK1_ESTAT			0x1D
#define ENC28J60_MEM_BK1_ECON2			0x1E
#define ENC28J60_MEM_BK1_ECON1			0x1F

/* Bank 2 */

#define ENC28J60_MEM_BK2_MACON1			0x00
#define ENC28J60_MEM_BK2_MACON3			0x02
#define ENC28J60_MEM_BK2_MACON4			0x03
#define ENC28J60_MEM_BK2_MABBIPG			0x04

#define ENC28J60_MEM_BK2_MAIPGL			0x06
#define ENC28J60_MEM_BK2_MAIPGH			0x07
#define ENC28J60_MEM_BK2_MACLCON1		0x08
#define ENC28J60_MEM_BK2_MACLCON2		0x09
#define ENC28J60_MEM_BK2_MAMXFLL			0x0A
#define ENC28J60_MEM_BK2_MAMXFLH			0x0B

#define ENC28J60_MEM_BK2_MICMD			0x12
#define ENC28J60_MEM_BK2_MIREGADR		0x14
#define ENC28J60_MEM_BK2_MIWRL			0x16
#define ENC28J60_MEM_BK2_MIWRH			0x17
#define ENC28J60_MEM_BK2_MIRDL			0x18
#define ENC28J60_MEM_BK2_MIRDH			0x19

#define ENC28J60_MEM_BK2_EIE				0x1B
#define ENC28J60_MEM_BK2_EIR				0x1C
#define ENC28J60_MEM_BK2_ESTAT			0x1D
#define ENC28J60_MEM_BK2_ECON2			0x1E
#define ENC28J60_MEM_BK2_ECON1			0x1F

/* Bank 3 */

#define ENC28J60_MEM_BK3_MAADR5			0x00
#define ENC28J60_MEM_BK3_MAADR6			0x01
#define ENC28J60_MEM_BK3_MAADR3			0x02
#define ENC28J60_MEM_BK3_MAADR4			0x03
#define ENC28J60_MEM_BK3_MAADR1			0x04
#define ENC28J60_MEM_BK3_MAADR2			0x05
#define ENC28J60_MEM_BK3_EBSTSD			0x06
#define ENC28J60_MEM_BK3_EBSTCON			0x07
#define ENC28J60_MEM_BK3_EBSTCSL			0x08
#define ENC28J60_MEM_BK3_EBSTCSH			0x09
#define ENC28J60_MEM_BK3_MISTAT			0x0A

#define ENC28J60_MEM_BK3_EREVID			0x12
#define ENC28J60_MEM_BK3_ECOCON			0x15
#define ENC28J60_MEM_BK3_EFLOCON			0x17
#define ENC28J60_MEM_BK3_EPAUSL			0x18
#define ENC28J60_MEM_BK3_EPAUSH			0x19

#define ENC28J60_MEM_BK3_EIE				0x1B
#define ENC28J60_MEM_BK3_EIR				0x1C
#define ENC28J60_MEM_BK3_ESTAT			0x1D
#define ENC28J60_MEM_BK3_ECON2			0x1E
#define ENC28J60_MEM_BK3_ECON1			0x1F

/****************************************************
 * Registers Descriptions
 ****************************************************/

/* ECON1 */

typedef enum {
	ENC28J60_BANK0 = 0b00,
	ENC28J60_BANK1,
	ENC28J60_BANK2,
	ENC28J60_BANK3
} enc28j60_econ1_bsel_t;

typedef struct {
	unsigned txrst		: 1;	/* Transmit Logic Reset bit */
	unsigned rxrst		: 1;	/* Receive Logic Reset bit */
	unsigned dmast		: 1;	/* DMA Start and Busy Status bit */
	unsigned csumen		: 1;	/* DMA Checksum Enable bit */
	unsigned txrts		: 1;	/* Transmit Request to Send bit */
	unsigned rxen		: 1;	/* Receive Enable bit */
	unsigned bsel		: 2;	/* Bank Select bits */
} enc28j60_econ1_t;

/* ECON2 */

typedef struct {
	unsigned autoinc	: 1;	/* Automatic Buffer Pointer Increment Enable bit */
	unsigned pktdec		: 1;	/* Packet Decrement bit */
	unsigned pwrsv		: 1;	/* Power Save Enable bit */
	unsigned reserved1	: 1;	/* Always Zero */
	unsigned vrps		: 1;	/* Voltage Regulator Power Save Enable bit */
	unsigned reserved2	: 3;	/* Always Zero */
} enc28j60_econ2_t;

/* ECOCON */

typedef enum {
	/* 25Mhz 	*/ ENC28J60_ECOCON_COCON_CLKDIV1 = 0b001,
	/* 12.5Mhz 	*/ ENC28J60_ECOCON_COCON_CLKDIV2,
	/* 8.33Mhz 	*/ ENC28J60_ECOCON_COCON_CLKDIV3,
	/* 6.25Mhz 	*/ ENC28J60_ECOCON_COCON_CLKDIV4,
	/* 3.125Mhz */ ENC28J60_ECOCON_COCON_CLKDIV8 = 0b101
} enc28j60_ecocon_cocon_t;

typedef struct {
	unsigned reserved 	: 5;
	unsigned cocon		: 3;
} enc28j60_ecocon_t;

/* PHLCON */

typedef enum {
	ENC28J60_PHLCON_LACFG_RESERVED1	= 0b0000,
	ENC28J60_PHLCON_LACFG_DISPLAY_TRANSMIT_ACTIVITY,
	ENC28J60_PHLCON_LACFG_DISPLAY_RECEIVE_ACTIVITY,
	ENC28J60_PHLCON_LACFG_DISPLAY_COLLISION_ACTIVITY,
	ENC28J60_PHLCON_LACFG_DISPLAY_LINK_STATUS,
	ENC28J60_PHLCON_LACFG_DISPLAY_DUPLEX_STATUS,
	ENC28J60_PHLCON_LACFG_RESERVED2,
	ENC28J60_PHLCON_LACFG_DISPLAY_TRANSMIT_AND_RECEIVE_ACTIVITY,
	ENC28J60_PHLCON_LACFG_ON,
	ENC28J60_PHLCON_LACFG_OFF,
	ENC28J60_PHLCON_LACFG_BLINK_FAST,
	ENC28J60_PHLCON_LACFG_BLINK_SLOW,
	ENC28J60_PHLCON_LACFG_DISPLAY_LINK_STATUS_AND_RECEIVE_ACTIVITY,
	ENC28J60_PHLCON_LACFG_DISPLAY_LINK_STATUS_AND_RXTX_ACTIVITY,
	ENC28J60_PHLCON_LACFG_DISPLAY_DUPLEX_STATUS_AND_COLLISION_ACTIVITY,
	ENC28J60_PHLCON_LACFG_RESERVED3
} enc28j60_phlcon_lxcfg_t;

typedef enum {
	ENC28J60_PHLCON_LFRQ_STRETCH_LED_EVENTS_BY_NS,
	ENC28J60_PHLCON_LFRQ_STRETCH_LED_EVENTS_BY_MS,
	ENC28J60_PHLCON_LFRQ_STRETCH_LED_EVENTS_BY_LS,
	ENC28J60_PHLCON_RESERVED
} enc28j60_phlcon_lfrq_t;

typedef struct {
	unsigned reserved1	: 4;
	unsigned lacfg		: 4;	/* LEDA Configuration Bits */
	unsigned lbcfg		: 4;	/* LEDB Configuration Bits */
	unsigned lfrq		: 2;	/* LED Pulse Stretch Time Configuration Bits */
	unsigned strch		: 1;	/* LED Pulse Stretching Enable Bit */
	unsigned reserved2	: 1;
} enc28j60_phlcon_t;

/****************************************************
 * Types
 ****************************************************/

typedef struct {
	/* Peripherals */
	uint32_t gpio_base;
	uint32_t gpio_cs;
	/* Device Addresses */
	uint8_t mac_addr[4];
	uint8_t ipv4_addr[4];
} enc28j60_config_t;

/****************************************************
 * Prototypes
 ****************************************************/

/**
 * Initializes the ENC28J60 Peripheral
 */
void enc28j60_init(enc28j60_config_t *cfg);

/**
 * Polls the ESTAT register until CLKRDY is set, which means
 *  that the oscillator is stable
 */
void enc28j60_await_clkrdy(enc28j60_config_t *cfg);

#endif
