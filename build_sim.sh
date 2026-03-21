#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

mkdir -p simulation_results

echo "======================================================="
echo " Experiment 1 - Light Load (IAT=900s, rho~0.27)"
echo "======================================================="
./bin/MULTISERVER_QUEUE 1 | tee simulation_results/experiment1_light_load_output.txt

echo "======================================================="
echo " Experiment 2 - Moderate Load (IAT=300s, rho~0.80)"
echo "======================================================="
./bin/MULTISERVER_QUEUE 2 | tee simulation_results/experiment2_moderate_load_output.txt

echo "======================================================="
echo " Experiment 3 - Heavy Load (IAT=180s, rho~1.33)"
echo "======================================================="
./bin/MULTISERVER_QUEUE 3 | tee simulation_results/experiment3_heavy_load_output.txt

echo ""
echo "All experiments complete."
echo "Results saved to simulation_results/"