#include "GraphCliffordSimulator.h"
#include "QuantumGraphState.h"
#include "RandomCliffordSimulator.h"
#include "QuantumAutomatonSimulator.h"
#include <iostream>

#define DEFAULT_EVOLUTION_TYPE "random_clifford"
#define DEFAULT_GATE_WIDTH 2

#define RANDOM_CLIFFORD "random_clifford"
#define QUANTUM_AUTOMATON "quantum_automaton"
#define UNITARY "unitary"
#define RANDOM_GRAPH "random_graph"

static inline float sample_powerlaw(float y, float x0, float x1, float a) {
	return std::pow((std::pow(x1, a+1) - std::pow(x0, a+1))*y + std::pow(x0, a+1), 1./(a+1.));
}

GraphCliffordSimulator::GraphCliffordSimulator(Params &params) : EntropySimulator(params) {
	evolution_type = params.get<std::string>("evolution_type", DEFAULT_EVOLUTION_TYPE);


	if (evolution_type == RANDOM_CLIFFORD) {
		gate_width = params.get<int>("gate_width", DEFAULT_GATE_WIDTH);
		initial_offset = false;
	} if (evolution_type == RANDOM_GRAPH) {
		m = params.get<int>("m");
		a = params.get<float>("a");
		rng = std::minstd_rand(params.get<int>("seed", -1));
	} else
		mzr_prob = params.get<float>("mzr_prob");

}

void GraphCliffordSimulator::init_state() {
	state = std::shared_ptr<QuantumGraphState>(new QuantumGraphState(system_size));

	if (evolution_type == QUANTUM_AUTOMATON) // quantum automaton circuit must be polarized
		for (uint i = 0; i < system_size; i++) state->h_gate(i);
}

void GraphCliffordSimulator::mzr(uint q) {
	state->mzr(q);
	if (evolution_type == QUANTUM_AUTOMATON) state->h_gate(q);
}

void GraphCliffordSimulator::qa_timesteps(uint num_steps) {
	assert(system_size % 2 == 0);

	for (uint i = 0; i < num_steps; i++) {
		qa_timestep(state);

		for (uint j = 0; j < system_size; j++) {
			if (state->randf() < mzr_prob)
				mzr(j);
		}
	}
}

void GraphCliffordSimulator::rc_timesteps(uint num_steps) {
	assert(system_size % gate_width == 0);
	assert(gate_width % 2 == 0); // So that offset is a whole number

	bool offset_layer = initial_offset;

	for (uint i = 0; i < num_steps; i++) {
		rc_timestep(state, gate_width, offset_layer);

		for (uint j = 0; j < system_size; j++) {
			if (state->randf() < mzr_prob)
				mzr(j);
		}

		offset_layer = !offset_layer;
	}

	initial_offset = offset_layer;
}

void GraphCliffordSimulator::unitary_timesteps(uint num_steps) {
	assert(system_size % gate_width == 0);
	assert(gate_width % 2 == 0); // So that offset is a whole number

	bool offset_layer = initial_offset;

	for (uint i = 0; i < num_steps; i++) {
		rc_timestep(state, gate_width, offset_layer);

		for (uint j = 0; j < system_size; j++) {
			auto neighbors = state->graph.neighbors(j);
			for (auto k : neighbors) {
				if (randf() < mzr_prob)
					state->toggle_edge_gate(j, k);
			}
		}

		offset_layer = !offset_layer;
	}

	initial_offset = offset_layer;
}

void GraphCliffordSimulator::generate_random_graph() {
	Graph graph(system_size);

	// Barabasi-Albert random graph model
	for (uint i = 0; i < system_size; i++) {
		std::vector<float> weights;
		std::vector<uint> sites;
		for (uint j = 0; j < system_size; j++) {
			if (i == j || graph.contains_edge(i, j)) continue;

			weights.push_back(std::pow(dist(i, j), a));
			sites.push_back(j);
		}

		
		for (uint j = 0; j < m; j++) {
			std::discrete_distribution<uint> distribution(weights.begin(), weights.end());
			uint k = distribution(rng);
			graph.add_edge(i, k);

			weights.erase(weights.begin() + k);
			sites.erase(sites.begin() + k);
		}
	}

	state = std::shared_ptr<QuantumGraphState>(new QuantumGraphState(graph));
}

void GraphCliffordSimulator::timesteps(uint num_steps) {
	if (evolution_type == RANDOM_CLIFFORD)
		rc_timesteps(num_steps);
	else if (evolution_type == QUANTUM_AUTOMATON)
		qa_timesteps(num_steps);
	else if (evolution_type == UNITARY)
		unitary_timesteps(num_steps);
	else if (evolution_type == RANDOM_GRAPH)
		generate_random_graph();
}

uint GraphCliffordSimulator::dist(int i, int j) const {
	uint d = std::abs(i - j);
	if (d > system_size/2) 
		return (system_size - d);
	else 
		return d;
}

void GraphCliffordSimulator::add_distance_distribution(data_t &samples) const {
	uint max_dist = system_size/2;
	std::vector<uint> distribution(max_dist, 0);
	for (uint i = 0; i < system_size; i++) {
		for (auto const j : state->graph.neighbors(i))
			distribution[dist(i, j)]++;
	}

	for (uint i = 0; i < max_dist; i++) 
		samples.emplace("dist_" + std::to_string(i), distribution[i]);
}


void GraphCliffordSimulator::add_avg_max_dist(data_t &samples) const {
	double p1 = 0.;
	double p2 = 0.;
	uint n = 0;
	for (uint i = 0; i < system_size; i++) {
		float max_d = 0.;
		for (auto j : state->graph.neighbors(i)) {
			float d = dist(i, j);
			p1 += d;
			if (d > max_d) max_d = d;

			n++;
		}

		p2 += max_d;
	}

	if (n == 0) {
		samples.emplace("avg_dist", 0.);
		samples.emplace("max_dist", 0.);
	} else {
		samples.emplace("avg_dist", p1/n);
		samples.emplace("max_dist", p2/system_size);
	}
}

void GraphCliffordSimulator::add_degree_distribution(data_t &samples) const {
	auto degree_counts = state->graph.compute_degree_counts();
	for (uint i = 0; i < system_size; i++) 
		samples.emplace("deg_" + std::to_string(i), degree_counts[i]);
}

data_t GraphCliffordSimulator::take_samples() {
	data_t samples = EntropySimulator::take_samples();
	
	add_avg_max_dist(samples);

	add_distance_distribution(samples);
	add_degree_distribution(samples);

	samples.emplace("global_clustering_coefficient", state->graph.global_clustering_coefficient());
	samples.emplace("average_cluster_size", state->graph.average_component_size());
	samples.emplace("max_cluster_size", state->graph.max_component_size());

	return samples;
}