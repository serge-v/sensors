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
#include <adc.h>

#define DEBUG

const uint8_t node_id = 12;
const uint8_t group_id = 212;
static uint8_t status = 0;
volatile uint8_t f_wdt = 1;
static uint8_t cycles = 0;
static int temperature = -100;
static int offset = 0;

void
setup(void)
{
	wdt_disable();
	dbg_uart_init();
	rf12_debug(1);
	rf12_initialize(node_id, group_id);
	timer0_start();
	_delay_ms(1000);
	printf("%lu\n", timer0_ms());
	printf("%s %s\n", __DATE__, __TIME__);
	rf12_send_sync("node12\n", 7);
	setup_watchdog(WDTO_8S);
}

static void
send_status(void)
{
	int n = 0;
	char s[20];
	n = sprintf(s, "t,%04X,%x\n", temperature, status);
	rf12_send_sync(s, n);
	printf("%s\n", s);
	status = 0;
}

static uint8_t
receive_command(void)
{
	enum rf12_state st = IDLE;
	unsigned long start = timer0_ms();

	printf("rx on\n");
	rf12_rx_on();

	while ((timer0_ms() - start) < 4000)
	{
		// wait first byte

		if (!rf12_wait_rx())
			continue;

		// read remaining bytes

		status |= 0x80;
		st = rf12_read_rx();
		if (st == RX_IN_PROGRESS)
			continue;

		status |= 0x40;
	}

	rf12_rx_off();
	printf("rx off. st: %d, len: %d\n", st, rf12_len);

	if (st == RX_DONE_OK)
	{
		status |= 0x20;
		rf12_data[rf12_len] = 0;
		printf("    %s", rf12_data);
	}

	return (st == RX_DONE_OK);
}

static void
calibrate_thermometer(void)
{
}

static void
communicate(void)
{
	send_status();

	if (receive_command())
	{
		const uint8_t* s = rf12_data;
		switch (*s)
		{
		case 'c':
			calibrate_thermometer();
			break;
		}
	}
}

void loop(void)
{
	if (f_wdt != 1)
		return;

	f_wdt = 0;

	if (cycles > 2)
	{
		adc_enable_temperature_sensor(offset);
		temperature = adc_get_temperature();
		adc_disable();
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
