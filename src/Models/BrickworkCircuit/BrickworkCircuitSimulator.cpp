#include "BrickworkCircuitSimulator.h"

#define RANDOM_HAAR 0
#define RANDOM_REAL 1

BrickworkCircuitSimulator::BrickworkCircuitSimulator(Params &params) : EntropySimulator(params) {
	mzr_prob = get<double>(params, "mzr_prob");
	gate_type = get<int>(params, "gate_type", RANDOM_HAAR);
	offset = false;
}

void BrickworkCircuitSimulator::mzr(uint32_t q) {
	state->measure(q);
}

void BrickworkCircuitSimulator::timesteps(uint32_t num_steps) {
	for (uint32_t k = 0; k < num_steps; k++) {
		for (uint32_t q = 0; q < system_size/2; q++) {
			uint32_t q1 = offset ? 2*q : (2*q + 1) % system_size;
			uint32_t q2 = offset ? (2*q + 1) % system_size : (2*q + 2) % system_size;

			std::vector<uint32_t> qubits{q1, q2};

			if (gate_type == RANDOM_HAAR)
				state->evolve(haar_unitary(2), qubits);
			else if (gate_type == RANDOM_REAL)
				state->evolve(random_real_unitary(), qubits);
		}

		for (uint32_t q = 0; q < system_size; q++) {
			if (randf() < mzr_prob)
				state->measure(q);
		}

		offset = !offset;
	}
}