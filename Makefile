CC=g++
# CFLAGS=-O3

ALL: MAIN MODERATOR

MAIN: main.o
	$(CC) -o bin/myprime object/main.o

main.o:
	$(CC) -c -o object/main.o src/main.cpp

MODERATOR: moderator.o
	$(CC) -o bin/moderator object/moderator.o

moderator.o:
	$(CC) -c -o object/moderator.o src/moderator.cpp

clean:
	rm bin/* object/*.o
