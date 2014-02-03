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


uint8_t rf12_cmd(uint8_t highbyte, uint8_t lowbyte);
void rf12_reset_fifo();
void rf12_rx_off();

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

uint16_t rf12_read_status()
{
	RF12_SELECT;
	uint16_t c = rf12_byte(0x00) << 8;
	c |= rf12_byte(0x00);
	RF12_UNSELECT;
	return c;
}

// packet format:
// [0]            : 0xAA
// [1]            : 0x2D
// [2]            : 0xD4
// [3]            : id   -- rf12_rx_buf
// [4]            : len
// [5]..[len-3]   : data -- rf12_data
// [len-2][len-1] : crc

uint8_t rf12_packet[30];
uint8_t* rf12_rx_buf = &rf12_packet[3]; // 3 preambe bytes are consumed by hardware
uint8_t* rf12_data = &rf12_packet[5];

volatile uint8_t rf12_len = 0;
volatile uint8_t sidx = 0;
volatile uint8_t receiving = 0;
volatile uint8_t rcv_done = 0;
uint8_t rf12_debug = 0;

static void rf12_tx_interrupt()
{
	rf12_cmd(0x00, 0x00);
	rf12_cmd(0xB8, rf12_packet[sidx]);
	sidx++;
}

uint8_t verify_data()
{
	uint16_t crc = ~0;
	crc = _crc16_update(crc, group);
	
	int i = 0;
	for (i = 0; i < rf12_len + 2; i++)
		crc = _crc16_update(crc, rf12_rx_buf[i]);

	uint16_t expected_crc = rf12_rx_buf[i++];
	expected_crc |= rf12_rx_buf[i] << 8;

	if (expected_crc != crc && rf12_debug)
	{
		Serial.print(" ex: ");
		Serial.print(expected_crc, HEX);
		Serial.print(" calc: ");
		Serial.println(crc, HEX);
	}
	
	return (expected_crc == crc);
}

static void rf12_rx_interrupt()
{
	if (rcv_done)
		return;

	uint16_t st = rf12_read_status();
	if (!(st & 0x8000))
	{
		if (rf12_debug)
		{
			Serial.print("rst:");
			Serial.println(st, HEX);
		}
		return;
	}

	uint8_t c = rf12_cmd(0xB0, 0x00);
	rf12_rx_buf[sidx++] = c;

	if (sidx == 1)
	{
		receiving = 1;
		return;
	}

	if (sidx == 2)
	{
		rf12_len = c;
		if (rf12_len == 0 || rf12_len > 20)
		{
			rf12_len = 0;
			rf12_rx_off();
			rcv_done = 1;
		}
		return;
	}

	if (sidx == rf12_len + 4)
	{
		rf12_rx_off();
		rcv_done = 1;
	}
}

void print_buf()
{
	if (!rf12_debug && rf12_len == 0)
		return;

	if (verify_data())
	{
		Serial.print("    len: ");
		Serial.print(rf12_len);
		Serial.print(" data: ");
		rf12_data[rf12_len] = 0;
		Serial.print((char*)rf12_data);
	}
	else
	{
		for (int i = 0; i < rf12_len+4; i++)
		{
			Serial.print(rf12_rx_buf[i], HEX);
			Serial.print(' ');
		}
		Serial.print(" len: ");
		Serial.println(rf12_len);
	}
}

static void respond(uint8_t len)
{
	attachInterrupt(0, rf12_tx_interrupt, LOW);

	rf12_len = len;
	uint8_t i = 0;

	rf12_packet[i++] = 0xAA;
	rf12_packet[i++] = 0x2D;
	rf12_packet[i++] = 0xD4;
	rf12_packet[i++] = 0x0A;
	rf12_packet[i++] = len;

	uint16_t crc = ~0;
	crc = _crc16_update(crc, group);

	for (i = 0; i < len + 2; i++)
		crc = _crc16_update(crc, rf12_rx_buf[i]);

	rf12_rx_buf[i++] = crc;
	rf12_rx_buf[i] = crc >> 8;
	
	// preamble(3) + (id, len)(2) + data len + crc(2)
	const uint8_t send_len = 3 + 2 + len + 2;
	sidx = 0;

	// fill fifo with preamble before turning on tx
	rf12_cmd(0xB8, 0xAA);
	rf12_cmd(0xB8, 0xAA);

	rf12_cmd(0x82, 0x3D); // start tx

	while (sidx < send_len);

	detachInterrupt(0);
	
	delay(100);

	if (rf12_debug)
	{
		Serial.print("sent: ");
		for (i = 0; i < send_len; i++)
		{
			Serial.print(rf12_packet[i], HEX);
			Serial.print(" ");
		}
		Serial.print("send_len: ");
		Serial.println(send_len);
	}
	
	rf12_cmd(0x82, 0x0D); // idle

}

#define WAIT_IRQ_LO() while( IRQ_PORT & _BV(IRQ_PIN) );

static void respond2(uint8_t len)
{
	sidx = 0;
	rf12_len = len;
	uint8_t i = 0;

	rf12_packet[i++] = 0xAA;
	rf12_packet[i++] = 0x2D;
	rf12_packet[i++] = 0xD4;
	rf12_packet[i++] = 0x0A;
	rf12_packet[i++] = len;

	uint16_t crc = ~0;
	crc = _crc16_update(crc, group);

	for (i = 0; i < len + 2; i++)
		crc = _crc16_update(crc, rf12_rx_buf[i]);

	rf12_rx_buf[i++] = crc;
	rf12_rx_buf[i] = crc >> 8;

	rf12_cmd(0x82, 0x3D); // start tx
	
	// preamble(3) + (id, len)(2) + data len + crc(2)
	const uint8_t send_len = 3 + 2 + len + 2;

	for (i = 0; i < send_len; i++)
	{
		WAIT_IRQ_LO();
		rf12_cmd(0xB8, rf12_packet[i]);
		rf12_cmd(0x00, 0x00);
	}
	
	WAIT_IRQ_LO();
	rf12_cmd(0xB8, 0x00);
	rf12_cmd(0x00, 0x00);

	rf12_cmd(0x82, 0x0D); // idle

	if (rf12_debug)
	{
		Serial.print("sent: ");
		for (i = 0; i < send_len; i++)
		{
			Serial.print(rf12_packet[i], HEX);
			Serial.print(" ");
		}
		Serial.print("send_len: ");
		Serial.println(send_len);
	}
}

void rf12_send(uint8_t len)
{
	respond(len);
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
	sidx = 0;
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

void rf12_rx_on()
{
	rf12_reset_fifo();
	sidx = 0;
	rf12_len = 0;
	rcv_done = 0;
	attachInterrupt(0, rf12_rx_interrupt, LOW);
	rf12_cmd(RF_PWR_MGMT, RF_PWR_ER|RF_PWR_EBB|RF_PWR_ES | RF_PWR_EX|RF_PWR_EB|RF_PWR_DC);
}

void rf12_rx_off()
{
	detachInterrupt(0); // detach before disabling rx, otherwise it will stack in the interrupt
	rf12_cmd(RF_PWR_MGMT, RF_PWR_EX|RF_PWR_EB|RF_PWR_DC);
	receiving = 0;
}
