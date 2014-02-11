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
	printf("l");
	rf12_rx_on();
	printf("i");
	setup_timer();
	printf("nk %s %s\n", __DATE__, __TIME__);
}

uint8_t buf[20];
uint8_t sbuf[20];
uint8_t idx = 0;
uint8_t len = 0;
unsigned int timeouts = 0;

uint8_t verify_buf(void)
{
	uint16_t crc = ~0;
	crc = _crc16_update(crc, 212);

	uint8_t i = 0;
	for (i = 0; i < len + 2; i++)
		crc = _crc16_update(crc, buf[i]);

	uint16_t expected_crc = buf[i++];
	expected_crc |= buf[i] << 8;

	return (expected_crc == crc);
}

void reset_buf(void)
{
	if (len > 0)
	{
		if (verify_buf())
		{
			buf[len+2] = 0;
			printf("    %lu: %s", tick_ms(), (char*)(buf+2));
		}
		else
		{
			if (idx > 0)
			{
				printf("    ");
				for (int i = 0; i < idx; i++)
					printf("%02X ", buf[i]);
				printf("\n");
			}
			printf("    n: %d, t: %u\n", len, timeouts);
		}
	}

	idx = 0;
	len = 0;
	rf12_rx_on();
	last_dump = tick_ms();
}

static void tx(uint8_t c)
{
	while (!rf12_read_status_MSB());
	rf12_cmd(0xB8, c);
}

static uint16_t humidity = 0;
static uint16_t temperature = 0;
static uint8_t sensor_errno = 0;

static void send_status(void)
{
	rf12_rx_off();
	printf("rx off\n");

	int n = 0;
	
	if (sensor_errno)
		n = sprintf((char*)sbuf, "e,%02X\n", sensor_errno); 
	else
		n = sprintf((char*)sbuf, "t,%d,h,%d\n", temperature, humidity); 

	uint16_t crc = ~0;
	crc = _crc16_update(crc, 212);
	crc = _crc16_update(crc, 0xA);
	crc = _crc16_update(crc, n);
	for (uint8_t i = 0; i < n; i++)
		crc = _crc16_update(crc, sbuf[i]);

	printf("sending %s", sbuf);
	rf12_reset_fifo();

	rf12_cmd(0x82, 0x3D); // start tx

	tx(0xAA);        // preamble
	tx(0x2D);        // sync hi byte
	tx(0xD4);        // sync low byte
	tx(0x0A);        // id
	tx(n);           // len
	for (uint8_t i = 0; i < n; i++)
		tx(sbuf[i]);
	
	tx(crc & 0xFF);  // crc lo
	tx(crc >> 8);    // crc hi
	tx(0);           // dummy byte
	tx(0);           // dummy byte
	while (!rf12_read_status_MSB()); // wait dummy byte
	_delay_ms(50);
	rf12_cmd(0x82, 0x0D); // idle
	printf("sent: %scrc: %02X %02X\n", (char*)sbuf, crc & 0xFF, crc >> 8);
	rf12_rx_on();
	printf("rx on\n");
}

void loop(void)
{
	uint16_t cnt = 0xFFFF;

	while (!rf12_read_status_MSB() && --cnt);

	if (cnt == 0)
	{
		timeouts++;
		if (tick_ms() - last_dump > 1000)
			reset_buf();

		if (tick_ms() - last_send > 10000)
		{
			cli();
			sensor_errno = am2302(&humidity, &temperature);
			sei();
			rf12_spi_init(); // restore pins mode
			send_status();
			last_send = tick_ms();
		}
		return;
	}

	uint8_t c = rf12_cmd(0xB0, 0x00);
	buf[idx++] = c;

	if (idx == 2)
		len = c;

	if (len >= 20)
		len = 20;

	timeouts = 0;

	if (idx == len + 4)
		reset_buf();
}

int main(void)
{
	setup();
	while (1)
		loop();
}
