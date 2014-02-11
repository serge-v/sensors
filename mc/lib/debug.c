#include "debug.h"
#include <Arduino.h>
#include <stdio.h>

static uint8_t led_pin = 0xFF;

void led_init(int pin)
{
	led_pin = pin;
	pinMode(led_pin, OUTPUT);
}

void led_dot()
{
	if (led_pin == 0xFF)
		return;

	digitalWrite(led_pin, HIGH);
	delay(80);
	digitalWrite(led_pin, LOW);
	delay(80);
}

void led_dash()
{
	if (led_pin == 0xFF)
		return;

	digitalWrite(led_pin, HIGH);
	delay(240);
	digitalWrite(led_pin, LOW);
	delay(80);
}

static void print_bin(const char*s, uint8_t c)
{
	printf("%s ", s);

	uint8_t r = 0x80;

	while (r)
	{
		if (c & r)
			printf("1");
		else
			printf("0");
		r >>= 1;
	}

	printf(" %02X\n", c);
}

void dump()
{
#ifdef USICR
	print_bin("USICR:", USICR);
#endif
	print_bin("PORTB:", PORTB);
	print_bin("DDRB :", DDRB);
	print_bin("PINB :", PINB);
}
