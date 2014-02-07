#include <stdio.h>
#include <stdint.h>
#include "rfm12b_defs.h"

#define rf12_cmd(a, b) printf("0x%02x 0x%02x\n", a, b)

int main()
{
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
	
	printf("CS  R  Rate\n");
	for (int cs = 0; cs < 2; cs++)
	{
		for (int R = 0; R < 128; R++)
		{
			float BR = 10000000.0f / 29.0f / (float)(R + 1) / (float)(1 + cs * 7);
			printf("%d  %2X  %3.2f\n", cs, R, BR);
		}
	}
}
