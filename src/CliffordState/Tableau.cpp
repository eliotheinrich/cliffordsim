#include "Tableau.h"
#include <assert.h>
#include <algorithm>
#include <iostream>

PauliString::PauliString(uint num_qubits) : num_qubits(num_qubits), bit_string(std::vector<bool>(2*num_qubits, false)), phase(false) {}

PauliString PauliString::rand(uint num_qubits, std::minstd_rand *r) {
    PauliString p(num_qubits);

    std::transform(p.bit_string.begin(), p.bit_string.end(), p.bit_string.begin(), [&r](bool) { return (*r)() % 2 == 0; });
    p.set_r((*r)() % 2 == 0);

	// Need to check that at least one bit is nonzero so that p is not the identity
	for (uint j = 0; j < 2*num_qubits; j++) {
		if (p.bit_string[j]) {
			return p;
		}
	}

    return PauliString::rand(num_qubits, r);
}

PauliString PauliString::copy() {
	PauliString p(num_qubits);
	std::copy(bit_string.begin(), bit_string.end(), p.bit_string.begin());
	p.set_r(r());
	return p;
}

std::string PauliString::to_op(uint i) const {
    bool xi = x(i); 
    bool zi = z(i);

    if (xi && zi) {
        return "Y";
    } else if (!xi && zi) {
        return "Z";
    } else if (xi && !zi) {
        return "X";
    } else {
        return "I";
    }
}

std::string PauliString::to_string(bool to_ops) const {
    if (to_ops) {
        std::string s = "[";
        if (phase) { s += "-"; } else { s += "+"; }
        for (uint i = 0; i < num_qubits; i++) {
            s += to_op(i);
        }
        s += "]";
        return s;
    } else {
        std::string s = "[";
        for (uint i = 0; i < 2*num_qubits; i++) {
            if (bit_string[i]) { s += "1"; } else { s += "0"; }
        }
        s += " | ";

        if (phase) { s += "1 ]"; } else { s += "0 ]"; }

        return s;
    }
}

bool PauliString::commutes_at(PauliString &p, uint i) const {
    if (x(i) == p.x(i) && z(i) == p.z(i)) { return true; } // operators are identical
    else if (!x(i) && !z(i)) { return true; } // this is identity
    else if (!p.x(i) && p.z(i)) { return true; } // other is identity
    else { return false; }
}

bool PauliString::commutes(PauliString &p) const {
    assert(num_qubits == p.num_qubits);
    uint anticommuting_indices = 0u;
    for (uint i = 0; i < num_qubits; i++) {
        if (!commutes_at(p, i)) {
            anticommuting_indices++;
        }
    }

    return anticommuting_indices % 2 == 0;
}

bool PauliString::operator==(const PauliString &rhs) {
	if (num_qubits != rhs.num_qubits) { return false; }
	if (r() != rhs.r()) { return false; }
	
	for (uint i = 0; i < num_qubits; i++) {
		if (x(i) != rhs.x(i)) { return false; }
		if (z(i) != rhs.z(i)) { return false; }
	}

	return true;
}

Tableau::Tableau(uint num_qubits) : num_qubits(num_qubits), 
                                    track_destabilizers(true),
                                    print_ops(false) {
    rows = std::vector<PauliString>(2*num_qubits + 1, PauliString(num_qubits));
    for (uint i = 0; i < num_qubits; i++) {
        rows[i].set_x(i, true);
        rows[i + num_qubits].set_z(i, true);
    }
}

Tableau::Tableau(uint num_qubits, std::vector<PauliString> rows) : num_qubits(num_qubits),
                                                                   rows(rows),
                                                                   track_destabilizers(false),
                                                                   print_ops(false) {};

std::string Tableau::to_string() const {
    std::string s = "";
    for (uint i = 0; i < num_rows(); i++) {
        if (i == 0) { s += "["; } else { s += " "; }
        s += rows[i].to_string(print_ops);
        if (i == 2*rows.size()) { s += "]"; } else { s += "\n"; }
    }
    return s;
}

int Tableau::g(bool x1, bool z1, bool x2, bool z2) {
    if (!x1 && !z1) { return 0; }
    else if (x1 && z2) {
        if (z2) { if (x2) { return 0; } else { return 1; }}
        else { if (x2) { return -1; } else { return 0; }}
    } else if (x1 && !z1) {
        if (z2) { if (x2) { return 1; } else { return -1; }}
        else { return 0; }
    } else {
        if (x2) { if (z2) { return -1; } else { return 1; }}
        else { return 0; }
    }
}

void Tableau::rowsum(uint h, uint i) {
    assert(track_destabilizers);
    int s = 0;
    if (r(i)) { s += 2; }
    if (r(h)) { s += 2; }

    for (uint j = 0; j < num_qubits; j++) {
        s += Tableau::g(x(i,j), z(i,j), x(h,j), z(h,j));
    }

    if (s % 4 == 0) {
        set_r(h, false);
    } else if (std::abs(s % 4) == 2) {
        set_r(h, true);
    }

    for (uint j = 0; j < num_qubits; j++) {
        set_x(h, j, x(i,j) != x(h,j));
        set_z(h, j, z(i,j) != z(h,j));
    }
}

void Tableau::h_gate(uint a) {
    for (uint i = 0; i < num_rows(); i++) {
        bool xia = x(i, a);
        bool zia = z(i, a);
        bool ri = r(i);

        set_r(i, ri != (xia && zia));
        set_x(i, a, zia);
        set_z(i, a, xia);
    }

}

void Tableau::s_gate(uint a) {
    for (uint i = 0; i < num_rows(); i++) {
        bool xia = x(i, a);
        bool zia = z(i, a);
        bool ri = r(i);

        set_r(i, ri != (xia && zia));
        set_z(i, a, xia != zia);
    }
}

void Tableau::cx_gate(uint a, uint b) {
    for (uint i = 0; i < num_rows(); i++) {
        bool xia = x(i, a);
        bool zia = z(i, a);
        bool xib = x(i, b);
        bool zib = z(i, b);
        bool ri = r(i);

        set_r(i, ri != ((xia && zib) && ((xib != zia) != true)));
        set_x(i, b, xia != xib);
        set_z(i, a, zia != zib);
    }
}

std::pair<bool, uint> Tableau::mzr_deterministic(uint a) {
    assert(track_destabilizers);

    for (uint i = num_qubits; i < 2*num_qubits; i++) {
        if (x(i, a)) { return std::pair(true, i); }
    }

    return std::pair(false, 0);
}

bool Tableau::mzr(uint a, bool outcome) {
	assert(track_destabilizers);

    std::pair<bool, uint> result_deterministic = mzr_deterministic(a);
    bool found_p = result_deterministic.first;
    uint p = result_deterministic.second;

    if (found_p) {
        for (uint i = 0; i < 2*num_qubits; i++) {
            if (i != p && x(i, a)) {
                rowsum(i, p);
            }
        }

        // TODO check that copy is happening, not passing reference
        rows[p - num_qubits] = rows[p];
        rows[p] = PauliString(num_qubits);

		if (outcome) {
			set_r(p, true);
		}
		set_z(p, a, true);

        return outcome;
    } else {
        rows[2*num_qubits] = PauliString(num_qubits);
        for (uint i = 0; i < num_qubits; i++) {
            rowsum(2*num_qubits, i + num_qubits);
        }

        return r(2*num_qubits);
    }
}
