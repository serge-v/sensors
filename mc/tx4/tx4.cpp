#include <Arduino.h>

#define myNodeID 10          //node ID of tx (range 0-30)
#define network     212      //network group (can be in the range 1-250).

const uint8_t led_pin = 9;

void rf12_initialize(uint8_t id, uint8_t g);
void rf12_send(uint8_t len);
void rf12_rx_on(void);
void rf12_rx_off(void);
extern char* rf12_data;

static void dot()
{
	digitalWrite(led_pin, HIGH);
	delay(100);
	digitalWrite(led_pin, LOW);
	delay(100);
}

void setup()
{
	pinMode(led_pin, OUTPUT);
	rf12_initialize(myNodeID, network);
	Serial.begin(115200);
	Serial.println("tx4 started");
	rf12_rx_on();
}

unsigned long last_send = 0;

void loop()
{
/*	if (millis() - last_send > 10000)
	{
		rf12_rx_off();
		unsigned long time = millis();
		uint8_t n = snprintf(rf12_data, 20, "%d,t,%lu\n", led_pin, time);
		rf12_data[n] = 0; // rf12_send will override it with crc
		Serial.print(rf12_data);
		rf12_send(n);
		last_send = millis();
		dot();
		dot();
		rf12_rx_on();
	}
*/	
	delay(500);
}
