#include <Arduino.h>
#include <rfm12b.h>

#define myNodeID 10          //node ID of tx (range 0-30)
#define network     212      //network group (can be in the range 1-250).

const uint8_t led_pin1 = 7;
const uint8_t led_pin2 = 9;

static void dot()
{
	digitalWrite(led_pin1, HIGH);
	digitalWrite(led_pin2, HIGH);
	delay(100);
	digitalWrite(led_pin1, LOW);
	digitalWrite(led_pin2, LOW);
	delay(100);
}

void setup()
{
	pinMode(led_pin1, OUTPUT);
	pinMode(led_pin2, OUTPUT);
	rf12_initialize(myNodeID, network);
	Serial.begin(115200);
	Serial.println("tx4 started");
	rf12_rx_on();
}

unsigned long last_send = 0;
unsigned long interval = 10000;

struct settings
{
	uint8_t auto_start: 1;
	uint8_t tx_enabled: 1;
	uint8_t rx_enabled: 1;
	uint8_t debug: 1;
	uint8_t master: 1;
};

struct settings sts = {
	.auto_start = 1,
	.tx_enabled = 0,
	.rx_enabled = 0,
	.debug = 0
};

static void handle_serial()
{
	sts.auto_start = 0;

	byte c = Serial.read();
	switch (c)
	{
	case 'i': // init
		interval = 10000;
		sts.rx_enabled = 0;
		sts.tx_enabled = 0;
		sts.auto_start = 1;
		last_send = millis();
		Serial.println("init");
		break;
	case 'g':
		sts.debug = !sts.debug;
		rf12_debug(sts.debug);
		Serial.print("debug: ");
		Serial.println(sts.debug);
		break;
	case '1':
		interval -= 1000;
		Serial.print("interval: ");
		Serial.println(interval);
		break;
	case '2':
		interval += 1000;
		Serial.print("interval: ");
		Serial.println(interval);
		break;
	case 't':
		sts.tx_enabled = !sts.tx_enabled;
		Serial.print("tx: ");
		Serial.println(sts.tx_enabled);
		break;
	case 'r':
		sts.rx_enabled = !sts.rx_enabled;
		Serial.print("rx: ");
		Serial.println(sts.rx_enabled);
		break;
	}
}

void loop()
{
	if (Serial.available())
		handle_serial();

	if (sts.tx_enabled && !receiving && millis() - last_send > interval)
	{
		if (sts.rx_enabled)
			rf12_rx_off();

		unsigned long time = millis();
		uint8_t n = snprintf((char*)rf12_data, 20, "%d,t,%lu\n", led_pin1, time);
		rf12_data[n] = 0; // rf12_send will override it with crc
		Serial.print((char*)rf12_data);
		rf12_send(n);
		last_send = millis();
		dot();
		dot();

		if (sts.rx_enabled)
			rf12_rx_on();
	}


	if (sts.rx_enabled && rcv_done)
	{
		print_buf();
		dot();
		rf12_rx_on();
	}

	if (sts.auto_start && !sts.rx_enabled && millis() - last_send > 30000)
	{
		sts.rx_enabled = 1;
		Serial.println("rx auto enabled");
	}
	else if (sts.auto_start && !sts.tx_enabled && millis() - last_send > 35000)
	{
		sts.auto_start = 0;
		sts.tx_enabled = 1;
		Serial.println("tx auto enabled");
	}
}
