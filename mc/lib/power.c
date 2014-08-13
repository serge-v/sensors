#include <avr/wdt.h>
#include <avr/sleep.h>
#include <power.h>

void system_sleep()
{
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
	sleep_enable();
	sleep_mode();                        // System sleeps here
	sleep_disable();                     // System continues execution here when watchdog timed out 
}

void setup_watchdog(int mode)
{
	uint8_t tout;
	if (mode > 9)
		mode = 9;
	tout = mode & 7;
	if (mode > 7)
		tout |= _BV(WDP3);

	tout |= _BV(WDCE);

	MCUSR &= ~_BV(WDRF);

#if defined(WDTCR)
	WDTCR |= _BV(WDCE) | _BV(WDE);
	WDTCR = tout;
	WDTCR |= _BV(WDIE);
#elif defined (WDTSCR)
	WDTCR |= _BV(WDCE) | _BV(WDE);
	WDTCR = tout;
	WDTCR |= _BV(WDIE);
#endif
}
