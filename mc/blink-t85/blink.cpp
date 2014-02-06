#include <Arduino.h>
#include <util/crc16.h>
#include "debug.h"
#include <rfm12b.h>

uint8_t rf12_cmd(uint8_t highbyte, uint8_t lowbyte);
void rf12_spi_init(void);
uint16_t rf12_read_status(void);
uint8_t rf12_read_status_MSB(void);

void setup()
{
	dump();
//	led_init();
	Serial.begin(9600);
	Serial.println("s1");
	dump();
	rf12_initialize(10, 212);
	dump();
	Serial.println("s2");

	rf12_rx_on();
}

volatile int c = 0;
unsigned long last_dump = 0;
unsigned long last_send = 0;

uint8_t buf[20];
uint8_t idx = 0;
uint8_t len = 0;
unsigned int timeouts = 0;

uint8_t verify_buf()
{
	uint16_t crc = ~0;
	crc = _crc16_update(crc, 212);

	uint8_t i = 0;
	for (i = 0; i < len + 2; i++)
		crc = _crc16_update(crc, buf[i]);

	uint16_t expected_crc = buf[i++];
	expected_crc |= buf[i] << 8;

	return (expected_crc == crc);
}

void reset_buf()
{
	if (verify_buf())
	{
		buf[len+2] = 0;
		Serial.print("  ");
		Serial.print((char*)(buf+2));
	}
	else
	{
		if (idx > 0)
		{
			Serial.print("  ");
			for (int i = 0; i < idx; i++)
			{
				Serial.print(buf[i], HEX);
				Serial.print(" ");
			}
			Serial.println();
		}
		Serial.print("  n: ");
		Serial.print(len, DEC);
		Serial.print(" t: ");
		Serial.println(timeouts, DEC);
	}

	idx = 0;
	len = 0;
	rf12_rx_on();
	last_dump = millis();
}

void loop()
{
	uint16_t cnt = 0xFFFF;

	while (!rf12_read_status_MSB() && --cnt);

	if (cnt == 0)
	{
		timeouts++;
		if (millis() - last_dump > 5000)
			reset_buf();
		return;
	}

	uint8_t c = rf12_cmd(0xB0, 0x00);
	buf[idx++] = c;

	if (idx == 2)
		len = c;

	if (len >= 20)
		len = 20;

	timeouts = 0;

	if (idx == len + 4)
		reset_buf();
}


