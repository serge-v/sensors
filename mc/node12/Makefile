MCU = attiny85
F_CPU = 1000000

PORT = /dev/ttyACM0

LIB = ../lib
CINCS = -I../lib

TARGET = node12

SRC = \
    $(TARGET).c \
    $(LIB)/dbg_uart.c \
    $(LIB)/rfm12b.c \
    $(LIB)/timer.c \
    $(LIB)/power.c \
    $(LIB)/adc.c

ASRC = \
    $(LIB)/basic_serial.S

FORMAT = ihex
UPLOAD_RATE = 19200

MAKEFILE = Makefile

#CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums

CFLAGS = -DF_CPU=$(F_CPU) -I../lib -Os -Wall -std=gnu99 -Wstrict-prototypes
LDFLAGS = -Wl,--gc-sections -ffunction-sections

# Program settings
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
REMOVE = rm -f
MV = mv -f

OBJ = $(notdir $(SRC:.c=.o) $(ASRC:.S=.o))
LST = $(ASRC:.S=.lst) $(SRC:.c=.lst)

ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)


all: build
	avr-size $(TARGET).elf
	@echo "\033[32mOK\033[0m"

build: elf hex

upload: $(TARGET).hex $(TARGET).eep
	-kill `pgrep -f "cat $(AVRDUDE_PORT)"`
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(TARGET).hex
	sleep 1
	echo 1 > $(AVRDUDE_PORT)

read_fuses:
	avrdude $(AVRDUDE_FLAGS) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h -U lock:r:-:h

-include $(TARGET).dep
include ../lib/make/elf.mk
include ../lib/make/gcc.mk
include ../lib/make/burn.mk
include ../lib/make/check.mk

.PHONY:	all build elf hex eep lss sym program coff extcoff clean depend

