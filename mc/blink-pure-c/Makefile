all: led.hex


main.o: main.c
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o main.o main.c

led: main.o
	avr-gcc -mmcu=atmega328p main.o -o led

led.hex: led
	avr-objcopy -O ihex -R .eeprom led led.hex

burn:
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:led.hex
