CC=gcc
CFLAGS=-Wall -I/usr/local/include
CFLAGSNOLINK=$(CFLAGS) -c 
LIBS= -lrt -lbcm2835

all: rx1 rx2

rx1: rx1.o
	$(CC) -o $@ $^ $(LIBS) 

rx2: rx2.o trx.o
	$(CC) -o $@ $^ $(LIBS) 

clean:
	rm rx1 rx2 *.o
