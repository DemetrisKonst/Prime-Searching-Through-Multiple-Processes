CC=g++
CFLAGS=-g

ALL: MAIN MODERATOR WORKER

MAIN: main.o
	$(CC) -o bin/myprime object/main.o $(CFLAGS)

main.o:
	$(CC) -c -o object/main.o src/main.cpp $(CFLAGS)

MODERATOR: moderator.o
	$(CC) -o bin/moderator object/moderator.o $(CFLAGS)

moderator.o:
	$(CC) -c -o object/moderator.o src/moderator.cpp $(CFLAGS)

WORKER: worker.o
	$(CC) -o bin/worker object/worker.o $(CFLAGS)

worker.o:
	$(CC) -c -o object/worker.o src/worker.cpp $(CFLAGS)

clean:
	rm bin/* object/*.o
