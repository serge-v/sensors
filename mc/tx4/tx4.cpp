#include <Arduino.h>

#define myNodeID 10          //node ID of tx (range 0-30)
#define network     212      //network group (can be in the range 1-250).

void rf12_initialize(uint8_t id, uint8_t g);
void rf12_send(char c);

char ch = 'A';

void setup()
{
  pinMode(7, OUTPUT);
  rf12_initialize(myNodeID, network);
  Serial.begin(115200);
  Serial.println("started");
}

void loop()
{
  if (ch++ > 'Z')
    ch = 'A';
  
  rf12_send(ch);
    
  Serial.println(ch); 
  
  digitalWrite(7, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for a second
  digitalWrite(7, LOW);    // turn the LED off by making the voltage LOW
  delay(2000);
}

