#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <rfm12b.h>
#include <stdio.h>
#include <debug.h>
#include "serial.h"

#define node_id     10       //node ID of tx (range 0-30)
#define network     212      //network group (can be in the range 1-250).

// ======== timer ==================
// overflow time = 1000ms/(16MHz/1024)*8bit = 16.384ms

unsigned long int time_ms = 0;
unsigned int _ns = 0;

ISR(TIMER0_OVF_vect)
{
	time_ms += 16;
	_ns += 384;
	if (_ns > 1000)
	{
		time_ms++;
		_ns -= 1000;
	}
}

unsigned long int tick_ms(void)
{
	uint64_t m;
	cli();
	m = time_ms;
	sei();
	return m;
}

void setup_timer(void)
{
	TCCR0B = _BV(CS00) | _BV(CS02); // F_CPU/1024 
	TIMSK0 = _BV(TOIE0); // enable timer overflow interrupt
	sei();
}

// ==============================================

FILE serial_stream = FDEV_SETUP_STREAM(serial_putchar, serial_getchar, _FDEV_SETUP_RW);

static void enter_setup_mode(void)
{
	printf("setup mode\n");

	char c = 0;

	do
	{
		c = getchar();

	}
	while (c != 'q');
}

static void blink_start(void)
{
	led_dash(); led_dot(); led_dot();
	led_space();
	led_dot();
	led_space();
	led_space();
	led_space();
	led_dash();
	led_space();
}

void setup(void)
{
	led_init();
	blink_start();

	serial_init();
	stdout = stdin = &serial_stream;

	printf("tx4 %s %s\n", __DATE__, __TIME__);
	setup_timer();
	
	uint8_t setup_mode = 0;
	
	for (uint8_t i = 0; i < 5; i++)
	{
		printf("%lu\n", tick_ms());
		_delay_ms(1000);
		if (serial_available())
		{
			char c = getchar();
			if (c == 's')
				setup_mode = 1;
		}
	}

	if (setup_mode)
		enter_setup_mode();

	printf("i");
	rf12_initialize(node_id, network);
	printf("n");
	printf("i");
	rf12_rx_on();
	printf("t\n");
}

unsigned long last_send = 0;
unsigned long interval = 10000;

struct settings
{
	uint8_t auto_start: 1;
	uint8_t tx_enabled: 1;
	uint8_t rx_enabled: 1;
	uint8_t debug: 1;
	uint8_t master: 1;
};

struct settings sts = {
	.auto_start = 1,
	.tx_enabled = 0,
	.rx_enabled = 1,
	.debug = 0
};

static void handle_serial(void)
{
	sts.auto_start = 0;

	char c = getchar();
	switch (c)
	{
	case 'i': // init
		interval = 10000;
		sts.rx_enabled = 0;
		sts.tx_enabled = 0;
		sts.auto_start = 1;
		last_send = tick_ms();
		printf("init\n");
		break;
	case 'g':
		sts.debug = !sts.debug;
		rf12_debug(sts.debug);
		printf("debug: %d\n", sts.debug);
		break;
	case '1':
		interval -= 1000;
		printf("interval: %lu\n", interval);
		break;
	case '2':
		interval += 1000;
		printf("interval: %lu\n", interval);
		break;
	case 't':
		sts.tx_enabled = !sts.tx_enabled;
		printf("tx: %d\n", sts.tx_enabled);
		break;
	case 'r':
		sts.rx_enabled = !sts.rx_enabled;
		if (sts.rx_enabled)
		{
			rf12_rx_on();
		}
		else
		{
			rf12_rx_off();
			rf12_state = IDLE;
		}
		printf("rx: %d\n", sts.rx_enabled);
		break;
	case 'd':
		printf("tick_ms: %lu\n", tick_ms());
		printf("last_send: %lu\n", last_send);
		printf("rf12_state: %d\n", rf12_state);
		break;
	}
}

void loop(void)
{
	if (serial_available())
		handle_serial();

	if (sts.tx_enabled && (rf12_state <= RX_ON) && ((tick_ms() - last_send) > interval))
	{
		if (sts.rx_enabled)
			rf12_rx_off();

		char s[30];
		unsigned long time = tick_ms();
		uint8_t n = snprintf(s, 20, "%d,t,%lu\n", node_id, time);
		/*
		rf12_data[n] = 0; // rf12_send will override it with crc
		printf("%s", rf12_data);
		rf12_send(n);
		
		*/
		
		printf("%s", s);
		rf12_send_sync(s, n);
		led_dot();
		led_dot();
		last_send = tick_ms() - 1;

		if (sts.rx_enabled)
			rf12_rx_on();
	}


	if (rf12_state >= RX_DONE_OK)
	{
		printf("    rx_state: %d, len: %d", rf12_state, rf12_len);
		if (rf12_state == RX_DONE_OK)
		{
			rf12_data[rf12_len] = 0;
			printf("    %s", rf12_data);
			rf12_state = IDLE;
		}
		else
			printf("\n");
		led_dot();
		
		if (sts.rx_enabled)
			rf12_rx_on();
	}
}

int main(void)
{
	setup();
	
	while(1)
		loop();
}
