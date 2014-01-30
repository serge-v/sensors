#include <avr/io.h>
#include <util/crc16.h>
#include <Arduino.h>
//#include <SoftwareSerial.h>
#include "trx.h"

int test = 2;
byte c1 = 0, c2 = 0;
byte wait_irq = 1;
byte buf[10];
byte idx = 0;
int total_chars = 0;
byte debug = 0;
byte spins = 0;
byte send_response = 0;
byte rx_is_on = 1;
byte delay_us = 0;

#include "debug.h"

//SoftwareSerial com3(0, 1);

void setup()
{
	pinMode(LED_PIN, OUTPUT);
	Serial.begin(115200);

	// wait for init
	while (!Serial);
	// wait for any characted from usb. Otherwise "cat /dev/ttyACM0" doesn't work.
	while (!Serial.available());
	Serial.read();
	Serial.println("started");

	rf12_setup();
	Serial.println("rf12_setup done");
}

// buffer format:
// [0]            : group
// [1]            : len
// [2]..[len-3]   : data
// [len-2][len-1] : crc

static uint8_t verify_buf(uint8_t group, uint8_t* buf, uint8_t buflen)
{
	if (buflen < 5)
		return 0;

	uint8_t len = buf[1];
	uint16_t crc = ~0;
	crc = _crc16_update(crc, group);
	crc = _crc16_update(crc, buf[0]); // network id
	crc = _crc16_update(crc, len);
	for (int i = 2; i < buflen - 2; i++)
		crc = _crc16_update(crc, buf[i]);

	uint16_t expected_crc = *(uint16_t*)(buf + len + 2);

	if (expected_crc != crc)
	{
		Serial.print("buf:");
		for (int i = 0; i < 5; i++)
		{
			Serial.print(' ');
			Serial.print(buf[i], HEX);

		}
		Serial.println();
		Serial.print("len: ");
		Serial.print(len, HEX);
		Serial.print(" ex: ");
		Serial.print(expected_crc, HEX);
		Serial.print(" calc: ");
		Serial.println(crc, HEX);
	}
	
	return (expected_crc == crc);
}

#define WAIT_IRQ_LO() while( IRQ_PORT & _BV(IRQ_PIN) );

void tx_byte(byte b)
{
	byte irq1 = (IRQ_PORT & _BV(IRQ_PIN));
	WAIT_IRQ_LO();
	byte cmd = rf12_cmd(0xB8, b);
/*	if (!rf12_wait_nirq())
	{
		Serial.println(" err tx hdr");
	} */
//	byte status = rf12_read_status();
//	WAIT_IRQ_HI();
	byte irq2 = (IRQ_PORT & _BV(IRQ_PIN));
	if (debug)
	{
		Serial.print(" cmd: ");
		Serial.print(cmd, HEX);
//		Serial.print(" st: ");
//		Serial.print(status, HEX);
		Serial.print(" irq1: ");
		Serial.print(irq1, HEX);
		Serial.print(" irq2: ");
		Serial.println(irq2, HEX);
	}
}

volatile byte sidx = 0;
byte sbuf[8];

static void rf12_interrupt()
{
	rf12_cmd(0x00, 0x00);
	rf12_cmd(0xB8, sbuf[sidx]);
	sidx++;
	if (sidx == 8)
		Serial.println("sbuf sent");
}

static void respond()
{
	rf12_rx_off();
	rf12_reset_fifo();
	Serial.print("resp");
	
	sidx = 0;
	attachInterrupt(0, rf12_interrupt, LOW);
	rf12_tx_on();

	sbuf[0] = 0xAA;
	sbuf[1] = 0x2D;
	sbuf[2] = 0xD4;
	
	int i = 0;
	for (i = 0; i < 5; i++)
		sbuf[i+3] = buf[i];

	sbuf[i] = 0;
	
	while(sidx < 8);

	rf12_tx_off();
	detachInterrupt(0);
	Serial.println(" sent");
	rf12_rx_on();
}

static void test2()
{
	uint8_t signaled = rf12_wait_nirq();

	if (!signaled)
	{
		return;
	}
	
	uint16_t c = rf12_read_status();

	if ((c & 0x8000) == 0)
	{
		if (debug)
		{
			Serial.print("bad status:");
			Serial.println(c, HEX);
		}
		return;
	}
	
	while (!(IRQ_PORT & _BV(IRQ_PIN)) && idx < 5)
	{
		buf[idx++] = rf12_rx_slow();
		total_chars++;
	}

	if (idx == 5 && buf[0] == 0xA && buf[1] == 1)
	{
		if (verify_buf(212, buf, 5))
		{
			Serial.print((char)buf[2]);
			Serial.print(' ');
			dot();
			send_response = 1;
		}
		else
			Serial.println(" crc error");
		idx = spins = 0;
		rf12_reset_fifo();
	}

	spins++;
	if (spins > 100)
	{
		idx = spins = 0;
		rf12_reset_fifo();
	}
	
	if (send_response)
	{
		respond();
		send_response = 0;
	}
}

static void setup_test()
{
	test = Serial.parseInt();
	if (test == 2)
	{
		rf12_setup();
		Serial.println("rf12_setup done");
	}
}

static void send_command()
{
	byte rc = rf12_cmd(c1, c2);
	Serial.print("sent: ");
	Serial.print(c1, HEX);
	Serial.print(' ');
	Serial.print(c2, HEX);
	Serial.print(" rc: ");
	Serial.println(rc, HEX);
}

static void read_command(byte* c1, byte* c2)
{
	byte c = Serial.read();
	if (c != ' ')
	{
		Serial.print("bad separator 1: ");
		Serial.println(c);
		return;
	}

	char s[2];

	if (Serial.readBytes(s, 2) != 2)
	{
		Serial.print("bad s1: ");
		Serial.println(s);
		return;
	}

	*c1 = strtoul(s, NULL, 16);
	*s = 0;

	c = Serial.read();
	if (c != ' ')
	{
		Serial.print("bad separator 2: ");
		Serial.println(c);
		return;
	}

	if (Serial.readBytes(s, 2) != 2)
	{
		Serial.println("bad s2: ");
		Serial.println(s);
		return;
	}

	*c2 = strtoul(s, NULL, 16);

	Serial.print("cmd: ");
	Serial.print(*c1, HEX);
	Serial.print(' ');
	Serial.println(*c2, HEX);
}

void loop()
{
	while (Serial.available() > 0)
	{
		delay(100);
		char c = Serial.read();
		switch (c)
		{
		case 't':
			setup_test();
			break;
		case 'c':
			read_command(&c1, &c2);
			break;
		case 's':
			send_command();
			break;
		case 'w':
			wipe();
			break;
		case 'q':
			wait_irq = !wait_irq;
			break;
		case 'g':
			debug = !debug;
			break;
		case 'd':
			dump();
			break;
		case 'a':
			delay_us++;
			Serial.print("delay: ");
			Serial.println(delay_us);
			break;
		case 'z':
			delay_us--;
			Serial.print("delay: ");
			Serial.println(delay_us);
			break;
		case 'p':
			rf12_spi_init();
			break;
		case 'x':
			rx_is_on = !rx_is_on;
			if (rx_is_on)
			{
				rf12_rx_on();
				Serial.println("rx is on");
			}
			else
			{
				rf12_rx_off();
				Serial.println("rx is off");
			}
			break;
		}
	}

	if (test == 2)
		test2();
	else
		blink_S();
	/*
	if (loop_count > 10)
		debug = 0;
	else
		loop_count++;
	*/
}
