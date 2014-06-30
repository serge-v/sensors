// receiver with original jeelib

#include <JeeLib.h>

#define myNodeID 30          //node ID of Rx (range 0-30) 
#define network     212      //network group (can be in the range 1-250).
#define freq RF12_433MHZ     //Freq of RF12B can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. Match freq to module

const uint8_t led_pin = 7;


void setup()
{
	pinMode(led_pin, OUTPUT);
	rf12_initialize(myNodeID,freq,network);   //Initialize RFM12 with settings defined above
	Serial.begin(115200);
	Serial.println("rx4 started");
}

static void dot()
{
	digitalWrite(led_pin, HIGH);
	delay(100);
	digitalWrite(led_pin, LOW);
	delay(100);
}

unsigned long last_send = millis();
char s[20];

void loop()
{

	if (rf12_recvDone())
	{
		uint8_t len = rf12_len;
		if (len > 19)
			len = 19;
		memcpy(s, (const char*)rf12_data, len);
		s[len] = 0;
		Serial.print("  ");
		Serial.print(s);
		dot();
	}
	
	if (millis() - last_send > 3000 && rf12_canSend())
	{
		unsigned long time = millis();
		uint8_t n = sprintf(s, "%d,t,%lu\n", led_pin, time);
		rf12_sendStart(0, s, n);
		last_send = millis();
		Serial.print(s);
		dot();
		dot();
	}

	delay(500);
}
