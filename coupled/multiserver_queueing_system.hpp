#ifndef MULTISERVER_QUEUEING_SYSTEM_HPP
#define MULTISERVER_QUEUEING_SYSTEM_HPP

// Level-1 Top Coupled Model: Multi-Server Queueing System
//
// Connects all submodels:
//   CustomerArrivalGenerator  → FifoDispatchQueue → ParallelServerPool
//                                      ↑_______server_free feedback_______↓
// =============================================================================

#include <cadmium/modeling/devs/coupled.hpp>
#include "../atomics/ArrivalGenerator.hpp"
#include "../atomics/Queue.hpp"
#include "parallel_server_pool.hpp"

namespace cadmium::example::queue {

class MultiserverQueueingSystem : public Coupled {
public:
    MultiserverQueueingSystem(const std::string& id,
                              double interArrivalTime = 300.0,
                              double serviceTime      = 480.0,
                              int    maxCustomers     = 20)
        : Coupled(id)
    {
        auto generator  = addComponent<ArrivalGenerator>(
                              "CustomerArrivalGenerator", interArrivalTime, maxCustomers);
        auto queue      = addComponent<Queue>("FifoDispatchQueue");
        auto serverPool = addComponent<ParallelServerPool>("ParallelServerPool", serviceTime);

        addCoupling(generator->out_customer,     queue->in_customer);
        addCoupling(queue->out_dispatch_s1,      serverPool->in_dispatch_server1);
        addCoupling(queue->out_dispatch_s2,      serverPool->in_dispatch_server2);
        addCoupling(serverPool->out_server_free, queue->in_serverFree);  // feedback
    }
};

} // namespace cadmium::example::queue
#endif