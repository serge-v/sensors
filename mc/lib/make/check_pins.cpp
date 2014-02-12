#include <avr/io.h>
#include <rfm12b.h>
__RFM12B_PINS__
"SELECT_PIN "   SELECT_PIN
"SPI_MOSI   "   SPI_MOSI
"SPI_MISO   "   SPI_MISO
"SPI_SCK    "   SPI_SCK
"F_CPU      "   F_CPU

#if defined(__AVR_ATmega328P__)
"MCU        " ATmega238P
#elif defined(__AVR_ATtiny85__)
"MCU        " ATtiny85
#else
"MCU " not defined
#endif
