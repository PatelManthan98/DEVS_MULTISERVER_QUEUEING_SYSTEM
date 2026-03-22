This folder contains the MULTI-SERVER QUEUEING SYSTEM DEVS model implemented in Cadmium v2

/**************************/
/****FILES ORGANIZATION****/
/**************************/

README.txt
makefile
build_sim.sh

atomics [This folder contains atomic models]
	ArrivalGenerator.hpp    CustomerArrivalGenerator — generates customers at fixed IAT intervals
	Queue.hpp               FifoDispatchQueue — FIFO buffer, dispatches customers to free servers
	Server.hpp              Server — IDLE/BUSY service cycle, signals free when done

coupled [This folder contains coupled models]
	parallel_server_pool.hpp          Level-2 coupled model containing Server1 and Server2
	multiserver_queueing_system.hpp   Level-1 top coupled model

input_data [This folder contains input files for tests]
	server_input_test.txt              Dispatch events for Server atomic model test
	queue_server_free_input_test.txt   Server-free signals for Queue atomic model test
	pool_server1_dispatch_test.txt     Dispatch events for Server1 in ParallelServerPool test
	pool_server2_dispatch_test.txt     Dispatch events for Server2 in ParallelServerPool test

test [This folder contains unit tests for each atomic and coupled model]
	main_arrival_generator_test.cpp      Isolated test for CustomerArrivalGenerator
	main_queue_test.cpp                  Isolated test for FifoDispatchQueue
	main_server_test.cpp                 Isolated test for Server
	main_parallel_server_pool_test.cpp   Isolated test for ParallelServerPool coupled model

top_model [This folder contains the top-level simulation runner]
	main_multiserver_queue.cpp   Simulation runner — selects experiment 1, 2, or 3

scripts [This folder contains shell scripts to run each test and experiment]
	run_arrival_generator.sh         Run CustomerArrivalGenerator test
	run_server.sh                    Run Server test
	run_queue.sh                     Run Queue test
	run_parallel_server_pool.sh      Run ParallelServerPool coupled model test
	run_multiserver_queueing_system.sh  Run all 3 experiments

bin [Created automatically on first compile — contains executables]
simulation_results [Created automatically on first run — contains output logs]


/*************/
/****STEPS****/
/*************/

0 - The docx file contains the full model description, formal specification,
    implementation notes, and experimentation results (Parts I, II, and III combined)

1 - Set the CADMIUM environment variable to your Cadmium v2 include path:
        export CADMIUM=/home/cadmium/rt_cadmium/include

    To make this permanent add it to your ~/.bashrc:
        echo 'export CADMIUM=/home/cadmium/rt_cadmium/include' >> ~/.bashrc
        source ~/.bashrc

2 - Compile the project:
        make clean
        make
        bash build_sim.sh

3 - Run the atomic model unit tests (incremental order — always test in this sequence):

    Stage 1 — CustomerArrivalGenerator:
        bash scripts/run_arrival_generator.sh
        Output: simulation_results/arrival_generator_test_output.txt

    Stage 2 — Server:
        bash scripts/run_server.sh
        Output: simulation_results/server_test_output.txt

    Stage 3 — FifoDispatchQueue:
        bash scripts/run_queue.sh
        Output: simulation_results/queue_test_output.txt

    Stage 4 — ParallelServerPool (coupled model):
        bash scripts/run_parallel_server_pool.sh
        Output: simulation_results/parallel_server_pool_test_output.txt

4 - Run the full system experiments:
        bash scripts/run_multiserver_queueing_system.sh

    Or run each experiment individually:
        ./bin/MULTISERVER_QUEUE 1    (Light load:    IAT=900s, rho~0.27)
        ./bin/MULTISERVER_QUEUE 2    (Moderate load: IAT=300s, rho~0.80)
        ./bin/MULTISERVER_QUEUE 3    (Heavy load:    IAT=180s, rho~1.33)

    Output files in simulation_results/:
        experiment1_light_load_output.txt
        experiment2_moderate_load_output.txt
        experiment3_heavy_load_output.txt
