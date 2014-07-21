#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <fcntl.h>
#include "trx.h"

static uint8_t group = 212;	    // network group

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
		return 0;

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
loop()
{
	int j;
	struct packet p;
	struct feed_data d;

	trx_enable_receiver();

	for(j = 0; j < MAX_ATTEMPTS; j++)
	{
		trx_reset();
	
		if (!packet_receive(&p))
			continue;
		
		if (!packet_parse(&p, &d))
			continue;

		print_feed(p.id, &d);

		sensors_read[p.id] = 0;
		if (all_read())
			return;
	}
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
	trx_init();
	init();
	loop();
	trx_close();
	return 0 ;
}



