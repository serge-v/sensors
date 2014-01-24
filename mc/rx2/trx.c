#include <Arduino.h>
#include "trx.h"

void dot()
{
	digitalWrite(7, HIGH);
	delay(50);
	digitalWrite(7, LOW);
	delay(100);
}

// returns: 0 -- timeout, 1 -- nIRQ signaled
uint8_t rf12_wait_nirq()
{
	unsigned int cnt = 0xFFFF;
	while ((PIND & _BV(PD2)) && --cnt);
	return (cnt != 0);
}

uint16_t crc16_update(uint16_t crc, uint8_t a)
{
	int i;

	crc ^= a;
	for (i = 0; i < 8; ++i)
	{
		if (crc & 1)
			crc = (crc >> 1) ^ 0xA001;
		else
			crc = (crc >> 1);
	}

	return crc;
}

uint16_t rf12_read_status()
{
	RF12_SELECT;
	SPDR = 0x00;
	while (!(SPSR & _BV(SPIF)));
	uint16_t c = SPDR << 8;
	SPDR = 0x00;
	while (!(SPSR & _BV(SPIF)));
	c |= SPDR;
	RF12_UNSELECT;
	return c;
}

uint8_t rf12_rx_slow()
{
	// slow down to under 2.5 MHz
	bitSet(SPCR, SPR0);

	RF12_SELECT;
	SPDR = 0xB0;
	while (!(SPSR & _BV(SPIF)));
	SPDR = 0x00;
	while (!(SPSR & _BV(SPIF)));
	RF12_UNSELECT;
	char c = SPDR;

	bitClear(SPCR, SPR0);

	return c;
}

uint8_t rf12_rx()
{
	RF12_SELECT;
	SPDR = 0xB0;
	while (!(SPSR & _BV(SPIF)));
	SPDR = 0x00;
	while (!(SPSR & _BV(SPIF)));
	RF12_UNSELECT;
	return SPDR;
}

uint8_t rf12_cmd(uint8_t highbyte, uint8_t lowbyte)
{
	RF12_SELECT;
	SPDR = highbyte;
	while (!(SPSR & _BV(SPIF)));
	SPDR = lowbyte;
	while (!(SPSR & _BV(SPIF)));
	RF12_UNSELECT;
	return SPDR;
}

void rf12_spi_init()
{
	bitSet(SS_PORT, SPI_SS);
	bitSet(SS_DDR, SPI_SS);
	digitalWrite(SPI_SS, 1);
	pinMode(SPI_SS, OUTPUT);
	pinMode(SPI_MOSI, OUTPUT);
	pinMode(SPI_MISO, INPUT);
	pinMode(SPI_SCK, OUTPUT);
	SPCR = _BV(SPE) | _BV(MSTR);
	// use clk/2 (2x 1/4th) for sending (and clk/8 for recv, see rf12_xferSlow)
	SPSR |= _BV(SPI2X);

	DDRD &= ~_BV(RFM_IRQ); // IRQ input
	PORTD |= _BV(RFM_IRQ); // IRQ pullup
}

void rf12_reset_fifo()
{
	rf12_cmd(0xCA, 0x81); // clear ef bit
	rf12_cmd(0xCA, 0x83); // set ef bit
}

void rf12_setup()
{
	rf12_spi_init();
	rf12_cmd(0, 0);
	rf12_cmd(0x82, 0x05);
	rf12_cmd(0x80, 0xD8); // EL (ena TX), EF (ena RX FIFO), 12.0pF // m: 80D7
	rf12_cmd(0xA6, 0x40); // 433.26MHz
	rf12_cmd(0xC6, 0x06); // approx 49.2 Kbps, i.e. 10000/29/(1+6) Kbps
	rf12_cmd(0x94, 0xA2); // VDI,FAST,134kHz,0dBm,-91dBm
	rf12_cmd(0xC2, 0xAC); // AL,!ml,DIG,DQD4
	rf12_cmd(0xCA, 0x81); // FIFO8,2-SYNC,!ff,DR
	rf12_cmd(0xCE, 0xd4); // SYNC=2DXX
	rf12_cmd(0xC4, 0x83); // @PWR,NO RSTRIC,!st,!fi,OE,EN
	rf12_cmd(0x98, 0x50); // !mp,90kHz,MAX OUT
	rf12_cmd(0xCC, 0x77); // OB1,OB0, LPX,!ddy,DDIT,BW0. CC67
	rf12_cmd(0xE0, 0x00); // NOT USE
	rf12_cmd(0xC8, 0x00); // NOT USE
	rf12_cmd(0xC0, 0x49); // 1.66MHz,3.1V  -- change V
	rf12_cmd(0x82, 0xDD); // receiver on. 8239
	rf12_reset_fifo();
}
