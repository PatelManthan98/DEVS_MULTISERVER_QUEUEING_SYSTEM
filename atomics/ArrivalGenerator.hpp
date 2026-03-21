#pragma once

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include <limits>

namespace multiserver_queue {

struct ArrivalGeneratorState {
    int    customerCount;
    int    maxCustomers;
    double sigma;
    bool   active;

    ArrivalGeneratorState(double iat, int maxC)
        : customerCount(0), maxCustomers(maxC), sigma(iat), active(true) {}
};

inline std::ostream& operator<<(std::ostream& out, const ArrivalGeneratorState& s) {
    out << "{customers_sent: " << s.customerCount
        << ", sigma: "         << s.sigma
        << ", active: "        << s.active << "}";
    return out;
}

class ArrivalGenerator : public cadmium::Atomic<ArrivalGeneratorState> {
public:
    cadmium::Port<int> out_customer;
    double interArrivalTime;

    ArrivalGenerator(const std::string& id, double iat, int maxC)
        : cadmium::Atomic<ArrivalGeneratorState>(id, ArrivalGeneratorState(iat, maxC)),
          interArrivalTime(iat)
    {
        out_customer = addOutPort<int>("out_customer");
    }

    double timeAdvance(const ArrivalGeneratorState& s) const override {
        return s.sigma;
    }

    void internalTransition(ArrivalGeneratorState& s) const override {
        s.customerCount++;
        if (s.customerCount >= s.maxCustomers) {
            s.active = false;
            s.sigma  = std::numeric_limits<double>::infinity();
        } else {
            s.sigma = interArrivalTime;
        }
    }

    void externalTransition(ArrivalGeneratorState& s, double e) const override {
        s.sigma -= e;
    }

    void confluentTransition(ArrivalGeneratorState& s, double e) const override {
        internalTransition(s);
    }

    void output(const ArrivalGeneratorState& s) const override {
        out_customer->addMessage(s.customerCount);
    }
};

} // namespace multiserver_queue