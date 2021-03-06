#include "dbg_uart.h"
#include "basic_serial.h"
#include <stdio.h>

static int
uart_putchar(char c, FILE *stream)
{
	if (c == '\n')
		uart_putchar('\r', stream);
	TxByte(c);
	return 0;
}

static int
uart_getchar(FILE *stream)
{
	return RxByte();
}

FILE uart_stream = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

void dbg_uart_init()
{
	stdout = stdin = &uart_stream;
}

