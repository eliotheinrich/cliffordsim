#pragma once

#include "QuantumCircuit.h"

#include <map>
#include <bitset>

#include <Eigen/Dense>
#include <itensor/all.h>

#define QS_ATOL 1e-8

namespace quantumstate_utils {
	static inline bool print_congruence(uint32_t z1, uint32_t z2, const std::vector<uint32_t>& pos, bool outcome) {
		std::bitset<8> b1(z1);
		std::bitset<8> b2(z2);
		if (outcome)
			std::cout << b1 << " and " << b2 << " are congruent at positions ";
		else
			std::cout << b1 << " and " << b2 << " are not congruent at positions ";

		for (auto p : pos) std::cout << p << " ";
		std::cout << "\n";

		return outcome;
	}

	static inline bool bits_congruent(uint32_t z1, uint32_t z2, const std::vector<uint32_t>& pos) {
		for (uint32_t j = 0; j < pos.size(); j++) {
			if (((z2 >> j) & 1) != ((z1 >> pos[j]) & 1))
				return false;
		}

		return true;
	}

	static inline uint32_t reduce_bits(uint32_t a, const std::vector<uint32_t>& v) {
		uint32_t b = 0;

		for (size_t i = 0; i < v.size(); i++) {
			// Get the ith bit of a
			int a_bit = (a >> v[i]) & 1;

			// Set the ith bit of b based on a_bit
			b |= (a_bit << i);
		}

		return b;
	}

	inline uint32_t set_bit(uint32_t b, uint32_t j, uint32_t a, uint32_t i) {
		uint32_t x = (a >> i) & 1u;
		return (b & ~(1u << j)) | (x << j);
	}


	static inline std::string print_binary(uint32_t a, uint32_t width=5) {
		std::string s = "";
		for (uint32_t i = 0; i < width; i++)
			s = std::to_string((a >> i) & 1u) + s;
		return s;
	}
}


class QuantumState {
	protected:
        std::minstd_rand rng;

        uint32_t rand() { return this->rng(); }
        double randf() { return double(rand())/double(RAND_MAX); }

	public:
		uint32_t num_qubits;
		uint32_t basis;

		QuantumState() = default;
		QuantumState(uint32_t num_qubits, int s=-1) : num_qubits(num_qubits), basis(1u << num_qubits) {
			if (s == -1)
				seed(std::rand());
			else
				seed(s);
		}

		void seed(int s) {
			rng.seed(s);
		}
		virtual std::string to_string() const=0;

		virtual double entropy(const std::vector<uint32_t> &qubits, uint32_t index) const=0;

		virtual void evolve(const Eigen::MatrixXcd& gate, const std::vector<uint32_t>& qbits)=0;
		virtual void evolve(const Eigen::MatrixXcd& gate) {
			std::vector<uint32_t> qbits(num_qubits);
			std::iota(qbits.begin(), qbits.end(), 0);
			evolve(gate, qbits);
		}
		virtual void evolve(const Eigen::Matrix2cd& gate, uint32_t q) { evolve(gate, {q}); }
		virtual void evolve(const Eigen::VectorXcd& gate, const std::vector<uint32_t>& qbits) { 
			evolve(static_cast<Eigen::MatrixXcd>(gate.asDiagonal()), qbits); 
		}
		virtual void evolve(const Eigen::VectorXcd& gate) { 
			evolve(static_cast<Eigen::MatrixXcd>(gate.asDiagonal())); 
		}
		virtual void evolve(const Measurement& measurement) {
			for (auto q : measurement.qbits)
				measure(q);
		}

		virtual void evolve(const Instruction& inst) {
			std::visit(quantumcircuit_utils::overloaded{
				[this](std::shared_ptr<Gate> gate) { evolve(gate->define(), gate->qbits); },
				[this](Measurement m) { 
					for (auto const &q : m.qbits)
						measure(q);
				},
			}, inst);
		}

		virtual void evolve(const QuantumCircuit& circuit) {
			if (circuit.num_params() > 0)
				throw std::invalid_argument("Unbound QuantumCircuit parameters; cannot evolve StateVector.");

			for (auto const &inst : circuit.instructions)
				evolve(inst);
		}

		virtual bool measure(uint32_t q)=0;
		virtual std::vector<bool> measure_all() {
			std::vector<bool> outcomes(num_qubits);
			for (uint32_t q = 0; q < num_qubits; q++)
				outcomes[q] = measure(q);
			return outcomes;
		}
};

class DensityMatrix;
class Statevector;
class UnitaryState;
class MatrixProductState;

class DensityMatrix : public QuantumState {
	public:
		Eigen::MatrixXcd data;

		DensityMatrix(uint32_t num_qubits);
		DensityMatrix(const Statevector& state);
		DensityMatrix(const QuantumCircuit& circuit);
		DensityMatrix(const DensityMatrix& rho);
		DensityMatrix(const Eigen::MatrixXcd& data);

		virtual std::string to_string() const override;

		DensityMatrix partial_trace(const std::vector<uint32_t>& traced_qubits) const;

		virtual double entropy(const std::vector<uint32_t> &qubits, uint32_t index) const override;

		virtual void evolve(const Eigen::MatrixXcd& gate) override;
		virtual void evolve(const Eigen::MatrixXcd& gate, const std::vector<uint32_t>& qbits) override;
		virtual void evolve(const Eigen::VectorXcd& gate, const std::vector<uint32_t>& qbits) override;
		virtual void evolve(const Eigen::VectorXcd& gate) override;

		virtual bool measure(uint32_t q) override;
		virtual std::vector<bool> measure_all() override;

		Eigen::VectorXd diagonal() const;
		std::map<uint32_t, double> probabilities() const;
};

class Statevector : public QuantumState {
	public:
		Eigen::VectorXcd data;

		Statevector(uint32_t num_qubits);
		Statevector(uint32_t num_qubits, uint32_t qregister);
		Statevector(const QuantumCircuit &circuit);
		Statevector(const Statevector& other);
		Statevector(const Eigen::VectorXcd& vec);
		Statevector(const MatrixProductState& state);

		virtual std::string to_string() const override;

		virtual double entropy(const std::vector<uint32_t> &qubits, uint32_t index) const override;

		virtual void evolve(const Eigen::MatrixXcd &gate, const std::vector<uint32_t> &qubits) override;
		virtual void evolve(const Eigen::MatrixXcd &gate) override;
		virtual void evolve(const Eigen::VectorXcd &gate, const std::vector<uint32_t> &qubits) override;
		virtual void evolve(const Eigen::VectorXcd &gate) override;

		double measure_probability(uint32_t q, bool outcome) const;
		virtual bool measure(uint32_t q) override;

		double norm() const;
		void normalize();
		double probabilities(uint32_t z, const std::vector<uint32_t>& qubits) const;
		std::complex<double> inner(const Statevector& other) const;

		Eigen::VectorXd svd(const std::vector<uint32_t>& qubits) const;
};

class UnitaryState : public QuantumState {
	public:
		Eigen::MatrixXcd unitary;

		UnitaryState(uint32_t num_qubits);

		std::string to_string() const;

		virtual double entropy(const std::vector<uint32_t> &sites, uint32_t index) const override;

		virtual void evolve(const Eigen::MatrixXcd &gate, const std::vector<uint32_t> &qubits);
		virtual void evolve(const Eigen::MatrixXcd &gate) override;

		virtual bool measure(uint32_t q) {
			throw std::invalid_argument("Cannot perform measurement on UnitaryState.");
		}

		void normalize();

		Statevector get_statevector() const;

		double probabilities(uint32_t z, const std::vector<uint32_t>& qubits) const;
};



itensor::ITensor tensor_slice(const itensor::ITensor& tensor, const itensor::Index& index, int i);
itensor::ITensor matrix_to_tensor(const Eigen::Matrix2cd& matrix, const itensor::Index i1, const itensor::Index i2);
itensor::ITensor matrix_to_tensor(const Eigen::Matrix4cd& matrix, const itensor::Index i1, const itensor::Index i2, const itensor::Index i3, const itensor::Index i4);
itensor::Index pad(itensor::ITensor& tensor, const itensor::Index& idx, uint32_t new_dim);

class MatrixProductState : public QuantumState {
	private:
		std::vector<itensor::ITensor> tensors;
		std::vector<itensor::ITensor> singular_values;
		std::vector<itensor::Index> external_indices;
		std::vector<itensor::Index> internal_indices;

		double sv_threshold;

		static Eigen::Matrix2cd zero_projector();
		static Eigen::Matrix2cd one_projector();

	public:
		uint32_t bond_dimension;

		MatrixProductState(uint32_t num_qubits, uint32_t bond_dimension, double sv_threshold=1e-4);

		virtual std::string to_string() const override;

		virtual double entropy(const std::vector<uint32_t>& qubits, uint32_t index) const override;
		double entropy(uint32_t q) const;

		void print_mps() const;

		itensor::ITensor coefficient_tensor() const;
		std::complex<double> coefficients(uint32_t z) const;
		Eigen::VectorXcd coefficients(const std::vector<uint32_t>& indices) const;
		Eigen::VectorXcd coefficients() const;

		virtual void evolve(const Eigen::Matrix2cd& gate, uint32_t qubit) override;
		virtual void evolve(const Eigen::MatrixXcd& gate, const std::vector<uint32_t>& qubits) override;

		double measure_probability(uint32_t q, bool outcome) const;
		virtual bool measure(uint32_t q) override;
		void measure_propagate(uint32_t q, const Eigen::Matrix2cd& proj);
};

