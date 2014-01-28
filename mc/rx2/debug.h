void space()
{
	delay(150);
}

void blink_DE()
{
	dash();	dot(); dot(); space();
	dot(); space();
	dot(); dash(); dot(); dot(); space();
}

static void blink_S()
{
	dot(); dot(); dot(); space();
}
/*
static void blink_TO()
{ 
	dash(); space();
	dash(); dash(); dash(); space();
}

static void blink_B()
{
	dash(); space();
	dot(); dot(); dot(); space();
}
*/

static void dump()
{
	Serial.print("PORTB: ");
	Serial.print(PORTB, HEX);
	Serial.print("  ");
	Serial.println(PORTB, BIN);
	Serial.print("DDRB: ");
	Serial.print(DDRB, HEX);
	Serial.print("  ");
	Serial.println(DDRB, BIN);
	Serial.print("PINB: ");
	Serial.print(PINB, HEX);
	Serial.print("  ");
	Serial.println(PINB, BIN);
#ifdef PORTC
	Serial.print("PORTC: ");
	Serial.print(PORTC, HEX);
	Serial.print("  ");
	Serial.println(PORTC, BIN);
	Serial.print("DDRC: ");
	Serial.print(DDRC, HEX);
	Serial.print("  ");
	Serial.println(DDRC, BIN);
	Serial.print("PINC: ");
	Serial.print(PINC, HEX);
	Serial.print("  ");
	Serial.println(PINC, BIN);

	Serial.print("PORTD: ");
	Serial.print(PORTD, HEX);
	Serial.print("  ");
	Serial.println(PORTD, BIN);
	Serial.print("DDRD: ");
	Serial.print(DDRD, HEX);
	Serial.print("  ");
	Serial.println(DDRD, BIN);
	Serial.print("PIND: ");
	Serial.print(PIND, HEX);
	Serial.print("  ");
	Serial.println(PIND, BIN);

	Serial.print("SPCR: ");
	Serial.print(SPCR, HEX);
	Serial.print("  ");
	Serial.println(SPCR, BIN);
	Serial.print("SPSR: ");
	Serial.print(SPSR, HEX);
	Serial.print("  ");
	Serial.println(SPSR, BIN);
	Serial.print("EIMSK: ");
	Serial.print(EIMSK, HEX);
	Serial.print("  ");
	Serial.println(EIMSK, BIN);
	Serial.print("irq: ");
	Serial.println(IRQ_PORT & _BV(IRQ_PIN));
#endif
	Serial.print("buf: ");
	for (int i = 0; i < 10; i++)
	{
		Serial.print(buf[i], HEX);
		Serial.print(' ');
	}

	Serial.print(" idx: ");
	Serial.println(idx);
	Serial.print("chars: ");
	Serial.println(total_chars);
}

static void wipe()
{
	for (int i = 0; i < 10; i++)
		Serial.println();
}
