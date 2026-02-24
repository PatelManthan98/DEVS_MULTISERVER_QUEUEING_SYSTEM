#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include "../coupled/multiserver_queueing_system.hpp"
#include <iostream>
#include <memory>
#include <limits>
#include <string>

using namespace cadmium::example::queue;

int main(int argc, char* argv[]) {
    int exp = (argc > 1) ? std::stoi(argv[1]) : 1;

    struct ExperimentConfig {
        double      interArrivalTime;
        double      serviceTime;
        int         maxCustomers;
        std::string label;
    };

    // rho = serviceTime / (N_servers * IAT), N=2, serviceTime=480
    ExperimentConfig experiments[] = {
        {900.0, 480.0, 25, "Light Load    (IAT=900s, rho~0.27)"},
        {300.0, 480.0, 25, "Moderate Load (IAT=300s, rho~0.80)"},
        {180.0, 480.0, 25, "Heavy Load    (IAT=180s, rho~1.33)"},
    };

    if (exp < 1 || exp > 3) {
        std::cerr << "Usage: ./multiserver_queue_sim [1|2|3]\n";
        std::cerr << "  1 = Light load    (IAT=900s)\n";
        std::cerr << "  2 = Moderate load (IAT=300s)\n";
        std::cerr << "  3 = Heavy load    (IAT=180s)\n";
        return 1;
    }

    auto& cfg = experiments[exp - 1];

    std::cout << "\n=======================================================\n";
    std::cout << " Multi-Server Queueing System — Experiment " << exp << "\n";
    std::cout << " " << cfg.label << "\n";
    std::cout << " IAT=" << cfg.interArrivalTime
              << "s  ServiceTime=" << cfg.serviceTime
              << "s  MaxCustomers=" << cfg.maxCustomers << "\n";
    std::cout << "=======================================================\n\n";

    auto model = std::make_shared<MultiserverQueueingSystem>(
        "MultiserverQueueingSystem",
        cfg.interArrivalTime,
        cfg.serviceTime,
        cfg.maxCustomers
    );

    auto rc = cadmium::RootCoordinator(model);
    rc.setLogger<cadmium::STDOUTLogger>(";");
    rc.start();
    rc.simulate(std::numeric_limits<double>::infinity());
    rc.stop();

    std::cout << "\n=======================================================\n";
    std::cout << " Simulation Complete — Experiment " << exp << "\n";
    std::cout << "=======================================================\n";
    return 0;
}