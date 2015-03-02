#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <dbg_uart.h>
#include <stdio.h>

uint16_t sample_vcc(void)
{
	ADMUX = _BV(MUX3) | _BV(MUX2); // ADC6
	ADCSRA = _BV(ADIF) | _BV(ADIE) | /* clear interrupt flag, enable interrupt */
		_BV(ADPS1) | _BV(ADPS0); /* prescaler /8 = 125kHz */
	ADCSRB = 0;

	uint16_t res;

	power_adc_enable();
	set_sleep_mode(SLEEP_MODE_ADC);
	ADCSRA |= _BV(ADEN) | _BV(ADSC);
	while(ADCSRA & _BV(ADSC))
		sleep_mode();
	res = ADC;
	ADCSRA &= ~(_BV(ADEN));
	power_adc_disable();
	return res;
}

uint16_t sample_temperature(void)
{
	ADMUX = _BV(REFS1) | _BV(MUX5) | _BV(MUX1);   // 1.1V ref on ADC8
	ADCSRA = _BV(ADIF) | _BV(ADIE) | /* clear interrupt flag, enable interrupt */
		_BV(ADPS1) | _BV(ADPS0); /* prescaler /8 = 125kHz */
	ADCSRB = 0;

	uint16_t res;

	power_adc_enable();
	set_sleep_mode(SLEEP_MODE_ADC);
	ADCSRA |= _BV(ADEN) | _BV(ADSC);
	while(ADCSRA & _BV(ADSC))
		sleep_mode();
	res = ADC;
	ADCSRA &= ~(_BV(ADEN));
	power_adc_disable();
	return res;
}

int main (void)
{
	wdt_disable();

	// debug uart
	dbg_uart_init();
	printf("\r\r\r\r\n\n\n\n\nblink\n");

	// led
	DDRB |= _BV(DDB2); // output

	uint8_t cnt = 0;
	uint8_t rows = 0;

	while(1)
	{
		PORTB |= _BV(PORTB2); // high
		_delay_ms(100);

		PORTB &= ~_BV(PORTB2); // low
		_delay_ms(1000);
		uint16_t v = sample_vcc();
		_delay_ms(1000);
		uint16_t t = sample_temperature();
		printf("v:%u, t:%d\n", v, t+17);
		cnt++;
		if (cnt % 20 == 0)
		{
			printf("=========== %2d\n", ++rows);
			cnt = 0;
		}
	}

	return 0;
}

ISR(ADC_vect) { /* empty, but must exist for ADC conversion */
}

ISR(WDT_vect) {
}
