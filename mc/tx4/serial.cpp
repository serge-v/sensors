#include <Arduino.h>

extern "C" {

void serial_putchar(char c, FILE *stream)
{
	Serial.print(c);
}

char serial_getchar(FILE *stream)
{
	Serial.read();
}

void serial_init()
{
	Serial.begin(115200);
}

uint8_t serial_available(void)
{
	return Serial.available();
}

}
