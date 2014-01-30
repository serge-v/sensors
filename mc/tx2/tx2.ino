//Simple RFM12B wireless demo - transimtter - no ack
//Glyn Hudson openenergymonitor.org GNU GPL V3 7/7/11
//Credit to JCW from Jeelabs.org for RFM12 

#include <JeeLib.h>  //from jeelabs.org

#define myNodeID 10          //node ID of tx (range 0-30)
#define network     212      //network group (can be in the range 1-250).
#define freq RF12_433MHZ     //Freq of RF12B can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. Match freq to module


char payload = 'A';
unsigned long last_send = 0;
int bad_count = 0;

void setup() {

  pinMode(7, OUTPUT);
  rf12_initialize(myNodeID, freq, network);   //Initialize RFM12 with settings defined above  
  Serial.begin(115200);
  Serial.read();
  Serial.println("setup");
}

void dot()
{
  digitalWrite(9, HIGH);
  delay(100);
  digitalWrite(9, LOW);
  delay(100);
}

void loop()
{
  if (rf12_recvDone())
  {
    payload=*(char*)rf12_data;
    Serial.print("--rx: ");
    Serial.println(payload); 
    if (payload++ > 'Z' || payload < 'A')
      payload = 'A';
    dot();
    dot();
  }

  if (millis() - last_send > 2000)
  {
    if (rf12_canSend())
    {
      rf12_sendStart(0, &payload, sizeof(payload));
      Serial.print("tx: "); 
      Serial.println(payload); 
      dot();
      last_send = millis();
      if (payload++ > 'Z' || payload < 'A')
        payload = 'A';
      bad_count = 0;
    }
    else
    {
      bad_count++;
    }
  }

 if (bad_count > 100)
 {
   setup();
   bad_count = 0;
 } 

}


