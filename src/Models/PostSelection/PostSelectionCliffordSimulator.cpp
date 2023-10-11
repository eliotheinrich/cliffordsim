#include "PostSelectionCliffordSimulator.h"
#include <RandomCliffordSimulator.h>
#include <iostream>

#define DEFAULT_CLIFFORD_TYPE "chp"

PostSelectionCliffordSimulator::PostSelectionCliffordSimulator(Params &params) : EntropySimulator(params) {
	mzr_prob = get<double>(params, "mzr_prob");
}

void PostSelectionCliffordSimulator::mzr(uint32_t i) {
	state->mzr(i);
}

void PostSelectionCliffordSimulator::timesteps(uint32_t num_steps) {
	for (uint32_t k = 0; k < num_steps; k++) {
		rc_timestep(state, 2, false);
		rc_timestep(state, 2, true);

		for (uint32_t i = 0; i < system_size; i++) {
			if (randf() < mzr_prob)
				mzr(i);
		}
	}
}

