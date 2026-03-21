#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <cadmium/lib/iestream.hpp>
#include <memory>
#include "../coupled/parallel_server_pool.hpp"

using namespace cadmium;
using namespace multiserver_queue;

const double service_time = 150.0;
// Server1 input: input_data/pool_server1_dispatch_test.txt
// Server2 input: input_data/pool_server2_dispatch_test.txt
// Format: time  customer_id
// ================================================

struct ParallelServerPoolTestSystem : public Coupled {
    ParallelServerPoolTestSystem(const std::string& id) : Coupled(id) {
        auto pool = addComponent<ParallelServerPool>("ParallelServerPool", service_time);
        auto inj1 = addComponent<lib::IEStream<int>>(
                        "DispatchToServer1",
                        "input_data/pool_server1_dispatch_test.txt");
        auto inj2 = addComponent<lib::IEStream<int>>(
                        "DispatchToServer2",
                        "input_data/pool_server2_dispatch_test.txt");
        addCoupling(inj1->out, pool->in_dispatch_s1);
        addCoupling(inj2->out, pool->in_dispatch_s2);
    }
};

int main() {
    auto test_system = std::make_shared<ParallelServerPoolTestSystem>("parallel_server_pool_test");

    cadmium::RootCoordinator rootCoordinator(test_system);
    rootCoordinator.setLogger<cadmium::STDOUTLogger>(";");

    rootCoordinator.start();
    rootCoordinator.simulate(500.0);
    rootCoordinator.stop();

    return 0;
}