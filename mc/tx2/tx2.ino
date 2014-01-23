//Simple RFM12B wireless demo - transimtter - no ack
//Glyn Hudson openenergymonitor.org GNU GPL V3 7/7/11
//Credit to JCW from Jeelabs.org for RFM12 

#include <JeeLib.h>  //from jeelabs.org

#define myNodeID 10          //node ID of tx (range 0-30)
#define network     212      //network group (can be in the range 1-250).
#define freq RF12_433MHZ     //Freq of RF12B can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. Match freq to module


char emontx = 'A';

void setup() {

  pinMode(7, OUTPUT);
  rf12_initialize(myNodeID,freq,network);   //Initialize RFM12 with settings defined above  
  Serial.begin(9600);
  Serial.println("RFM12B Transmitter - Simple demo");



 Serial.print("Node: "); 
 Serial.print(myNodeID); 
 Serial.print(" Freq: "); 
 if (freq == RF12_433MHZ) Serial.print("433Mhz");
 if (freq == RF12_868MHZ) Serial.print("868Mhz");
 if (freq == RF12_915MHZ) Serial.print("915Mhz"); 
 Serial.print(" Network: "); 
 Serial.println(network);

}

void loop()
{
  if (emontx++ > 'Z')
    emontx = 'A';
  
 
  int i = 0;

  while (!rf12_canSend() && i<10)
  {
    rf12_recvDone();
    i++;
  }
  
  rf12_sendStart(0, &emontx, sizeof emontx);
    
  Serial.println(emontx); 
  
  digitalWrite(7, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for a second
  digitalWrite(7, LOW);    // turn the LED off by making the voltage LOW
  delay(1000); 
  
  delay(2000);
}

