#include <avr/io.h>
#include <util/crc16.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "rfm12b.h"

#define RF12_SELECT   (SELECT_PORT &= ~_BV(SELECT_PIN))
#define RF12_UNSELECT (SELECT_PORT |= _BV(SELECT_PIN))

static uint8_t node_id;
static uint8_t group;

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

volatile uint8_t          rf12_len = 0;
volatile uint8_t          sidx = 0;
volatile enum rf12_state  rf12_state = IDLE;

uint8_t rf12_cmd(uint8_t highbyte, uint8_t lowbyte);
void rf12_reset_fifo(void);
uint8_t rf12_read_status_MSB(void);

struct config
{
	uint8_t debug: 1;
};

static struct config cfg = {
	.debug = 0
};

static void
start_tx(void)
{
	rf12_state = TX_IN_PROGRESS;
	rf12_cmd(0x82, 0x3D);
}

static void
set_idle(void)
{
	rf12_cmd(0x82, 0x0D);
	rf12_state = IDLE;
}

uint8_t
verify_data(void)
{
	uint16_t crc = ~0;
	crc = _crc16_update(crc, group);

	int i = 0;
	for (i = 0; i < rf12_len + 2; i++)
		crc = _crc16_update(crc, rf12_rx_buf[i]);

	uint16_t expected_crc = rf12_rx_buf[i++];
	expected_crc |= rf12_rx_buf[i] << 8;

	if (expected_crc != crc && cfg.debug)
		printf(" ex: %4X, calc: %4X\n", expected_crc, crc);

	return (expected_crc == crc);
}

#if defined(__AVR_ATmega328P__)

#define MOSI_LOW     (PORTB &= ~_BV(SPI_MOSI))
#define MISO_LEVEL   (PINB & _BV(SPI_MISO))

void (*int0_handler)(void) = NULL;

ISR(INT0_vect)
{
	if (int0_handler)
		int0_handler();
}

static uint8_t
rf12_byte(uint8_t c)
{
	SPDR = c;
	while (!(SPSR & _BV(SPIF)));
	return SPDR;
}

uint16_t
rf12_read_status(void)
{
	RF12_SELECT;
	uint16_t c = rf12_byte(0x00) << 8;
	c |= rf12_byte(0x00);
	RF12_UNSELECT;
	return c;
}

static void
enable_interrupt(void (*handler)(void))
{
	int0_handler = handler;
	EICRA &= ~(_BV(ISC01) | _BV(ISC00)); // low level   
	EIMSK |= _BV(INT0);
}

static void
disable_interrupt(void)
{
	EIMSK &= ~(_BV(INT0));
	int0_handler = NULL;
}

static void
rx_interrupt(void)
{
	if (rf12_state >= RX_DONE_OK)
		return;

	if (sidx == 0)
		rf12_state = RX_IN_PROGRESS;

	uint16_t st = rf12_read_status();
	if (!(st & 0x8000))
	{
		if (cfg.debug)
			printf("rx st: %04X, rf12_state: %d\n", st, rf12_state);
		return;
	}

	uint8_t c = rf12_cmd(0xB0, 0x00);
	rf12_rx_buf[sidx++] = c;

	if (sidx == 2)
	{
		rf12_len = c;
		if (rf12_len == 0 || rf12_len > 20)
		{
			rf12_len = 0;
			rf12_state = RX_DONE_OVERFLOW;
			rf12_rx_off();
		}
		return;
	}

	if (sidx == rf12_len + 4)
	{
		if (verify_data())
			rf12_state = RX_DONE_OK;
		else
			rf12_state = RX_DONE_BADCRC;
		rf12_rx_off();
	}
}

static void
tx_interrupt(void)
{
	if (rf12_state != TX_IN_PROGRESS)
		return;

	uint16_t st = rf12_read_status();
	if (!(st & 0x8000))
	{
		if (cfg.debug)
			printf("tx st: %04X\n", st);
		return;
	}

	rf12_cmd(0xB8, rf12_packet[sidx]);
	sidx++;
	if (rf12_len == sidx)
		rf12_state = TX_DONE;
}

static void
respond(uint8_t len)
{
	enable_interrupt(tx_interrupt);

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

	start_tx();

	while (rf12_state != TX_DONE);

	disable_interrupt();

	_delay_ms(100);

	if (cfg.debug)
	{
		printf("sent: ");
		for (i = 0; i < send_len; i++)
		{
			printf("%02X ", rf12_packet[i]);
		}
		printf("send_len: %d\n", send_len);
	}

	set_idle();

}

#elif defined(__AVR_ATtiny85__)

#define MOSI_LOW     (PORTB &= ~_BV(PB1))
#define MISO_LEVEL   (PINB & _BV(PB0))

static void
spi_run_clock(void)
{
	USICR = _BV(USIWM0) | _BV(USITC);
	USICR = _BV(USIWM0) | _BV(USITC) | _BV(USICLK);
	USICR = _BV(USIWM0) | _BV(USITC);
	USICR = _BV(USIWM0) | _BV(USITC) | _BV(USICLK);
	USICR = _BV(USIWM0) | _BV(USITC);
	USICR = _BV(USIWM0) | _BV(USITC) | _BV(USICLK);
	USICR = _BV(USIWM0) | _BV(USITC);
	USICR = _BV(USIWM0) | _BV(USITC) | _BV(USICLK);
	USICR = _BV(USIWM0) | _BV(USITC);
	USICR = _BV(USIWM0) | _BV(USITC) | _BV(USICLK);
	USICR = _BV(USIWM0) | _BV(USITC);
	USICR = _BV(USIWM0) | _BV(USITC) | _BV(USICLK);
	USICR = _BV(USIWM0) | _BV(USITC);
	USICR = _BV(USIWM0) | _BV(USITC) | _BV(USICLK);
	USICR = _BV(USIWM0) | _BV(USITC);
	USICR = _BV(USIWM0) | _BV(USITC) | _BV(USICLK);
}

uint16_t
rf12_read_status(void)
{
	RF12_SELECT;
	USIDR = 0x00;	//Status Read Command
	spi_run_clock();
	uint16_t c = USIDR << 8;
	USIDR = 0x00; 	//Status Read Command
	spi_run_clock();
	c |= USIDR;
	RF12_UNSELECT;
	return c;
}

#define WAIT_IRQ_LO() while( IRQ_PORT & _BV(IRQ_PIN) );

static void
respond(uint8_t len)
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

	start_tx();

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

	set_idle();

	if (cfg.debug)
	{
		printf("sent: ");
		for (i = 0; i < send_len; i++)
		{
			printf("%02X ", rf12_packet[i]);
		}
		printf("send_len: %d\n", send_len);
	}
}

uint8_t rf12_wait_rx()
{
	uint16_t cnt = 0xFFFF;
	while (!rf12_read_status_MSB() && --cnt);
	return (cnt > 0);
}

uint8_t rf12_read_rx(void)
{
	if (sidx == 0)
		rf12_state = RX_IN_PROGRESS;

	uint8_t c = rf12_cmd(0xB0, 0x00);
	rf12_rx_buf[sidx++] = c;

	if (sidx == 2)
		rf12_len = c;

	if (rf12_len >= 20)
	{
		rf12_state = RX_DONE_OVERFLOW;
		rf12_len = 20;
	}

	if (sidx == rf12_len + 4)
	{
		if (verify_data())
			rf12_state = RX_DONE_OK;
		else
			rf12_state = RX_DONE_BADCRC;
	}
	
	return rf12_state;
}


// ATTiny85
#else
#pragma error ("MCU is not specified")
#endif

uint8_t
rf12_read_status_MSB(void)
{
    RF12_SELECT;
    MOSI_LOW;
    asm volatile("nop");
    return MISO_LEVEL;
}

static void
tx(uint8_t c)
{
	while (!rf12_read_status_MSB());
	rf12_cmd(0xB8, c);
}

void
rf12_send_sync(const char*s, uint8_t n)
{
	uint16_t crc = ~0;

	crc = _crc16_update(crc, group);
	crc = _crc16_update(crc, node_id);
	crc = _crc16_update(crc, n);

	for (uint8_t i = 0; i < n; i++)
		crc = _crc16_update(crc, s[i]);

	rf12_reset_fifo();

	start_tx();

	tx(0xAA);        // preamble
	tx(0x2D);        // sync hi byte
	tx(0xD4);        // sync low byte
	tx(0x0A);        // id
	tx(n);           // len
	
	for (uint8_t i = 0; i < n; i++)
		tx(s[i]);
	
	tx(crc & 0xFF);  // crc lo
	tx(crc >> 8);    // crc hi
	tx(0);           // dummy byte
	tx(0);           // dummy byte
	while (!rf12_read_status_MSB()); // wait dummy byte to complete
	_delay_ms(50); // not sure if this is required

	set_idle();
}

void
print_buf(void)
{
	if (!cfg.debug && rf12_len == 0)
		return;

	if (verify_data())
	{
		rf12_data[rf12_len] = 0;
		printf("    len: %d, data: %s", rf12_len, (char*)rf12_data);
	}
	else
	{
		for (int i = 0; i < rf12_len+4; i++)
			printf("%02X ", rf12_rx_buf[i]);
		printf(" len: %d\n", rf12_len);
	}
}


void
rf12_send(uint8_t len)
{
	respond(len);
}

void
rf12_spi_init(void)
{
	SELECT_DDR |= _BV(SELECT_PIN) | _BV(SPI_SCK); // out
	RF12_UNSELECT;

#ifdef SPCR
	SPCR = _BV(SPE) | _BV(MSTR);

	SELECT_DDR |= _BV(SPI_MOSI);
	SELECT_DDR &= ~_BV(SPI_MISO);

	IRQ_DDR &= ~_BV(IRQ_PIN); // IRQ input
	IRQ_PORT |= _BV(IRQ_PIN); // IRQ pullup
#else
	SELECT_DDR |= _BV(SPI_MISO); // in attiny85 SO goes to SDO
	SELECT_DDR &= ~_BV(SPI_MOSI); // in attiny85 SI goes to SDI
	USICR = _BV(USIWM0);
#endif
}

#ifdef SPCR

uint8_t
rf12_cmd(uint8_t highbyte, uint8_t lowbyte)
{
	SPCR |= _BV(SPR0);

	RF12_SELECT;
	rf12_byte(highbyte);
	uint8_t c = rf12_byte(lowbyte);
	RF12_UNSELECT;

	SPCR &= ~_BV(SPR0);
	return c;
}

#else

uint8_t
rf12_cmd(uint8_t highbyte, uint8_t lowbyte)
{
	RF12_SELECT;
	USIDR = highbyte;
	spi_run_clock();
	USIDR = lowbyte;
	spi_run_clock();
	RF12_UNSELECT;
	return USIDR;
}
#endif

void
rf12_reset_fifo()
{
	rf12_cmd(0xCA, 0x81); // clear ef bit
	sidx = 0;
	rf12_cmd(0xCA, 0x83); // set ef bit
}

#include "rfm12b_defs.h"

void
rf12_setup(void)
{
	rf12_spi_init();

	rf12_cmd(0, 0);

	_delay_ms(200);
	rf12_cmd(0, 0);

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
//	rf12_cmd(RF_BATT_CFG, 0x00); // 1MHz,2.2V
	rf12_cmd(RF_PWR_MGMT, RF_PWR_ER|RF_PWR_EBB|RF_PWR_ES | RF_PWR_EX|RF_PWR_EB|RF_PWR_DC);
	rf12_reset_fifo();
}

void
rf12_initialize(uint8_t id, uint8_t g)
{
	node_id = id;
	group = g;
	rf12_setup();
}

void
rf12_rx_on()
{
	rf12_reset_fifo();
	sidx = 0;
	rf12_len = 0;
	rf12_state = RX_ON;
#if defined(__AVR_ATmega328P__)
	enable_interrupt(rx_interrupt);
	if (cfg.debug)
		printf("int0 on rx\n");
#endif
	rf12_cmd(RF_PWR_MGMT, RF_PWR_ER|RF_PWR_EBB|RF_PWR_ES | RF_PWR_EX|RF_PWR_EB|RF_PWR_DC);
}

void
rf12_rx_off()
{
#if defined(__AVR_ATmega328P__)
	disable_interrupt();
	if (cfg.debug)
		printf("int0 off\n");
#endif
	// set idle but do not change rf12_state
	rf12_cmd(RF_PWR_MGMT, RF_PWR_EX|RF_PWR_EB|RF_PWR_DC);
}

void
rf12_debug(uint8_t flag)
{
	cfg.debug = flag;
}

