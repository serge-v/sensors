#include <Arduino.h>
#include <util/crc16.h>
#include "debug.h"
#include <rfm12b.h>

uint8_t rf12_cmd(uint8_t highbyte, uint8_t lowbyte);
void rf12_spi_init(void);
uint16_t rf12_read_status(void);
uint8_t rf12_read_status_MSB(void);

volatile int c = 0;
unsigned long last_dump = 0;
unsigned long last_send = 0;

void setup()
{
	Serial.begin(9600);
	rf12_initialize(10, 212);
	rf12_rx_on();
	last_send = last_dump = millis();
	Serial.println("blink-t85. rx on.");
}

uint8_t buf[20];
uint8_t sbuf[20];
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

static void tx(uint8_t c)
{
	while (!rf12_read_status_MSB());
	rf12_cmd(0xB8, c);
}

static void send_status()
{
	rf12_rx_off();

	int n = sprintf((char*)sbuf, "s,%lu\n", millis()); 
	
	uint16_t crc = ~0;
	crc = _crc16_update(crc, 212);
	crc = _crc16_update(crc, 0xA);
	crc = _crc16_update(crc, n);
	for (uint8_t i = 0; i < n; i++)
		crc = _crc16_update(crc, sbuf[i]);
	
	rf12_reset_fifo();

	rf12_cmd(0x82, 0x3D); // start tx

	tx(0xAA);        // preamble
	tx(0x2D);        // sync hi byte
	tx(0xD4);        // sync low byte
	tx(0x0A);        // id
	tx(n);           // len
	for (uint8_t i = 0; i < n; i++)
		tx(sbuf[i]);
	
	tx(crc & 0xFF);  // crc lo
	tx(crc >> 8);    // crc hi
	tx(0);           // dummy byte
	while (!rf12_read_status_MSB()); // wait dummy byte
	rf12_cmd(0x82, 0x0D); // idle
	Serial.print((char*)sbuf);
	Serial.print(" ");
	Serial.print(crc & 0xFF, HEX);
	Serial.print(" ");
	Serial.println(crc >> 8, HEX);
	rf12_rx_on();
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

		if (millis() - last_send > 20000)
		{
			send_status();
			last_send = millis();
		}
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


