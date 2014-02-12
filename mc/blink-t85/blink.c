#include <stdio.h>
#include <avr/interrupt.h>
#include <util/crc16.h>
#include <util/delay.h>
#include <debug.h>
#include <rfm12b.h>
#include <dbg_uart.h>
#include <am2302.h>

// ======== timer ==================
unsigned long int _millis = 0;
uint16_t _1000us = 0;

// timer overflow occur every 2.048 ms
ISR(TIM0_OVF_vect)
{
	_1000us += 2048;
	while (_1000us > 10000)
	{
		_millis += 10;
		_1000us -= 10000;
	}
}

unsigned long int tick_ms(void)
{
	uint64_t m;
	cli();
	m = _millis;
	sei();
	return m;
}

void setup_timer(void)
{
	TCCR0B = (1<<CS01); // F_CPU/8 
	TIMSK = 1 << TOIE0; // enable timer overflow interrupt
	sei();
}

unsigned long last_dump = 0;
unsigned long last_send = 0;

void setup(void)
{
	dbg_uart_init();
	printf("\n\n\n\nb");
	rf12_initialize(10, 212);
	setup_timer();
	printf("link %s %s\n", __DATE__, __TIME__);
	rf12_send_sync("blink\n", 6);
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

static void send_status(void)
{
	rf12_rx_off();
	printf("-rx off\n");

	int n = 0;
	char s[20];
	
	if (sensor.error)
		n = sprintf(s, "e,%02X\n", sensor.error); 
	else
		n = sprintf(s, "t,%d,h,%d,%02X\n", sensor.temperature, sensor.humidity, dbgstatus); 

	dbgstatus = 0;
	rf12_send_sync(s, n);
	printf("%s\n", s);
	rf12_rx_on();
	printf("-rx on\n");
}

void loop(void)
{
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
			_delay_ms(300);
			rf12_send_sync("a\n", 2);
		}
		else
		{
			printf("    rx12_state: %d\n", st);
			_delay_ms(300);
			char s[20];
			uint8_t n = snprintf(s, 20, "n,%d\n", rf12_state);
			rf12_send_sync(s, n);
		}
		
		rf12_rx_on();
		return;

	}

	if (tick_ms() - last_send > 10000)
	{
		cli();
		sensor.error = am2302(&sensor.humidity, &sensor.temperature);
		sei();
		send_status();
		last_send = tick_ms();
	}
}

int main(void)
{
	setup();
	while (1)
		loop();
}
