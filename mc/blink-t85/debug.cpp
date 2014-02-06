#include "debug.h"
#include <Arduino.h>

static uint8_t led_pin = 4;

void led_init()
{
	pinMode(led_pin, OUTPUT);
}

void led_dot()
{
	digitalWrite(led_pin, HIGH);
	delay(80);
	digitalWrite(led_pin, LOW);
	delay(80);
}

void led_dash()
{
	digitalWrite(led_pin, HIGH);
	delay(240);
	digitalWrite(led_pin, LOW);
	delay(80);
}

static void print_bin(const char*s, uint8_t c)
{
	Serial.print(s);
	Serial.print(' ');

	uint8_t r = 0x80;
	
	while (r)
	{
		if (c & r)
			Serial.print('1');
		else
			Serial.print('0');
		r >>= 1;
	}
	Serial.print(' ');
	
	r = c >> 4;
	if (r > 9)
		Serial.print((char)(r + 55));
	else
		Serial.print((char)(r + 48));

	r = c & 0x0F;
	if (r > 9)
		Serial.print((char)(r + 55));
	else
		Serial.print((char)(r + 48));
	Serial.print('\n');
}

void dump()
{
	print_bin("USICR:", USICR);
	print_bin("PORTB:", PORTB);
	print_bin("DDRB :", DDRB);
	print_bin("PINB :", PINB);
}
