#ifndef CLIFFORDSIM_H
#define CLIFFORDSIM_H

#include "Tableau.h"
#include "Simulator.hpp"
#include <random>
#include <deque>
#include <algorithm>
#include <assert.h>

enum CliffordType { CHP, GraphSim };

static CliffordType parse_clifford_type(std::string s) {
    if (s == "chp") return CliffordType::CHP;
    else if (s == "graph") return CliffordType::GraphSim;
    else {
        assert(false);
        return CliffordType::CHP;
    }
}


template <typename T>
static void remove_even_indices(std::vector<T> &v) {
    uint vlen = v.size();
    for (uint i = 0; i < vlen; i++) {
        uint j = vlen - i - 1;
        if (!(j % 2)) v.erase(v.begin() + j);
    }
}

class CliffordState: public Entropy {
    private:
        std::minstd_rand rng;


        void single_qubit_random_clifford(uint a, uint r) {
            if (r == 1) {
                x_gate(a);
            } else if (r == 2) {
                y_gate(a);
            } else if (r == 3) {
                z_gate(a);
            } else if (r == 4) {
                h_gate(a);
                s_gate(a);
                h_gate(a);
                s_gate(a);
            } else if (r == 5) {
                h_gate(a);
                s_gate(a);
                h_gate(a);
                s_gate(a);
                x_gate(a);
            } else if (r == 6) {
                h_gate(a);
                s_gate(a);
                h_gate(a);
                s_gate(a);
                y_gate(a);
            } else if (r == 7) {
                h_gate(a);
                s_gate(a);
                h_gate(a);
                s_gate(a);
                z_gate(a);
            } else if (r == 8) {
                h_gate(a);
                s_gate(a);
            } else if (r == 9) {
                h_gate(a);
                s_gate(a);
                x_gate(a);
            } else if (r == 10) {
                h_gate(a);
                s_gate(a);
                y_gate(a);
            } else if (r == 11) {
                h_gate(a);
                s_gate(a);
                z_gate(a);
            } else if (r == 12) {
                h_gate(a);
            } else if (r == 13) {
                h_gate(a);
                x_gate(a);
            } else if (r == 14) {
                h_gate(a);
                y_gate(a);
            } else if (r == 15) {
                h_gate(a);
                z_gate(a);
            } else if (r == 16) {
                s_gate(a);
                h_gate(a);
                s_gate(a);
            } else if (r == 17) {
                s_gate(a);
                h_gate(a);
                s_gate(a);
                x_gate(a);
            } else if (r == 18) {
                s_gate(a);
                h_gate(a);
                s_gate(a);
                y_gate(a);
            } else if (r == 19) {
                s_gate(a);
                h_gate(a);
                s_gate(a);
                z_gate(a);
            } else if (r == 20) {
                s_gate(a);
            } else if (r == 21) {
                s_gate(a);
                x_gate(a);
            } else if (r == 22) {
                s_gate(a);
                y_gate(a);
            } else if (r == 23) {
                s_gate(a);
                z_gate(a);
            }
        }

        // Performs an iteration of the random clifford algorithm outlined in https://arxiv.org/pdf/2008.06011.pdf
        void random_clifford_iteration(std::deque<uint> &qubits) {
            uint num_qubits = qubits.size();

            // If only acting on one qubit, can easily lookup from a table
            if (num_qubits == 1) {
                single_qubit_random_clifford(qubits[0], rand() % 24);
                return;
			}

            PauliString p1 = PauliString::rand(num_qubits, &rng);
            PauliString p2 = PauliString::rand(num_qubits, &rng);
            while (p1.commutes(p2)) {
                p2 = PauliString::rand(num_qubits, &rng);
            }

            Tableau tableau = Tableau(num_qubits, std::vector<PauliString>{p1, p2});

            // Step one
            for (uint i = 0; i < num_qubits; i++) {
                if (tableau.z(0, i)) {
                    if (tableau.x(0, i)) {
                        tableau.s_gate(i);
                        s_gate(qubits[i]);
                    } else {
                        tableau.h_gate(i);
                        h_gate(qubits[i]);
                    }
                }
            }

            // Step two
            std::vector<uint> nonzero_idx(0);
            for (uint i = 0; i < num_qubits; i++) {
                if (tableau.x(0, i)) {
                    nonzero_idx.push_back(i);
                }
            }

            while (nonzero_idx.size() > 1) {
                for (uint j = 0; j < nonzero_idx.size()/2; j++) {
                    uint q1 = nonzero_idx[2*j];
                    uint q2 = nonzero_idx[2*j+1];
                    tableau.cx_gate(q1, q2);
                    cx_gate(qubits[q1], nonzero_idx[q2]);
                }

                remove_even_indices(nonzero_idx);
            }

            // Step three
            uint ql = nonzero_idx[0];
            if (ql != 0) {
                for (uint i = 0; i < num_qubits; i++) {
                    if (tableau.x(0, i)) {
                        tableau.cx_gate(0, ql);
                        tableau.cx_gate(ql, 0);
                        tableau.cx_gate(0, ql);

                        cx_gate(qubits[0], qubits[ql]);
                        cx_gate(qubits[ql], qubits[0]);
                        cx_gate(qubits[0], qubits[ql]);

                        break;
                    }
                }
            }

            // Step four
            PauliString z1_p(num_qubits);
            z1_p.set_z(0, true);
            PauliString z1_m(num_qubits);
            z1_m.set_z(0, true);
            z1_m.set_r(true);

            if (tableau.rows[1] != z1_p && tableau.rows[1] != z1_m) {
                tableau.h_gate(0);
                h_gate(qubits[0]);


                // Repeat step one
                for (uint i = 0; i < num_qubits; i++) {
                    if (tableau.z(1, i)) {
                        if (tableau.x(1, i)) {
                            tableau.s_gate(i);
                            s_gate(qubits[i]);
                        } else {
                            tableau.h_gate(i);
                            h_gate(qubits[i]);
                        }
                    }
                }

                // ... and step two
                nonzero_idx = std::vector<uint>(0);
                for (uint i = 0; i < num_qubits; i++) {
                    if (tableau.x(1, i)) {
                        nonzero_idx.push_back(i);
                    }
                }

                while (nonzero_idx.size() > 1) {
                    for (uint j = 0; j < nonzero_idx.size()/2; j++) {
                        uint q1 = nonzero_idx[2*j];
                        uint q2 = nonzero_idx[2*j+1];
                        tableau.cx_gate(q1, q2);
                        cx_gate(qubits[q1], nonzero_idx[q2]);
                    }

                    remove_even_indices(nonzero_idx);
                }

                tableau.h_gate(0);
                h_gate(qubits[0]);
            }

            // Step five: correct phase
            if (tableau.r(0) && !tableau.r(1)) {
                y_gate(qubits[0]);
            } else if (!tableau.r(0) && tableau.r(1)) {
                x_gate(qubits[0]);
            } else {
                z_gate(qubits[0]);
            }
        }

    public:
        CliffordState() : rng(std::minstd_rand()) {}
        virtual uint system_size() const=0;
        uint rand() {
            return this->rng();
        }
        float randf() {
            return float(rand())/float(RAND_MAX);
        }

        virtual void h_gate(uint a)=0;
        virtual void s_gate(uint a)=0;

        virtual void x_gate(uint a) {
            h_gate(a);
            z_gate(a);
            h_gate(a);
        }
        virtual void y_gate(uint a) {
            x_gate(a);
            z_gate(a);
        }
        virtual void z_gate(uint a) {
            s_gate(a);
            s_gate(a);
        }

        virtual void cz_gate(uint a, uint b)=0;
        virtual void cx_gate(uint a, uint b) {
            h_gate(b);
            cz_gate(a, b);
            h_gate(b);
        }
        virtual void cy_gate(uint a, uint b) {
            s_gate(b);
            h_gate(b);
            cz_gate(a, b);
            h_gate(b);
            s_gate(b);
            s_gate(b);
            s_gate(b);
        }

        virtual bool mzr(uint a)=0;
        virtual bool mxr(uint a) {
            h_gate(a);
            bool outcome = mzr(a);
            h_gate(a);
            return outcome;
        }
        virtual bool myr(uint a) {
            s_gate(a);
            h_gate(a);
            bool outcome = mzr(a);
            h_gate(a);
            s_gate(a);
            s_gate(a);
            s_gate(a);
            return outcome;
        }

        void random_clifford(std::vector<uint> &qubits) {
            uint num_qubits = qubits.size();
            std::deque<uint> dqubits(num_qubits);
            std::copy(qubits.begin(), qubits.end(), dqubits.begin());
            for (uint i = 0; i < num_qubits; i++) {
                random_clifford_iteration(dqubits);
                dqubits.pop_front();
            }
        }
};

#endif