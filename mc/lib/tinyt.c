/*
Adopted from:
Andrey Mikhalchuk's Blog: Jun 20, 2011 Reading ATtiny85/45/25 Internal Temperature Sensor
http://andrey.mikhalchuk.com/2011/06/20/reading-attiny854525-internal-temperature-sensor.html
Ref: ATTiny85 datasheet p140 (17.13.2), p137 (17.12)
*/

#include <tinyt.h>
#include <avr/io.h>

#define TEMPERATURE_SAMPLES 30
#define TEMPERATURE_ADJUSTMENT -13
#define EXTREMES_RATIO 5

int readings[TEMPERATURE_SAMPLES];
const int offset = 0;

int
tinyt_read_raw(void)
{
	if (ADCSRA & _BV(ADSC))
		return -1;

	int ret = ADCL | (ADCH << 8);   // Get the previous conversion result
	ADCSRA |= _BV(ADSC); // Start new conversion
	return ret;
}

void
tinyt_init(void)
{
	ADMUX = 0xF;                           // Select temperature sensor (ADC4)
	ADMUX &= ~_BV(ADLAR);                  // Right-adjust result
	ADMUX |= _BV(REFS1);                   // Set Ref voltage
	ADMUX &= ~(_BV(REFS0) | _BV(REFS2));   // to 1.1V

	ADCSRA &= ~(_BV(ADATE) | _BV(ADIE));   // Disable autotrigger, Disable Interrupt
	ADCSRA |= _BV(ADEN);                   // Enable ADC
	ADCSRA |= _BV(ADSC);                   // Start first conversion

	// Seed samples
	int raw_temp;
	while ((raw_temp = tinyt_read_raw()) < 0);
	for (int i = 0; i < TEMPERATURE_SAMPLES; i++)
		readings[i] = raw_temp;
}

static int pos = 0;

static int
tinyt_read_lsb(void)
{
	int readings_dup[TEMPERATURE_SAMPLES];
	int raw_temp;
	// remember the sample
	if ((raw_temp = tinyt_read_raw()) > 0)
	{
		readings[pos] = raw_temp;
		pos++;
		pos %= TEMPERATURE_SAMPLES;
	}

	// copy the samples
	for (int i = 0; i < TEMPERATURE_SAMPLES; i++)
		readings_dup[i] = readings[i];

	// bubble extremes to the ends of the array
	int extremes_count = TEMPERATURE_SAMPLES / EXTREMES_RATIO;
	int swap;
	for (int i = 0; i < extremes_count; ++i)
	{
		// percent of iterations of bubble sort on small N works faster than Q-sort
		for (int j = 0; j < TEMPERATURE_SAMPLES - 1; j++)
		{
			if (readings_dup[i] > readings_dup[i + 1])
			{
				// could be done with 3 XORs and no swap if you like fancy
				swap = readings_dup[i];
				readings_dup[i] = readings_dup[i + 1];
				readings_dup[i + 1] = swap;
			}
		}
	}

	// average the middle of the array
	int sum_temp = 0;
	for (int i = extremes_count; i < TEMPERATURE_SAMPLES - extremes_count; i++)
		sum_temp += readings_dup[i];
	return sum_temp / (TEMPERATURE_SAMPLES - extremes_count * 2);
}

int
tinyt_read_k(void)
{
	// for simplicty we use k=1, use the next line if you want K!=1.0
	return tinyt_read_lsb() + TEMPERATURE_ADJUSTMENT;
	//return (int)( tinyt_read_lsb() * coefficient ) + offset;
}

int
tinyt_read_c(void)
{
	return tinyt_read_k() - 273;
}

int
tinyt_read_f(void)
{
	return tinyt_read_c() * 9 / 5 + 32;
}
