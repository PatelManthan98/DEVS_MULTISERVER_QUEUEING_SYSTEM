#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <memory>
#include <iostream>
#include <string>
#include "../coupled/multiserver_queueing_system.hpp"

using namespace cadmium;
using namespace multiserver_queue;

int main(int argc, char* argv[]) {
    int exp = (argc > 1) ? std::stoi(argv[1]) : 1;

    struct ExperimentConfig {
        double interArrivalTime;
        double serviceTime;
        int    maxCustomers;
        std::string label;
    };

    // rho = serviceTime / (numServers x IAT) = 480 / (2 x IAT)
    ExperimentConfig experiments[] = {
        {900.0, 480.0, 25, "Light Load    (IAT=900s, rho~0.27)"},
        {300.0, 480.0, 25, "Moderate Load (IAT=300s, rho~0.80)"},
        {180.0, 480.0, 25, "Heavy Load    (IAT=180s, rho~1.33)"},
    };

    if (exp < 1 || exp > 3) {
        std::cerr << "Usage: ./bin/MULTISERVER_QUEUE [1|2|3]\n";
        std::cerr << "  1 = Light load    (IAT=900s, rho~0.27)\n";
        std::cerr << "  2 = Moderate load (IAT=300s, rho~0.80)\n";
        std::cerr << "  3 = Heavy load    (IAT=180s, rho~1.33)\n";
        return 1;
    }

    auto& cfg = experiments[exp - 1];

    std::cout << "\n=======================================================\n";
    std::cout << " Multi-Server Queueing System -- Experiment " << exp << "\n";
    std::cout << " " << cfg.label << "\n";
    std::cout << " IAT=" << cfg.interArrivalTime
              << "s  ServiceTime=" << cfg.serviceTime
              << "s  MaxCustomers=" << cfg.maxCustomers << "\n";
    std::cout << "=======================================================\n\n";

    auto test_system = std::make_shared<MultiserverQueueingSystem>(
        "MultiserverQueueingSystem",
        cfg.interArrivalTime, cfg.serviceTime, cfg.maxCustomers);

    cadmium::RootCoordinator rootCoordinator(test_system);
    rootCoordinator.setLogger<cadmium::STDOUTLogger>(";");

    rootCoordinator.start();
    rootCoordinator.simulate(std::numeric_limits<double>::infinity());
    rootCoordinator.stop();

    return 0;
}