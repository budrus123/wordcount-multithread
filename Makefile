CC=gcc
CFLAGS=-g -lm

main : main.o helpers.o
	$(CC) -o main helpers.o main.o

.PHONY : clean

clean : 
	rm *.o $(objects) main

