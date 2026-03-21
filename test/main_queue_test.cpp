#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <cadmium/lib/iestream.hpp>
#include <memory>
#include "../atomics/ArrivalGenerator.hpp"
#include "../atomics/Queue.hpp"

using namespace cadmium;
using namespace multiserver_queue;

// ================================================
//  CONFIG
// ================================================
const double inter_arrival_time = 100.0;
const int    max_customers      = 5;
// Server-free input: input_data/queue_server_free_input_test.txt
// Format: time  server_id
// ================================================

struct QueueTestSystem : public Coupled {
    QueueTestSystem(const std::string& id) : Coupled(id) {
        auto generator   = addComponent<ArrivalGenerator>(
                               "CustomerArrivalGenerator", inter_arrival_time, max_customers);
        auto queue       = addComponent<Queue>("FifoDispatchQueue");
        auto server_free = addComponent<lib::IEStream<int>>(
                               "ServerFreeInjector",
                               "input_data/queue_server_free_input_test.txt");
        addCoupling(generator->out_customer, queue->in_customer);
        addCoupling(server_free->out,        queue->in_server_free);
    }
};

int main() {
    auto test_system = std::make_shared<QueueTestSystem>("queue_test");

    cadmium::RootCoordinator rootCoordinator(test_system);
    rootCoordinator.setLogger<cadmium::STDOUTLogger>(";");

    rootCoordinator.start();
    rootCoordinator.simulate(std::numeric_limits<double>::infinity());
    rootCoordinator.stop();

    return 0;
}