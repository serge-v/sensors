/*
node12.c
temperature transmitter using internal ATTint85 sensor
*/

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/crc16.h>
#include <util/delay.h>
#include <debug.h>
#include <rfm12b.h>
#include <dbg_uart.h>
#include <am2302.h>
#include <timer.h>
#include <power.h>
#include <tinyt.h>

#define DEBUG

const uint8_t node_id = 12;
const uint8_t group_id = 212;
uint8_t status = 0;
volatile uint8_t f_wdt = 1;
uint8_t cycles = 0;

void
setup(void)
{
	wdt_disable();
	dbg_uart_init();
	printf("\n\n\n\nnode12\n");
	rf12_initialize(node_id, group_id);
	timer0_start();
	printf("%s %s\n", __DATE__, __TIME__);
	for (uint8_t i = 0; i < 5; i++)
	{
		printf("%lu\n", timer0_ms());
		_delay_ms(1000);
	}
	rf12_send_sync("node12\n", 7);
	setup_watchdog(WDTO_8S);
}

static void
send_status(void)
{
	int n = 0;
	char s[20];
	int temperature = tinyt_read_c();
	n = sprintf(s, "t,%04X,%x\n", temperature, status);
	rf12_send_sync(s, n);
	printf("%s\n", s);
	status = 0;
}

static uint8_t
receive_command()
{
	enum rf12_state st = IDLE;
	unsigned long start = timer0_ms();

	rf12_rx_on();

	while ((timer0_ms() - start) < 2000)
	{
		if (!rf12_wait_rx())
			continue;

		status |= 0x80;
		st = rf12_read_rx();
		if (st == RX_IN_PROGRESS)
			continue;

		status |= 0x40;
	}

	rf12_rx_off();

	if (st == RX_DONE_OK)
	{
		status |= 0x20;
		rf12_data[rf12_len] = 0;
		printf("    %s", rf12_data);
	}

	return (st == RX_DONE_OK);
}

static void
communicate()
{
	send_status();

	if (receive_command())
	{
		const char* s = rf12_data;
		switch (*s)
		{
		case 'g':
			send_debug_info();
			break;
		case 'c':
			calibrate_thermometer();
			break;
	}
}

void loop(void)
{
	if (f_wdt != 1)
		return;

	f_wdt = 0;

	if (cycles > 2)
	{
		communicate();
		cycles = 0;
	}

	system_sleep();
	cycles++;
}

int main(void)
{
	setup();
	while (1)
		loop();
}

// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect)
{
	f_wdt = 1;  // set global flag
}
