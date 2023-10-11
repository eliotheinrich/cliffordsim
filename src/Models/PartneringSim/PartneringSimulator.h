#pragma once

#include <Simulator.hpp>
#include <Graph.h>

#include <climits>


class PartneringSimulator : public Simulator {
	private:
		uint32_t num_nodes;
		uint32_t affinity_type;

		double relaxation_time;

		bool sample_global_properties;
		bool sample_local_properties;
		bool sample_affinity;
		bool sample_counts;

		bool start_sampling = false;


		std::vector<std::vector<uint32_t>> counts;

		double affinity(uint32_t i, uint32_t j) const {
			return double(affinity_graph.edge_weight(i, j))/INT_MAX;
		}

		double last_contact(uint32_t i, uint32_t j) const {
			return augmented_graph.edge_weight(i, j);
		}

		void add_affinity_samples(data_t& samples) const;
		void add_global_properties_samples(data_t& samples) const;
		void add_local_properties_samples(data_t& samples) const;
		void add_counts_samples(data_t& samples) const;

	public:
		Graph partner_graph;
		Graph affinity_graph;
		Graph augmented_graph;

		PartneringSimulator(Params &params);

		virtual void init_state(uint32_t) override;
		virtual void timesteps(uint32_t num_steps) override;
		virtual void equilibration_timesteps(uint32_t num_steps) override {
			start_sampling = false;
			timesteps(num_steps);
			start_sampling = true;
		}

		virtual data_t take_samples() override;

		CLONE(Simulator, PartneringSimulator)
};