#include <JeeLib.h>

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

void setup() {
  pinMode(led_pin, OUTPUT);
  Serial.begin(9600); 
  Serial.println("rx2:"); 
  Serial.println("t1 -- jeelib rx test"); 
  Serial.println("t2 -- my rx test"); 
}


static void setup1()
{
  rf12_initialize(myNodeID,freq,network);   //Initialize RFM12 with settings defined above  
  Serial.print("Node: "); 
  Serial.print(myNodeID); 
  Serial.print(" Freq: "); 
  if (freq == RF12_433MHZ) Serial.print("433Mhz");
  if (freq == RF12_868MHZ) Serial.print("868Mhz");
  if (freq == RF12_915MHZ) Serial.print("915Mhz"); 
  Serial.print(" Network: "); 
  Serial.println(network);
}

static void test1()
{
  if (!rf12_recvDone())
    return;

  int node_id = (rf12_hdr & 0x1F);
  emontx=*(char*)rf12_data;
  Serial.print("c: ");
  Serial.println(emontx); 
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

  while((PORTD & _BV(PD2)) && --cnt);
   
  if (cnt == 0)
  {
    dot();
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

static void setup2()
{
  detachInterrupt(0); // cancel jeelib int0 interrupt
  bitSet(PORTD, RFM_IRQ);
}

byte c1 = 0, c2 = 0;
byte wait_irq = 0;
byte buf[20];
byte idx = 0;

static void test2()
{
  if (wait_irq)
  {
    if (!wait_nirq())
    {
      delay(300);
      return;
    }
  }

  byte c = rf12_read_status();
  if ((c & 0x80) == 0)
    return;
  
  c = rf12_rx_slow();
  buf[idx] = c;
  idx++;
  
  if (idx >= 20)
  {
    Serial.print("buf: ");
    for (int i = 0; i < 20; i++)
    {
      Serial.print(buf[i], HEX);
      Serial.print(' ');
      buf[i] = 0;
    }
    Serial.println();
    idx = 0;
  }
}

static void setup_test()
{
  test = Serial.parseInt();
  if (test == 1)
    setup1();
  else if (test == 2)
    setup2();
  Serial.print("setup test: ");  Serial.println(test);
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
  pinMode(RFM_IRQ, INPUT);
  digitalWrite(RFM_IRQ, 1); // pull-up
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

void loop() {

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
      Serial.print("PORTB: "); Serial.println(PORTB, HEX);
      Serial.print("PORTC: "); Serial.println(PORTC, HEX);
      Serial.print("PORTD: "); Serial.println(PORTD, HEX);
      Serial.print("SPCR: ");  Serial.println(SPCR, HEX);
      Serial.print("SPSR: ");  Serial.println(SPSR, HEX);
      Serial.print("EIMSK: ");  Serial.println(EIMSK, HEX);
      Serial.print("wait_irq: "); Serial.println(wait_irq);
    }
    else if (c == 'p')
    {
      spi_init();
    }
    else if (c == 'r')
    {
      c = Serial.read();
      if (c != ' ')
      {
        Serial.print("bad separator 1: ");
        Serial.println(c);
        continue;
      }

      byte pin = Serial.parseInt();
      Serial.print("pin: ");
      Serial.print(pin);
      Serial.print(" set: ");
      Serial.print(digitalRead(pin));
      Serial.println(c);
    }
  }
  
  if (test == 1)
    test1();
  else if (test == 2)
    test2();
  else if (test == 5)
  {
    dot();
    dot();
    dot();
    delay(2500);
  }
}

