#include "dbg_uart.h"
#include "basic_serial.h"
#include <stdio.h>

static void uart_putchar(char c, FILE *stream)
{
	TxByte(c);
}

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
//FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void dbg_uart_init()
{
	stdout = &uart_output;
}

