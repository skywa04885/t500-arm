#ifndef _SRC_INTERNET_CUSTOM_CONTROL_H
#define _SRC_INTERNET_CUSTOM_CONTROL_H

/*********************************************
 * C Standard Library
 *********************************************/

#include <string.h>
#include <stdbool.h>

/*********************************************
 * C Standard Library
 *********************************************/

#include "../../types.h"

/*********************************************
 * Macros
 *********************************************/

#define CONTROL_PORT						7588

#define CONTROL_PACKET_FLAG_ERR				0
#define CONTROL_PACKET_FLAG_OK				1

/*********************************************
 * Data Types
 *********************************************/

typedef enum
{
	CONTROL_PKT_MOTOR_MODE_AUTO = 0, 		/* Enable/Disable are not allowed, and managed by software */
	CONTROL_PKT_MOTOR_MODE_MANUAL = 1		/* Enable/Disable are allowed, and managed by controlling software */
} control_pkt_motor_mode_t;

typedef enum
{
	CONTROL_PKT_MOTOR_TYPE_STEPPER = 0,
	CONTROL_PKT_MOTOR_TYPE_SERVO = 1,
	CONTROL_PKT_MOTOR_TYPE_DC = 2
} control_pkt_motor_type_t;

typedef enum
{
	CONTROL_PKT_TYPE_REQUEST = 0,
	CONTROL_PKT_TYPE_REPLY = 1
} control_pkt_type_t;

typedef enum
{
	CONTROL_PKT_OP_MOTOR_INFO = 0,
	CONTROL_PKT_OP_STEPPER_MOVE_TO = 1,
	CONTROL_PKT_OP_STEPPER_ENABLE = 2,
	CONTROL_PKT_OP_STEPPER_DISABLE = 3,
	CONTROL_PKT_OP_STEPPER_STATUS = 4
} control_pkt_opcode_t;

typedef enum
{
	CONTROL_PKT_ARG_TYPE_PADD = 0,
	CONTROL_PKT_ARG_TYPE_MOTOR = 1,
	CONTROL_PKT_ARG_TYPE_STRING = 2,
	CONTROL_PKT_ARG_TYPE_U8 = 3,
	CONTROL_PKT_ARG_TYPE_U16 = 4,
	CONTROL_PKT_ARG_TYPE_U32 = 5,
	CONTROL_PKT_ARG_TYPE_U64 = 6,
	CONTROL_PKT_ARG_TYPE_I8 = 7,
	CONTROL_PKT_ARG_TYPE_I16 = 8,
	CONTROL_PKT_ARG_TYPE_I32 = 9,
	CONTROL_PKT_ARG_TYPE_I64 = 10,
	CONTROL_PKT_ARG_TYPE_MOTOR_STATUS = 11,
	CONTROL_PKT_ARG_TYPE_END = 0xFFFF
} control_pkt_arg_type_t;

/* Signed Numbers */

typedef struct __attribute__ (( packed ))
{
	i8			val;
	u8			next[0];
} control_pkt_arg_i8_t;

typedef struct __attribute__ (( packed ))
{
	i16			val;
	u8			next[0];
} control_pkt_arg_i16_t;

typedef struct __attribute__ (( packed ))
{
	i32			val;
	u8			next[0];
} control_pkt_arg_i32_t;

typedef struct __attribute__ (( packed ))
{
	i64			val;
	u8			next[0];
} control_pkt_arg_i64_t;

/* Unsigned Numbers */

typedef struct __attribute__ (( packed ))
{
	u8			val;
	u8			next[0];
} control_pkt_arg_u8_t;

typedef struct __attribute__ (( packed ))
{
	u16			val;
	u8			next[0];
} control_pkt_arg_u16_t;

typedef struct __attribute__ (( packed ))
{
	u32			val;
	u8			next[0];
} control_pkt_arg_u32_t;

typedef struct __attribute__ (( packed ))
{
	u64			val;
	u8			next[0];
} control_pkt_arg_u64_t;

/* Custom Datatypes */

#define CONTROL_PKT_ARG_MOTOR_FLAG_ENABLED			0
#define CONTROL_PKT_ARG_MOTOR_FLAG_MOVING			1

typedef struct __attribute__ (( packed ))
{
	u8			id;					/* The Motor ID (0-255) */
	u8			t;					/* The Motor Type */
	u8			m;					/* The Motor Mode */
	u16			mnsps;				/* Minimum Steps/Second */
	u16			mxsps;				/* Maximum Steps/Second */
	u8			next[0];
} control_pkt_arg_motor_t;

typedef struct __attribute__ (( packed ))
{
	u8			id;					/* The Motor ID (0-255) */
	u8			flags;				/* The Status Flags */
	u16			sps;				/* Steps / Second */
	i32			cpos;				/* Current Position */
	i32			tpos;				/* Target Position */
	u8			next[0];
} control_pkt_arg_stepper_status_t;

/* Packet itsef */

typedef struct __attribute__ (( packed ))
{
	u16 		t;					/* Argument Type, indicates the data type */
	u16			l;					/* Argument Length, indicates the length of the data */
	u8			payload[0];			/* The payload of the argument */
} control_pkt_arg_t;

typedef struct __attribute__ (( packed ))
{
	char		prefix[10];			/* Packet-Prefix */
	unsigned 	type 		: 1;	/* Packet Type, request (0), reply (1) */
	unsigned 	opcode 		: 7;	/* Operation Code */
	u8			f;					/* Flags */
	u32			sn;					/* Sequence Number */
	u16			tl;					/* Total length of the packet */
	u8			payload[0];			/* The payload of the packet */
} control_pkt_t;

/*********************************************
 * Prototypes
 *********************************************/

void control_pkt_add_prefix(control_pkt_t *pkt);
bool control_pkt_check_prefix(control_pkt_t *pkt);

control_pkt_arg_t *control_pkt_arg_add_motor(control_pkt_arg_t *arg, control_pkt_arg_motor_t *motor);
control_pkt_arg_t *control_pkt_arg_add_stepper_status(control_pkt_arg_t *arg, control_pkt_arg_stepper_status_t *status);
control_pkt_arg_t *control_pkt_arg_end(control_pkt_arg_t *arg);

control_pkt_arg_t *control_pkt_arg_parse_next(control_pkt_arg_t *arg);

#endif
