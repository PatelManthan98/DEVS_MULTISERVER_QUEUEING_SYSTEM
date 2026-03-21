#!/usr/bin/env bash
cd "$(dirname "$0")/.."
mkdir -p simulation_results
./bin/PARALLEL_SERVER_POOL_TEST | tee simulation_results/parallel_server_pool_test_output.txt
echo "Done. Results in simulation_results/parallel_server_pool_test_output.txt"
