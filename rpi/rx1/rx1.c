#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <fcntl.h>
#include <bcm2835.h>
#include "../../mc/lib/rfm12b_defs.h"

static uint8_t group = 212;         // network group

#define RFM_IRQ 22              //IRQ GPIO pin.
#define RFM_CE BCM2835_SPI_CS1  //SPI chip select
#define MAX_SENSORS 20

uint8_t sensors_read[MAX_SENSORS];

static void
spi_init()
{
	if (!bcm2835_init())
		exit (1);
	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_128);   // 2MHz
	bcm2835_spi_chipSelect(RFM_CE);
	bcm2835_spi_setChipSelectPolarity(RFM_CE, LOW);

	//Set IRQ pin details
	bcm2835_gpio_fsel(RFM_IRQ, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_set_pud(RFM_IRQ, BCM2835_GPIO_PUD_UP);
}

static uint16_t
_crc16_update(uint16_t crc, uint8_t a)
{
	int i;

	crc ^= a;
	for (i = 0; i < 8; ++i)
	{
		if (crc & 1)
			crc = (crc >> 1) ^ 0xA001;
		else
			crc = (crc >> 1);
	}

	return crc;
}

static uint16_t
rf12_xfer(uint16_t cmd)
{
	unsigned char buffer[2];
	uint16_t reply;
	buffer[0] = cmd >> 8;
	buffer[1] = cmd;
	bcm2835_spi_transfern((char*)buffer, 2);
	reply = buffer[0] << 8;
	reply |= buffer[1];
	return reply;
}

static void
rf12_cmd(uint8_t cmd, uint8_t d)
{
	unsigned char buffer[2];
	buffer[0] = cmd;
	buffer[1] = d;
	bcm2835_spi_transfern((char*)buffer, 2);
}

static void
rf12_init()
{
	rf12_cmd(RF_PWR_MGMT, RF_PWR_EB | RF_PWR_DC);
	rf12_cmd(RF_CONFIG, RF_CONFIG_EL | RF_CONFIG_EF | RF_FFREQ_433 | RF_CAP_120pF);
	rf12_cmd(RF_FREQ_CFG, 0x40); // 433.26MHz
	rf12_cmd(RF_DRATE_CFG, 0x06); // approx 49.2 Kbps, i.e. 10000/29/(1+6) Kbps
	rf12_cmd(RF_DRATE_CFG, 0x11); // 19200
	rf12_cmd(RF_RX_CTRL|RF_RX_VDI_OUT, RF_RX_RESP_FAST | RF_RX_BW_134 | RF_RX_GAIN_0 | RF_RX_RSSI_M91);
	rf12_cmd(RF_DF, RF_DF_AL | RF_DF_SBITS | RF_DF_DQD4);
	rf12_cmd(RF_FIFO, 0x81); // FIFO8,2-SYNC,!ff,DR
	rf12_cmd(RF_PATTERN, 0xD4); // SYNC=2DXX
	rf12_cmd(RF_AFC, 0x83); // @PWR,NO RSTRIC,!st,!fi,OE,EN
	rf12_cmd(RF_TX_CFG, 0x50); // !mp,90kHz,MAX OUT
	rf12_cmd(RF_PLL_CFG, 0x77); // OB1,OB0, LPX,!ddy,DDIT,BW0. CC67
	rf12_cmd(RF_WAKEUP_CFG, 0x00); // always on
	rf12_cmd(RF_DUTY_CFG, 0x00); // no low duty mode
	rf12_cmd(RF_BATT_CFG, 0x49); // 1.66MHz,3.1V  -- change V
	rf12_cmd(RF_PWR_MGMT, RF_PWR_ER|RF_PWR_EBB|RF_PWR_ES | RF_PWR_EX|RF_PWR_EB|RF_PWR_DC);
}

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

	rf12_xfer(0x82dd);

	for(j = 0; j < 10; j++)
	{
		rf12_xfer(0xCA80); //reset the sync cicuit to look for a new packet
		rf12_xfer(0xCA83);

		rf12_xfer(0);

		while(bcm2835_gpio_lev(RFM_IRQ));
		rf12_xfer(0);
		uint8_t id = rf12_xfer(0xB000);

		while(bcm2835_gpio_lev(RFM_IRQ));
		rf12_xfer(0);
		uint8_t len = rf12_xfer(0xB000);
		
		for(i = 0; i < len + 2; i++)
		{
			do
			{
				while(bcm2835_gpio_lev(RFM_IRQ));

				rf12_xfer(0);
				buffer[i] = rf12_xfer(0xB000);
			}
			while (buffer[i] == 0);
		}

//		printf("%d  len: %d  ", id, len);
		
		uint16_t rf12_crc = 0, sent_crc = 0;

		sent_crc = buffer[len + 1] << 8;
		sent_crc |= buffer[len];

		rf12_crc = ~0;
		rf12_crc = _crc16_update(rf12_crc, group);
		rf12_crc = _crc16_update(rf12_crc, id);
		rf12_crc = _crc16_update(rf12_crc, len);
		
		for (i = 0; i < len; i++)
			rf12_crc = _crc16_update(rf12_crc, buffer[i]);

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
			}

			sensors_read[id] = 0;
			if (all_read())
				return;
		}
/*		else
		{
			for (i = 0; i < len + 2; i++)
				printf(" %02X", buffer[i]);
			printf(" (crc = %04X, sent_crc = %04X)", rf12_crc, sent_crc);
			printf("\n");
		}
*/
		memset(buffer, 0, len + 2);
	}
}

static void
init()
{
	memset(sensors_read, 0, MAX_SENSORS);
	sensors_read[10] = 1;
	sensors_read[11] = 1;
}

int main (void)
{
	// printf("Read sensors\n") ;

	spi_init();
	rf12_init();
	init();

	loop();

	bcm2835_spi_end();

	return 0 ;
}
