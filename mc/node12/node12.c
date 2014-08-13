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

uint8_t status = 0;

static void
send_status(void)
{
#ifdef RECEIVER_CODE
	rf12_rx_off();
	printf("-rx off\n");
#endif
	int n = 0;
	char s[20];
	int temperature = tinyt_read_c();
	n = sprintf(s, "t,%04X,%x\n", temperature, status);

	status = 0;
	rf12_send_sync(s, n);
	printf("%s\n", s);
#ifdef RECEIVER_CODE
	rf12_rx_on();
	printf("-rx on\n");
#endif
}

volatile uint8_t f_wdt = 1;
uint8_t loop_count = 0;

static void receive()
{
}

void loop(void)
{
	if (f_wdt != 1)
		return;

	f_wdt = 0;

#ifdef RECEIVER_CODE

	if (rf12_wait_rx())
	{
		status |= 0x80;

		enum rf12_state st = rf12_read_rx();

		if (st == RX_IN_PROGRESS)
			return;

		dbgstatus |= 0x40;

		rf12_rx_off();
		if (st == RX_DONE_OK)
		{
			dbgstatus |= 0x20;
			rf12_data[rf12_len] = 0;
			printf("    %s", rf12_data);
			_delay_ms(2000);
			rf12_send_sync("a\n", 2);
		}
		else
		{
			printf("    rx12_state: %d\n", st);
			_delay_ms(2000);
			char s[20];
			uint8_t n = snprintf(s, 20, "n,%d\n", rf12_state);
			rf12_send_sync(s, n);
		}

		rf12_rx_on();
		return;

	}
#endif // RECEIVER_CODE

	if (loop_count > 2)
	{
		cli();
		sei();
		send_status();
		loop_count = 0;
	}

	system_sleep();
	loop_count++;
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

