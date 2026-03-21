#pragma once

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include <limits>
#include <deque>
#include <vector>

namespace multiserver_queue {

struct QueueState {
    std::deque<int>  waiting;
    bool             dispatchPending;
    int              nextCustomer;
    int              targetServer;
    std::vector<int> freeServerIds;

    QueueState()
        : dispatchPending(false), nextCustomer(-1), targetServer(-1)
    {
        freeServerIds = {1, 2};
    }
};

inline std::ostream& operator<<(std::ostream& out, const QueueState& s) {
    out << "{queue_len: "    << s.waiting.size()
        << ", dispatching: " << s.dispatchPending
        << ", free_servers: "<< s.freeServerIds.size() << "}";
    return out;
}

class Queue : public cadmium::Atomic<QueueState> {
public:
    cadmium::Port<int> in_customer;
    cadmium::Port<int> in_server_free;
    cadmium::Port<int> out_dispatch_s1;
    cadmium::Port<int> out_dispatch_s2;

    explicit Queue(const std::string& id)
        : cadmium::Atomic<QueueState>(id, QueueState())
    {
        in_customer     = addInPort<int>("in_customer");
        in_server_free  = addInPort<int>("in_server_free");
        out_dispatch_s1 = addOutPort<int>("out_dispatch_s1");
        out_dispatch_s2 = addOutPort<int>("out_dispatch_s2");
    }

    double timeAdvance(const QueueState& s) const override {
        return s.dispatchPending ? 0.0 : std::numeric_limits<double>::infinity();
    }

    void internalTransition(QueueState& s) const override {
        s.dispatchPending = false;
        s.nextCustomer    = -1;
        s.targetServer    = -1;
        if (!s.waiting.empty() && !s.freeServerIds.empty()) {
            s.targetServer    = s.freeServerIds.front();
            s.freeServerIds.erase(s.freeServerIds.begin());
            s.nextCustomer    = s.waiting.front();
            s.waiting.pop_front();
            s.dispatchPending = true;
        }
    }

    void externalTransition(QueueState& s, double e) const override {
        if (!in_customer->empty())
            for (const auto& cid : in_customer->getBag())
                s.waiting.push_back(cid);

        if (!in_server_free->empty())
            for (const auto& sid : in_server_free->getBag())
                s.freeServerIds.push_back(sid);

        if (!s.dispatchPending && !s.waiting.empty() && !s.freeServerIds.empty()) {
            s.targetServer    = s.freeServerIds.front();
            s.freeServerIds.erase(s.freeServerIds.begin());
            s.nextCustomer    = s.waiting.front();
            s.waiting.pop_front();
            s.dispatchPending = true;
        }
    }

    void confluentTransition(QueueState& s, double e) const override {
        internalTransition(s);
        externalTransition(s, 0.0);
    }

    void output(const QueueState& s) const override {
        if (s.dispatchPending) {
            if (s.targetServer == 1)
                out_dispatch_s1->addMessage(s.nextCustomer);
            else
                out_dispatch_s2->addMessage(s.nextCustomer);
        }
    }
};

} // namespace multiserver_queue