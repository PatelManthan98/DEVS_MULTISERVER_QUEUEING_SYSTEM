#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <memory>
#include "../atomics/ArrivalGenerator.hpp"

using namespace cadmium;
using namespace multiserver_queue;

// ================================================
//  CONFIG
// ================================================
const double inter_arrival_time = 100.0;
const int    max_customers      = 5;
// ================================================

struct ArrivalGeneratorTestSystem : public Coupled {
    ArrivalGeneratorTestSystem(const std::string& id) : Coupled(id) {
        addComponent<ArrivalGenerator>(
            "CustomerArrivalGenerator", inter_arrival_time, max_customers);
    }
};

int main() {
    auto test_system = std::make_shared<ArrivalGeneratorTestSystem>("arrival_generator_test");

    cadmium::RootCoordinator rootCoordinator(test_system);
    rootCoordinator.setLogger<cadmium::STDOUTLogger>(";");

    rootCoordinator.start();
    rootCoordinator.simulate(std::numeric_limits<double>::infinity());
    rootCoordinator.stop();

    return 0;
}