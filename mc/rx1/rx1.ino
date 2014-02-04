//Simple RFM12B wireless demo - Receiver - no ack
//Glyn Hudson openenergymonitor.org GNU GPL V3 12/4/12
//Credit to JCW from Jeelabs.org for RFM12 

#include <JeeLib.h>

#define myNodeID 30          //node ID of Rx (range 0-30) 
#define network     212      //network group (can be in the range 1-250).
#define freq RF12_433MHZ     //Freq of RF12B can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. Match freq to module

char emontx;  

const int emonTx_NodeID=10;            //emonTx node ID

void setup() {
  pinMode(9, OUTPUT);
  rf12_initialize(myNodeID,freq,network);   //Initialize RFM12 with settings defined above  
  Serial.begin(9600); 
  Serial.println("RF12B demo Receiver - Simple demo"); 
  
 Serial.print("Node: "); 
 Serial.print(myNodeID); 
 Serial.print(" Freq: "); 
 if (freq == RF12_433MHZ) Serial.print("433Mhz");
 if (freq == RF12_868MHZ) Serial.print("868Mhz");
 if (freq == RF12_915MHZ) Serial.print("915Mhz"); 
 Serial.print(" Network: "); 
 Serial.println(network);
}

void loop() {
  
 if (rf12_recvDone()){    
       emontx=*(char*) rf12_data;            // Extract the data from the payload 
       Serial.print(emontx); 
       Serial.print(' '); 
 }

  digitalWrite(9, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for a second
  digitalWrite(9, LOW);    // turn the LED off by making the voltage LOW
  delay(500); 

}

