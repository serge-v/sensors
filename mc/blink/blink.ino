const int led = 3;

void setup() {                
  pinMode(led, OUTPUT);     
}

void dot()
{
  digitalWrite(led, HIGH);
  delay(100);            
  digitalWrite(led, LOW);
  delay(100); 
}

void dash()
{
  digitalWrite(led, HIGH);
  delay(350);            
  digitalWrite(led, LOW);
  delay(100); 
}

void loop() {

  dot();
  dash();
  delay(300); // a
  
  dash();
  delay(300); // t

  dash();
  delay(300); // t

  dot();
  dot();
  delay(300); // i
  
  dash();
  dot();
  delay(300); // n

  dash();
  dot();
  dash();
  dash();
  delay(300); // y

  delay(1000);
}


