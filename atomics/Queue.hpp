#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <deque>
#include <iostream>
#include <limits>
#include <vector>

namespace cadmium::example::queue {

struct QueueState {
    std::deque<int>  waiting;
    bool             dispatchPending;
    int              nextCustomer;
    std::vector<int> freeServerIds; // track WHICH servers are free, not just count

    QueueState()
        : dispatchPending(false), nextCustomer(-1)
    {
        // Both servers start free
        freeServerIds = {1, 2};
    }
};

std::ostream& operator<<(std::ostream& out, const QueueState& s) {
    out << "{queue_len:"    << s.waiting.size()
        << " dispatch:"     << s.dispatchPending
        << " free_servers:" << s.freeServerIds.size() << "}";
    return out;
}

class Queue : public Atomic<QueueState> {
public:
    Port<int> in_customer;
    Port<int> in_serverFree;
    Port<int> out_dispatch_s1; // dedicated port for Server1
    Port<int> out_dispatch_s2; // dedicated port for Server2

    int targetServer; // which server to dispatch to next

    explicit Queue(const std::string& id)
        : Atomic<QueueState>(id, QueueState()), targetServer(-1)
    {
        in_customer    = addInPort<int>("in_customer");
        in_serverFree  = addInPort<int>("in_serverFree");
        out_dispatch_s1 = addOutPort<int>("out_dispatch_s1");
        out_dispatch_s2 = addOutPort<int>("out_dispatch_s2");
    }

    void internalTransition(QueueState& s) const override {
        s.dispatchPending = false;
        s.nextCustomer    = -1;
        // Try next dispatch immediately if possible
        if (!s.waiting.empty() && !s.freeServerIds.empty()) {
            s.nextCustomer    = s.waiting.front();
            s.waiting.pop_front();
            s.dispatchPending = true;
            // targetServer stays set from previous output() call
        }
    }

    void externalTransition(QueueState& s, double /*e*/) const override {
        // Enqueue new arrivals
        for (const auto& cid : in_customer->getBag())
            s.waiting.push_back(cid);

        // Register newly freed servers by ID
        for (const auto& sid : in_serverFree->getBag())
            s.freeServerIds.push_back(sid);

        // Dispatch if possible
        if (!s.dispatchPending && !s.waiting.empty() && !s.freeServerIds.empty()) {
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
        if (s.dispatchPending && !s.freeServerIds.empty()) {
            // Pick the first available server
            int srv = s.freeServerIds.front();
            // Remove it from free list (const hack via cast)
            const_cast<QueueState&>(s).freeServerIds.erase(
                const_cast<QueueState&>(s).freeServerIds.begin());

            // Send to the correct server's dedicated port
            if (srv == 1)
                out_dispatch_s1->addMessage(s.nextCustomer);
            else
                out_dispatch_s2->addMessage(s.nextCustomer);
        }
    }

    double timeAdvance(const QueueState& s) const override {
        return s.dispatchPending ? 0.0
                                 : std::numeric_limits<double>::infinity();
    }
};

} // namespace cadmium::example::queue
#endif