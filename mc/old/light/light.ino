#include <SoftwareSerial.h>
#include <DHT22.h>


/*

Light sensor:

5V ---+
      |
     LTR
      |
A0----+
      |
     [1K]
      |
GND---+


*/

const int led = 13;
const int lightSensor = A0;
const int switchSensor = 2;
const int speaker = 8;
const int tempSensor = 7;

DHT22 myDHT22(tempSensor);

void setup() {                
  pinMode(led, OUTPUT);     
  pinMode(switchSensor, INPUT_PULLUP);
  Serial.begin(9600);
}

void dot()
{
  digitalWrite(led, HIGH);
  tone(speaker, 800);
  delay(100);       
  noTone(speaker);  
  digitalWrite(led, LOW);
  delay(100); 
}

void dash()
{
  digitalWrite(led, HIGH);
  tone(speaker, 800);
  delay(300);            
  noTone(speaker);  
  digitalWrite(led, LOW);
  delay(100); 
}

static const byte numbers[] = 
{
  0b11111000,
  0b01111000,
  0b00111000,
  0b00011000,
  0b00001000,
  0b00000000,
  0b10000000,
  0b11000000,
  0b11100000,
  0b11110000,
};

void blink_num(int number)
{
  byte n = 0b00110000; // question sign if out of range

  if (number >= 0 && number <= 9)
    n = numbers[number];

  int mask = 0x80;
  for (int i = 0; i < 5; i++)
  {
    
    if ((n & mask) == 0)
      dot();
    else
      dash();
      
    mask >>= 1;
  }
  delay(300);            
}

void blink_L()
{
  dot();
  dash();
  dot();
  dot();
  delay(300);            
}

void blink_D()
{
  dash();
  dot();
  dot();
  delay(300);            
}

void read_dht_sensor(void)
{ 
  DHT22_ERROR_t errorCode;
  
  // The sensor can only be read from every 1-2s, and requires a minimum
  // 2s warm-up after power-on.
  delay(2000);
  
  errorCode = myDHT22.readData();
  switch(errorCode)
  {
    case DHT_ERROR_NONE:
      Serial.print("temp: ");
      Serial.print(myDHT22.getTemperatureC());
      Serial.print("C, humidity: ");
      Serial.print(myDHT22.getHumidity());
      Serial.print("% ");
      break;
    case DHT_ERROR_CHECKSUM:
      Serial.print("check sum error ");
      Serial.print(myDHT22.getTemperatureC());
      Serial.print("C ");
      Serial.print(myDHT22.getHumidity());
      Serial.println("%");
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
}

void read_sensors() {
  
  read_dht_sensor();

  int light = analogRead(lightSensor);
  int door1 = digitalRead(switchSensor);
  delay(2);
  int door2 = digitalRead(switchSensor);
  int door_opened = 0; 
  if (door1 == HIGH and door2 == HIGH)
    door_opened = 1;
  
  Serial.print(", light: ");         
  Serial.print(light);         
  Serial.print(", door: ");         
  Serial.println(door_opened);         

  delay(1000);
  
  while (Serial.available() > 0)
  {
    int n = Serial.parseInt();
    dot();
    dot();
    dot();
    delay(300);
    blink_num(n);
    continue;
  }

  blink_L();

  int n[4];

  n[0] = light / 1000;
  n[1] = light % 1000 / 100;
  n[2] = light % 100 / 10;
  n[3] = light % 10;
  
  for (int i = 0; i < 4; i++)
  {
    if (n[i] == 0)
    {
      dash();
      delay(300);
    }
    else
      blink_num(n[i]);
  }

  delay(1000);
  blink_D();

  if (door_opened)
    blink_num(1);
  else
    dash();

}

unsigned long time;

void loop() {
  time = millis();
}

