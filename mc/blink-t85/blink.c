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

#define DEBUG

const uint8_t node_id = 10;
const uint8_t group_id = 212;

unsigned long last_dump = 0;
unsigned long last_send = 0;

void system_sleep(void);

void setup_watchdog(int ii)
{
	uint8_t tout;
	if (ii > 9)
		ii = 9;
	tout = ii & 7;
	if (ii > 7)
		tout |= _BV(WDP3);

	tout |= _BV(WDCE);

	MCUSR &= ~_BV(WDRF);

#if defined(WDTCR)
	WDTCR |= _BV(WDCE) | _BV(WDE);
	WDTCR = tout;
	WDTCR |= _BV(WDIE);
#elif defined (WDTSCR)
	WDTCR |= _BV(WDCE) | _BV(WDE);
	WDTCR = tout;
	WDTCR |= _BV(WDIE);
#endif
}


static void
setup(void)
{
	wdt_disable();
	dbg_uart_init();
	printf("\n\n\n\nb");
	rf12_initialize(node_id, group_id);
	timer0_start();
	printf("link %s %s\n", __DATE__, __TIME__);
	for (uint8_t i = 0; i < 5; i++)
	{
		printf("%lu\n", timer0_ms());
		_delay_ms(1000);
	}
	rf12_send_sync("blink\n", 6);
	setup_watchdog(WDTO_8S);
}

struct rht03_status
{
	uint16_t humidity;
	uint16_t temperature;
	uint8_t error;
};

static struct rht03_status sensor = {
	.humidity = 0,
	.temperature = 0,
	.error = 0
};

uint8_t dbgstatus = 0;

static void
send_status(void)
{
#ifdef RECEIVER_CODE
	rf12_rx_off();
	printf("-rx off\n");
#endif
	int n = 0;
	char s[20];

	if (sensor.error)
		n = sprintf(s, "e,%x\n", sensor.error); 
	else
		n = sprintf(s, "t,%04X,h,%04X,d,%x\n",
			sensor.temperature,
			sensor.humidity,
			dbgstatus);

	dbgstatus = 0;
	rf12_send_sync(s, n);
	printf("%s\n", s);
#ifdef RECEIVER_CODE
	rf12_rx_on();
	printf("-rx on\n");
#endif
}

volatile uint8_t f_wdt = 1;
uint8_t loop_count = 0;

static void
loop(void)
{
	if (f_wdt != 1)
		return;

	f_wdt = 0;

#ifdef RECEIVER_CODE

	if (rf12_wait_rx())
	{
		dbgstatus |= 0x80;

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
#ifdef DEBUG
		sensor.error = 0;
		sensor.temperature = 0x8020;
		sensor.humidity = 312;
#else
		sensor.error = am2302(&sensor.humidity, &sensor.temperature);
#endif
		sei();
		send_status();
		last_send = timer0_ms();
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

static void
system_sleep()
{
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
	sleep_enable();
	sleep_mode();                        // System sleeps here
	sleep_disable();                     // System continues execution here when watchdog timed out 
}

// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect)
{
	f_wdt = 1;  // set global flag
}
