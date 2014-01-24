#include <avr/io.h>
#include <Arduino.h>
#include "trx.h"

const int led_pin = 7;

int test = 0;
byte c1 = 0, c2 = 0;
byte wait_irq = 1;
byte buf[10];
byte idx = 0;
int total_chars = 0;


void setup()
{
	pinMode(led_pin, OUTPUT);
	Serial.begin(9600);
	while (!Serial);
	while (!Serial.available());
	Serial.read();
	Serial.println("rx2:");
	Serial.println("t1 -- jeelib rx test");
	Serial.println("t2 -- my rx test");
}

/*

- After switching nIRQ POR goes low due to, and / or EXT IRQ
 - Read the status from the Init of the RFM at the end deletes the IRQ

 - Send: after the last byte has been sent to the RFM, immediately
 et = 0.  Otherwise RGIT IRQ is active again and nIRQ low.  This can
 are not cleared by reading the status, only by transmitting a
 Bytes in the TX buffer.  Is of course only when the TX buffer used
 will.

 - Receive: after the last byte has been retrieved from the FIFO immediately
 Off or run FIFO FIFO reset.  Otherwise, the FFIT IRQ
 set and nIRQ is low.  This can only be read out of the FIFO
 be deleted.
 Once the FIFO has even recognized the sync bytes, it also provides no
 active transmitter or data (random values).  Due to the FIFO is reset
 again waiting for sync byte

 - Wake-up timer: Reading the register clears the congestion WKUP IRQ and nIRQ
 is high again

*/

static void test2()
{
	if (wait_irq)
	{
		if (!rf12_wait_nirq())
			return;
	}

	uint16_t c = rf12_read_status();

	if ((c & 0x8000) == 0)
		return;

	idx = 0;

	while ((PIND & _BV(PD2)) && idx < 5)
	{
		buf[idx++] = rf12_rx_slow();
		total_chars++;
	}

	rf12_reset_fifo();

	uint16_t crc = 0xFFFF;
	if (idx == 5 && buf[0] == 'A' && buf[1] == 1)
	{
		crc = crc16_update(crc, buf[3]);
		crc = crc16_update(crc, buf[4]);
		Serial.print("c: ");
		Serial.println(buf[2]); // data
	}
	else
	{
		Serial.print("bad: ");
		for (int i = 0; i < 10; i++)
		{
			Serial.print(' ');
			Serial.print(buf[i]);
		}
		Serial.println();
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
	Serial.println(PORTB, BIN);
	Serial.print("DDRB: ");
	Serial.print(DDRB, HEX);
	Serial.println(DDRB, BIN);
	Serial.print("PINB: ");
	Serial.print(PINB, HEX);
	Serial.println(PINB, BIN);

	Serial.print("PORTC: ");
	Serial.print(PORTC, HEX);
	Serial.println(PORTC, BIN);
	Serial.print("DDRC: ");
	Serial.print(DDRC, HEX);
	Serial.println(DDRC, BIN);
	Serial.print("PINC: ");
	Serial.print(PINC, HEX);
	Serial.println(PINC, BIN);

	Serial.print("PORTD: ");
	Serial.print(PORTD, HEX);
	Serial.println(PORTD, BIN);
	Serial.print("DDRD: ");
	Serial.print(DDRD, HEX);
	Serial.println(DDRD, BIN);
	Serial.print("PIND: ");
	Serial.print(PIND, HEX);
	Serial.println(PIND, BIN);

	Serial.print("SPCR: ");
	Serial.print(SPCR, HEX);
	Serial.println(SPCR, BIN);
	Serial.print("SPSR: ");
	Serial.print(SPSR, HEX);
	Serial.println(SPSR, BIN);
	Serial.print("EIMSK: ");
	Serial.print(EIMSK, HEX);
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

