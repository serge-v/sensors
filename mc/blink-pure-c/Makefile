MCU = attiny84
F_CPU = 1000000

all: led.hex

main.o: main.c
	avr-gcc -I../lib -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -c -o main.o main.c

dbg_uart.o: ../lib/dbg_uart.c
	avr-gcc -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -c -o $@ $<

basic_serial.o: ../lib/basic_serial.S
	avr-gcc -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -c $< -o $@


led.elf: main.o dbg_uart.o basic_serial.o
	avr-gcc -mmcu=$(MCU) $^ -o led.elf

led.hex: led.elf
	avr-objcopy -O ihex -R .eeprom led.elf led.hex

burn: led.hex
	avrdude -c stk500v1 -p $(MCU) -P /dev/ttyACM1 -b 19200 -U flash:w:led.hex

burn_bootloader:
	avrdude -c stk500v1 -p $(MCU) -P /dev/ttyACM1 -b 19200 -U flash:w:tsb_tn84_b1b0_20131023.hex

clean:
	rm *.o led.hex led.elf
