#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

static unsigned long int time_ms = 0;
static unsigned int _ns = 0;

ISR(TIMER0_OVF_vect)
{
	time_ms += OVERFLOW_MS;
	_ns += OVERFLOW_NS;
	if (_ns > 1000)
	{
		time_ms++;
		_ns -= 1000;
	}
}

unsigned long int
timer0_ms()
{
	uint64_t m;
	cli();
	m = time_ms;
	sei();
	return m;
}

#if CLK_DIVIDER == 1024
#	define TCCVALUE (_BV(CS00) | _BV(CS02))
#elif CLK_DIVIDER == 8
#	define TCCVALUE (_BV(CS01))
#else
#	error "CLK_DIVIDER is not defined or is not supported"
#endif

void
timer0_start()
{
	TCCR0B = TCCVALUE;
#if defined(TIMSK0)
	TIMSK0 = _BV(TOIE0); // enable timer overflow interrupt
#elif defined(TIMSK)
	TIMSK = _BV(TOIE0);  // enable timer overflow interrupt
#else
#	error "Cannot find suitable timer"
#endif
	sei();
}
