#ifndef _SRC_COMMANDS_H
#define _SRC_COMMANDS_H

#include "usart.h"

#define COMMAND_FLAG_BYTE				0xD3

typedef enum __attribute__ (( packed ))
{
	COMMAND_ERROR_MOTOR_NOT_KNOWN = 0,
	COMMAND_ERROR_NOT_KNOWN
} command_error_t;

typedef enum __attribute__ (( packed ))
{
	COMMAND_LOG = 0,
	COMMAND_SET_MOTOR_POS,
	COMMAND_SET_MOTOR_MIN_SPS,
	COMMAND_SET_MOTOR_MAX_SPS,
	COMMAND_SET_MOTOR_ENABLED,
	COMMAND_READ_MOTOR_STATUS,
	COMMAND_READ_INFO
} command_type_t;

typedef struct __attribute__ (( packed ))
{
	int32_t pos;
	uint16_t current_sps;
	unsigned enabled 	: 1;
	unsigned moving 	: 1;
	unsigned reserved	: 6;
} command_response_motor_status_t;

typedef struct __attribute__ (( packed ))
{
	uint8_t motor_count;
	char device_id[8];
} command_arg_info_t;

typedef struct __attribute__ (( packed ))
{
	command_error_t code;
} command_arg_error_t;

typedef struct __attribute__ (( packed ))
{
	uint8_t motor;
	int32_t pos;
} command_arg_set_motor_pos_t;

typedef struct __attribute__ (( packed ))
{
	unsigned error : 1;
	unsigned reserved : 7;
} command_hdr_flags_t;;

typedef struct __attribute__ (( packed ))
{
	command_type_t type;
	command_hdr_flags_t flags;
} command_hdr_t;

typedef struct __attribute__ (( packed ))
{
	uint16_t size;
	uint8_t payload[];
} command_body_t;

typedef struct __attribute__ (( packed )) {
	command_hdr_t hdr;
	command_body_t body;
} command_packet_t;

void command_write_byte(uint8_t b);
uint8_t command_read_byte(void);

void command_read(char *buffer, uint32_t size);
void command_write(command_packet_t *pkt);

#endif
