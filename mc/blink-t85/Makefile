MCU = attiny85
F_CPU = 1000000

PORT = /dev/ttyACM0

ARDUINO = /usr/share/arduino/hardware/attiny/cores/tiny
LIB = ../lib
CINCS = -I$(ARDUINO) -I../lib

TARGET = blink

SRC = \
    $(ARDUINO)/pins_arduino.c \
    $(ARDUINO)/wiring.c \
    $(ARDUINO)/wiring_digital.c \
    $(ARDUINO)/WInterrupts.c

CXXSRC = $(TARGET).cpp \
    $(LIB)/debug.cpp \
    $(LIB)/rfm12b.cpp \
    $(ARDUINO)/main.cpp \
    $(ARDUINO)/Print.cpp \
    $(ARDUINO)/TinyDebugSerial.cpp \
    $(ARDUINO)/TinyDebugSerial9600.cpp

#    $(ARDUINO)/WString.cpp \
#    $(ARDUINO)/HardwareSerial.cpp \

FORMAT = ihex
UPLOAD_RATE = 19200

MAKEFILE = Makefile

DEBUG = stabs
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
LDFLAGS = -Wl,--gc-sections -ffunction-sections


# Programming support using avrdude. Settings and variables.
AVRDUDE_PROGRAMMER = stk500v1
AVRDUDE_PORT = $(PORT)
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
AVRDUDE_FLAGS = \
  -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) \
  -b $(UPLOAD_RATE)

#-C/usr/share/arduino/hardware/tools/avrdude.conf
#-pattiny85 -cstk500v1 -P/dev/ttyACM0 -b19200 -Uflash:w:1.hex:i


# Program settings
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
MV = mv -f

# Define all object files.
OBJ = $(notdir $(SRC:.c=.o) $(CXXSRC:.cpp=.o) $(ASRC:.S=.o))

# Define all listing files.
LST = $(ASRC:.S=.lst) $(CXXSRC:.cpp=.lst) $(SRC:.c=.lst)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CXXFLAGS = -mmcu=$(MCU) -I. $(CXXFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)


# Default target.
all: build
	@echo "\033[32mOK\033[0m"

build: elf hex

# Program the device.
upload: $(TARGET).hex
	-kill `pgrep -f "cat $(PORT)"`
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)


include ../lib/make/elf.mk
include ../lib/make/gcc.mk
include ../lib/make/check.mk

.PHONY:	all build elf hex eep lss sym program coff extcoff clean depend
