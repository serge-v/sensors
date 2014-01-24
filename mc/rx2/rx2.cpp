#include <avr/io.h>
#include <Arduino.h>
//#include <JeeLib.h>

#define myNodeID 30          //node ID of Rx (range 0-30) 
#define network     212      //network group (can be in the range 1-250).
#define freq RF12_433MHZ     //Freq of RF12B can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. Match freq to module

#define RFM_IRQ     2
#define SS_DDR      DDRB
#define SS_PORT     PORTB
#define SS_BIT      2     // for PORTB: 2 = d.10, 1 = d.9, 0 = d.8

#define SPI_SS      10    // PB2, pin 16
#define SPI_MOSI    11    // PB3, pin 17
#define SPI_MISO    12    // PB4, pin 18
#define SPI_SCK     13    // PB5, pin 19

char emontx;

const int emonTx_NodeID=10;            //emonTx node ID

const int led_pin = 7;

int test = 0;

void setup()
{
	pinMode(led_pin, OUTPUT);
	Serial.begin(9600);
	while (!Serial);
	while (!Serial.available());
	Serial.read();
	Serial.println("rx2:");
	Serial.println("t1 -- jeelib rx test");
	Serial.println("t2 -- my rx test");
}

static void dot()
{
	digitalWrite(7, HIGH);   // turn the LED on (HIGH is the voltage level)
	delay(50);               // wait for a second
	digitalWrite(7, LOW);    // turn the LED off by making the voltage LOW
	delay(100);
}

static void RF12_CS()
{
	PORTB &= ~_BV(PB2);
}

static void RF12_NOT_CS()
{
	PORTB |= _BV(PB2);
}

static byte wait_nirq()
{
	int cnt = 32000;

	while ((PIND & _BV(PD2)) && --cnt);

	if (cnt == 0)
	{
//		dot();
		return 0;
	}

	return 1;
}

static byte rf12_read_status()
{
	RF12_CS();
	SPDR = 0x00;
	while (!(SPSR & _BV(SPIF)));
	byte c1 = SPDR;
	SPDR = 0x00;
	while (!(SPSR & _BV(SPIF)));
	byte c2 = SPDR;
	RF12_NOT_CS();
	return c1;
}

static uint8_t rf12_rx_slow()
{
	// slow down to under 2.5 MHz
	bitSet(SPCR, SPR0);

	RF12_CS();
	SPDR = 0xB0;
	while (!(SPSR & _BV(SPIF)));
	SPDR = 0x00;
	while (!(SPSR & _BV(SPIF)));
	RF12_NOT_CS();
	char c = SPDR;

	bitClear(SPCR, SPR0);

	return c;
}

uint8_t rf12_RX(void)
{
//    rf12_loop_until_FFIT_RGIT();
	RF12_CS();
	SPDR = 0xB0;
	while (!(SPSR & _BV(SPIF)));
	SPDR = 0x00;
	while (!(SPSR & _BV(SPIF)));
	RF12_NOT_CS();
	return SPDR;
}

static byte rf12_cmd(uint8_t highbyte, uint8_t lowbyte)
{
	RF12_CS();
	SPDR = highbyte;
	while (!(SPSR & _BV(SPIF)));
	SPDR = lowbyte;
	while (!(SPSR & _BV(SPIF)));
	RF12_NOT_CS();
	return SPDR;
}

static void spi_init()
{
	bitSet(SS_PORT, SPI_SS);
	bitSet(SS_DDR, SPI_SS);
	digitalWrite(SPI_SS, 1);
	pinMode(SPI_SS, OUTPUT);
	pinMode(SPI_MOSI, OUTPUT);
	pinMode(SPI_MISO, INPUT);
	pinMode(SPI_SCK, OUTPUT);
	SPCR = _BV(SPE) | _BV(MSTR);
	// use clk/2 (2x 1/4th) for sending (and clk/8 for recv, see rf12_xferSlow)
	SPSR |= _BV(SPI2X);
	
	DDRD &= ~_BV(RFM_IRQ); // IRQ input
	PORTD |= _BV(RFM_IRQ); // IRQ pullup
}

static void setup2()
{
	spi_init();
	rf12_cmd(0, 0);
	rf12_cmd(0x82, 0x05);
	rf12_cmd(0x80, 0xD8); // EL (ena TX), EF (ena RX FIFO), 12.0pF
	rf12_cmd(0xA6, 0x40); // 433.26MHz
	rf12_cmd(0xC6, 0x06); // approx 49.2 Kbps, i.e. 10000/29/(1+6) Kbps
	rf12_cmd(0x94, 0xA2); // VDI,FAST,134kHz,0dBm,-91dBm
	rf12_cmd(0xC2, 0xAC); // AL,!ml,DIG,DQD4
	rf12_cmd(0xCA, 0x83); // FIFO8,2-SYNC,!ff,DR
	rf12_cmd(0xCE, 0xd4); // SYNC=2DXX
	rf12_cmd(0xC4, 0x83); // @PWR,NO RSTRIC,!st,!fi,OE,EN
	rf12_cmd(0x98, 0x50); // !mp,90kHz,MAX OUT
	rf12_cmd(0xCC, 0x77); // OB1,OB0, LPX,!ddy,DDIT,BW0
	rf12_cmd(0xE0, 0x00); // NOT USE
	rf12_cmd(0xC8, 0x00); // NOT USE
	rf12_cmd(0xC0, 0x49); // 1.66MHz,3.1V
	rf12_cmd(0xCA, 0x81); // clear ef bit
	rf12_cmd(0xCA, 0x83); // set ef bit
	rf12_cmd(0x82, 0xDD); // receiver on
}

byte c1 = 0, c2 = 0;
byte wait_irq = 1;
byte buf[20];
byte idx = 0;
int total_chars = 0;

static void test2()
{
	if (wait_irq)
	{
		if (!wait_nirq())
			return;
	}

	byte c = rf12_read_status();

	if ((c & 0x80) == 0)
		return;

	c = rf12_rx_slow();
	buf[idx] = c;
	idx++;
	total_chars++;
	if (idx >= 20)
		idx = 0;

	while (!(PIND & _BV(PD2)));
}

static void setup_test()
{
	test = Serial.parseInt();
	if (test == 2)
		setup2();
	Serial.print("setup test: ");
	Serial.println(test);
}

static void send_command()
{
	byte rc = rf12_cmd(c1, c2);
	Serial.print("sent: ");
	Serial.print(c1, HEX);
	Serial.print(' ');
	Serial.print(c2, HEX);
	Serial.print(" rc: ");
	Serial.println(rc, HEX);
}

void loop()
{

	while (Serial.available() > 0)
	{
		delay(100);
		char c = Serial.read();
		if (c == 't')
			setup_test();
		else if (c == 'c')
		{
			c = Serial.read();
			if (c != ' ')
			{
				Serial.print("bad separator 1: ");
				Serial.println(c);
				continue;
			}

			char s[2];

			if (Serial.readBytes(s, 2) != 2)
			{
				Serial.print("bad s1: ");
				Serial.println(s);
				continue;
			}

			c1 = strtoul(s, NULL, 16);
			*s = 0;

			c = Serial.read();
			if (c != ' ')
			{
				Serial.print("bad separator 2: ");
				Serial.println(c);
				continue;
			}

			if (Serial.readBytes(s, 2) != 2)
			{
				Serial.println("bad s2: ");
				Serial.println(s);
				continue;
			}

			c2 = strtoul(s, NULL, 16);

			Serial.print("cmd: ");
			Serial.print(c1, HEX);
			Serial.print(' ');
			Serial.println(c2, HEX);
		}
		else if (c == 's')
		{
			send_command();
		}
		else if (c == 'w')
		{
			for (int i = 0; i < 10; i++)
			{
				Serial.println();
			}
		}
		else if (c == 'q')
		{
			wait_irq = !wait_irq;
		}
		else if (c == 'd')
		{
			Serial.print("PORTB: ");
			Serial.println(PORTB, HEX);
			Serial.print("DDRB: ");
			Serial.println(DDRB, HEX);
			Serial.print("PINB: ");
			Serial.println(PINB, HEX);

			Serial.print("PORTC: ");
			Serial.println(PORTC, HEX);
			Serial.print("DDRC: ");
			Serial.println(DDRC, HEX);
			Serial.print("PINC: ");
			Serial.println(PINC, HEX);

			Serial.print("PORTD: ");
			Serial.println(PORTD, HEX);
			Serial.print("DDRD: ");
			Serial.println(DDRD, HEX);
			Serial.print("PIND: ");
			Serial.println(PIND, HEX);

			Serial.print("SPCR: ");
			Serial.println(SPCR, HEX);
			Serial.print("SPSR: ");
			Serial.println(SPSR, HEX);
			Serial.print("EIMSK: ");
			Serial.println(EIMSK, HEX);
			Serial.print("wait_irq: ");
			Serial.println(wait_irq);
			
			Serial.print("buf: ");
			for (int i = 0; i < 20; i++)
			{
				Serial.print(buf[i], HEX);
				Serial.print(' ');
			}
			Serial.print(" idx: ");
			Serial.println(idx);
			Serial.print("chars: ");
			Serial.println(total_chars);
		}
		else if (c == 'p')
		{
			spi_init();
		}
	}

	if (test == 2)
		test2();
	else if (test == 5)
	{
		dot();
		dot();
		dot();
		delay(2500);
	}
}

