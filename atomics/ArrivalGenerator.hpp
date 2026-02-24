#ifndef ARRIVAL_GENERATOR_HPP
#define ARRIVAL_GENERATOR_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include <limits>

namespace cadmium::example::queue {

struct ArrivalGeneratorState {
    double sigma;
    int    customerCount;
    bool   active;
    explicit ArrivalGeneratorState(double iat)
        : sigma(iat), customerCount(0), active(true) {}
};

std::ostream& operator<<(std::ostream& out, const ArrivalGeneratorState& s) {
    out << "{customers_sent:" << s.customerCount
        << " sigma:"          << s.sigma
        << " active:"         << s.active << "}";
    return out;
}

class ArrivalGenerator : public Atomic<ArrivalGeneratorState> {
public:
    Port<int> out_customer;
    double    interArrivalTime;
    int       maxCustomers;

    ArrivalGenerator(const std::string& id, double iat = 300.0, int maxC = 20)
        : Atomic<ArrivalGeneratorState>(id, ArrivalGeneratorState(iat)),
          interArrivalTime(iat), maxCustomers(maxC)
    {
        out_customer = addOutPort<int>("out_customer");
    }

    void internalTransition(ArrivalGeneratorState& s) const override {
        s.customerCount++;
        if (s.customerCount >= maxCustomers) {
            s.active = false;
            s.sigma  = std::numeric_limits<double>::infinity();
        } else {
            s.sigma = interArrivalTime;
        }
    }

    void externalTransition(ArrivalGeneratorState& s, double e) const override {
        s.sigma -= e;
    }

    // FIXED: confluentTransition (not confluenceTransition)
    void confluentTransition(ArrivalGeneratorState& s, double e) const override {
        internalTransition(s);
    }

    void output(const ArrivalGeneratorState& s) const override {
        out_customer->addMessage(s.customerCount);
    }

    double timeAdvance(const ArrivalGeneratorState& s) const override {
        return s.sigma;
    }
};

} // namespace cadmium::example::queue
#endif