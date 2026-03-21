#!/usr/bin/env bash
cd "$(dirname "$0")/.."
mkdir -p simulation_results
./bin/ARRIVAL_GENERATOR_TEST | tee simulation_results/arrival_generator_test_output.txt
echo "Done. Results in simulation_results/arrival_generator_test_output.txt"
