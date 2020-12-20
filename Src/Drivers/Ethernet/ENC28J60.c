#include "../Ethernet/ENC28J60.h"

/**
 * Selects the device
 */
void enc28j60_select(enc28j60_config_t *cfg)
{
	// CS: Low, selected
	*GPIO_ODR(cfg->gpio_base) &= ~(1 << cfg->gpio_cs);
}

/**
 * Deslects the device
 */
void enc28j60_deselect(enc28j60_config_t *cfg)
{
	// CS: High, not selected
	*GPIO_ODR(cfg->gpio_base) |= (1 << cfg->gpio_cs);
}

/**
 * Writes an command to the ENC28J60
 */
void enc28j60_write_cmd(enc28j60_config_t *cfg, uint8_t opcode, uint8_t arg)
{
	spi_transceive(((opcode << 5) & 0b11100000) | (arg & 0b00011111));
}

/**
 * Selects the memory bank of the ENC28J60
 */
void enc28j60_bank_select(enc28j60_config_t *cfg, enc28j60_econ1_bsel_t bank)
{
	uint8_t reg;

	// Reads original value of ECON1
	enc28j60_select(cfg);
	enc28j60_write_cmd(cfg, ENC28J60_OPCODE_RCR, ENC28J60_MEM_BKX_ECON1);
	reg = spi_transceive(0xFF);
	enc28j60_deselect(cfg);

	// Sets the new memory bank
	((enc28j60_econ1_t *) &reg)->bsel = bank;

	// Updates ECON1
	enc28j60_select(cfg);
	enc28j60_write_cmd(cfg, ENC28J60_OPCODE_WCR, ENC28J60_MEM_BKX_ECON1);
	spi_transceive(reg);
	enc28j60_deselect(cfg);
}

/**
 * Initializes the ENC28J60 Peripheral
 */
void enc28j60_init(enc28j60_config_t *cfg)
{
	// Configures GPIO
	*GPIO_MODER(cfg->gpio_base) |= GPIO_MODE(GPIO_OUTPUT, cfg->gpio_cs);
	*GPIO_ODR(cfg->gpio_base) |= (1 << cfg->gpio_cs); // High: not selected
	*GPIO_OSPEEDR(cfg->gpio_base) |= GPIO_SPEED(GPIO_HIGH_SPEED, cfg->gpio_cs);

	// Performs soft reset
	enc28j60_select(cfg);
	enc28j60_write_cmd(cfg, ENC28J60_OPCODE_SRC, 0xFF);
	enc28j60_deselect(cfg);

	// Waits for the clock to be ready
	printf("Waiting for clock\r\n");
	enc28j60_await_clkrdy(cfg);
	printf("Clock ready\r\n");

	// Sets the default MAC address
	enc28j60_set_mac(cfg, cfg->mac_addr);
}

/**
 * Polls the ESTAT register until CLKRDY is set, which means
 *  that the oscillator is stable
 */
void enc28j60_await_clkrdy(enc28j60_config_t *cfg)
{
	uint8_t reg;

	for (;;)
	{
		// Reads the ESTAT register, which will contain the CLKRDY bit
		enc28j60_select(cfg);
		enc28j60_write_cmd(cfg, ENC28J60_OPCODE_RCR, ENC28J60_MEM_BKX_ESTAT);
		reg = spi_transceive(0xFF);
		enc28j60_deselect(cfg);

		// Checks if the bit is set
		if (((enc28j60_estat_t *) &reg)->clkrdy) break;

	}
}

/**
 * Waits for the busy flag to be cleared in the MISTAT register
 */
void enc28j60_mistat_wait_busy(enc28j60_config_t *cfg)
{
	// Selects BANK3
	enc28j60_bank_select(cfg, ENC28J60_BANK3);

	uint8_t reg;
	for (;;)
	{
		// Reads the MISTAT Register which will contain the BUSY flag
		enc28j60_select(cfg);
		enc28j60_write_cmd(cfg, ENC28J60_OPCODE_RCR, ENC28J60_MEM_BK3_MISTAT);
		spi_transceive(0xFF); // Dummy byte
		reg = spi_transceive(0xFF);
		enc28j60_deselect(cfg);

		// Checks if the bit is cleared
		if (!((enc28j60_mistat_t *) &reg)->busy) break;
	}
}

static uint8_t mac_registers[6] = {
		ENC28J60_MEM_BK3_MAADR1,
		ENC28J60_MEM_BK3_MAADR2,
		ENC28J60_MEM_BK3_MAADR3,
		ENC28J60_MEM_BK3_MAADR4,
		ENC28J60_MEM_BK3_MAADR5,
		ENC28J60_MEM_BK3_MAADR6
};

/**
 * Sets the device MAC
 */
void enc28j60_set_mac(enc28j60_config_t *cfg, uint8_t *mac)
{
	for (uint8_t i = 0; i < 6; ++i)
	{
		enc28j60_select(cfg);
		enc28j60_write_cmd(cfg, ENC28J60_OPCODE_WCR, mac_registers[i]);
		spi_transceive(mac[i]);
		enc28j60_deselect(cfg);
	}
}

/**
 * Gets the device MAc
 */
void enc28j60_get_mac(enc28j60_config_t *cfg, uint8_t *mac)
{
	uint8_t reg;

	for (uint8_t i = 0; i < 6; ++i)
	{
		enc28j60_select(cfg);
		enc28j60_write_cmd(cfg, ENC28J60_OPCODE_RCR, mac_registers[i]);
		spi_transceive(0xFF); // Dummy byte
		reg = spi_transceive(0xFF);
		enc28j60_deselect(cfg);

		mac[i] = reg;
	}
}

/**
 * Writes to an PHY register
 */
void enc28j60_phy_write(enc28j60_config_t *cfg, uint8_t address, uint16_t halfword)
{
	// Selects BANK2
	enc28j60_bank_select(cfg, ENC28J60_BANK2);

	// 1. Write the address of the PHY register to write to into the MIREGADR register.
	enc28j60_select(cfg);
	enc28j60_write_cmd(cfg, ENC28J60_OPCODE_WCR, ENC28J60_MEM_BK2_MIREGADR);
	spi_transceive(address);
	enc28j60_deselect(cfg);

	// 2. Write the lower 8 bits of data to write into the MIWRL register.
	enc28j60_select(cfg);
	enc28j60_write_cmd(cfg, ENC28J60_OPCODE_WCR, ENC28J60_MEM_BK2_MIWRL);
	spi_transceive((uint8_t) halfword);
	enc28j60_deselect(cfg);

	// 3. Write the upper 8 bits of data to write into theM IWRH register.
	//  Writing to this register automatically begins the MIIM transaction,
	//  so it must be written to after MIWRL. The MISTAT.BUSY bit becomes set.
	enc28j60_select(cfg);
	enc28j60_write_cmd(cfg, ENC28J60_OPCODE_WCR, ENC28J60_MEM_BK2_MIWRH);
	spi_transceive((uint8_t) (halfword >> 8));
	enc28j60_deselect(cfg);
}

/**
 * Reads the value from an PHY register
 */
uint16_t enc28j60_phy_read(enc28j60_config_t *cfg, uint8_t address)
{
	uint8_t reg8;
	uint16_t reg16;

	// Selects BANK2
	enc28j60_bank_select(cfg, ENC28J60_BANK2);

	// 1. Write the address of the PHY register to read from into the MIREGADR register.
	enc28j60_select(cfg);
	enc28j60_write_cmd(cfg, ENC28J60_OPCODE_WCR, ENC28J60_MEM_BK2_MIREGADR);
	spi_transceive(address);
	enc28j60_deselect(cfg);

	// 2. Set the MICMD.MIIRD bit. The read operation begins and the MISTAT.BUSY bit is set.

	{
		// Reads the current value of MICMD
		enc28j60_select(cfg);
		enc28j60_write_cmd(cfg, ENC28J60_OPCODE_RCR, ENC28J60_MEM_BK2_MICMD);
		spi_transceive(0xFF);
		reg8 = spi_transceive(0xFF);
		enc28j60_deselect(cfg);

		// Sets the MIIRD bit
		((enc28j60_micmd_t *) &reg8)->miird = 1;

		// Writes the new value into MICMD
		enc28j60_select(cfg);
		enc28j60_write_cmd(cfg, ENC28J60_OPCODE_WCR, ENC28J60_MEM_BK2_MICMD);
		spi_transceive(reg8);
		enc28j60_deselect(cfg);
	}

	// 3. Wait 10.24μs. Poll the MISTAT.BUSY bit to be certain that the operation is complete.
	//  While busy, the host controller should not start any MIISCAN operations or write to the MIWRH register.
	//  When the MAC has obtained the register contents, the BUSY bit will clear itself.

	// Waits 10 microseconds, and than polls the wait busy
	delay_us(10);
	enc28j60_mistat_wait_busy(cfg);

	// Selects BANK2, since it was deselected by wait busy
	enc28j60_bank_select(cfg, ENC28J60_BANK2);

	// 4. Clear the MICMD.MIIRD bit.

	{
		// Clears the MIIRD bit
		((enc28j60_micmd_t *) &reg8)->miird = 0;

		// Writes the new value into MICMD
		enc28j60_select(cfg);
		enc28j60_write_cmd(cfg, ENC28J60_OPCODE_WCR, ENC28J60_MEM_BK2_MICMD);
		spi_transceive(reg8);
		enc28j60_deselect(cfg);
	}

	// 5. Read the desired data from the MIRDL and MIRDH registers.
	//  The order that these bytes are accessed is unimportant.

	// Reads the high byte
	enc28j60_select(cfg);
	enc28j60_write_cmd(cfg, ENC28J60_OPCODE_RCR, ENC28J60_MEM_BK2_MIRDH);
	spi_transceive(0xFF);
	reg16 = spi_transceive(0xFF);
	enc28j60_deselect(cfg);

	// Shifts the high byte to the higher bits of reg16
	reg16 <<= 8;

	// Reads the low byte
	enc28j60_select(cfg);
	enc28j60_write_cmd(cfg, ENC28J60_OPCODE_RCR, ENC28J60_MEM_BK2_MIRDL);
	spi_transceive(0xFF);
	reg8 |= spi_transceive(0xFF);
	enc28j60_deselect(cfg);

	// Returns the result
	return reg16;
}

