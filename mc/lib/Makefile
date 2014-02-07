MCU = attiny85
F_CPU = 16000000

ARDUINO = /usr/share/arduino/hardware/arduino/cores/arduino
CINCS = -I$(ARDUINO)/../../variants/standard -I$(ARDUINO)

MAKEFILE = Makefile

OPT = s

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU) -DARDUINO=100
CXXDEFS = -DF_CPU=$(F_CPU) -DARDUINO=100

# Place -I options here
CXXINCS = -I$(ARDUINO)

CSTANDARD = -std=gnu99 -Wstrict-prototypes
CWARN = -Wall
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums

CFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA)
CXXFLAGS = $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections


# Program settings
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
REMOVE = rm -f
MV = mv -f

ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CXXFLAGS = -mmcu=$(MCU) -I. $(CXXFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)


# Default target.
all: uart_test.hex rfm12b_calc

uart_test.elf: uart_test.o basic_serial.o
	$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)

.SUFFIXES: .elf .hex .eep .lss .sym

.elf.hex:
	$(OBJCOPY) -O ihex -R .eeprom $< $@

rfm12b_calc:
	gcc rfm12b_calc.c -o rfm12b_calc

include ../lib/make/gcc.mk
include ../lib/make/check.mk
