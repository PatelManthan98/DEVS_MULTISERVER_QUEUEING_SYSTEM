#pragma once

#include <cadmium/modeling/devs/coupled.hpp>
#include "../atomics/Server.hpp"

namespace multiserver_queue {

struct ParallelServerPool : public cadmium::Coupled {
    cadmium::Port<int> in_dispatch_s1;
    cadmium::Port<int> in_dispatch_s2;
    cadmium::Port<int> out_server_free;

    ParallelServerPool(const std::string& id, double serviceTime)
        : cadmium::Coupled(id)
    {
        in_dispatch_s1  = addInPort<int>("in_dispatch_s1");
        in_dispatch_s2  = addInPort<int>("in_dispatch_s2");
        out_server_free = addOutPort<int>("out_server_free");

        auto server1 = addComponent<Server>("Server1", 1, serviceTime);
        auto server2 = addComponent<Server>("Server2", 2, serviceTime);

        addCoupling(in_dispatch_s1, server1->in_dispatch);
        addCoupling(in_dispatch_s2, server2->in_dispatch);
        addCoupling(server1->out_server_free, out_server_free);
        addCoupling(server2->out_server_free, out_server_free);
    }
};

} // namespace multiserver_queue