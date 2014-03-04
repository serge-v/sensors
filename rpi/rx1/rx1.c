#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <bcm2835.h>
#include "../../mc/lib/rfm12b_defs.h"

static uint8_t group = 212;         // network group
volatile uint16_t rf12_crc;         // running crc value

#define RFM_IRQ 22              //IRQ GPIO pin.
#define RFM_CE BCM2835_SPI_CS1  //SPI chip select

void spi_init()
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

static uint16_t _crc16_update(uint16_t crc, uint8_t a)
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

uint16_t rf12_xfer(uint16_t cmd)
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

void rf12_cmd(uint8_t cmd, uint8_t d)
{
	unsigned char buffer[2];
	buffer[0] = cmd;
	buffer[1] = d;
	bcm2835_spi_transfern((char*)buffer, 2);
}

void rf12_init()
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

int wait_packet(int irq_fd)
{
	fd_set rfds;
	struct timeval tv;
	int retval;

	FD_ZERO(&rfds);
	FD_SET(irq_fd, &rfds);

	tv.tv_sec = 1;
	tv.tv_usec = 0;

	retval = select(1, NULL, NULL, &rfds, &tv);

	if (retval == -1)
		return -1;
	else if (retval)
		return 1;
	return 0;
}

int open_irq_pin()
{
	char s[100];
	snprintf(s, 100, "/sys/class/gpio/gpio%d/value", RFM_IRQ);
	int fd = open(s, O_RDONLY);
	if (fd < 0)
		return -1;
	return fd;
}

void loop()
{
	unsigned char buffer[128];
	uint8_t i;
	int j;

	int irq_fd = open_irq_pin();
	if (irq_fd < 0)
	{
		perror("cannot open irq pin");
		exit(1);
	}

	rf12_xfer(0x82dd);

	for(j = 0; j<1000; j++)
	{
		rf12_xfer(0xCA80); //reset the sync cicuit to look for a new packet
		rf12_xfer(0xCA83);

		rf12_xfer(0);

		int rc = wait_packet(irq_fd);
		if (rc == -1)
		{
			perror("irq fd select");
			exit(1);
		}

		if (rc == 0)
		{
			printf("%d", bcm2835_gpio_lev(RFM_IRQ));
			fflush(stdout);
			continue;
		}

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

		printf("%d  len: %d  ", id, len);
		
		rf12_crc = ~0;
		rf12_crc = _crc16_update(rf12_crc, group);
		rf12_crc = _crc16_update(rf12_crc, id);
		rf12_crc = _crc16_update(rf12_crc, len);
		
		for (i = 0; i < len; i++)
			rf12_crc = _crc16_update(rf12_crc, buffer[i]);

		for (i = 0; i < len + 2; i++)
		{
			printf(" %02X", buffer[i]);
			buffer[i] = 0;
		}

		printf(" (crc = %04X)", rf12_crc);
		printf("\n");
	}
}

int main (void)
{
	printf ("Raspberry Pi RFM12B test program\n") ;

	spi_init();
	rf12_init();

	loop();

	bcm2835_spi_end();

	printf("Done\n");
	return 0 ;
}
