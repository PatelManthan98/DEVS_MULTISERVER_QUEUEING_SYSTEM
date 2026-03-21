#pragma once

#include <cadmium/modeling/devs/coupled.hpp>
#include "../atomics/ArrivalGenerator.hpp"
#include "../atomics/Queue.hpp"
#include "../coupled/parallel_server_pool.hpp"

namespace multiserver_queue {

struct MultiserverQueueingSystem : public cadmium::Coupled {

    MultiserverQueueingSystem(const std::string& id,
                              double interArrivalTime,
                              double serviceTime,
                              int    maxCustomers)
        : cadmium::Coupled(id)
    {
        auto generator = addComponent<ArrivalGenerator>(
                             "CustomerArrivalGenerator", interArrivalTime, maxCustomers);
        auto queue     = addComponent<Queue>("FifoDispatchQueue");
        auto pool      = addComponent<ParallelServerPool>("ParallelServerPool", serviceTime);

        addCoupling(generator->out_customer, queue->in_customer);
        addCoupling(queue->out_dispatch_s1,  pool->in_dispatch_s1);
        addCoupling(queue->out_dispatch_s2,  pool->in_dispatch_s2);
        addCoupling(pool->out_server_free,   queue->in_server_free);
    }
};

} // namespace multiserver_queue