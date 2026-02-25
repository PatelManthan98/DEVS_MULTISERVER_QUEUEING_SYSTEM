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
	parallel_server_pool.hpp                Container holding both servers in parallel
        multiserver_queueing_system.hpp         Top model connecting all components

input_data [This folder contains experiment parameter documentation]
	experiment_parameters.txt  IAT, serviceTime, maxCustomers for each of the 3 experiments

test [This folder contains unit tests for each atomic model]
	main_arrival_generator_test.cpp  Isolated test for CustomerArrivalGenerator
	main_queue_test.cpp              Isolated test for FifoDispatchQueue
	main_server_test.cpp             Isolated test for Server

top_model [This folder contains the top-level model and coupled submodels]
	main_multiserver_queue.cpp       Simulation runner — selects experiment 1, 2, or 3
	multiserver_queueing_system.hpp  Level-1 top coupled model
	parallel_server_pool.hpp         Level-2 coupled model containing Server1 and Server2

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

4 - Run the atomic model unit tests:
    a) Open a terminal in this folder
    b) Run each test individually:
            make arrival_generator_test
            make queue_test
            make server_test
    c) Check output in simulation_results/
            arrival_generator_test_output.txt
            queue_test_output.txt
            server_test_output.txt

5 - Run the full system experiments:
    a) Open a terminal in this folder
    b) Run each experiment:
            make run_experiment1    (Light load:    IAT=900s, rho~0.27)
            make run_experiment2    (Moderate load: IAT=300s, rho~0.80)
            make run_experiment3    (Heavy load:    IAT=180s, rho~1.33)
    c) Check output in simulation_results/
            experiment1_light_load_output.txt
            experiment2_moderate_load_output.txt
            experiment3_heavy_load_output.txt

6 - Reading the output logs:
    Each line format:  time ; model_id ; model_name ; port_name ; state_or_value
    Example:
      900 ; 5 ; CustomerArrivalGenerator ; out_customer ; 0
        → At t=900s customer ID 0 was generated

      900 ; 3 ; Server1 ; ; {server:1 phase:BUSY customer:0 sigma:480}
        → Server1 accepted customer 0 and will serve for 480s

      1380 ; 3 ; Server1 ; out_serverFree ; 1
        → Server1 finished serving and notified the queue it is free

7 - Expected results summary:
    Experiment 1 (rho=0.27): queue_len always 0, no waiting, servers idle frequently
    Experiment 2 (rho=0.80): queue_len 0-2, both servers active, short occasional waits
    Experiment 3 (rho=1.33): queue grows continuously, servers always busy, system overloaded