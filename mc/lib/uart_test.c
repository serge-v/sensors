#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

int tick = 0;

int main(void)
{
	dbg_uart_init();

	printf("started\n");

	while (1)
	{
		printf("tick: %d\n", tick++);
		_delay_ms(1000);
	}

	return 0;
}
