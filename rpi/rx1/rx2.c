/*

Raspbery PI based receiver for rfm12 sensors.

Algorithm:
1. Pool on GPIO for interrupt pin (NINTQ).
2. Read data using SPI and bcm library.
3. Decode and print CSV data.

*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <stdarg.h>
#include "trx.h"
#include "gpio.h"

static uint8_t group = 212;	    // network group
static int gpio = 22;
static int gpio_fd = 22;

#define MAX_SENSORS 20
#define MAX_ATTEMPTS 10

uint8_t sensors_read[MAX_SENSORS];

struct packet
{
	uint8_t id;      // sensor id
	uint8_t len;     // packet length
	uint8_t b[254];  // data
	uint16_t crc;    // original crc
	uint16_t crc2;   // calculated crc
};

struct feed_data
{
	time_t time;     // time of the measurement
	int temperature; // temperature in C
	int humidity;    // humidity in %
};


static void
logi(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	fprintf(stderr, "\x1B[33m");
	vfprintf(stderr, format , args);
	fprintf(stderr, "\x1B[0m\n");
	va_end(args);
}

static void
logd(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	fprintf(stderr, "\x1B[32m");
	vfprintf(stderr, format , args);
	fprintf(stderr, "\x1B[0m\n");
	va_end(args);
}

static void
loge(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	fprintf(stderr, "\x1B[31m");
	vfprintf(stderr, format , args);
	fprintf(stderr, "\x1B[0m\n");
	va_end(args);
}

static int
packet_receive(struct packet* p)
{
	int i;
	memset(p, 0, sizeof(struct packet));

	p->id = trx_recv();
	p->len = trx_recv();

	for(i = 0; i < p->len + 2; i++)
	{
		do
		{
			p->b[i] = trx_recv();
		}
		while (p->b[i] == 0);
	}

	p->crc = p->b[p->len + 1] << 8;
	p->crc |= p->b[p->len];

	p->crc2 = calc_crc(group, p->id, p->len, p->b);

	if (p->crc != p->crc2)
	{
		loge("wrong crc");
		return 0;
	}

	p->b[p->len] = 0;
	return 1;
}

static int
packet_parse(const struct packet* p, struct feed_data* d)
{
	unsigned int tmr = 100;
	unsigned int hum = 0;
	d->time = time(NULL);
	
	int n = sscanf((char*)p->b, "t,%04X,h,%04X,d", &tmr, &hum);
	if (n != 2)
		return 0;

	d->temperature = (tmr & 0x7FF) / 10;
	if (tmr & 0x8000)
	    d->temperature = -d->temperature;
	d->humidity = hum / 10;
	return 1;
}

static void
print_feed(uint8_t id, struct feed_data* d)
{
	char time_str[50];
	struct tm tm;
	gmtime_r(&d->time, &tm);
	strftime(time_str, 50, "%Y-%m-%dT%H:%M:%SZ", &tm);
	printf("%d,%s,%d\n", id, time_str, d->temperature);
	printf("%d,%s,%d\n", id + 1, time_str, d->humidity);
	logd("%d,%s, temperature: %d", id, time_str, d->temperature);
	logd("%d,%s, humidity: %d", id + 1, time_str, d->humidity);
}

static int
all_read()
{
	uint8_t i;
	for (i = 0; i < MAX_SENSORS; i++)
		if (sensors_read[i] > 0)
			return 0;
	return 1;
}

static void
reset_irq()
{
	char buf[64];
	lseek(gpio_fd, 0, SEEK_SET);
	int len = read(gpio_fd, buf, 64);
	unsigned int val = 2;
	int rc = gpio_get_value(gpio, &val);
	logi("GPIO %d, len: %d, rc: %d, val: %u",
		gpio, len, rc, val);
}

static int
wait_event()
{
	struct pollfd fdset[1];
	int nfds = 1;
	int timeout = 60000;

	memset((void*)fdset, 0, sizeof(fdset));

	fdset[0].fd = gpio_fd;
	fdset[0].events = POLLPRI | POLLERR;

	int rc = poll(fdset, nfds, timeout);

	if (rc < 0)
	{
		loge("poll() failed: %d, %s", rc, strerror(rc));
		return -1;
	}

	if (rc == 0)
	{
		unsigned int val = 2;
		int rc = gpio_get_value(gpio, &val);
		printf("timeout: %d,%d ", rc, val);
		return 0;
	}

	if (fdset[0].revents & (POLLPRI | POLLERR))
	{
		return 1;
	}

	loge("wait_event: rc: %d", rc);
	return -1;
}

static void
loop()
{
	int j;
	struct packet p;
	struct feed_data d;

	trx_enable_receiver();
	logi("rx on");

	for(j = 0; j < MAX_ATTEMPTS; j++)
	{
		reset_irq();
		trx_reset();
		logi("waiting");
		int rc = wait_event();
		if (rc != 1)
		{
			loge("wait_event: %d", rc);
			continue;
		}

		if (!packet_receive(&p))
		{
			loge("packet_receive error");
			continue;
		}
		
		if (!packet_parse(&p, &d))
		{
			loge("packet_parse error");
			continue;
		}

		logi("packet parsed");
		print_feed(p.id, &d);

		sensors_read[p.id] = 0;
		if (all_read())
			return;
	}
}

static void
gpio_init()
{
	gpio_export(gpio);
	gpio_set_dir(gpio, 0);
	gpio_set_edge(gpio, "falling");
	gpio_set_active_low(gpio, 0);
	gpio_fd = gpio_fd_open(gpio);
}


static void
init()
{
	memset(sensors_read, 0, MAX_SENSORS);
//	sensors_read[10] = 1;
	sensors_read[11] = 1;
}

int main (void)
{
	logi("start");
	trx_init();
	gpio_init();
	init();
	loop();
	trx_close();
	return 0 ;
}



