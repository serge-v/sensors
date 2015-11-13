#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <rfm12b.h>
#include <stdio.h>
#include <stdlib.h>
#include <debug.h>
#include <timer.h>
#include "serial.h"

#define node_id     9        //node ID of tx (range 0-30)
#define network     212      //network group (can be in the range 1-250).

FILE serial_stream = FDEV_SETUP_STREAM(serial_putchar, serial_getchar, _FDEV_SETUP_RW);

static void
blink_start(void)
{
	led_dash();
	led_dot();
	led_dot();
}

static void
setup(void)
{
	led_init();
	blink_start();

	serial_init();
	stdout = stdin = &serial_stream;

	printf("tx4 %s %s\n", __DATE__, __TIME__);
	timer0_start();

	for (uint8_t i = 0; i < 5; i++)
	{
		printf("%lu\n", timer0_ms());
		_delay_ms(100);
	}

	printf("i1\n");
	rf12_initialize(node_id, network);
	printf("i2\n");
	rf12_rx_on();
	printf("i3\n");
}

static unsigned long last_send = 0;
static unsigned long interval = 10000;

struct settings
{
	uint8_t auto_start: 1;
	uint8_t tx_enabled: 1;
	uint8_t rx_enabled: 1;
	uint8_t debug: 1;
	uint8_t master: 1;
	uint8_t rx_spin_mode: 1;
};

struct settings sts = {
	.auto_start = 1,
	.tx_enabled = 0,
	.rx_enabled = 1,
	.debug = 0,
	.rx_spin_mode = 0,
};

static void
handle_serial(void)
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
		last_send = timer0_ms();
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
	case 's':
		sts.rx_spin_mode = !sts.rx_spin_mode;
		printf("rx spin mode: %d\n", sts.rx_spin_mode);
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
		printf("timer0_ms: %lu\n", timer0_ms());
		printf("last_send: %lu\n", last_send);
		printf("rf12_state: %d\n", rf12_state);
		break;
	}
}

static void
print_temperature_sensor(void)
{
	if (rf12_data[0] != 't' || rf12_data[1] != ',' ||
		rf12_data[7] != 'h' || rf12_data[8] != ',')
	{
		printf("%d  badrec: %s\n", rf12_node, rf12_data);
		return;
	}

	uint16_t sensor_t = strtoul((const char*)&rf12_data[2], NULL, 16);
	int16_t humidity = strtoul((const char*)&rf12_data[9], NULL, 16) / 10;

	int8_t temperature = (sensor_t & 0x7FFF) / 10;
	if (sensor_t & 0x8000)
		temperature = -temperature;

	int8_t temperatureF = (float)temperature * 9.0 / 5.0 + 32;

	printf("%d  %dC %dF RH %d%%\n", rf12_node, temperature, temperatureF, humidity);
}

static void
loop(void)
{
	if (serial_available())
		handle_serial();

	if (sts.tx_enabled && ((timer0_ms() - last_send) > interval))
	{
		if (sts.rx_enabled)
			rf12_rx_off();

		char s[30];
		unsigned long time = timer0_ms();
		uint8_t n = snprintf(s, 20, "%d,t,%lu\n", node_id, time);

		printf("%s", s);
		rf12_send_sync(s, n);
		printf("m0\n");
		led_dot();
		led_dot();
		last_send = timer0_ms() - 1;
		printf("m1\n");
		if (sts.rx_enabled)
			rf12_rx_on();
		printf("m2\n");
	}

	if (rf12_state >= RX_DONE_OK)
	{
		if (sts.debug)
			printf("%d  rx_state: %d, len: %d\n",
				rf12_node, rf12_state, rf12_len);

		if (rf12_state == RX_DONE_OK)
		{
			rf12_data[rf12_len] = 0;
			if (rf12_node >= 10 && rf12_node < 20)
				print_temperature_sensor();
			else
				printf("%d  %s\n", rf12_node, rf12_data);

			rf12_state = IDLE;
		}
		else if (sts.debug)
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
