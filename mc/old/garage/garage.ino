/*
Release on attiny85:

      -------------
      | RST   VCC |
      | PB3   PB2 |
      | PB4   PB1 |
      | GND   PB0 |
      -------------
*/

#include <avr/sleep.h>
#include <avr/wdt.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

volatile boolean f_wdt = 1;

unsigned long door_opened_ts;
byte door_opened;
unsigned long timeout = 0;

const int door_pin = 4;
const int speaker_pin = 3;
const unsigned long DOOR_TIMEOUT = 10000;
byte counter = 0;

void dot()
{
  digitalWrite(speaker_pin, HIGH);
  delay(300);
  digitalWrite(speaker_pin, LOW);
  delay(300); 
}

void dot2()
{
  digitalWrite(speaker_pin, HIGH);
  delay(100);
  digitalWrite(speaker_pin, LOW);
  delay(100); 
}

static void play_sound()
{
  dot();
  dot();
  dot();
  dot2();
  dot2();
  dot2();
}

void setup()
{
  pinMode(speaker_pin, OUTPUT);     
  pinMode(door_pin, INPUT_PULLUP);
  door_opened_ts = 0;
  dot();
  dot();
  dot();
  dot();
  door_opened = millis();
  setup_watchdog(9); // approximately 4 seconds sleep
  // 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
}

void loop()
{
  if (f_wdt != 1)
    return;
    
  f_wdt = 0;
  door_opened = digitalRead(door_pin);

  if (door_opened)
  {
    counter++;
    if (counter > 3)
    {
      play_sound();
      counter = 0;
    }
  }
  else
  {
      door_opened_ts = 0;
      counter = 0;
  }
  
  system_sleep();
  byte i;
  for (i = 0; i < counter; i++)
  {
    dot2();
  }
  dot2();
//  delay(1000);
}

// set system into the sleep state 
// system wakes up when wtchdog is timed out
void system_sleep() {
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out 
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}

// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii)
{
  byte bb;
  int ww;
  if (ii > 9)
    ii = 9;
  bb=ii & 7;
  if (ii > 7)
    bb|= (1<<5);

  bb|= (1<<WDCE);
  ww=bb;

  MCUSR &= ~(1<<WDRF);
  // start timed sequence````
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}
  
// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect)
{
  f_wdt=1;  // set global flag
}

