#include "QuantumAutomatonSimulator.h"
#include "RandomCliffordSimulator.h"
#include "SelfOrganizedRandomCliffordSimulator.h"
#include "SelfOrganizedCliffordSimulator.h"
#include "MinCutSimulator.h"
#include "BlockSimulator.h"

#include <DataFrame.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <memory>

using namespace std;
using json = nlohmann::json;

#include "QuantumCHPState.h"
#include "QuantumGraphState.h"

void defaultf() {
    cout << "Default behavior\n";
}

bool file_valid(string filename) {
    uint strlen = filename.length();
    if (strlen < 6) { return false; }
    
    string extension = filename.substr(strlen - 5, strlen);
    string json_ext = ".json";
    if (extension != json_ext) { return false; }

    ifstream f(filename);

    return f.good();
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        defaultf();
        return 1;
    }

    if (argc != 3) cout << "Incorrect arguments.\n";

    string filename = argv[1];
    uint num_threads = std::stoi(argv[2]);
    bool valid = file_valid(filename);
    if (!valid) {
        cout << "Cannot find " << filename << "; aborting.\n";
        return 1;
    }

    std::ifstream f(filename);
    json data = json::parse(f);
    std::string circuit_type = data["circuit_type"];

    std::cout << "Starting job\n";

    std::string data_filename = data["filename"];
    auto params = Params::load_json(data, true);
    std::vector<std::unique_ptr<Config>> configs;

    std::string data_prefix = "../data/";
    for (auto param : params) {
        std::unique_ptr<TimeConfig> config(new TimeConfig(param));
        std::unique_ptr<Simulator> sim;

        if (circuit_type == "quantum_automaton") sim = std::unique_ptr<Simulator>(new QuantumAutomatonSimulator(param));
        else if (circuit_type == "random_clifford") sim = std::unique_ptr<Simulator>(new RandomCliffordSimulator(param));
        else if (circuit_type == "soc_random_clifford") sim = std::unique_ptr<Simulator>(new SelfOrganizedRandomCliffordSimulator(param));
        else if (circuit_type == "soc_clifford") sim = std::unique_ptr<Simulator>(new SelfOrganizedCliffordSimulator(param));
        else if (circuit_type == "mincut") sim = std::unique_ptr<Simulator>(new MinCutSimulator(param));
        else if (circuit_type == "blocksim") sim = std::unique_ptr<Simulator>(new BlockSimulator(param));
        else {
            defaultf();
            return 0;
        }

        config->init_simulator(std::move(sim));
        configs.push_back(std::move(config));
    }

    ParallelCompute pc(std::move(configs));
    DataFrame df = pc.compute(num_threads, true);
    df.write_json(data_prefix + data_filename);
    std::cout << "Finishing job\n";
}