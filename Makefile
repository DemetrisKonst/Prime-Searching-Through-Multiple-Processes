CC=g++
CFLAGS=-g

ALL: ROOT INNER WORKER

ROOT: root_main.o root_handler.o
	$(CC) -o bin/myprime object/root_main.o object/root_handler.o $(CFLAGS)

root_main.o:
	$(CC) -c -o object/root_main.o src/root/main.cpp $(CFLAGS)

root_handler.o:
	$(CC) -c -o object/root_handler.o src/root/handler.cpp $(CFLAGS)

INNER: inner_main.o inner_handler.o
	$(CC) -o bin/inner object/inner_main.o object/inner_handler.o $(CFLAGS)

inner_main.o:
	$(CC) -c -o object/inner_main.o src/inner/main.cpp $(CFLAGS)

inner_handler.o:
	$(CC) -c -o object/inner_handler.o src/inner/handler.cpp $(CFLAGS)

WORKER: worker_main.o worker_handler.o
	$(CC) -o bin/worker object/worker_main.o object/worker_handler.o $(CFLAGS)

worker_main.o:
	$(CC) -c -o object/worker_main.o src/worker/main.cpp $(CFLAGS)

worker_handler.o:
	$(CC) -c -o object/worker_handler.o src/worker/handler.cpp $(CFLAGS)

clean:
	rm bin/* object/*.o
