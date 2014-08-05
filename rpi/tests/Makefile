CC=gcc
CFLAGS=-Wall

all: gpio-irq-demo gpio-poll-test spidev-test

gpio-poll-test: gpio-poll-test.o
	$(CC) -o $@ $^

gpio-irq-demo: gpio-irq-demo.o
	$(CC) -o $@ $^

spidev-test: spidev-test.o
	$(CC) -o $@ $^


clean:
	rm *.o gpio-irq-demo gpio-poll-test
