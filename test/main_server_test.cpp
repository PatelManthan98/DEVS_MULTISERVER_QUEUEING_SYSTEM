#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <cadmium/lib/iestream.hpp>
#include <memory>
#include "../atomics/Server.hpp"

using namespace cadmium;
using namespace multiserver_queue;

// ================================================
//  CONFIG
// ================================================
const int    server_id    = 1;
const double service_time = 120.0;
// Input: input_data/server_input_test.txt
// Format: time  customer_id
// ================================================

struct ServerTestSystem : public Coupled {
    ServerTestSystem(const std::string& id) : Coupled(id) {
        auto injector = addComponent<lib::IEStream<int>>(
                            "DispatchInput",
                            "input_data/server_input_test.txt");
        auto server   = addComponent<Server>("Server1", server_id, service_time);
        addCoupling(injector->out, server->in_dispatch);
    }
};

int main() {
    auto test_system = std::make_shared<ServerTestSystem>("server_test");

    cadmium::RootCoordinator rootCoordinator(test_system);
    rootCoordinator.setLogger<cadmium::STDOUTLogger>(";");

    rootCoordinator.start();
    rootCoordinator.simulate(std::numeric_limits<double>::infinity());
    rootCoordinator.stop();

    return 0;
}