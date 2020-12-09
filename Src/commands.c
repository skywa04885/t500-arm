#include "commands.h"

void command_write_byte(uint8_t b)
{
	usart_write_char((b >> 4) & 0x0F);
	usart_write_char(b & 0x0F);
}

uint8_t command_read_byte(void)
{
	uint8_t res, temp;

	// Reads the higher nibble
	temp = usart_read_char();
	if (temp == COMMAND_FLAG_BYTE) return temp;
	res = (temp & 0x0F)<< 4;

	// Reads the lower nibble
	temp = usart_read_char();
	if (temp == COMMAND_FLAG_BYTE) return temp;
	res |= (temp & 0x0F);

	return res;
}

void command_read(char *buffer, uint32_t size)
{
	uint32_t i = 0;
	uint8_t b;

	// Waits for the start flag
	while (command_read_byte() != COMMAND_FLAG_BYTE);

	// Starts reading the command
	while (i < size)
	{
		b = command_read_byte();
		if (b == COMMAND_FLAG_BYTE) break;
		else buffer[i++] = b;
	}
}

void command_write(command_packet_t *pkt)
{
	usart_write_char(COMMAND_FLAG_BYTE);

	// Writes the packet
	for (uint8_t i = 0; i < sizeof (command_packet_t); ++i)
		command_write_byte(((uint8_t *) pkt)[i]);

	// Writes the packet payload
	for (uint16_t i = 0; i < pkt->body.size; ++i)
		command_write_byte(pkt->body.payload[i]);

	usart_write_char(COMMAND_FLAG_BYTE);
}
