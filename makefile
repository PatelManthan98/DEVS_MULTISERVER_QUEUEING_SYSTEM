CC=g++
CFLAGS=-std=c++17
INCLUDECADMIUM=-I $(CADMIUM)

bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

all: tests multiserver_queue

build/main_arrival_generator_test.o: test/main_arrival_generator_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) test/main_arrival_generator_test.cpp -o build/main_arrival_generator_test.o

build/main_server_test.o: test/main_server_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) test/main_server_test.cpp -o build/main_server_test.o

build/main_queue_test.o: test/main_queue_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) test/main_queue_test.cpp -o build/main_queue_test.o

build/main_parallel_server_pool_test.o: test/main_parallel_server_pool_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) test/main_parallel_server_pool_test.cpp -o build/main_parallel_server_pool_test.o

build/main.o: top_model/main_multiserver_queue.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) top_model/main_multiserver_queue.cpp -o build/main.o

tests: build/main_arrival_generator_test.o build/main_server_test.o build/main_queue_test.o build/main_parallel_server_pool_test.o
	$(CC) -g -o bin/ARRIVAL_GENERATOR_TEST build/main_arrival_generator_test.o
	$(CC) -g -o bin/SERVER_TEST build/main_server_test.o
	$(CC) -g -o bin/QUEUE_TEST build/main_queue_test.o
	$(CC) -g -o bin/PARALLEL_SERVER_POOL_TEST build/main_parallel_server_pool_test.o

multiserver_queue: build/main.o
	$(CC) -g -o bin/MULTISERVER_QUEUE build/main.o

clean:
	rm -rf bin/* build/*