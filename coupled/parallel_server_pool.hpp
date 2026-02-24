#ifndef PARALLEL_SERVER_POOL_HPP
#define PARALLEL_SERVER_POOL_HPP
//
// Level-2 Coupled Model: Parallel Server Pool
//
// Contains Server1 and Server2 running independently in parallel.
// Uses dedicated input ports per server so messages are never shared.
//
// Ports:
//   in_dispatch_server1  → routes exclusively to Server1
//   in_dispatch_server2  → routes exclusively to Server2
//   out_server_free      → forwards completion signals from either server
// =============================================================================

#include <cadmium/modeling/devs/coupled.hpp>
#include "../atomics/Server.hpp"

namespace cadmium::example::queue {

class ParallelServerPool : public Coupled {
public:
    Port<int> in_dispatch_server1;
    Port<int> in_dispatch_server2;
    Port<int> out_server_free;

    ParallelServerPool(const std::string& id, double serviceTime = 480.0)
        : Coupled(id)
    {
        in_dispatch_server1 = addInPort<int>("in_dispatch_server1");
        in_dispatch_server2 = addInPort<int>("in_dispatch_server2");
        out_server_free     = addOutPort<int>("out_server_free");

        auto server1 = addComponent<Server>("Server1", 1, serviceTime);
        auto server2 = addComponent<Server>("Server2", 2, serviceTime);

        addCoupling(in_dispatch_server1, server1->in_dispatch);
        addCoupling(in_dispatch_server2, server2->in_dispatch);
        addCoupling(server1->out_serverFree, out_server_free);
        addCoupling(server2->out_serverFree, out_server_free);
    }
};

} 
#endif 