//#define _DEBUG

/*
Debug on Arduinouno:

Serial cable (http://www.adafruit.com/products/954) connection:


 ICSP
.
 ----
 |G.|
 |W.|
 |.B|
 ----

Green - TX
White - RX
Black - GND


Release on attiny85:

      -------------
      | PB5   VCC |
      | PB3   PB2 |
      | PB4   PB1 |
      | GND   PB0 |
      -------------
*/

unsigned long door_opened_ts;
byte door_opened;
unsigned long timeout = 0;

const int door_pin = 2;
const int speaker_pin = 8;
const unsigned long DOOR_TIMEOUT = 60000;

#ifdef _DEBUG
#include <SoftwareSerial.h>
SoftwareSerial com3(12, 13); // RX, TX

static void setup_com3()
{
  com3.begin(9600);
  com3.println("com3 started");
}

static void debug_com3()
{
  if (com3.available() <= 0)
    return;

  byte c = com3.read();
  if (c == 's')
  {
    com3.print("door: ");
    com3.print(door_opened);
    com3.print(", timeout: ");
    com3.print(timeout);
    com3.print(", ts: ");
    com3.println(door_opened_ts);
  }
  else
  {
    com3.print("unknown command: ");
    com3.println(c);
  }
}
#else
static void setup_com3(){}
static void debug_com3(){}
#endif

void setup()
{
  setup_com3();
  pinMode(door_pin, INPUT_PULLUP);
  door_opened_ts = 0;
  door_opened = millis();
}

static void play_sound()
{
    tone(speaker_pin, 440, 300);
    delay(100);
    tone(speaker_pin, 880, 300);
    delay(100);
    tone(speaker_pin, 660, 300);
    delay(100);
    tone(speaker_pin, 1320, 300);
    delay(100);
}

void loop()
{
  door_opened = digitalRead(door_pin);
  
  if (door_opened)
  {
    if (door_opened_ts == 0)
      door_opened_ts = millis();

    timeout = millis() - door_opened_ts;
    if (timeout > DOOR_TIMEOUT)
    {
      play_sound();
      door_opened_ts = 0;
    }
  }
  else
  {
      door_opened_ts = 0;
  }
  
  debug_com3();
}

