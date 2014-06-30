MCU = atmega328p
F_CPU = 16000000
#MCU = attiny85
#F_CPU = 1000000
PORT = /dev/ttyACM0

ARDUINO_LIBS_DIR = /usr/share/arduino/libraries/SoftwareSerial
ARDUINO_LIBS_SRC = /usr/share/arduino/libraries/SoftwareSerial.cpp

ifeq ($(MCU),attiny85)
    ARDUINO = /usr/share/arduino/hardware/attiny/cores/tiny
    CINCS = -I$(ARDUINO) -I$(ARDUINO_LIBS_DIR)

else
    ARDUINO = /usr/share/arduino/hardware/arduino/cores/arduino
    CINCS = -I$(ARDUINO)/../../variants/standard -I$(ARDUINO) -I$(ARDUINO_LIBS_DIR)
endif

TARGET = rx2

SRC = trx.c \
    $(ARDUINO)/wiring.c \
    $(ARDUINO)/wiring_digital.c \
    $(ARDUINO)/WInterrupts.c

CXXSRC = $(TARGET).cpp \
    $(ARDUINO)/main.cpp \
    $(ARDUINO)/HardwareSerial.cpp \
    $(ARDUINO)/Print.cpp \
    $(ARDUINO)/WString.cpp \
    $(ARDUINO_LIBS_SRC)

ifeq ($(MCU),attiny85)
    SRC += $(ARDUINO)/pins_arduino.c
    CXXSRC += $(ARDUINO)/TinyDebugSerial.cpp \
        $(ARDUINO)/TinyDebugSerial115200.cpp \
        $(ARDUINO)/TinyDebugSerial9600.cpp
else
    CXXSRC += $(ARDUINO)/Stream.cpp
endif

FORMAT = ihex
UPLOAD_RATE = 115200

# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs

OPT = s

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU) -DARDUINO=100
CXXDEFS = -DF_CPU=$(F_CPU) -DARDUINO=100

# Place -I options here
CXXINCS = -I$(ARDUINO)

# Compiler flag to set the C Standard level.
CSTANDARD = -std=gnu99 -Wstrict-prototypes
#CDEBUG = -g$(DEBUG)
CWARN = -Wall
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)

CFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA)
CXXFLAGS = $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) -ffunction-sections -fdata-sections
#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs
LDFLAGS = -Wl,--gc-sections


# Programming support using avrdude. Settings and variables.
AVRDUDE_PROGRAMMER = arduino
AVRDUDE_PORT = $(PORT)
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
AVRDUDE_FLAGS = -F -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) -D \
  -b $(UPLOAD_RATE)
AVRDUDE_T85_FLAGS = -F -p t85 -P $(AVRDUDE_PORT) -c avrisp -b 19200

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

build: elf hex eep

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym


#fuses:
#	$(AVRDUDE) -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) \
#	-U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h -U lock:r:-:h

upload: $(TARGET).hex $(TARGET).eep
	-kill `pgrep -f "cat $(AVRDUDE_PORT)"`
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)
	sleep 5
	echo dd > $(AVRDUDE_PORT)

upload85: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_T85_FLAGS) $(AVRDUDE_WRITE_FLASH)


# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000


coff: $(TARGET).elf
	$(COFFCONVERT) -O coff-avr $(TARGET).elf $(TARGET).cof


extcoff: $(TARGET).elf
	$(COFFCONVERT) -O coff-ext-avr $(TARGET).elf $(TARGET).cof


.SUFFIXES: .elf .hex .eep .lss .sym

.elf.hex:
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@



# Link: create ELF output file from object files.
$(TARGET).elf: $(OBJ)
	$(CC) $(ALL_CFLAGS) $(OBJ) --output $@ $(LDFLAGS)


# Compile: create object files from C++ source files.
%.o: $(ARDUINO)/%.cpp
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@

%.o: $(ARDUINO_LIBS_DIR)/%.cpp
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@

# Compile: create object files from C source files.
%.o: $(ARDUINO)/%.c
	$(CC) -c $(ALL_CFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(ALL_CFLAGS) $< -o $@

# Compile: create assembler files from C source files.
.cpp.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@

.c.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
.S.o:
	$(CC) -c $(ALL_ASFLAGS) $< -o $@



# Target: clean project.
clean:
	$(REMOVE) $(TARGET).hex $(TARGET).eep $(TARGET).cof $(TARGET).elf \
	$(TARGET).map $(TARGET).sym $(TARGET).lss \
	$(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d) $(CXXSRC:.cpp=.s) $(CXXSRC:.cpp=.d)

depend:
	if grep '^# DO NOT DELETE' $(MAKEFILE) >/dev/null; \
	then \
		sed -e '/^# DO NOT DELETE/,$$d' $(MAKEFILE) > \
			$(MAKEFILE).$$$$ && \
		$(MV) $(MAKEFILE).$$$$ $(MAKEFILE); \
	fi
	echo '# DO NOT DELETE THIS LINE -- make depend depends on it.' \
		>> $(MAKEFILE); \
	$(CC) -M -mmcu=$(MCU) $(CDEFS) $(CINCS) $(SRC) $(ASRC) >> $(MAKEFILE)

.PHONY:	all build elf hex eep lss sym program coff extcoff clean depend
# DO NOT DELETE THIS LINE -- make depend depends on it.
trx.o: trx.c /usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h \
 /usr/lib/avr/include/stdlib.h /usr/lib/gcc/avr/4.8.2/include/stddef.h \
 /usr/lib/avr/include/string.h /usr/lib/avr/include/math.h \
 /usr/lib/avr/include/avr/pgmspace.h /usr/lib/avr/include/inttypes.h \
 /usr/lib/gcc/avr/4.8.2/include/stdint.h /usr/lib/avr/include/stdint.h \
 /usr/lib/avr/include/avr/io.h /usr/lib/avr/include/avr/sfr_defs.h \
 /usr/lib/avr/include/avr/iom328p.h /usr/lib/avr/include/avr/portpins.h \
 /usr/lib/avr/include/avr/common.h /usr/lib/avr/include/avr/version.h \
 /usr/lib/avr/include/avr/fuse.h /usr/lib/avr/include/avr/lock.h \
 /usr/lib/avr/include/avr/interrupt.h \
 /usr/share/arduino/hardware/arduino/cores/arduino/binary.h \
 /usr/share/arduino/hardware/arduino/cores/arduino/../../variants/standard/pins_arduino.h \
 trx.h
wiring.o: /usr/share/arduino/hardware/arduino/cores/arduino/wiring.c \
 /usr/share/arduino/hardware/arduino/cores/arduino/wiring_private.h \
 /usr/lib/avr/include/avr/io.h /usr/lib/avr/include/avr/sfr_defs.h \
 /usr/lib/avr/include/inttypes.h /usr/lib/gcc/avr/4.8.2/include/stdint.h \
 /usr/lib/avr/include/stdint.h /usr/lib/avr/include/avr/iom328p.h \
 /usr/lib/avr/include/avr/portpins.h /usr/lib/avr/include/avr/common.h \
 /usr/lib/avr/include/avr/version.h /usr/lib/avr/include/avr/fuse.h \
 /usr/lib/avr/include/avr/lock.h /usr/lib/avr/include/avr/interrupt.h \
 /usr/lib/avr/include/stdio.h /usr/lib/gcc/avr/4.8.2/include/stdarg.h \
 /usr/lib/gcc/avr/4.8.2/include/stddef.h \
 /usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h \
 /usr/lib/avr/include/stdlib.h /usr/lib/avr/include/string.h \
 /usr/lib/avr/include/math.h /usr/lib/avr/include/avr/pgmspace.h \
 /usr/share/arduino/hardware/arduino/cores/arduino/binary.h \
 /usr/share/arduino/hardware/arduino/cores/arduino/../../variants/standard/pins_arduino.h
wiring_digital.o: \
 /usr/share/arduino/hardware/arduino/cores/arduino/wiring_digital.c \
 /usr/share/arduino/hardware/arduino/cores/arduino/wiring_private.h \
 /usr/lib/avr/include/avr/io.h /usr/lib/avr/include/avr/sfr_defs.h \
 /usr/lib/avr/include/inttypes.h /usr/lib/gcc/avr/4.8.2/include/stdint.h \
 /usr/lib/avr/include/stdint.h /usr/lib/avr/include/avr/iom328p.h \
 /usr/lib/avr/include/avr/portpins.h /usr/lib/avr/include/avr/common.h \
 /usr/lib/avr/include/avr/version.h /usr/lib/avr/include/avr/fuse.h \
 /usr/lib/avr/include/avr/lock.h /usr/lib/avr/include/avr/interrupt.h \
 /usr/lib/avr/include/stdio.h /usr/lib/gcc/avr/4.8.2/include/stdarg.h \
 /usr/lib/gcc/avr/4.8.2/include/stddef.h \
 /usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h \
 /usr/lib/avr/include/stdlib.h /usr/lib/avr/include/string.h \
 /usr/lib/avr/include/math.h /usr/lib/avr/include/avr/pgmspace.h \
 /usr/share/arduino/hardware/arduino/cores/arduino/binary.h \
 /usr/share/arduino/hardware/arduino/cores/arduino/../../variants/standard/pins_arduino.h
WInterrupts.o: \
 /usr/share/arduino/hardware/arduino/cores/arduino/WInterrupts.c \
 /usr/lib/avr/include/inttypes.h /usr/lib/gcc/avr/4.8.2/include/stdint.h \
 /usr/lib/avr/include/stdint.h /usr/lib/avr/include/avr/io.h \
 /usr/lib/avr/include/avr/sfr_defs.h /usr/lib/avr/include/avr/iom328p.h \
 /usr/lib/avr/include/avr/portpins.h /usr/lib/avr/include/avr/common.h \
 /usr/lib/avr/include/avr/version.h /usr/lib/avr/include/avr/fuse.h \
 /usr/lib/avr/include/avr/lock.h /usr/lib/avr/include/avr/interrupt.h \
 /usr/lib/avr/include/avr/pgmspace.h \
 /usr/lib/gcc/avr/4.8.2/include/stddef.h /usr/lib/avr/include/stdio.h \
 /usr/lib/gcc/avr/4.8.2/include/stdarg.h \
 /usr/share/arduino/hardware/arduino/cores/arduino/wiring_private.h \
 /usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h \
 /usr/lib/avr/include/stdlib.h /usr/lib/avr/include/string.h \
 /usr/lib/avr/include/math.h \
 /usr/share/arduino/hardware/arduino/cores/arduino/binary.h \
 /usr/share/arduino/hardware/arduino/cores/arduino/../../variants/standard/pins_arduino.h
