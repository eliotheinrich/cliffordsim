#pragma once

#include <Simulator.hpp>
#include <QuantumState.h>

class BrickworkCircuitSimulator : public Simulator {
	private:
		uint32_t system_size;

		double mzr_prob;
		int gate_type;

		bool offset;

		EntropySampler sampler;

		void mzr(uint32_t q);

	public:
		std::shared_ptr<Statevector> state;

		BrickworkCircuitSimulator(Params &params, uint32_t num_threads);

		virtual void timesteps(uint32_t num_steps) override;

		virtual data_t take_samples() override;
};