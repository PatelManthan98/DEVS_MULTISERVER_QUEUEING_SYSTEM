#ifndef SERVER_HPP
#define SERVER_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include <limits>

namespace cadmium::example::queue {

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

std::ostream& operator<<(std::ostream& out, const ServerState& s) {
    out << "{server:"   << s.serverId
        << " phase:"    << (s.phase == ServerPhase::IDLE ? "IDLE" : "BUSY")
        << " customer:" << s.currentCustomer
        << " sigma:"    << s.sigma << "}";
    return out;
}

class Server : public Atomic<ServerState> {
public:
    Port<int> in_dispatch;
    Port<int> out_serverFree;
    double    serviceTime;

    Server(const std::string& id, int serverId, double svcTime = 480.0)
        : Atomic<ServerState>(id, ServerState(serverId)),
          serviceTime(svcTime)
    {
        in_dispatch    = addInPort<int>("in_dispatch");
        out_serverFree = addOutPort<int>("out_serverFree");
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
        for (const auto& cid : in_dispatch->getBag()) {
            s.currentCustomer = cid;
            s.phase           = ServerPhase::BUSY;
            s.sigma           = serviceTime;
            break;
        }
    }

    void confluentTransition(ServerState& s, double e) const override {
        internalTransition(s);
        externalTransition(s, 0.0);
    }

    void output(const ServerState& s) const override {
        if (s.phase == ServerPhase::BUSY)
            out_serverFree->addMessage(s.serverId);
    }

    double timeAdvance(const ServerState& s) const override {
        return s.sigma;
    }
};

} 
#endif