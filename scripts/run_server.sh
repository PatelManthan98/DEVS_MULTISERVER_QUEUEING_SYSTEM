#!/usr/bin/env bash
cd "$(dirname "$0")/.."
mkdir -p simulation_results
./bin/SERVER_TEST | tee simulation_results/server_test_output.txt
echo "Done. Results in simulation_results/server_test_output.txt"
