all: node7

node7: node7.o
	gcc $< -o $@  

.c.o:
	gcc -Wall -I../../../rfm12b-linux/ -c $< -o $@    

clean:
	rm *.o node7
