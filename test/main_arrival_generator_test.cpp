#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <cadmium/modeling/devs/coupled.hpp>
#include "../atomics/ArrivalGenerator.hpp"
#include <iostream>
#include <fstream>
#include <memory>
#include <limits>

using namespace cadmium::example::queue;
using namespace cadmium;

// ═══════════════════════════════════════════════
//  CONFIG — change these to test different inputs
// ═══════════════════════════════════════════════
const double inter_arrival_time = 150.0;  // seconds between customers
const int    max_customers      = 5;      // total customers to generate
// ═══════════════════════════════════════════════

class ArrivalGeneratorTestTop : public Coupled {
public:
    ArrivalGeneratorTestTop() : Coupled("ArrivalGeneratorTest") {
        addComponent<ArrivalGenerator>(
            "CustomerArrivalGenerator", inter_arrival_time, max_customers);
    }
};

int main() {
    const std::string messages_file = "../simulation_results/arrival_generator_test_output_messages.txt";
    const std::string state_file    = "../simulation_results/arrival_generator_test_output_state.txt";

    std::cout << "=======================================================\n";
    std::cout << " TEST: CustomerArrivalGenerator\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << " inter_arrival_time = " << inter_arrival_time << " s\n";
    std::cout << " max_customers      = " << max_customers << "\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << " Expected outputs:\n";
    for (int i = 1; i <= max_customers; i++)
        std::cout << "   t=" << inter_arrival_time * i
                  << "  customer_id=" << (i-1) << "\n";
    std::cout << "=======================================================\n\n";

    auto model = std::make_shared<ArrivalGeneratorTestTop>();
    auto rc    = RootCoordinator(model);
    rc.setLogger<STDOUTLogger>(";");
    rc.start();
    rc.simulate(inter_arrival_time * max_customers + inter_arrival_time);
    rc.stop();

    return 0;
}