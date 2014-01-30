#include <RFM12B.h>
#include <avr/sleep.h>

#define NODEID        2  //network ID used for this unit
#define NETWORKID    99  //the network ID we are on
#define GATEWAYID     1  //the node ID we're sending to
#define ACK_TIME     50  // # of ms to wait for an ack
#define SERIAL_BAUD  115200
uint8_t KEY[] = "ABCDABCDABCDABCD";

int interPacketDelay = 1000; //wait this many ms between sending packets
char input = 0;

RFM12B radio;
byte sendSize = 0;
char payload[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890~!@#$%^&*(){}[]`|<>?+=:;,.";
bool requestACK=false;

static bool waitForAck()
{
	long now = millis();
	while (millis() - now <= ACK_TIME)
		if (radio.ACKReceived(GATEWAYID))
			return true;
	return false;
}

void setup()
{
	Serial.begin(SERIAL_BAUD);
	radio.Initialize(NODEID, RF12_433MHZ, NETWORKID);
	radio.Encrypt(KEY);
	radio.Sleep(); //sleep right away to save power
	Serial.println("Transmitting...\n\n");
}

void loop()
{
	//serial input of [0-9] will change the transmit delay between 100-1000ms
	if (Serial.available() > 0)
	{
		input = Serial.read();
		if (input >= 48 && input <= 57) //[1..9] = {100..900}ms; [0]=1000ms
		{
			interPacketDelay = 100 * (input-48);
			if (interPacketDelay == 0) interPacketDelay = 1000;
			Serial.print("\nChanging delay to ");
			Serial.print(interPacketDelay);
			Serial.println("ms\n");
		}
	}

	Serial.print("Sending[");
	Serial.print(sendSize+1);
	Serial.print("]:");
	for(byte i = 0; i < sendSize+1; i++)
		Serial.print((char)payload[i]);

	requestACK = !(sendSize % 3); //request ACK every 3rd xmission

	radio.Wakeup();
	radio.Send(GATEWAYID, payload, sendSize+1, requestACK);
	if (requestACK)
	{
		Serial.print(" - waiting for ACK...");
		if (waitForAck()) Serial.print("ok!");
		else Serial.print("nothing...");
	}
	radio.Sleep();

	sendSize = (sendSize + 1) % 88;
	Serial.println();
	delay(interPacketDelay);
}
