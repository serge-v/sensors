#include <DHT22.h>
#include <SoftwareSerial.h>

unsigned long time;
unsigned long count = 0;
unsigned long light_read_ts;
int refresh_period;

const int door_pin = 2;
const int dht_pin = 7;
const int speaker_pin = 8;
const int ltr_pin = A0;

DHT22 dht(dht_pin);
SoftwareSerial com3(12, 13); // RX, TX

void setup()
{
  pinMode(door_pin, INPUT_PULLUP);
  Serial.begin(9600);
  time = millis();
  Serial.print(time);
  com3.begin(9600);
  com3.println("com3 started");
  Serial.println(" start");
  refresh_period = 10000;
  light_read_ts = time + refresh_period;
}

void loop()
{
  if (Serial.available() > 0)
  {
    byte c = Serial.read();
    com3.print(c);
    if (c == 'r')
    {
      refresh_period += 500;
      if (refresh_period > 60000)
        refresh_period = 60000;
      Serial.print("refresh_period: ");
      Serial.println(refresh_period);
    }
    else if (c == 'e')
    {
      refresh_period -= 500;
      if (refresh_period < 0)
        refresh_period = 0;
      Serial.print("refresh_period: ");
      Serial.println(refresh_period);
    }
    else if (c == 's')
    {
      read_sensors();
    }
    else if (c == 'b')
    {
      int freq = Serial.parseInt();
      int dur = Serial.parseInt();
      tone(speaker_pin, freq, dur);
    }
    else
    {
      Serial.println("ERROR. unknown command");
    }

    while (Serial.available() > 0) {
      Serial.read();
    }
    light_read_ts = time + refresh_period;
  }
  
  time = millis();
  if (time >= light_read_ts)
  {
    read_sensors();
    light_read_ts = time + refresh_period;
  }
}

void read_sensors()
{
    byte light = analogRead(ltr_pin) / 4;
    byte door = digitalRead(door_pin);
    Serial.print(time);
    Serial.print(" L:");
    Serial.print(light);
    Serial.print(" D:");
    Serial.print(door);
    
    float temp = 0, hum = 0;
    
    if (read_dht_sensor(temp, hum) == 0)
    {
      Serial.print(" T:");
      Serial.print(temp);
      Serial.print(" H:");
      Serial.print(hum);
    }

    Serial.println("");
}

int read_dht_sensor(float& temp, float& humidity)
{ 
  DHT22_ERROR_t rc;
  
  // The sensor can only be read from every 1-2s, and requires a minimum
  // 2s warm-up after power-on.
  delay(2000);
  
  rc = dht.readData();
  if (rc == DHT_ERROR_NONE)
  {
      temp = dht.getTemperatureC();
      humidity = dht.getHumidity();
      return 0;
  }

  switch (rc)
  {
    case DHT_ERROR_CHECKSUM:
      Serial.println("Check sum error ");
      break;
    case DHT_BUS_HUNG:
      Serial.println("BUS Hung ");
      break;
    case DHT_ERROR_NOT_PRESENT:
      Serial.println("Not Present ");
      break;
    case DHT_ERROR_ACK_TOO_LONG:
      Serial.println("ACK time out ");
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      Serial.println("Sync Timeout ");
      break;
    case DHT_ERROR_DATA_TIMEOUT:
      Serial.println("Data Timeout ");
      break;
    case DHT_ERROR_TOOQUICK:
      Serial.println("Polled too quick ");
      break;
  }
  
  return -1;
}

