MCU = atmega328p
F_CPU = 16000000

PORT1 = /dev/tty.usbmodem1411
PORT2 = /dev/ttyACM1

LIB = ../lib
CINCS = -I../lib

TARGET = tx4

SRC = tx4.c \
    $(LIB)/rfm12b.c \
    $(LIB)/serial.c \
    $(LIB)/debug.c  \
    $(LIB)/timer.c

CXXSRC =

FORMAT = ihex
UPLOAD_RATE = 115200

DEBUG = stabs
OPT = s

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU)
CXXDEFS = -DF_CPU=$(F_CPU)

# Place -I options here
CXXINCS =

CSTANDARD = -std=gnu99 -Wstrict-prototypes
CWARN = -Wall
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums

CFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA)
CXXFLAGS = $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections


# Programming support using avrdude. Settings and variables.
AVRDUDE_PROGRAMMER = arduino
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
AVRDUDE_FLAGS1 = -F -p $(MCU) -P $(PORT1) -c $(AVRDUDE_PROGRAMMER) -D \
  -b $(UPLOAD_RATE)
AVRDUDE_FLAGS2 = -F -p $(MCU) -P $(PORT2) -c $(AVRDUDE_PROGRAMMER) -D \
  -b $(UPLOAD_RATE)

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
	@echo $(TARGET): $(MCU) $(F_CPU) Hz
	@avr-size $(TARGET).elf
	@echo "\033[32mOK\033[0m"

build: elf hex

# Program the device.
u1: $(TARGET).hex $(TARGET).eep
	-kill `pgrep -f "cat $(PORT1)"`
	$(AVRDUDE) $(AVRDUDE_FLAGS1) $(AVRDUDE_WRITE_FLASH)
	sleep 5
	echo g > $(PORT1)

u2: $(TARGET).hex $(TARGET).eep
	-kill `pgrep -f "cat $(PORT2)"`
	$(AVRDUDE) $(AVRDUDE_FLAGS2) $(AVRDUDE_WRITE_FLASH)
	sleep 5
	echo g > $(PORT2)

include ../lib/make/elf.mk
include ../lib/make/gcc.mk
include ../lib/make/check.mk
-include $(TARGET).dep

.PHONY:	all build elf hex eep lss sym program coff extcoff clean depend
