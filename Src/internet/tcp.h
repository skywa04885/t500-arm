/*
	STM32 ProjectA source code, made by Skywa04885... All Rights Reserved !

	contact: sw04885@fannst.nl
*/

#ifndef _SRC_INTERNET_TCP_H
#define _SRC_INTERNET_TCP_H

/*********************************************
 * Project Headers
 *********************************************/

#include "../types.h"

/*********************************************
 * Data Types
 *********************************************/

typedef enum
{
	TCP_OPTION_KIND_END_OF_OPTION_LIST = 0,
	TCP_OPTION_KIND_NOP,
	TCP_OPTION_KIND_MAG_SEG_SIZE
} tcp_option_kind_t;

typedef struct __attribute__ (( packed ))
{
	u16 		sp;			/* Source Port */
	u16			dp;			/* Destination Port */
	u32			sn;			/* Sequence-Number */
	u32			an;			/* Acknowledgment Number */
	u8			df;			/* Data Offset */
	u8			cb;			/* Control Bits */
	u16			w;			/* Window */
	u16			cs;			/* Checksum */
	u16			up;			/* Urgent Pointer */
	u8			payload[0];
} tcp_pkt_t;

typedef struct __attribute__ (( packed ))
{
	u8			k;			/* Option-Kind */
	u8			l;			/* Option Length */
	u8			data[0];
} tcp_option_t;

typedef struct __attribute__ (( packed ))
{
	u16			val;
	u8			next[0];
} tcp_option_data_u16_t;

/*********************************************
 * Prototypes
 *********************************************/

#endif
