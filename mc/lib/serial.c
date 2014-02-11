#include <avr/io.h>
#include <stdio.h>

#define BAUD 19200
#include <util/setbaud.h>

void serial_putchar(char c, FILE *stream)
{
	if (c == '\n')
		serial_putchar('\r', stream);

    	loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
	UDR0 = c;
}

char serial_getchar(FILE *stream)
{
	loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
	return UDR0;
}

void serial_init(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	UCSR0A &= ~(_BV(U2X0));

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

uint8_t serial_available(void)
{
	return (UCSR0A & _BV(RXC0));
}
