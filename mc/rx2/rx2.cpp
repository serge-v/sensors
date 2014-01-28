#include <avr/io.h>
#include <util/crc16.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "trx.h"

int test = 2;
byte c1 = 0, c2 = 0;
byte wait_irq = 1;
byte buf[10];
byte idx = 0;
int total_chars = 0;
byte debug = 1;
byte spins = 0;
byte loop_count = 0;

SoftwareSerial com3(0, 1);

static void space()
{
	delay(150);
}

static void blink_DE()
{
	dash();	dot(); dot(); space();
	dot(); space();
	dot(); dash(); dot(); dot(); space();
}

void setup()
{
	pinMode(LED_PIN, OUTPUT);
	com3.begin(4800);
	com3.println("started");

	rf12_setup();
	// wait for init
//	while (!Serial);
	// wait for any characted from usb. Otherwise "cat /dev/ttyACM0" doesn't work.
//	while (!Serial.available());
//	Serial.read();
	blink_DE();
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
	for (int i = 2; i < len - 3; i++)
		crc = _crc16_update(crc, buf[i]);

	uint16_t expected_crc = *(uint16_t*)(buf + len + 2);
	return (expected_crc == crc);
}

static void blink_S()
{
	dot(); dot(); dot(); space();
}

static void blink_TO()
{ 
	dash(); space();
	dash(); dash(); dash(); space();
}

static void blink_B()
{
	dash(); space();
	dot(); dot(); dot(); space();
}

static void test2()
{
	uint8_t signaled = rf12_wait_nirq();

	if (!signaled)
	{
		if (debug)
			blink_TO();
		return;
	}
	
	uint16_t c = rf12_read_status();

	if ((c & 0x8000) == 0)
	{
		if (debug)
		{
			Serial.print("bad status:");
			Serial.println(c, HEX);
			blink_B();
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
			Serial.println((char)buf[2]);
			dot(); dash(); dash();
			dot();
			dot();
			space();
		}
		else
		{
			Serial.println(" crc error");
			dot(); dot(); space();
		}
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
/*	test = Serial.parseInt();
	if (test == 2)
		rf12_setup();
	Serial.print("setup test: ");
	Serial.println(test);
*/
	test = 2;
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
#ifdef PORTC
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
	Serial.print("irq: ");
	Serial.println(PIND & _BV(PD2));
#endif
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
/*
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
*/
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
//			read_command(&c1, &c2);
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

	test2();
	
	if (loop_count > 10)
		debug = 0;
	else
		loop_count++;
}
