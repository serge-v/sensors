#include <avr/io.h>
#include <util/crc16.h>
#include <Arduino.h>
#include "trx.h"

const int led_pin = 7;

int test = 0;
byte c1 = 0, c2 = 0;
byte wait_irq = 1;
byte buf[10];
byte idx = 0;
int total_chars = 0;
byte debug = 0;


void setup()
{
	pinMode(led_pin, OUTPUT);
	Serial.begin(115200);
	// wait for init
	while (!Serial);
	// wait for any characted from usb. Otherwise "cat /dev/ttyACM0" doesn't work.
	while (!Serial.available());
	Serial.read();
	Serial.println("started");
}

uint16_t rf12_read_fifo()
{
	bitSet(SPCR, SPR0);
	RF12_SELECT;

	SPDR = 0x00;
	while (!(SPSR & _BV(SPIF)));
	if (debug)
	{
		Serial.print("f1: ");
		Serial.println(SPDR, HEX);
	}
	SPDR = 0x00;
	while (!(SPSR & _BV(SPIF)));
	if (debug)
	{
		Serial.print("f2: ");
		Serial.println(SPDR, HEX);
	}

	SPDR = 0x00;
	while (!(SPSR & _BV(SPIF)));
	byte c = SPDR;

	if (debug)
	{
		Serial.print("f3: ");
		Serial.println(c, HEX);
	}

	RF12_UNSELECT;
	bitClear(SPCR, SPR0);

	return c;
}


static void test2()
{
	while (PIND & _BV(PD2));

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

	if (debug)
	{
		Serial.print("status:");
		Serial.println(c, HEX);
	}

	while (!(PIND & _BV(PD2)) && idx < 5)
	{
		buf[idx++] = rf12_rx_slow();
		total_chars++;
	}

	if (idx == 5 && buf[0] == 0xA && buf[1] == 1)
	{
		uint8_t len = buf[1];
		uint16_t crc = ~0;
		crc = _crc16_update(crc, 212);
		crc = _crc16_update(crc, buf[0]);
		crc = _crc16_update(crc, len);
		crc = _crc16_update(crc, buf[2]);
		Serial.print("buf: ");
		for (int i = 0; i < 5; i++)
		{
			Serial.print(" ");
			Serial.print(buf[i], HEX);
		}
		uint16_t expected_crc = *(uint16_t*)(buf + len + 2);
		if (expected_crc == crc)
		{
			Serial.println(" : ok");
			dot();
		}
		else
		{
			Serial.print(" crc error: ");
			Serial.print(expected_crc, HEX);
			Serial.print(" != ");
			Serial.println(crc, HEX);
			dot();
			dot();
		}
		idx = 0;
		rf12_reset_fifo();
	}
}

static void setup_test()
{
	test = Serial.parseInt();
	if (test == 2)
		rf12_setup();
	Serial.print("setup test: ");
	Serial.println(test);
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

static void dump()
{
	Serial.print("PORTB: ");
	Serial.print(PORTB, HEX);
	Serial.print("  ");
	Serial.println(PORTB, BIN);
	Serial.print("DDRB: ");
	Serial.print(DDRB, HEX);
	Serial.print("  ");
	Serial.println(DDRB, BIN);
	Serial.print("PINB: ");
	Serial.print(PINB, HEX);
	Serial.print("  ");
	Serial.println(PINB, BIN);

	Serial.print("PORTC: ");
	Serial.print(PORTC, HEX);
	Serial.print("  ");
	Serial.println(PORTC, BIN);
	Serial.print("DDRC: ");
	Serial.print(DDRC, HEX);
	Serial.print("  ");
	Serial.println(DDRC, BIN);
	Serial.print("PINC: ");
	Serial.print(PINC, HEX);
	Serial.print("  ");
	Serial.println(PINC, BIN);

	Serial.print("PORTD: ");
	Serial.print(PORTD, HEX);
	Serial.print("  ");
	Serial.println(PORTD, BIN);
	Serial.print("DDRD: ");
	Serial.print(DDRD, HEX);
	Serial.print("  ");
	Serial.println(DDRD, BIN);
	Serial.print("PIND: ");
	Serial.print(PIND, HEX);
	Serial.print("  ");
	Serial.println(PIND, BIN);

	Serial.print("SPCR: ");
	Serial.print(SPCR, HEX);
	Serial.print("  ");
	Serial.println(SPCR, BIN);
	Serial.print("SPSR: ");
	Serial.print(SPSR, HEX);
	Serial.print("  ");
	Serial.println(SPSR, BIN);
	Serial.print("EIMSK: ");
	Serial.print(EIMSK, HEX);
	Serial.print("  ");
	Serial.println(EIMSK, BIN);
	Serial.print("wait_irq: ");
	Serial.println(wait_irq);

	Serial.print("buf: ");
	for (int i = 0; i < 10; i++)
	{
		Serial.print(buf[i], HEX);
		Serial.print(' ');
	}

	Serial.print(" idx: ");
	Serial.println(idx);
	Serial.print("chars: ");
	Serial.println(total_chars);
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

static void wipe()
{
	for (int i = 0; i < 10; i++)
		Serial.println();
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
	else if (test == 5)
	{
		dot();
		dot();
		dot();
		delay(2500);
	}
}

