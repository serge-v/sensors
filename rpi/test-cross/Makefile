ifneq ($(`uname -m`),armv61)
	PATH:=/home/noro/src/x-tools6h/arm-unknown-linux-gnueabihf/bin:${PATH}
endif


CC=gcc
CFLAGS=-Wall

all: test
	which gcc
	readelf -h test | grep -e Machine -e Class

test: test.o
	$(CC) -o $@ $^

clean:
	rm *.o test

