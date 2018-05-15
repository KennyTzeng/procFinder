CC = gcc
CFLAGS = -g -Wall

.c.o:
	$(CC) -c $< $(CFLAGS)

link: procFinder.o ps.o
	$(CC) -o procFinder $^
