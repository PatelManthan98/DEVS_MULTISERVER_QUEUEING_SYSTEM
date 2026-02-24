#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/modeling/devs/atomic.hpp>
#include "../atomics/ArrivalGenerator.hpp"
#include "../atomics/Queue.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <limits>
#include <vector>
#include <string>

using namespace cadmium::example::queue;
using namespace cadmium;

// ═══════════════════════════════════════════════
//  CONFIG — change these to test different inputs
// ═══════════════════════════════════════════════
const double inter_arrival_time = 100.0;  // seconds between customer arrivals
const int    max_customers      = 5;      // number of customers to generate
// ═══════════════════════════════════════════════

// ── Read server-free signals from file ────────────────────────────────────────
// Format: one event per line → "time  server_id"
std::vector<std::pair<double,int>> read_server_free_file(const std::string& filepath) {
    std::vector<std::pair<double,int>> events;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << " WARNING: cannot open " << filepath << "\n";
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

// ── Server-free signal injector ───────────────────────────────────────────────
struct InjectorState {
    std::vector<std::pair<double,int>> events;
    int index; double sigma;
    InjectorState(std::vector<std::pair<double,int>> e)
        : events(e), index(0),
          sigma(e.empty() ? std::numeric_limits<double>::infinity() : e[0].first) {}
};
std::ostream& operator<<(std::ostream& o, const InjectorState& s) {
    return o << "{injector index:" << s.index << " sigma:" << s.sigma << "}";
}
class ServerFreeInjector : public Atomic<InjectorState> {
public:
    Port<int> out;
    ServerFreeInjector(const std::string& id, std::vector<std::pair<double,int>> events)
        : Atomic<InjectorState>(id, InjectorState(events))
    { out = addOutPort<int>("out"); }
    void internalTransition(InjectorState& s) const override {
        s.index++;
        s.sigma = (s.index >= (int)s.events.size())
            ? std::numeric_limits<double>::infinity()
            : s.events[s.index].first - s.events[s.index-1].first;
    }
    void externalTransition(InjectorState& s, double e) const override { s.sigma -= e; }
    void confluentTransition(InjectorState& s, double e) const override { internalTransition(s); }
    void output(const InjectorState& s) const override {
        if (s.index < (int)s.events.size())
            out->addMessage(s.events[s.index].second);
    }
    double timeAdvance(const InjectorState& s) const override { return s.sigma; }
};

// ── Top model: ArrivalGenerator + ServerFreeInjector → Queue ─────────────────
class QueueTestTop : public Coupled {
public:
    QueueTestTop(std::vector<std::pair<double,int>> server_free_events)
        : Coupled("QueueTest")
    {
        // ArrivalGenerator produces customers internally
        auto generator  = addComponent<ArrivalGenerator>(
                              "CustomerArrivalGenerator",
                              inter_arrival_time, max_customers);

        // ServerFreeInjector sends server-free signals from file
        auto server_src = addComponent<ServerFreeInjector>(
                              "ServerFreeInput", server_free_events);

        auto queue      = addComponent<Queue>("FifoDispatchQueue");

        addCoupling(generator->out_customer, queue->in_customer);
        addCoupling(server_src->out,         queue->in_serverFree);
    }
};

int main() {
    const std::string server_free_file = "../input_data/queue_server_free_input_test.txt";
    const std::string output_messages  = "../simulation_results/queue_test_output_messages.txt";
    const std::string output_state     = "../simulation_results/queue_test_output_state.txt";

    auto server_free_events = read_server_free_file(server_free_file);

    std::cout << "=======================================================\n";
    std::cout << " TEST: FifoDispatchQueue\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << " Customer source (internal ArrivalGenerator):\n";
    std::cout << "   inter_arrival_time = " << inter_arrival_time << " s\n";
    std::cout << "   max_customers      = " << max_customers << "\n";
    std::cout << " Server-free signals (" << server_free_file << "):\n";
    for (auto& [t, sid] : server_free_events)
        std::cout << "   t=" << t << "  server_id=" << sid << "\n";
    std::cout << "-------------------------------------------------------\n\n";

    double sim_end = inter_arrival_time * max_customers + 500.0;

    auto model = std::make_shared<QueueTestTop>(server_free_events);
    auto rc    = RootCoordinator(model);
    rc.setLogger<STDOUTLogger>(";");
    rc.start();
    rc.simulate(sim_end);
    rc.stop();
    return 0;
}