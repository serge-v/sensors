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
byte loop_count = 0;

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

static void test2()
{
	uint8_t signaled = rf12_wait_nirq();

	if (!signaled)
	{
		if (debug)
			Serial.print('.');
		return;
	}
	
	uint16_t c = rf12_read_status();

	if ((c & 0x8000) == 0)
	{
		if (debug)
		{
			Serial.println("bad status:");
			Serial.println(c, HEX);
		}
		return;
	}

	if (debug)
	{
		Serial.print("status:");
		Serial.println(c, HEX);
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
}

static void setup_test()
{
	test = Serial.parseInt();
	if (test == 2)
	{
		rf12_setup();
		Serial.print("rf12_setup done");
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
		case 'p':
			rf12_spi_init();
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
