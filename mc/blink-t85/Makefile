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

# Program settings
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
REMOVE = rm -f
MV = mv -f

OBJ = $(notdir $(SRC:.c=.o) $(CXXSRC:.cpp=.o) $(ASRC:.S=.o))
LST = $(ASRC:.S=.lst) $(CXXSRC:.cpp=.lst) $(SRC:.c=.lst)

ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CXXFLAGS = -mmcu=$(MCU) -I. $(CXXFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)


all: build
	@echo "\033[32mOK\033[0m"

build: elf hex

include ../lib/make/elf.mk
include ../lib/make/gcc.mk
include ../lib/make/burn.mk
include ../lib/make/check.mk

.PHONY:	all build elf hex eep lss sym program coff extcoff clean depend
