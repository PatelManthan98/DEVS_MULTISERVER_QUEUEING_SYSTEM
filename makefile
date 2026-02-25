CADMIUM = $(HOME)/cadmium_v2/include
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -I$(CADMIUM) -I.

BIN = bin
SIM = simulation_results

.PHONY: all clean arrival_generator_test queue_test server_test experiment1 experiment2 experiment3 

all: $(BIN)/ARRIVAL_GENERATOR_TEST $(BIN)/QUEUE_TEST $(BIN)/SERVER_TEST $(BIN)/MULTISERVER_QUEUE

$(BIN):
	mkdir -p $(BIN)

$(SIM):
	mkdir -p $(SIM)

$(BIN)/ARRIVAL_GENERATOR_TEST: test/main_arrival_generator_test.cpp | $(BIN) $(SIM)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(BIN)/QUEUE_TEST: test/main_queue_test.cpp | $(BIN) $(SIM)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(BIN)/SERVER_TEST: test/main_server_test.cpp | $(BIN) $(SIM)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(BIN)/MULTISERVER_QUEUE: top_model/main_multiserver_queue.cpp | $(BIN) $(SIM)
	$(CXX) $(CXXFLAGS) -o $@ $<

arrival_generator_test: $(BIN)/ARRIVAL_GENERATOR_TEST
	cd bin && ./ARRIVAL_GENERATOR_TEST 2>&1 | tee ../$(SIM)/arrival_generator_test_output_messages.txt

queue_test: $(BIN)/QUEUE_TEST
	cd bin && ./QUEUE_TEST 2>&1 | tee ../$(SIM)/queue_test_output_messages.txt

server_test: $(BIN)/SERVER_TEST
	cd bin && ./SERVER_TEST 2>&1 | tee ../$(SIM)/server_test_output_messages.txt

experiment1: $(BIN)/MULTISERVER_QUEUE
	cd bin && ./MULTISERVER_QUEUE 1 2>&1 | tee ../$(SIM)/experiment1_light_load_output_messages.txt

experiment2: $(BIN)/MULTISERVER_QUEUE
	cd bin && ./MULTISERVER_QUEUE 2 2>&1 | tee ../$(SIM)/experiment2_moderate_load_output_messages.txt

experiment3: $(BIN)/MULTISERVER_QUEUE
	cd bin && ./MULTISERVER_QUEUE 3 2>&1 | tee ../$(SIM)/experiment3_heavy_load_output_messages.txt

clean:
	rm -rf $(BIN)