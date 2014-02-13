#if !defined(CLK_DIVIDER)
#	if F_CPU == 16000000
#		define CLK_DIVIDER 1024
#	elif F_CPU == 1000000
#		define CLK_DIVIDER 8
#	endif
#endif

#define OVERFLOW_MS (1000.0 / ((float)F_CPU / (float)CLK_DIVIDER) * 256)
#define OVERFLOW_NS ((uint16_t)(1000000.0 / ((float)F_CPU / (float)CLK_DIVIDER) * 256) % 1000)

void timer0_start(void);
unsigned long int timer0_ms(void);
