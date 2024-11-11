#pragma once

#include <Simulator.hpp>
#include <Samplers.h>

#define MPSS_PROJECTIVE 0
#define MPSS_WEAK 1

#define MPSS_HAAR 0
#define MPSS_CLIFFORD 1
#define MPSS_Z2_CLIFFORD 2

#define TWO_QUBIT_PAULI PauliString("+XX")
#define ONE_QUBIT_PAULI PauliString("+Z")

static CliffordTable get_z2_table() {
  auto symm = [](const QuantumCircuit& qc) {
    PauliString p = PauliString("+ZZ");
    PauliString p_ = p;
    qc.apply(p);
    return p == p_;
  };

  return CliffordTable(symm);
}

class MatrixProductSimulator : public dataframe::Simulator {
	private:
		uint32_t system_size;
		double beta;
    double p;
    size_t bond_dimension;

    int measurement_type;
    int unitary_type;

		InterfaceSampler interface_sampler;
    QuantumStateSampler quantum_sampler;

    CliffordTable z2_table;

    bool offset;

    std::vector<double> mzr_freq;
    std::vector<double> mxxr_freq;

    void measure(uint32_t i, uint32_t j) {
      // TODO check that single-qubit and two-qubit measurements are balanced
      if (measurement_type == MPSS_PROJECTIVE) {
        // Do projective measurement
        if (randf() < beta) {
          if (randf() < p) {
            mxxr_freq[i] += 1.0;
            mxxr_freq[j] += 1.0;
            state->measure(TWO_QUBIT_PAULI, {i, j});
          } else {
            if (rand() % 2) {
              mzr_freq[i] += 1.0;
              state->measure(ONE_QUBIT_PAULI, {i});
            } else {
              mzr_freq[j] += 1.0;
              state->measure(ONE_QUBIT_PAULI, {j});
            }
          }
        }
      } else if (measurement_type == MPSS_WEAK) {
        // Do weak measurement
        if (randf() < p) {
          mxxr_freq[i] += 1.0;
          mxxr_freq[j] += 1.0;
          state->weak_measure(TWO_QUBIT_PAULI, {i, j}, beta);
        } else {
          if (rand() % 2) {
            mzr_freq[i] += 1.0;
            state->weak_measure(ONE_QUBIT_PAULI, {i}, beta);
          } else {
            mzr_freq[j] += 1.0;
            state->weak_measure(ONE_QUBIT_PAULI, {j}, beta);
          }
        }
      }

    }

    void unitary(uint32_t i, uint32_t j) {
      if (unitary_type == MPSS_HAAR) {
        Eigen::Matrix4cd gate = haar_unitary(2, rng);
        state->evolve(gate, {i, j});
      } else if (unitary_type == MPSS_CLIFFORD) {
        state->random_clifford({i, j});
      } else if (unitary_type == MPSS_Z2_CLIFFORD) {
        z2_table.apply_random(rng, {i, j}, *state.get());
      } else {
        throw std::runtime_error(std::format("Invalid unitary type {}.", unitary_type));
      }
    }

	public:
    std::shared_ptr<MatrixProductState> state;
		MatrixProductSimulator(dataframe::Params &params, uint32_t num_threads) : Simulator(params), interface_sampler(params), quantum_sampler(params), z2_table(get_z2_table()) {
      system_size = dataframe::utils::get<int>(params, "system_size");
      beta = dataframe::utils::get<double>(params, "beta");
      p = dataframe::utils::get<double>(params, "p");
      bond_dimension = dataframe::utils::get<int>(params, "bond_dimension", 32);

      measurement_type = dataframe::utils::get<int>(params, "measurement_type", MPSS_PROJECTIVE);
      unitary_type = dataframe::utils::get<int>(params, "unitary_type", MPSS_HAAR);

      mzr_freq = std::vector<double>(system_size, 0.0);
      mxxr_freq = std::vector<double>(system_size, 0.0);

      offset = false;

      state = std::make_shared<MatrixProductState>(system_size, bond_dimension);
      state->seed(rand());

      PauliMutationFunc z2_mutation = [](PauliString& p, std::minstd_rand& rng) {
        size_t n = p.num_qubits;
        PauliString q(n);
        if (rng() % 2) {
          // Single-qubit mutation
          q.set_z(rng() % n, 1);
        } else {
          size_t i = rng() % n;
          size_t j = rng() % n;
          while (j == i) {
            j = rng() % n;
          }

          q.set_x(i, 1);
          q.set_x(j, 1);
        }

        p = p * q;
      };

      quantum_sampler.set_montecarlo_update(z2_mutation);
    }

		virtual void timesteps(uint32_t num_steps) override {
      for (size_t t = 0; t < num_steps; t++) {
        for (size_t i = 0; i < system_size/2 - offset; i++) {
          size_t q1 = 2*i + offset;
          size_t q2 = 2*i + 1 + offset;
          unitary(q1, q2);
        }

        for (size_t i = 0; i < system_size-1; i++) {
          // do measurements
          measure(i, i+1);
        }

        // -------- 
        //std::string sx, sz;
        //for (size_t i = 0; i < system_size; i++) {
        //  sx += "X";
        //  sz += "Z";
        //}
        //PauliString X(sx);
        //PauliString Z(sz);
        //double Tx = state->expectation(X);
        //double Tz = state->expectation(Z);
        //std::cout << std::format("After timestep {}, X = {}, Z = {}\n", t, Tx, Tz);
        // -------- 

        offset = !offset;
      }
    }

    virtual dataframe::data_t take_samples() override {
      dataframe::data_t samples;

      std::vector<int> surface = state->get_entropy_surface<int>(1u);
      interface_sampler.add_samples(samples, surface);

      quantum_sampler.add_samples(samples, state);

      std::vector<double> bond_dimensions(system_size - 1);
      for (size_t i = 0; i < system_size - 1; i++) {
        bond_dimensions[i] = static_cast<double>(state->bond_dimension(i));
      }
      dataframe::utils::emplace(samples, "bond_dimension_at_site", bond_dimensions);
      dataframe::utils::emplace(samples, "mzr_freq", mzr_freq);
      mzr_freq = std::vector<double>(system_size);

      dataframe::utils::emplace(samples, "mxxr_freq", mxxr_freq);
      mxxr_freq = std::vector<double>(system_size);

      return samples;
    }
};
