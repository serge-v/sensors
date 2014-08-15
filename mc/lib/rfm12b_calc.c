#include <stdio.h>
#include <stdint.h>
#include "rfm12b_defs.h"

#define rf12_cmd(a, b) printf("    0x%02X 0x%02X\n", a, b)

static void
calc_data_rates()
{
	printf("\nRF_DRATE_CFG(0x%02X) values:\n\n", RF_DRATE_CFG);
	printf("    CS  R  Speed, Kbps\n");

	int cs = 0;
	for (cs = 0; cs < 2; cs++)
	{
		int R = 0;
		for (R = 0; R < 128; R++)
		{
			float BR = 10000000.0f / 29.0f / (float)(R + 1) / (float)(1 + cs * 7);
			printf("    %d  %2X  %3.2f\n", cs, R, BR);
		}
	}
}

static void
calc_init_params()
{
	printf("rfm12b init params:\n\n");

	rf12_cmd(RF_PWR_MGMT, RF_PWR_EB | RF_PWR_DC);
	rf12_cmd(RF_CONFIG, RF_CONFIG_EL | RF_CONFIG_EF | RF_FFREQ_433 | RF_CAP_120pF);
	rf12_cmd(RF_FREQ_CFG, 0x40); // 433.26MHz
//	rf12_cmd(RF_DRATE_CFG, 0x06); // approx 49.2 Kbps, i.e. 10000/29/(1+6) Kbps
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
//	rf12_cmd(RF_BATT_CFG, 0x49); // 1.66MHz,3.1V  -- change V
	rf12_cmd(RF_BATT_CFG, 0x00); // 1MHz,2.2V
	rf12_cmd(RF_PWR_MGMT, RF_PWR_ER|RF_PWR_EBB|RF_PWR_ES | RF_PWR_EX|RF_PWR_EB|RF_PWR_DC);
	rf12_cmd(RF_PWR_MGMT, RF_PWR_EX|RF_PWR_EB|RF_PWR_DC);
	
	printf("%04X\n", (RF_PWR_MGMT << 8) | RF_PWR_ER|RF_PWR_EBB|RF_PWR_ES | RF_PWR_EX|RF_PWR_EB|RF_PWR_DC);
}

#define OVERFLOW_MS (1000.0 / ((float)F_CPU / (float)CLK_DIVIDER) * 256)
#define OVERFLOW_NS ((uint16_t)(1000000.0 / ((float)F_CPU / (float)CLK_DIVIDER) * 256) % 1000)

static void
calc_timer()
{

#define F_CPU 16000000
#define CLK_DIVIDER 1024

	uint64_t ms = OVERFLOW_MS;
	uint64_t ns = OVERFLOW_NS;
	printf("ms: %lu, ns: %lu\n", ms, ns);

#undef F_CPU
#undef CLK_DIVIDER

#define F_CPU 1000000
#define CLK_DIVIDER 8

	ms = OVERFLOW_MS;
	ns = OVERFLOW_NS;
	printf("ms: %lu, ns: %lu\n", ms, ns);
}
/*
4． Frequency Setting Command

bit 15  14  13  12  11  10  9  8  7  6  5  4  3  2  1  0    POR
     1   0   1   0 f11 f10 f9 f8 f7 f6 f5 f4 f3 f2 f1 f0    A680h

f11..f0: Set operation frequency:

	433 band: Fc = 430 + F * 0.0025 MHz
	868 band: Fc = 860 + F * 0.0050 MHz
	915 band: Fc = 900 + F * 0.0075 MHz

Fc is carrier frequency and F is the frequency parameter. 36<=F<=3903
*/
static void
calc_frequency()
{
	printf("\nRF_FREQ_CFG\nBand F      Freq, Mhz\n");
	int F;
	for (F = 36; F <= 3903; F++)
	{
		float freq = (430.0f + (float)F * 0.0025f);
		printf("433  A%03X    %.4f\n", F, freq);
	}
	printf("\n");
}

int main()
{
	calc_init_params();
	calc_data_rates();
	calc_timer();
	calc_frequency();
}
