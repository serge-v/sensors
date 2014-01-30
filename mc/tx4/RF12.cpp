#include <avr/io.h>
#include <util/crc16.h>
#include <Arduino.h> // Arduino 1.0

#if defined(__AVR_ATmega328P__)
#	define IRQ_DDR DDRD
#	define IRQ_PORT    PIND
#	define IRQ_PIN     PD2
#	define SELECT_DDR  PORTB
#	define SELECT_PORT PORTB
#	define SELECT_PIN  PB2
#	define LED_PIN     9
#	define SPI_SS      10    // PB2, pin 16
#	define SPI_MOSI    11    // PB3, pin 17
#	define SPI_MISO    12    // PB4, pin 18
#	define SPI_SCK     13    // PB5, pin 19
#else
#	define IRQ_DDR     DDRB
#	define IRQ_PORT    PINB
#	define IRQ_PIN     PB3
#	define SELECT_DDR  PORTB
#	define SELECT_PORT PORTB
#	define SELECT_PIN  PB4
#	define LED_PIN     0
#	define SPI_SS      4    // PB4, pin 3
#	define SPI_MOSI    0    // PB0, pin 5
#	define SPI_MISO    1    // PB1, pin 6
#	define SPI_SCK     2    // PB2, pin 7
#endif

#define RF12_SELECT   (SELECT_PORT &= ~_BV(SELECT_PIN))
#define RF12_UNSELECT (SELECT_PORT |= _BV(SELECT_PIN))

static uint8_t nodeid;              // address of this node
static uint8_t group;               // network group


// my

uint8_t rf12_cmd(uint8_t highbyte, uint8_t lowbyte);

static uint8_t rf12_byte(uint8_t c)
{
#ifdef SPDR
	SPDR = c;
	while (!(SPSR & _BV(SPIF)));
	return SPDR;
#else
	USIDR = c;
	byte v1 = bit(USIWM0) | bit(USITC);
	byte v2 = bit(USIWM0) | bit(USITC) | bit(USICLK);
	USICR = v1; USICR = v2;
	USICR = v1; USICR = v2;
	USICR = v1; USICR = v2;
	USICR = v1; USICR = v2;
	USICR = v1; USICR = v2;
	USICR = v1; USICR = v2;
	USICR = v1; USICR = v2;
	USICR = v1; USICR = v2;
	return USIDR;
#endif
}

volatile uint8_t sidx = 0;
uint8_t sbuf[8] = 
{
	0xAA, 0x2D, 0xD4,
	0x0A, 0x01, 0x44, 0x18, 0x75,
	

};

static void rf12_interrupt()
{
	rf12_cmd(0x00, 0x00);
	rf12_cmd(0xB8, sbuf[sidx]);
	sidx++;
	if (sidx == 8)
	{
		rf12_cmd(0x82, 0x0D); // idle
//		rf12_cmd(0xB8, 0xAA);
		detachInterrupt(0);
	}
}


static void respond(char c)
{
	attachInterrupt(0, rf12_interrupt, LOW);

	sidx = 0;
	sbuf[5] = c;
	
	uint16_t crc = ~0;
	crc = _crc16_update(crc, group);
	uint8_t idx = 3;
	crc = _crc16_update(crc, sbuf[idx++]);
	crc = _crc16_update(crc, sbuf[idx++]);
	crc = _crc16_update(crc, sbuf[idx++]);

	sbuf[idx++] = crc;
	sbuf[idx++] = crc >> 8;
	
	rf12_cmd(0x82, 0x3D);
}

#define WAIT_IRQ_LO() while( IRQ_PORT & _BV(IRQ_PIN) );

static void respond2(char c)
{
	sidx = 0;
	sbuf[5] = c;
	
	uint16_t crc = ~0;
	crc = _crc16_update(crc, group);
	uint8_t idx = 3;
	crc = _crc16_update(crc, sbuf[idx++]);
	crc = _crc16_update(crc, sbuf[idx++]);
	crc = _crc16_update(crc, sbuf[idx++]);

	sbuf[idx++] = crc;
	sbuf[idx++] = crc >> 8;
	
	rf12_cmd(0x82, 0x3D); // start tx
	
	for (int i = 0; i < 8; i++)
	{
		WAIT_IRQ_LO();
		rf12_cmd(0xB8, sbuf[i]);
		rf12_cmd(0x00, 0x00);
	}
	
	rf12_cmd(0x82, 0x0D); // idle
}

#define MISO_LEVEL() (PINB & _BV(SPI_MISO))
#define MOSI_LOW() PORTB &= ~_BV(SPI_MOSI)

uint8_t rf12_read_status_MSB(void)
{
    RF12_SELECT;
    MOSI_LOW();
    asm volatile("nop");
    uint8_t c = (MISO_LEVEL());
    RF12_UNSELECT;
    return c;
}

static void respond3(char c)
{
	sidx = 0;
	sbuf[5] = c;
	
	uint16_t crc = ~0;
	crc = _crc16_update(crc, group);
	uint8_t idx = 3;
	crc = _crc16_update(crc, sbuf[idx++]);
	crc = _crc16_update(crc, sbuf[idx++]);
	crc = _crc16_update(crc, sbuf[idx++]);

	sbuf[idx++] = crc;
	sbuf[idx++] = crc >> 8;
	
	rf12_cmd(0x82, 0x3D); // start tx
	
	for (int i = 0; i < 8; i++)
	{
		while (!rf12_read_status_MSB());

		rf12_cmd(0xB8, sbuf[i]);
		rf12_cmd(0x00, 0x00);
	}
	
	rf12_cmd(0x82, 0x0D); // idle
}

uint8_t cycles = 0;

void rf12_send(char c)
{
	if (cycles % 2 == 0)
	{
		Serial.print("r1 ");
		respond(c);
	}
	else
	{
		Serial.print("r2 ");
		respond2(c);
	}

	cycles++;
}
/*
 * =========================================================
*/

void rf12_spi_init()
{
	bitSet(SELECT_PORT, SPI_SS);
	bitSet(SELECT_DDR, SPI_SS);
	digitalWrite(SPI_SS, 1);
	pinMode(SPI_SS, OUTPUT);
	pinMode(SPI_MOSI, OUTPUT);
	pinMode(SPI_MISO, INPUT);
	pinMode(SPI_SCK, OUTPUT);
#ifdef SPCR
//	SPCR = _BV(SPE) | _BV(MSTR);
	SPCR = _BV(SPE) | _BV(MSTR);
	// use clk/2 (2x 1/4th) for sending (and clk/8 for recv, see rf12_xferSlow)
//	SPSR |= _BV(SPI2X);
#else
	USICR = bit(USIWM0);
#endif
	IRQ_DDR &= ~_BV(IRQ_PIN); // IRQ input
	IRQ_PORT |= _BV(IRQ_PIN); // IRQ pullup
}

uint8_t rf12_cmd(uint8_t highbyte, uint8_t lowbyte)
{
	bitSet(SPCR, SPR0);

	RF12_SELECT;
	rf12_byte(highbyte);
	uint8_t c = rf12_byte(lowbyte);
	RF12_UNSELECT;

	bitClear(SPCR, SPR0);
	return c;
}

void rf12_reset_fifo()
{
	rf12_cmd(0xCA, 0x81); // clear ef bit
	rf12_cmd(0xCA, 0x83); // set ef bit
}

// 1. Configuration Setting Command
// ===============================================
#define RF_CONFIG       0x80
#define RF_CONFIG_EL    0x80 // enable TX register
#define RF_CONFIG_EF    0x40 // enable FIFO

//                            b0 b1
#define RF_FFREQ_433   0x10 // 0  1
#define RF_FFREQ_866   0x20 // 1  0
#define RF_FFREQ_915   0x30 // 1  1

//                              x3 x2 x1 x0 pF
#define RF_CAP_85pF     0x0 //  0  0  0  0  8.5
#define RF_CAP_90pF     0x1 //  0  0  0  1  9.0
#define RF_CAP_95pF     0x2 //  0  0  1  0  9.5
#define RF_CAP_100pF    0x3 // 0  0  1  1  10.0
#define RF_CAP_105pF    0x4 // 0  1  0  0  10.5
#define RF_CAP_110pF    0x5 // 0  1  0  1  11.0
#define RF_CAP_115pF    0x6 // 0  1  1  0  11.5
#define RF_CAP_120pF    0x7 // 0  1  1  1  12.0
#define RF_CAP_125pF    0x8 // 1  0  0  0  12.5
#define RF_CAP_130pF    0x9 // 1  0  0  1  13.0
#define RF_CAP_135pF    0xA // 1  0  1  0  13.5
#define RF_CAP_140pF    0xB // 1  0  1  1  14.0
#define RF_CAP_145pF    0xC // 1  1  0  0  14.5
#define RF_CAP_150pF    0xD // 1  1  0  1  15.0
#define RF_CAP_155pF    0xE // 1  1  1  0  15.5
#define RF_CAP_160pF    0xF // 1  1  1  1  16.0

// 2. Power Management Command
// ===============================================
#define RF_PWR_MGMT       0x82
#define RF_PWR_ER  0x80 //  Enables the whole receiver chain: RF front end, baseband, synthesizer, crystal oscillator
#define RF_PWR_EBB 0x40 // The receiver baseband circuit can be separately switched on
#define RF_PWR_ET  0x20 // Switches on the PLL, pamp, and starts the transmission (if RF_CONFIG_EL is set)
#define RF_PWR_ES  0x10 // Turns on the synthesizer
#define RF_PWR_EX  0x08 // Turns on the crystal oscillator
#define RF_PWR_EB  0x04 // Enables the low battery detector
#define RF_PWR_EW  0x02 // Enables the wake-up timer
#define RF_PWR_DC  0x01 // Disables the clock output (pin 8)

// 3. Frequency Setting Command
// ===============================================
#define RF_FREQ_CFG       0xA6

// 4. Data Rate Command
// ===============================================
#define RF_DRATE_CFG      0xC6

// 5. Receiver Control Command
// ===============================================
#define RF_RX_CTRL        0x90
#define RF_RX_VDI_OUT     0x04

// Bits 9-8 (d1 to d0): VDI (valid data indicator) signal response time setting
//                               d1 d0
#define RF_RX_RESP_FAST   0x0 // 0  0
#define RF_RX_RESP_MED    0x1 // 0  1
#define RF_RX_RESP_LOW    0x2 // 1  0
#define RF_RX_RESP_ON     0x3 // 1  1

// Bits 7-5 (i2 to i0): Receiver baseband bandwidth (BW) select
//                          i2 i1 i0 BW [kHz]
#define RF_RX_BW_400 0x2 // 0  0  1  400
#define RF_RX_BW_340 0x3 // 0  1  0  340
#define RF_RX_BW_270 0x6 // 0  1  1  270
#define RF_RX_BW_200 0x8 // 1  0  0  200
#define RF_RX_BW_134 0xA // 1  0  1  134
#define RF_RX_BW_67  0xC // 1  1  0   67

// Bits 4-3 (g1 to g0): LNA gain select:
//                              g1 g0  Gain relative to maximum [dB]
#define RF_RX_GAIN_0   0x00  // 0  0    0
#define RF_RX_GAIN_M6  0x08  // 0  1   -6
#define RF_RX_GAIN_M14 0x10  // 1  0  -14
#define RF_RX_GAIN_M20 0x18  // 1  1  -20

// Bits 2-0 (r2 to r0): RSSI detector threshold:
//                              r2 r1 r0 RSSI
#define RF_RX_RSSI_M103 0x00 // 0  0  0  -103
#define RF_RX_RSSI_M97  0x01 // 0  0  1  -97
#define RF_RX_RSSI_M91  0x02 // 0  1  0  -91
#define RF_RX_RSSI_M85  0x03 // 0  1  1  -85
#define RF_RX_RSSI_M79  0x04 // 1  0  0  -79
#define RF_RX_RSSI_M73  0x05 // 1  0  1  -73

// 6. Data Filter Command
// ===============================================
#define RF_DF 0xC2

// Bit 7 (al): Clock recovery (CR) auto lock control:
#define RF_DF_AL 0x80 // auto lock

// Bit 6 (ml): Clock recovery lock control:
#define RF_DF_ML 0x40 // fast mode: 4 to 8-bit preamble (1010...) is recommended

// Bit 5 and 3 are always set
#define RF_DF_SBITS 0x28

// Bit 4 (s): Select the type of the data filter:
#define RF_DF_S 0x00 // digital

// Bits 2-0 (f2 to f0): DQD threshold parameter.
#define RF_DF_DQD4 0x0C // digital

// 7. FIFO and Reset Mode Command
// ===============================================
#define RF_FIFO 0xCA

// 8. Synchron Pattern Command
// ===============================================
#define RF_PATTERN 0xCE

// 10. AFC Command
// ===============================================
#define RF_AFC 0xC4

// 11. TX Configuration Control Command
// ===============================================
#define RF_TX_CFG 0x98

// 12. PLL Setting Command
// ===============================================
#define RF_PLL_CFG 0xCC

// 14. Wake-Up Timer Command
// ===============================================
#define RF_WAKEUP_CFG 0xE0

// 15. Low Duty-Cycle Command
// ===============================================
#define RF_DUTY_CFG 0xC8

// 16. Low Battery Detector and Microcontroller Clock Divider Command
// ===============================================
#define RF_BATT_CFG 0xC0

void rf12_setup()
{
	rf12_spi_init();

	rf12_cmd(0, 0);
	delay(200);
	rf12_cmd(0, 0);

	rf12_cmd(RF_PWR_MGMT, RF_PWR_EB | RF_PWR_DC);
	rf12_cmd(RF_CONFIG, RF_CONFIG_EL | RF_CONFIG_EF | RF_FFREQ_433 | RF_CAP_120pF);
	rf12_cmd(RF_FREQ_CFG, 0x40); // 433.26MHz
	rf12_cmd(RF_DRATE_CFG, 0x06); // approx 49.2 Kbps, i.e. 10000/29/(1+6) Kbps
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
	rf12_reset_fifo();
}

void rf12_initialize(uint8_t id, uint8_t g)
{
	nodeid = id;
	group = g;
	rf12_setup();
}


