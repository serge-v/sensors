#include <JeeLib.h>
#include <RF12sio.h>

RF12 RF12;

void setup() {
    Serial.begin(9600);
    Serial.print("\n[rf12serial]");
    rf12_initialize(9, RF12_433MHZ, 5);
}

void loop() {
    if (Serial.available())
        RF12.send(Serial.read());

    if (RF12.poll())
        Serial.print(RF12.read());
}

