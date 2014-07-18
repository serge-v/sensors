#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <fcntl.h>
#include "trx.h"

static uint8_t group = 212;         // network group

#define RFM_IRQ 22              //IRQ GPIO pin.
#define RFM_CE BCM2835_SPI_CS1  //SPI chip select
#define MAX_SENSORS 20

uint8_t sensors_read[MAX_SENSORS];

static int
all_read()
{
	uint8_t i;
	for (i = 0; i < MAX_SENSORS; i++)
	{
		if (sensors_read[i] > 0)
			return 0;
	}
	
	return 1;
}

static void
loop()
{
	unsigned char buffer[128];
	uint8_t i;
	int j;

	trx_enable_receiver();

	for(j = 0; j < 10; j++)
	{
		trx_reset();

		uint8_t id = trx_recv();
		uint8_t len = trx_recv();

		for(i = 0; i < len + 2; i++)
		{
			do
			{
				buffer[i] = trx_recv();
			}
			while (buffer[i] == 0);
		}

//		printf("%d  len: %d  ", id, len);

		uint16_t sent_crc = buffer[len + 1] << 8;
		sent_crc |= buffer[len];

		uint16_t rf12_crc = calc_crc(group, id, len, buffer);

		if (sent_crc == rf12_crc)
		{
			buffer[len] = 0;
			// printf("%s", buffer);

			unsigned int tmr = 100;
			unsigned int hum = 0;

			int n = sscanf((char*)buffer, "t,%04X,h,%04X,d", &tmr, &hum);
			if (n != 2)
				continue;

			int temperature = (tmr & 0x7FF) / 10;

			if (tmr & 0x8000)
				temperature = -temperature;

			int humidity = hum / 10;
			int temperatureF = (float)temperature * 9.0 / 5.0 + 32;

			if (0)
			{
				printf("%d  %dC %dF RH %d%%\n", id, temperature, temperatureF, humidity);
			}
			else
			{
				char time_str[50];
				struct tm tm;
				time_t now = time(NULL);
				gmtime_r(&now, &tm);
				strftime(time_str, 50, "%Y-%m-%dT%H:%M:%SZ", &tm);
				printf("%d,%s,%d\n", id, time_str, temperature);
				printf("%d,%s,%d\n", 12, time_str, humidity);
			}

			sensors_read[id] = 0;
			if (all_read())
				return;
		}
		memset(buffer, 0, len + 2);
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
