#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/modeling/devs/atomic.hpp>
#include "../coupled/parallel_server_pool.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <limits>
#include <vector>
#include <string>

using namespace cadmium::example::queue;
using namespace cadmium;

// ═══════════════════════════════════════════════════════
//  CONFIG — change these to test different scenarios
// ═══════════════════════════════════════════════════════
const double service_time    = 150.0;   // seconds to serve one customer
const double sim_duration    = 500.0;   // total simulation time
// ═══════════════════════════════════════════════════════

// ── Reads {time, value} pairs from a .txt file ───────────────────────────────
std::vector<std::pair<double,int>> read_input_file(const std::string& filepath) {
    std::vector<std::pair<double,int>> events;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "WARNING: could not open " << filepath << "\n";
        return events;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        double t; int v;
        if (iss >> t >> v) events.push_back({t, v});
    }
    return events;
}

// ── File-driven injector atomic model ────────────────────────────────────────
struct InjectorState {
    std::vector<std::pair<double,int>> events;
    int    index;
    double sigma;
    InjectorState(std::vector<std::pair<double,int>> e)
        : events(e), index(0),
          sigma(e.empty() ? std::numeric_limits<double>::infinity() : e[0].first) {}
};
std::ostream& operator<<(std::ostream& o, const InjectorState& s) {
    return o << "{injector index:" << s.index << " sigma:" << s.sigma << "}";
}
class Injector : public Atomic<InjectorState> {
public:
    Port<int> out;
    Injector(const std::string& id, std::vector<std::pair<double,int>> events)
        : Atomic<InjectorState>(id, InjectorState(events))
    { out = addOutPort<int>("out"); }

    void internalTransition(InjectorState& s) const override {
        s.index++;
        if (s.index >= (int)s.events.size())
            s.sigma = std::numeric_limits<double>::infinity();
        else
            s.sigma = s.events[s.index].first - s.events[s.index-1].first;
    }
    void externalTransition(InjectorState& s, double e) const override { s.sigma -= e; }
    void confluentTransition(InjectorState& s, double e) const override { internalTransition(s); }
    void output(const InjectorState& s) const override {
        if (s.index < (int)s.events.size())
            out->addMessage(s.events[s.index].second);
    }
    double timeAdvance(const InjectorState& s) const override { return s.sigma; }
};

// ── Top: wire two injectors → ParallelServerPool ─────────────────────────────
class PoolTestTop : public Coupled {
public:
    PoolTestTop(
        std::vector<std::pair<double,int>> s1_events,
        std::vector<std::pair<double,int>> s2_events
    ) : Coupled("PoolTest") {

        auto injector1 = addComponent<Injector>("DispatchToServer1", s1_events);
        auto injector2 = addComponent<Injector>("DispatchToServer2", s2_events);
        auto pool      = addComponent<ParallelServerPool>("ParallelServerPool", service_time);

        // Dedicated wires — each injector feeds only its own server
        addCoupling(injector1->out, pool->in_dispatch_server1);
        addCoupling(injector2->out, pool->in_dispatch_server2);
    }
};

int main() {
    const std::string s1_file = "../input_data/pool_server1_dispatch_test.txt";
    const std::string s2_file = "../input_data/pool_server2_dispatch_test.txt";

    auto s1_events = read_input_file(s1_file);
    auto s2_events = read_input_file(s2_file);

    std::cout << "=======================================================\n";
    std::cout << " TEST: ParallelServerPool\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << " service_time = " << service_time << " s\n";
    std::cout << " Server1 dispatch file : " << s1_file << "\n";
    for (auto& [t, cid] : s1_events)
        std::cout << "   t=" << t << " customer_id=" << cid
                  << "  (Server1 done at t=" << t + service_time << ")\n";
    std::cout << " Server2 dispatch file : " << s2_file << "\n";
    for (auto& [t, cid] : s2_events)
        std::cout << "   t=" << t << " customer_id=" << cid
                  << "  (Server2 done at t=" << t + service_time << ")\n";
    std::cout << "-------------------------------------------------------\n";

    auto model = std::make_shared<PoolTestTop>(s1_events, s2_events);
    auto rc    = RootCoordinator(model);
    rc.setLogger<STDOUTLogger>(";");
    rc.start();
    rc.simulate(sim_duration);
    rc.stop();
    return 0;
}