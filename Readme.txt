This folder contains the MULTI-SERVER QUEUEING SYSTEM DEVS model implemented in Cadmium v2

/**************************/
/****FILES ORGANIZATION****/
/**************************/

README.txt
multiserver_queue.docx
makefile

atomics [This folder contains atomic models implemented in Cadmium v2]
	arrival_generator.hpp   CustomerArrivalGenerator — generates customers at fixed IAT intervals
	queue.hpp               FifoDispatchQueue — FIFO buffer, dispatches customers to free servers
	server.hpp              Server — IDLE/BUSY service cycle, signals free when done

Coupled [This folder contains Coupled models]
	parallel_server_pool.hpp                Level-2 coupled model containing Server1 and Server2
        multiserver_queueing_system.hpp         Level-1 top coupled model

input_data [This folder contains experiment parameter documentation]
	experiment_parameters.txt  IAT, serviceTime, maxCustomers for each of the 3 experiments

test [This folder contains unit tests for each atomic model]
	main_arrival_generator_test.cpp  Isolated test for CustomerArrivalGenerator
	main_queue_test.cpp              Isolated test for FifoDispatchQueue
	main_server_test.cpp             Isolated test for Server


top_model [This folder contains the top-level model and coupled submodels]
	main_multiserver_queue.cpp       Simulation runner — selects experiment 1, 2, or 3

bin [Created automatically on first compile — contains executables]
simulation_results [Created automatically on first run — contains output logs]


/*************/
/****STEPS****/
/*************/

0 - multiserver_queue.docx contains the full model description, formal specification,
    implementation notes, and experimentation results (Parts I, II, and III combined)

1 - Install Cadmium v2 (if not already installed):
        git clone https://github.com/SimulationEverywhere/cadmium_v2 ~/cadmium_v2

2 - Update the CADMIUM include path in the makefile if needed:
        CADMIUM = $(HOME)/cadmium_v2/include

3 - Compile the project and all tests:
        make clean; make all

4 - Run the atomic model unit tests (incremental order — always test in this sequence):

    Stage 1 — CustomerArrivalGenerator:
        make arrival_generator_test
        Output: simulation_results/arrival_generator_test_output_messages.txt

    Stage 2 — FifoDispatchQueue:
        make queue_test
        Output: simulation_results/queue_test_output_messages.txt

    Stage 3 — Server:
        make server_test
        Output: simulation_results/server_test_output_messages.txt

    Stage 4 — ParallelServerPool (coupled model):
        make parallel_server_pool
        Output: simulation_results/parallel_server_pool_test_output_messages.txt

5 - Run the full system experiments:
    a) Open a terminal in this folder
    b) Run each experiment:
            make experiment1    (Light load:    IAT=900s)
            make experiment2    (Moderate load: IAT=300s)
            make experiment3    (Heavy load:    IAT=180s)
    c) Check output in simulation_results/
            experiment1_light_load_output_messages.txt
            experiment2_moderate_load_output_messages.txt
            experiment3_heavy_load_output_messages.txt

6 - Reading the output logs:
    Each line format:  time ; model_id ; model_name ; port_name ; state_or_value
    Examples:
      900 ; 5 ; CustomerArrivalGenerator ; out_customer ; 0
        -> At t=900s customer ID 0 was generated

      900 ; 3 ; Server1 ; ; {server:1 phase:BUSY customer:0 sigma:480}
        -> Server1 accepted customer 0 and will serve for 480s

      1380 ; 3 ; Server1 ; out_serverFree ; 1
        -> Server1 finished serving and notified the queue it is free

      300 ; 4 ; FifoDispatchQueue ; out_dispatch_s1 ; 0
        -> Queue dispatched customer 0 to Server1

      300 ; 4 ; FifoDispatchQueue ; ; {queue_len:0 dispatch:0 free_servers:1}
        -> Queue state: no waiting customers, 1 server free

7 - Expected results summary:
    Experiment 1 : queue_len always 0, no waiting, servers idle frequently
    Experiment 2 : queue_len 0-1, both servers active, short occasional waits
    Experiment 3 : queue grows continuously, servers always busy, system overloaded