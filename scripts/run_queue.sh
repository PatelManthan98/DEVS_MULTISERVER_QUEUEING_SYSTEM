#!/usr/bin/env bash
cd "$(dirname "$0")/.."
mkdir -p simulation_results
./bin/QUEUE_TEST | tee simulation_results/queue_test_output.txt
echo "Done. Results in simulation_results/queue_test_output.txt"
