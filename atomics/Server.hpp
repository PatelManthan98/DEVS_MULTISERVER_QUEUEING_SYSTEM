#pragma once

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include <limits>

namespace multiserver_queue {

enum class ServerPhase { IDLE, BUSY };

struct ServerState {
    ServerPhase phase;
    int         serverId;
    int         currentCustomer;
    double      sigma;

    explicit ServerState(int id)
        : phase(ServerPhase::IDLE), serverId(id),
          currentCustomer(-1),
          sigma(std::numeric_limits<double>::infinity()) {}
};

inline std::ostream& operator<<(std::ostream& out, const ServerState& s) {
    out << "{server: "    << s.serverId
        << ", phase: "    << (s.phase == ServerPhase::IDLE ? "IDLE" : "BUSY")
        << ", customer: " << s.currentCustomer
        << ", sigma: "    << s.sigma << "}";
    return out;
}

class Server : public cadmium::Atomic<ServerState> {
public:
    cadmium::Port<int> in_dispatch;
    cadmium::Port<int> out_server_free;
    double serviceTime;

    Server(const std::string& id, int serverId, double svcTime)
        : cadmium::Atomic<ServerState>(id, ServerState(serverId)), serviceTime(svcTime)
    {
        in_dispatch     = addInPort<int>("in_dispatch");
        out_server_free = addOutPort<int>("out_server_free");
    }

    double timeAdvance(const ServerState& s) const override {
        return s.sigma;
    }

    void internalTransition(ServerState& s) const override {
        s.phase           = ServerPhase::IDLE;
        s.currentCustomer = -1;
        s.sigma           = std::numeric_limits<double>::infinity();
    }

    void externalTransition(ServerState& s, double e) const override {
        if (s.phase == ServerPhase::BUSY) {
            s.sigma -= e;
            return;
        }
        if (!in_dispatch->empty()) {
            s.currentCustomer = in_dispatch->getBag().back();
            s.phase           = ServerPhase::BUSY;
            s.sigma           = serviceTime;
        }
    }

    void confluentTransition(ServerState& s, double e) const override {
        internalTransition(s);
        externalTransition(s, 0.0);
    }

    void output(const ServerState& s) const override {
        if (s.phase == ServerPhase::BUSY)
            out_server_free->addMessage(s.serverId);
    }
};

} // namespace multiserver_queue