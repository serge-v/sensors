CC=gcc
CFLAGS=-Wall
CFLAGSNOLINK=$(CFLAGS) -c 
LIBS= -lrt -lbcm2835
INC= -I/usr/local/include -I.

all: rx1

rx1: rx1.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) 

rx1.o: rx1.c
	$(CC) $(CFLAGSNOLINK) $(LIBS) -Wall $<


