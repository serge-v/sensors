#include "debug.h"
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

static uint8_t led_pin = 7;
static const uint8_t dot_len = 80; // ms

void led_init(void)
{
	DDRD |= _BV(led_pin);
}

void led_dot()
{
	PORTD |= _BV(led_pin);
	_delay_ms(dot_len);
	PORTD &= ~(_BV(led_pin));
	_delay_ms(dot_len);
}


void led_dash()
{
	PORTD |= _BV(led_pin);
	_delay_ms(dot_len * 3);
	PORTD &= ~(_BV(led_pin));
	_delay_ms(dot_len);
}

void led_space()
{
	_delay_ms(dot_len * 3);
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
