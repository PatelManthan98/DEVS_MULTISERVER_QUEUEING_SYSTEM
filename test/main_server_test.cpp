#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/stdout.hpp>
#include "../atomics/Server.hpp"
#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <limits>
#include <vector>
#include <string>

using namespace cadmium::example::queue;
using namespace cadmium;

const int    server_id    = 2;      // which server to test (1 or 2)
const double service_time = 120.0;  // seconds to serve one customer
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

struct FileInputState {
    std::vector<std::pair<double,int>> events;
    int index; double sigma;
    FileInputState(std::vector<std::pair<double,int>> e)
        : events(e), index(0),
          sigma(e.empty() ? std::numeric_limits<double>::infinity() : e[0].first) {}
};
std::ostream& operator<<(std::ostream& o, const FileInputState& s) {
    return o << "{file_input index:" << s.index << " sigma:" << s.sigma << "}";
}
class FileInputGenerator : public Atomic<FileInputState> {
public:
    Port<int> out;
    FileInputGenerator(const std::string& id, std::vector<std::pair<double,int>> events)
        : Atomic<FileInputState>(id, FileInputState(events))
    { out = addOutPort<int>("out"); }

    void internalTransition(FileInputState& s) const override {
        s.index++;
        if (s.index >= (int)s.events.size())
            s.sigma = std::numeric_limits<double>::infinity();
        else
            s.sigma = s.events[s.index].first - s.events[s.index-1].first;
    }

    void externalTransition(FileInputState& s, double e) const override { s.sigma -= e; }

    void confluentTransition(FileInputState& s, double e) const override { internalTransition(s); }
    
    void output(const FileInputState& s) const override {
        if (s.index < (int)s.events.size())
            out->addMessage(s.events[s.index].second);
    }

    double timeAdvance(const FileInputState& s) const override { return s.sigma; }
};

// ── Top: wire file input → Server ────────────────────────────────────────────
class ServerTestTop : public Coupled {
public:
    ServerTestTop(std::vector<std::pair<double,int>> dispatch_events) : Coupled("ServerTest") {
        auto injector = addComponent<FileInputGenerator>("DispatchInput", dispatch_events);
        auto server   = addComponent<Server>(
                            "Server" + std::to_string(server_id), server_id, service_time);
        addCoupling(injector->out, server->in_dispatch);
    }
};

int main() {
    const std::string input_file           = "../input_data/server_input_test.txt";
    const std::string output_messages_file = "../simulation_results/server_test_output_messages.txt";
    const std::string output_state_file    = "../simulation_results/server_test_output_state.txt";

    auto dispatch_events = read_input_file(input_file);

    std::cout << "=======================================================\n";
    std::cout << " TEST: Server\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << " server_id    = " << server_id    << "\n";
    std::cout << " service_time = " << service_time << " s\n";
    std::cout << " Dispatch events loaded:\n";
    for (auto& [t, cid] : dispatch_events)
        std::cout << "   t=" << t << " customer_id=" << cid
                  << "  (done at t=" << t + service_time << ")\n";
    std::cout << "=======================================================\n\n";

    auto model = std::make_shared<ServerTestTop>(dispatch_events);
    auto rc    = RootCoordinator(model);
    rc.setLogger<STDOUTLogger>(";");
    rc.start();
    rc.simulate(600.0);
    rc.stop();

    std::cout << "\n=======================================================\n";
    return 0;
}