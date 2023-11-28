#include "QuantumGraphState.h"
#include <climits>

		
const uint32_t QuantumGraphState::ZGATES[4] = {IDGATE, ZGATE, SGATE, SDGATE};

const uint32_t QuantumGraphState::CONJUGATION_TABLE[24] = {3, 6, 6, 3, 1, 1, 4, 4, 5, 2, 5, 2, 1, 1, 4, 4, 5, 2, 5, 2, 3, 6, 6, 3};

// TODO check
const uint32_t QuantumGraphState::HERMITIAN_CONJUGATE_TABLE[24] = {0, 1, 2, 3, 8, 11, 9, 10, 4, 6, 7, 5, 12, 15, 14, 13, 17, 16, 18, 19, 23, 21, 22, 20};

const uint32_t QuantumGraphState::CLIFFORD_DECOMPS[24][5] =
   {{IDGATE,    IDGATE,    IDGATE,    IDGATE,    IDGATE   },
	{SQRTXGATE, SQRTXGATE, IDGATE,    IDGATE,    IDGATE   },
	{SQRTZGATE, SQRTZGATE, SQRTXGATE, SQRTXGATE, IDGATE   },
	{SQRTZGATE, SQRTZGATE, IDGATE,    IDGATE,    IDGATE   },
	{SQRTZGATE, SQRTZGATE, SQRTZGATE, SQRTXGATE, IDGATE   },
	{SQRTZGATE, SQRTXGATE, SQRTXGATE, SQRTXGATE, IDGATE   },
	{SQRTZGATE, SQRTXGATE, SQRTZGATE, SQRTZGATE, IDGATE   },
	{SQRTZGATE, SQRTXGATE, IDGATE,    IDGATE,    IDGATE   },
	{SQRTXGATE, SQRTXGATE, SQRTXGATE, SQRTZGATE, IDGATE   },
	{SQRTXGATE, SQRTZGATE, IDGATE,    IDGATE,    IDGATE   },
	{SQRTZGATE, SQRTXGATE, SQRTZGATE, SQRTXGATE, IDGATE   },
	{SQRTXGATE, SQRTZGATE, SQRTZGATE, SQRTZGATE, IDGATE   },
	{SQRTXGATE, SQRTZGATE, SQRTZGATE, SQRTZGATE, SQRTXGATE},
	{SQRTXGATE, SQRTZGATE, SQRTXGATE, SQRTXGATE, SQRTXGATE},
	{SQRTZGATE, SQRTXGATE, SQRTZGATE, IDGATE,    IDGATE   },
	{SQRTZGATE, SQRTXGATE, SQRTZGATE, SQRTZGATE, SQRTZGATE},
	{SQRTXGATE, SQRTXGATE, SQRTXGATE, IDGATE,    IDGATE   },
	{SQRTXGATE, IDGATE,    IDGATE,    IDGATE,    IDGATE   },
	{SQRTZGATE, SQRTZGATE, SQRTXGATE, IDGATE,    IDGATE   },
	{SQRTZGATE, SQRTZGATE, SQRTXGATE, SQRTXGATE, SQRTXGATE},
	{SQRTZGATE, SQRTZGATE, SQRTZGATE, IDGATE,    IDGATE   },
	{SQRTXGATE, SQRTXGATE, SQRTZGATE, SQRTZGATE, SQRTZGATE},
	{SQRTXGATE, SQRTXGATE, SQRTZGATE, IDGATE,    IDGATE   },
	{SQRTZGATE, IDGATE,    IDGATE,    IDGATE,    IDGATE   }};

const uint32_t QuantumGraphState::CLIFFORD_PRODUCTS[24][24] = 
   {{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
	{ 1,  0,  3,  2,  6,  7,  4,  5, 11, 10,  9,  8, 15, 14, 13, 12, 17, 16, 19, 18, 22, 23, 20, 21},
	{ 2,  3,  0,  1,  7,  6,  5,  4,  9,  8, 11, 10, 14, 15, 12, 13, 19, 18, 17, 16, 21, 20, 23, 22},
	{ 3,  2,  1,  0,  5,  4,  7,  6, 10, 11,  8,  9, 13, 12, 15, 14, 18, 19, 16, 17, 23, 22, 21, 20},
	{ 4,  5,  6,  7,  8,  9, 10, 11,  0,  1,  2,  3, 23, 22, 21, 20, 13, 12, 15, 14, 18, 19, 16, 17},
	{ 5,  4,  7,  6, 10, 11,  8,  9,  3,  2,  1,  0, 20, 21, 22, 23, 12, 13, 14, 15, 16, 17, 18, 19},
	{ 6,  7,  4,  5, 11, 10,  9,  8,  1,  0,  3,  2, 21, 20, 23, 22, 14, 15, 12, 13, 19, 18, 17, 16},
	{ 7,  6,  5,  4,  9,  8, 11, 10,  2,  3,  0,  1, 22, 23, 20, 21, 15, 14, 13, 12, 17, 16, 19, 18},
	{ 8,  9, 10, 11,  0,  1,  2,  3,  4,  5,  6,  7, 17, 16, 19, 18, 22, 23, 20, 21, 15, 14, 13, 12},
	{ 9,  8, 11, 10,  2,  3,  0,  1,  7,  6,  5,  4, 18, 19, 16, 17, 23, 22, 21, 20, 13, 12, 15, 14},
	{10, 11,  8,  9,  3,  2,  1,  0,  5,  4,  7,  6, 19, 18, 17, 16, 21, 20, 23, 22, 14, 15, 12, 13},
	{11, 10,  9,  8,  1,  0,  3,  2,  6,  7,  4,  5, 16, 17, 18, 19, 20, 21, 22, 23, 12, 13, 14, 15},
	{12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,  0,  1,  2,  3,  4,  5, 6,  7,  8,  9,  10, 11},
	{13, 12, 15, 14, 18, 19, 16, 17, 23, 22, 21, 20,  3,  2,  1,  0,  5,  4, 7,  6,  10, 11,  8,  9},
	{14, 15, 12, 13, 19, 18, 17, 16, 21, 20, 23, 22,  2,  3,  0,  1,  7,  6, 5,  4,   9,  8, 11, 10},
	{15, 14, 13, 12, 17, 16, 19, 18, 22, 23, 20, 21,  1,  0,  3,  2,  6,  7, 4,  5,  11, 10,  9,  8},
	{16, 17, 18, 19, 20, 21, 22, 23, 12, 13, 14, 15, 11, 10,  9,  8,  1,  0, 3,  2,   6,  7,  4,  5},
	{17, 16, 19, 18, 22, 23, 20, 21, 15, 14, 13, 12,  8,  9, 10, 11,  0,  1, 2,  3,   4,  5,  6,  7},
	{18, 19, 16, 17, 23, 22, 21, 20, 13, 12, 15, 14,  9,  8, 11, 10,  2,  3, 0,  1,   7,  6,  5,  4},
	{19, 18, 17, 16, 21, 20, 23, 22, 14, 15, 12, 13, 10, 11,  8,  9,  3,  2, 1,  0,   5,  4,  7,  6},
	{20, 21, 22, 23, 12, 13, 14, 15, 16, 17, 18, 19,  5,  4,  7,  6, 10, 11, 8,  9,   3,  2,  1,  0},
	{21, 20, 23, 22, 14, 15, 12, 13, 19, 18, 17, 16,  6,  7,  4,  5, 11, 10, 9,  8,   1,  0,  3,  2},
	{22, 23, 20, 21, 15, 14, 13, 12, 17, 16, 19, 18,  7,  6,  5,  4,  9,  8, 11, 10,  2,  3,  0,  1},
	{23, 22, 21, 20, 13, 12, 15, 14, 18, 19, 16, 17,  4,  5,  6,  7,  8,  9, 10, 11,  0,  1,  2,  3}};

const uint32_t QuantumGraphState::CZ_LOOKUP[24][24][2][3] = 
  {{{{1, 0, 0}, {0, 0, 0}}, {{1, 0, 0}, {0, 3, 0}}, {{1, 0, 3}, {0, 3, 2}}, {{1, 0, 3}, {0, 0, 3}},
	{{0, 0, 4}, {1, 0, 5}}, {{0, 0, 4}, {1, 0, 4}}, {{0, 3, 6}, {1, 0, 6}}, {{0, 3, 6}, {1, 0, 7}},
	{{1, 0, 23}, {0, 23, 8}}, {{1, 0, 23}, {0, 20, 8}}, {{1, 0, 20}, {0, 23, 10}}, {{1, 0, 20}, {0, 20, 10}},
	{{0, 0, 4}, {1, 0, 13}}, {{0, 0, 4}, {1, 0, 12}}, {{0, 3, 6}, {1, 0, 14}}, {{0, 3, 6}, {1, 0, 15}},
	{{1, 0, 0}, {0, 23, 0}}, {{1, 0, 0}, {0, 20, 0}}, {{1, 0, 3}, {0, 23, 2}}, {{1, 0, 3}, {0, 20, 2}},
	{{1, 0, 20}, {0, 0, 20}}, {{1, 0, 20}, {0, 3, 10}}, {{1, 0, 23}, {0, 3, 8}}, {{1, 0, 23}, {0, 0, 23}}},
   {{{1, 0, 0}, {0, 0, 3}}, {{1, 0, 0}, {0, 2, 2}}, {{1, 0, 3}, {0, 2, 0}}, {{1, 0, 3}, {0, 0, 0}},
	{{0, 0, 4}, {1, 0, 7}}, {{0, 0, 4}, {1, 0, 6}}, {{0, 2, 6}, {1, 0, 4}}, {{0, 2, 6}, {1, 0, 5}},
	{{1, 0, 23}, {0, 10, 10}}, {{1, 0, 23}, {0, 8, 10}}, {{1, 0, 20}, {0, 10, 8}}, {{1, 0, 20}, {0, 8, 8}},
	{{0, 0, 4}, {1, 0, 15}}, {{0, 0, 4}, {1, 0, 14}}, {{0, 2, 6}, {1, 0, 12}}, {{0, 2, 6}, {1, 0, 13}},
	{{1, 0, 0}, {0, 10, 2}}, {{1, 0, 0}, {0, 8, 2}}, {{1, 0, 3}, {0, 10, 0}}, {{1, 0, 3}, {0, 8, 0}},
	{{1, 0, 20}, {0, 0, 23}}, {{1, 0, 20}, {0, 2, 8}}, {{1, 0, 23}, {0, 2, 10}}, {{1, 0, 23}, {0, 0, 20}}},
   {{{1, 2, 3}, {0, 2, 3}}, {{1, 0, 1}, {0, 0, 2}}, {{1, 0, 2}, {0, 0, 0}}, {{1, 2, 0}, {0, 2, 0}},
	{{0, 2, 4}, {1, 0, 6}}, {{0, 2, 4}, {1, 0, 7}}, {{0, 0, 6}, {1, 0, 5}}, {{0, 0, 6}, {1, 0, 4}},
	{{1, 0, 22}, {0, 8, 10}}, {{1, 0, 22}, {0, 10, 10}}, {{1, 0, 21}, {0, 8, 8}}, {{1, 0, 21}, {0, 10, 8}},
	{{0, 2, 4}, {1, 0, 14}}, {{0, 2, 4}, {1, 0, 15}}, {{0, 0, 6}, {1, 0, 13}}, {{0, 0, 6}, {1, 0, 12}},
	{{1, 0, 1}, {0, 8, 2}}, {{1, 0, 1}, {0, 10, 2}}, {{1, 0, 2}, {0, 8, 0}}, {{1, 0, 2}, {0, 10, 0}},
	{{1, 2, 23}, {0, 2, 23}}, {{1, 0, 21}, {0, 0, 8}}, {{1, 0, 22}, {0, 0, 10}}, {{1, 2, 20}, {0, 2, 20}}},
   {{{1, 3, 0}, {0, 3, 0}}, {{1, 0, 1}, {0, 0, 0}}, {{1, 0, 2}, {0, 0, 2}}, {{1, 3, 3}, {0, 3, 3}},
	{{0, 3, 4}, {1, 0, 4}}, {{0, 3, 4}, {1, 0, 5}}, {{0, 0, 6}, {1, 0, 7}}, {{0, 0, 6}, {1, 0, 6}},
	{{1, 0, 22}, {0, 20, 8}}, {{1, 0, 22}, {0, 23, 8}}, {{1, 0, 21}, {0, 20, 10}}, {{1, 0, 21}, {0, 23, 10}},
	{{0, 3, 4}, {1, 0, 12}}, {{0, 3, 4}, {1, 0, 13}}, {{0, 0, 6}, {1, 0, 15}}, {{0, 0, 6}, {1, 0, 14}},
	{{1, 0, 1}, {0, 20, 0}}, {{1, 0, 1}, {0, 23, 0}}, {{1, 0, 2}, {0, 20, 2}}, {{1, 0, 2}, {0, 23, 2}},
	{{1, 3, 20}, {0, 3, 20}}, {{1, 0, 21}, {0, 0, 10}}, {{1, 0, 22}, {0, 0, 8}}, {{1, 3, 23}, {0, 3, 23}}},
   {{{0, 4, 0}, {1, 4, 3}}, {{0, 4, 0}, {1, 0, 6}}, {{0, 4, 2}, {1, 0, 7}}, {{0, 4, 3}, {1, 4, 0}},
	{{0, 4, 4}, {0, 8, 8}}, {{0, 4, 4}, {0, 8, 10}}, {{0, 4, 6}, {0, 10, 10}}, {{0, 4, 6}, {0, 10, 8}},
	{{0, 4, 8}, {0, 0, 0}}, {{0, 4, 8}, {0, 2, 2}}, {{0, 4, 10}, {0, 0, 2}}, {{0, 4, 10}, {0, 2, 0}},
	{{0, 4, 4}, {0, 8, 0}}, {{0, 4, 4}, {0, 8, 2}}, {{0, 4, 6}, {0, 10, 2}}, {{0, 4, 6}, {0, 10, 0}},
	{{0, 4, 0}, {0, 0, 10}}, {{0, 4, 0}, {0, 2, 8}}, {{0, 4, 2}, {0, 0, 8}}, {{0, 4, 2}, {0, 2, 10}},
	{{0, 4, 20}, {1, 4, 23}}, {{0, 4, 10}, {1, 0, 14}}, {{0, 4, 8}, {1, 0, 15}}, {{0, 4, 23}, {1, 4, 20}}},
   {{{0, 4, 0}, {1, 4, 0}}, {{0, 4, 0}, {1, 0, 7}}, {{0, 4, 2}, {1, 0, 6}}, {{0, 4, 3}, {1, 4, 3}},
	{{0, 4, 4}, {0, 10, 8}}, {{0, 4, 4}, {0, 10, 10}}, {{0, 4, 6}, {0, 8, 10}}, {{0, 4, 6}, {0, 8, 8}},
	{{0, 4, 8}, {0, 2, 0}}, {{0, 4, 8}, {0, 0, 2}}, {{0, 4, 10}, {0, 2, 2}}, {{0, 4, 10}, {0, 0, 0}},
	{{0, 4, 4}, {0, 10, 0}}, {{0, 4, 4}, {0, 10, 2}}, {{0, 4, 6}, {0, 8, 2}}, {{0, 4, 6}, {0, 8, 0}},
	{{0, 4, 0}, {0, 2, 10}}, {{0, 4, 0}, {0, 0, 8}}, {{0, 4, 2}, {0, 2, 8}}, {{0, 4, 2}, {0, 0, 10}},
	{{0, 4, 20}, {1, 4, 20}}, {{0, 4, 10}, {1, 0, 15}}, {{0, 4, 8}, {1, 0, 14}}, {{0, 4, 23}, {1, 4, 23}}},
   {{{0, 6, 3}, {1, 6, 0}}, {{0, 6, 2}, {1, 0, 4}}, {{0, 6, 0}, {1, 0, 5}}, {{0, 6, 0}, {1, 6, 3}},
	{{0, 6, 4}, {0, 10, 10}}, {{0, 6, 4}, {0, 10, 8}}, {{0, 6, 6}, {0, 8, 8}}, {{0, 6, 6}, {0, 8, 10}},
	{{0, 6, 10}, {0, 0, 2}}, {{0, 6, 10}, {0, 2, 0}}, {{0, 6, 8}, {0, 0, 0}}, {{0, 6, 8}, {0, 2, 2}},
	{{0, 6, 4}, {0, 10, 2}}, {{0, 6, 4}, {0, 10, 0}}, {{0, 6, 6}, {0, 8, 0}}, {{0, 6, 6}, {0, 8, 2}},
	{{0, 6, 2}, {0, 0, 8}}, {{0, 6, 2}, {0, 2, 10}}, {{0, 6, 0}, {0, 0, 10}}, {{0, 6, 0}, {0, 2, 8}},
	{{0, 6, 23}, {1, 6, 20}}, {{0, 6, 8}, {1, 0, 12}}, {{0, 6, 10}, {1, 0, 13}}, {{0, 6, 20}, {1, 6, 23}}},
   {{{0, 6, 3}, {1, 6, 3}}, {{0, 6, 2}, {1, 0, 5}}, {{0, 6, 0}, {1, 0, 4}}, {{0, 6, 0}, {1, 6, 0}},
	{{0, 6, 4}, {0, 8, 10}}, {{0, 6, 4}, {0, 8, 8}}, {{0, 6, 6}, {0, 10, 8}}, {{0, 6, 6}, {0, 10, 10}},
	{{0, 6, 10}, {0, 2, 2}}, {{0, 6, 10}, {0, 0, 0}}, {{0, 6, 8}, {0, 2, 0}}, {{0, 6, 8}, {0, 0, 2}},
	{{0, 6, 4}, {0, 8, 2}}, {{0, 6, 4}, {0, 8, 0}}, {{0, 6, 6}, {0, 10, 0}}, {{0, 6, 6}, {0, 10, 2}},
	{{0, 6, 2}, {0, 2, 8}}, {{0, 6, 2}, {0, 0, 10}}, {{0, 6, 0}, {0, 2, 10}}, {{0, 6, 0}, {0, 0, 8}},
	{{0, 6, 23}, {1, 6, 23}}, {{0, 6, 8}, {1, 0, 13}}, {{0, 6, 10}, {1, 0, 12}}, {{0, 6, 20}, {1, 6, 20}}},
   {{{1, 8, 20}, {0, 8, 23}}, {{1, 0, 16}, {0, 10, 10}}, {{1, 0, 18}, {0, 10, 8}}, {{1, 8, 23}, {0, 8, 20}},
	{{0, 8, 4}, {0, 0, 0}}, {{0, 8, 4}, {0, 0, 2}}, {{0, 10, 6}, {0, 2, 0}}, {{0, 10, 6}, {0, 2, 2}},
	{{1, 0, 8}, {1, 0, 5}}, {{1, 0, 8}, {1, 0, 7}}, {{1, 0, 10}, {1, 0, 4}}, {{1, 0, 10}, {1, 0, 6}},
	{{0, 8, 4}, {0, 0, 10}}, {{0, 8, 4}, {0, 0, 8}}, {{0, 10, 6}, {0, 2, 10}}, {{0, 10, 6}, {0, 2, 8}},
	{{1, 0, 16}, {1, 0, 13}}, {{1, 0, 16}, {1, 0, 15}}, {{1, 0, 18}, {1, 0, 12}}, {{1, 0, 18}, {1, 0, 14}},
	{{1, 8, 3}, {0, 8, 0}}, {{1, 0, 10}, {0, 10, 2}}, {{1, 0, 8}, {0, 10, 0}}, {{1, 8, 0}, {0, 8, 3}}},
   {{{1, 8, 20}, {0, 8, 20}}, {{1, 0, 16}, {0, 10, 8}}, {{1, 0, 18}, {0, 10, 10}}, {{1, 8, 23}, {0, 8, 23}},
	{{0, 8, 4}, {0, 2, 2}}, {{0, 8, 4}, {0, 2, 0}}, {{0, 10, 6}, {0, 0, 2}}, {{0, 10, 6}, {0, 0, 0}},
	{{1, 0, 8}, {1, 0, 6}}, {{1, 0, 8}, {1, 0, 4}}, {{1, 0, 10}, {1, 0, 7}}, {{1, 0, 10}, {1, 0, 5}},
	{{0, 8, 4}, {0, 2, 8}}, {{0, 8, 4}, {0, 2, 10}}, {{0, 10, 6}, {0, 0, 8}}, {{0, 10, 6}, {0, 0, 10}},
	{{1, 0, 16}, {1, 0, 14}}, {{1, 0, 16}, {1, 0, 12}}, {{1, 0, 18}, {1, 0, 15}}, {{1, 0, 18}, {1, 0, 13}},
	{{1, 8, 3}, {0, 8, 3}}, {{1, 0, 10}, {0, 10, 0}}, {{1, 0, 8}, {0, 10, 2}}, {{1, 8, 0}, {0, 8, 0}}},
   {{{1, 10, 20}, {0, 10, 23}}, {{1, 0, 17}, {0, 8, 10}}, {{1, 0, 19}, {0, 8, 8}}, {{1, 10, 23}, {0, 10, 20}},
	{{0, 10, 4}, {0, 2, 0}}, {{0, 10, 4}, {0, 2, 2}}, {{0, 8, 6}, {0, 0, 0}}, {{0, 8, 6}, {0, 0, 2}},
	{{1, 0, 9}, {1, 0, 4}}, {{1, 0, 9}, {1, 0, 6}}, {{1, 0, 11}, {1, 0, 5}}, {{1, 0, 11}, {1, 0, 7}},
	{{0, 10, 4}, {0, 2, 10}}, {{0, 10, 4}, {0, 2, 8}}, {{0, 8, 6}, {0, 0, 10}}, {{0, 8, 6}, {0, 0, 8}},
	{{1, 0, 17}, {1, 0, 12}}, {{1, 0, 17}, {1, 0, 14}}, {{1, 0, 19}, {1, 0, 13}}, {{1, 0, 19}, {1, 0, 15}},
	{{1, 10, 3}, {0, 10, 0}}, {{1, 0, 11}, {0, 8, 2}}, {{1, 0, 9}, {0, 8, 0}}, {{1, 10, 0}, {0, 10, 3}}},
   {{{1, 10, 20}, {0, 10, 20}}, {{1, 0, 17}, {0, 8, 8}}, {{1, 0, 19}, {0, 8, 10}}, {{1, 10, 23}, {0, 10, 23}},
	{{0, 10, 4}, {0, 0, 2}}, {{0, 10, 4}, {0, 0, 0}}, {{0, 8, 6}, {0, 2, 2}}, {{0, 8, 6}, {0, 2, 0}},
	{{1, 0, 9}, {1, 0, 7}}, {{1, 0, 9}, {1, 0, 5}}, {{1, 0, 11}, {1, 0, 6}}, {{1, 0, 11}, {1, 0, 4}},
	{{0, 10, 4}, {0, 0, 8}}, {{0, 10, 4}, {0, 0, 10}}, {{0, 8, 6}, {0, 2, 8}}, {{0, 8, 6}, {0, 2, 10}},
	{{1, 0, 17}, {1, 0, 15}}, {{1, 0, 17}, {1, 0, 13}}, {{1, 0, 19}, {1, 0, 14}}, {{1, 0, 19}, {1, 0, 12}},
	{{1, 10, 3}, {0, 10, 3}}, {{1, 0, 11}, {0, 8, 0}}, {{1, 0, 9}, {0, 8, 2}}, {{1, 10, 0}, {0, 10, 0}}},
   {{{0, 4, 0}, {1, 4, 23}}, {{0, 4, 0}, {1, 0, 15}}, {{0, 4, 2}, {1, 0, 14}}, {{0, 4, 3}, {1, 4, 20}},
	{{0, 4, 4}, {0, 0, 8}}, {{0, 4, 4}, {0, 0, 10}}, {{0, 4, 6}, {0, 2, 10}}, {{0, 4, 6}, {0, 2, 8}},
	{{0, 4, 8}, {0, 10, 0}}, {{0, 4, 8}, {0, 8, 2}}, {{0, 4, 10}, {0, 10, 2}}, {{0, 4, 10}, {0, 8, 0}},
	{{0, 4, 4}, {0, 0, 0}}, {{0, 4, 4}, {0, 0, 2}}, {{0, 4, 6}, {0, 2, 2}}, {{0, 4, 6}, {0, 2, 0}},
	{{0, 4, 0}, {0, 10, 10}}, {{0, 4, 0}, {0, 8, 8}}, {{0, 4, 2}, {0, 10, 8}}, {{0, 4, 2}, {0, 8, 10}},
	{{0, 4, 20}, {1, 4, 0}}, {{0, 4, 10}, {1, 0, 6}}, {{0, 4, 8}, {1, 0, 7}}, {{0, 4, 23}, {1, 4, 3}}},
   {{{0, 4, 0}, {1, 4, 20}}, {{0, 4, 0}, {1, 0, 14}}, {{0, 4, 2}, {1, 0, 15}}, {{0, 4, 3}, {1, 4, 23}},
	{{0, 4, 4}, {0, 2, 8}}, {{0, 4, 4}, {0, 2, 10}}, {{0, 4, 6}, {0, 0, 10}}, {{0, 4, 6}, {0, 0, 8}},
	{{0, 4, 8}, {0, 8, 0}}, {{0, 4, 8}, {0, 10, 2}}, {{0, 4, 10}, {0, 8, 2}}, {{0, 4, 10}, {0, 10, 0}},
	{{0, 4, 4}, {0, 2, 0}}, {{0, 4, 4}, {0, 2, 2}}, {{0, 4, 6}, {0, 0, 2}}, {{0, 4, 6}, {0, 0, 0}},
	{{0, 4, 0}, {0, 8, 10}}, {{0, 4, 0}, {0, 10, 8}}, {{0, 4, 2}, {0, 8, 8}}, {{0, 4, 2}, {0, 10, 10}}, 
	{{0, 4, 20}, {1, 4, 3}}, {{0, 4, 10}, {1, 0, 7}}, {{0, 4, 8}, {1, 0, 6}}, {{0, 4, 23}, {1, 4, 0}}},
   {{{0, 6, 3}, {1, 6, 23}}, {{0, 6, 2}, {1, 0, 12}}, {{0, 6, 0}, {1, 0, 13}}, {{0, 6, 0}, {1, 6, 20}},
	{{0, 6, 4}, {0, 2, 10}}, {{0, 6, 4}, {0, 2, 8}}, {{0, 6, 6}, {0, 0, 8}}, {{0, 6, 6}, {0, 0, 10}},
	{{0, 6, 10}, {0, 10, 2}}, {{0, 6, 10}, {0, 8, 0}}, {{0, 6, 8}, {0, 10, 0}}, {{0, 6, 8}, {0, 8, 2}},
	{{0, 6, 4}, {0, 2, 2}}, {{0, 6, 4}, {0, 2, 0}}, {{0, 6, 6}, {0, 0, 0}}, {{0, 6, 6}, {0, 0, 2}},
	{{0, 6, 2}, {0, 10, 8}}, {{0, 6, 2}, {0, 8, 10}}, {{0, 6, 0}, {0, 10, 10}}, {{0, 6, 0}, {0, 8, 8}},
	{{0, 6, 23}, {1, 6, 0}}, {{0, 6, 8}, {1, 0, 5}}, {{0, 6, 10}, {1, 0, 4}}, {{0, 6, 20}, {1, 6, 3}}},
   {{{0, 6, 3}, {1, 6, 20}}, {{0, 6, 2}, {1, 0, 13}}, {{0, 6, 0}, {1, 0, 12}}, {{0, 6, 0}, {1, 6, 23}},
	{{0, 6, 4}, {0, 0, 10}}, {{0, 6, 4}, {0, 0, 8}}, {{0, 6, 6}, {0, 2, 8}}, {{0, 6, 6}, {0, 2, 10}},
	{{0, 6, 10}, {0, 8, 2}}, {{0, 6, 10}, {0, 10, 0}}, {{0, 6, 8}, {0, 8, 0}}, {{0, 6, 8}, {0, 10, 2}},
	{{0, 6, 4}, {0, 0, 2}}, {{0, 6, 4}, {0, 0, 0}}, {{0, 6, 6}, {0, 2, 0}}, {{0, 6, 6}, {0, 2, 2}},
	{{0, 6, 2}, {0, 8, 8}}, {{0, 6, 2}, {0, 10, 10}}, {{0, 6, 0}, {0, 8, 10}}, {{0, 6, 0}, {0, 10, 8}},
	{{0, 6, 23}, {1, 6, 3}}, {{0, 6, 8}, {1, 0, 4}}, {{0, 6, 10}, {1, 0, 5}}, {{0, 6, 20}, {1, 6, 0}}},
   {{{1, 0, 0}, {0, 0, 23}}, {{1, 0, 0}, {0, 2, 10}}, {{1, 0, 3}, {0, 2, 8}}, {{1, 0, 3}, {0, 0, 20}},
	{{0, 0, 4}, {0, 10, 0}}, {{0, 0, 4}, {0, 10, 2}}, {{0, 2, 6}, {0, 8, 0}}, {{0, 2, 6}, {0, 8, 2}},
	{{1, 0, 23}, {1, 0, 13}}, {{1, 0, 23}, {1, 0, 14}}, {{1, 0, 20}, {1, 0, 12}}, {{1, 0, 20}, {1, 0, 15}},
	{{0, 0, 4}, {0, 10, 10}}, {{0, 0, 4}, {0, 10, 8}}, {{0, 2, 6}, {0, 8, 10}}, {{0, 2, 6}, {0, 8, 8}},
	{{1, 0, 0}, {1, 0, 4}}, {{1, 0, 0}, {1, 0, 7}}, {{1, 0, 3}, {1, 0, 5}}, {{1, 0, 3}, {1, 0, 6}},
	{{1, 0, 20}, {0, 0, 0}}, {{1, 0, 20}, {0, 2, 2}}, {{1, 0, 23}, {0, 2, 0}}, {{1, 0, 23}, {0, 0, 3}}},
   {{{1, 0, 0}, {0, 0, 20}}, {{1, 0, 0}, {0, 2, 8}}, {{1, 0, 3}, {0, 2, 10}}, {{1, 0, 3}, {0, 0, 23}},
	{{0, 0, 4}, {0, 8, 2}}, {{0, 0, 4}, {0, 8, 0}}, {{0, 2, 6}, {0, 10, 2}}, {{0, 2, 6}, {0, 10, 0}},
	{{1, 0, 23}, {1, 0, 15}}, {{1, 0, 23}, {1, 0, 12}}, {{1, 0, 20}, {1, 0, 14}}, {{1, 0, 20}, {1, 0, 13}},
	{{0, 0, 4}, {0, 8, 8}}, {{0, 0, 4}, {0, 8, 10}}, {{0, 2, 6}, {0, 10, 8}}, {{0, 2, 6}, {0, 10, 10}},
	{{1, 0, 0}, {1, 0, 6}}, {{1, 0, 0}, {1, 0, 5}}, {{1, 0, 3}, {1, 0, 7}}, {{1, 0, 3}, {1, 0, 4}},
	{{1, 0, 20}, {0, 0, 3}}, {{1, 0, 20}, {0, 2, 0}}, {{1, 0, 23}, {0, 2, 2}}, {{1, 0, 23}, {0, 0, 0}}},
   {{{1, 2, 3}, {0, 2, 23}}, {{1, 0, 1}, {0, 0, 10}}, {{1, 0, 2}, {0, 0, 8}}, {{1, 2, 0}, {0, 2, 20}},
	{{0, 2, 4}, {0, 8, 0}}, {{0, 2, 4}, {0, 8, 2}}, {{0, 0, 6}, {0, 10, 0}}, {{0, 0, 6}, {0, 10, 2}},
	{{1, 0, 22}, {1, 0, 12}}, {{1, 0, 22}, {1, 0, 15}}, {{1, 0, 21}, {1, 0, 13}}, {{1, 0, 21}, {1, 0, 14}},
	{{0, 2, 4}, {0, 8, 10}}, {{0, 2, 4}, {0, 8, 8}}, {{0, 0, 6}, {0, 10, 10}}, {{0, 0, 6}, {0, 10, 8}},
	{{1, 0, 1}, {1, 0, 5}}, {{1, 0, 1}, {1, 0, 6}}, {{1, 0, 2}, {1, 0, 4}}, {{1, 0, 2}, {1, 0, 7}},
	{{1, 2, 23}, {0, 2, 0}}, {{1, 0, 21}, {0, 0, 2}}, {{1, 0, 22}, {0, 0, 0}}, {{1, 2, 20}, {0, 2, 3}}},
   {{{1, 2, 3}, {0, 2, 20}}, {{1, 0, 1}, {0, 0, 8}}, {{1, 0, 2}, {0, 0, 10}}, {{1, 2, 0}, {0, 2, 23}}, 
	{{0, 2, 4}, {0, 10, 2}}, {{0, 2, 4}, {0, 10, 0}}, {{0, 0, 6}, {0, 8, 2}}, {{0, 0, 6}, {0, 8, 0}},
	{{1, 0, 22}, {1, 0, 14}}, {{1, 0, 22}, {1, 0, 13}}, {{1, 0, 21}, {1, 0, 15}}, {{1, 0, 21}, {1, 0, 12}},
	{{0, 2, 4}, {0, 10, 8}}, {{0, 2, 4}, {0, 10, 10}}, {{0, 0, 6}, {0, 8, 8}}, {{0, 0, 6}, {0, 8, 10}},
	{{1, 0, 1}, {1, 0, 7}}, {{1, 0, 1}, {1, 0, 4}}, {{1, 0, 2}, {1, 0, 6}}, {{1, 0, 2}, {1, 0, 5}},
	{{1, 2, 23}, {0, 2, 3}}, {{1, 0, 21}, {0, 0, 0}}, {{1, 0, 22}, {0, 0, 2}}, {{1, 2, 20}, {0, 2, 0}}},
   {{{1, 20, 0}, {0, 20, 0}}, {{1, 0, 17}, {0, 23, 0}}, {{1, 0, 19}, {0, 23, 2}}, {{1, 20, 3}, {0, 20, 3}},
	{{0, 20, 4}, {1, 0, 13}}, {{0, 20, 4}, {1, 0, 12}}, {{0, 23, 6}, {1, 0, 15}}, {{0, 23, 6}, {1, 0, 14}},
	{{1, 0, 9}, {0, 0, 8}}, {{1, 0, 9}, {0, 3, 8}}, {{1, 0, 11}, {0, 0, 10}}, {{1, 0, 11}, {0, 3, 10}},
	{{0, 20, 4}, {1, 0, 4}}, {{0, 20, 4}, {1, 0, 5}}, {{0, 23, 6}, {1, 0, 6}}, {{0, 23, 6}, {1, 0, 7}},
	{{1, 0, 17}, {0, 0, 0}}, {{1, 0, 17}, {0, 3, 0}}, {{1, 0, 19}, {0, 0, 2}}, {{1, 0, 19}, {0, 3, 2}},
	{{1, 20, 20}, {0, 20, 20}}, {{1, 0, 11}, {0, 23, 10}}, {{1, 0, 9}, {0, 23, 8}}, {{1, 20, 23}, {0, 20, 23}}},
   {{{1, 10, 20}, {0, 10, 3}}, {{1, 0, 17}, {0, 8, 2}}, {{1, 0, 19}, {0, 8, 0}}, {{1, 10, 23}, {0, 10, 0}},
	{{0, 10, 4}, {1, 0, 14}}, {{0, 10, 4}, {1, 0, 15}}, {{0, 8, 6}, {1, 0, 12}}, {{0, 8, 6}, {1, 0, 13}},
	{{1, 0, 9}, {0, 2, 10}}, {{1, 0, 9}, {0, 0, 10}}, {{1, 0, 11}, {0, 2, 8}}, {{1, 0, 11}, {0, 0, 8}},
	{{0, 10, 4}, {1, 0, 7}}, {{0, 10, 4}, {1, 0, 6}}, {{0, 8, 6}, {1, 0, 5}}, {{0, 8, 6}, {1, 0, 4}},
	{{1, 0, 17}, {0, 2, 2}}, {{1, 0, 17}, {0, 0, 2}}, {{1, 0, 19}, {0, 2, 0}}, {{1, 0, 19}, {0, 0, 0}},
	{{1, 10, 3}, {0, 10, 23}}, {{1, 0, 11}, {0, 8, 8}}, {{1, 0, 9}, {0, 8, 10}}, {{1, 10, 0}, {0, 10, 20}}},
   {{{1, 8, 20}, {0, 8, 3}}, {{1, 0, 16}, {0, 10, 2}}, {{1, 0, 18}, {0, 10, 0}}, {{1, 8, 23}, {0, 8, 0}},
	{{0, 8, 4}, {1, 0, 15}}, {{0, 8, 4}, {1, 0, 14}}, {{0, 10, 6}, {1, 0, 13}}, {{0, 10, 6}, {1, 0, 12}},
	{{1, 0, 8}, {0, 0, 10}}, {{1, 0, 8}, {0, 2, 10}}, {{1, 0, 10}, {0, 0, 8}}, {{1, 0, 10}, {0, 2, 8}},
	{{0, 8, 4}, {1, 0, 6}}, {{0, 8, 4}, {1, 0, 7}}, {{0, 10, 6}, {1, 0, 4}}, {{0, 10, 6}, {1, 0, 5}},
	{{1, 0, 16}, {0, 0, 2}}, {{1, 0, 16}, {0, 2, 2}}, {{1, 0, 18}, {0, 0, 0}}, {{1, 0, 18}, {0, 2, 0}},
	{{1, 8, 3}, {0, 8, 23}}, {{1, 0, 10}, {0, 10, 8}}, {{1, 0, 8}, {0, 10, 10}}, {{1, 8, 0}, {0, 8, 20}}},
   {{{1, 23, 0}, {0, 23, 0}}, {{1, 0, 16}, {0, 20, 0}}, {{1, 0, 18}, {0, 20, 2}}, {{1, 23, 3}, {0, 23, 3}},
	{{0, 23, 4}, {1, 0, 12}}, {{0, 23, 4}, {1, 0, 13}}, {{0, 20, 6}, {1, 0, 14}}, {{0, 20, 6}, {1, 0, 15}},
	{{1, 0, 8}, {0, 3, 8}}, {{1, 0, 8}, {0, 0, 8}}, {{1, 0, 10}, {0, 3, 10}}, {{1, 0, 10}, {0, 0, 10}},
	{{0, 23, 4}, {1, 0, 5}}, {{0, 23, 4}, {1, 0, 4}}, {{0, 20, 6}, {1, 0, 7}}, {{0, 20, 6}, {1, 0, 6}},
	{{1, 0, 16}, {0, 3, 0}}, {{1, 0, 16}, {0, 0, 0}}, {{1, 0, 18}, {0, 3, 2}}, {{1, 0, 18}, {0, 0, 2}},
	{{1, 23, 20}, {0, 23, 20}}, {{1, 0, 10}, {0, 20, 10}}, {{1, 0, 8}, {0, 20, 8}}, {{1, 23, 23}, {0, 23, 23}}}};


QuantumGraphState::QuantumGraphState(uint32_t num_qubits, int seed) : CliffordState(num_qubits, seed), num_qubits(num_qubits) {
	graph = Graph();
	for (uint32_t i = 0; i < num_qubits; i++) graph.add_vertex(HGATE);
}

QuantumGraphState::QuantumGraphState(Graph &graph, int seed) : CliffordState(graph.num_vertices, seed) {
	this->graph = Graph(graph);
}

QuantumCHPState<Tableau> QuantumGraphState::to_chp() const {
	uint32_t num_qubits = graph.num_vertices;

	QuantumCHPState<Tableau> chp(num_qubits);

	for (uint32_t i = 0; i < num_qubits; i++)
		chp.h_gate(i);

	for (uint32_t i = 0; i < num_qubits; i++) {
		for (auto j : graph.neighbors(i)) {
			if (i < j)
				chp.cz_gate(i, j);
		}
	}

	for (uint32_t i = 0; i < num_qubits; i++) {
		uint32_t v = graph.get_val(i);
		for (uint32_t j = 0; j < 5; j++) {
			uint32_t vj = CLIFFORD_DECOMPS[v][j];
			if (vj == SQRTXGATE)
				chp.sx_gate(i);
			else if (vj == SQRTYGATE)
				chp.sy_gate(i);
			else if (vj == SQRTZGATE)
				chp.sz_gate(i);
		}
	}

	return chp;
}

void QuantumGraphState::apply_gatel(uint32_t a, uint32_t gate_id) {
	auto r = CLIFFORD_PRODUCTS[gate_id][graph.get_val(a)];
	graph.set_val(a, r);
}

void QuantumGraphState::apply_gater(uint32_t a, uint32_t gate_id) {
	graph.set_val(a, CLIFFORD_PRODUCTS[graph.get_val(a)][gate_id]);
}

void QuantumGraphState::local_complement(uint32_t a) {
	graph.local_complement(a);

	apply_gater(a, SQRTXDGATE);
	for (auto e : graph.neighbors(a)) {
		apply_gater(e, SQRTZGATE);
	}
}

void QuantumGraphState::remove_vop(uint32_t a, uint32_t b) {
	uint32_t vop_decomp[5];
	for (uint32_t i = 0; i < 5; i ++) {
		vop_decomp[i] = CLIFFORD_DECOMPS[graph.get_val(a)][i];
	}
	uint32_t c = b;

	for (auto e : graph.neighbors(a)) {
		if (e != b) {
			c = e;
			break;
		}
	}

	for (auto op : vop_decomp) {
		if (op == SQRTXGATE) local_complement(a);
		else if (op == SQRTZGATE) local_complement(c);
	}
}

bool QuantumGraphState::isolated(uint32_t a, uint32_t b) {
	uint32_t deg = graph.degree(a);
	if (deg == 0) return true;
	if (deg == 1) return graph.contains_edge(a, b);
	return false;
}

void QuantumGraphState::mxr_graph(uint32_t a, bool outcome) {
	uint32_t b = graph.neighbors(a)[0];

	std::vector<uint32_t> ngbh_a = graph.neighbors(a);
	std::vector<uint32_t> ngbh_b = graph.neighbors(b);
	
	if (outcome) {
		apply_gater(a, ZGATE);
		for (auto n : ngbh_b) {
			if ((n != a) && !std::binary_search(ngbh_a.begin(), ngbh_a.end(), n)) {
				apply_gater(n, ZGATE);
			}
		}
		apply_gater(b, SQRTYGATE);
	} else {
		for (auto n : ngbh_a) {
			if ((n != b) && !std::binary_search(ngbh_b.begin(), ngbh_b.end(), n)) {
				apply_gater(n, ZGATE);
			}
		}
		apply_gater(b, SQRTYDGATE);
	}

	for (auto c : ngbh_a) {
		for (auto d : ngbh_b) {
			graph.toggle_edge(c, d);
		}
	}

	for (auto c : ngbh_a) {
		if (std::binary_search(ngbh_b.begin(), ngbh_b.end(), c)) {
			for (auto d : ngbh_a) {
				if (std::binary_search(ngbh_b.begin(), ngbh_b.end(), d)) {
					graph.toggle_edge(c, d);
				}
			}
		}
	}

	for (auto d : ngbh_a) {
		if (d != b) {
			graph.toggle_edge(b, d);
		}
	}
}

void QuantumGraphState::myr_graph(uint32_t a, bool outcome) {
	uint32_t gate_id = outcome ? SQRTZDGATE : SQRTZGATE;
	std::vector<uint32_t> ngbh = graph.neighbors(a);

	for (auto n : ngbh) apply_gater(n, gate_id);
	apply_gater(a, gate_id);

	ngbh.push_back(a);

	for (auto i : ngbh) {
		for (auto j : ngbh) {
			if (i < j) {
				graph.toggle_edge(i, j);
			}
		}
	}
}

void QuantumGraphState::mzr_graph(uint32_t a, bool outcome) {
	std::vector<uint32_t> ngbh = graph.neighbors(a);

	for (auto n : ngbh) {
		graph.remove_edge(a, n);
		if (outcome) apply_gater(n, ZGATE);
	}

	if (outcome) apply_gater(a, XGATE);
	apply_gater(a, HGATE);
}

std::string QuantumGraphState::to_string() const {
	return graph.to_string();
}

void QuantumGraphState::x_gate(uint32_t a) {
	apply_gatel(a, XGATE);
}

void QuantumGraphState::y_gate(uint32_t a) {
	apply_gatel(a, YGATE);
}

void QuantumGraphState::z_gate(uint32_t a) {
	apply_gatel(a, ZGATE);
}

void QuantumGraphState::h_gate(uint32_t a) {
	apply_gatel(a, HGATE);
}

void QuantumGraphState::s_gate(uint32_t a) {
	apply_gatel(a, SGATE);
}

void QuantumGraphState::cz_gate(uint32_t a, uint32_t b) {
	assert((a < num_qubits) && (b < num_qubits) && (a != b));

	if (!isolated(a, b)) remove_vop(a, b);
	if (!isolated(b, a)) remove_vop(b, a);
	if (!isolated(a, b)) remove_vop(a, b);

	uint32_t lookup[3];
	for (uint32_t i = 0; i < 3; i ++) {
		lookup[i] = CZ_LOOKUP[graph.get_val(a)][graph.get_val(b)][graph.contains_edge(a, b)][i];
	}
	graph.set_val(a, lookup[1]);
	graph.set_val(b, lookup[2]);

	if (lookup[0] != graph.contains_edge(a, b)) graph.toggle_edge(a, b);
}

double QuantumGraphState::mzr_expectation(uint32_t a) {
	uint32_t basis = CONJUGATION_TABLE[graph.get_val(a)];
	bool positive = basis > 3;
	if ((basis == 1) || (basis == 4)) {
		if (graph.degree(a) == 0) return 2*int(positive) - 1.0;
	}

	return 0.0;
}


bool QuantumGraphState::mzr(uint32_t a) {
	uint32_t basis = CONJUGATION_TABLE[graph.get_val(a)];
	bool positive = basis > 3;
	bool outcome = rand() % 2;
	if ((basis == 1) || (basis == 4)) {
		if (graph.degree(a) == 0) return positive;
	}

	if      ((basis == 1) || (basis == 4)) mxr_graph(a, outcome);
	else if ((basis == 2) || (basis == 5)) myr_graph(a, outcome);
	else if ((basis == 3) || (basis == 6)) mzr_graph(a, outcome);

	return outcome ^ positive;
}

void QuantumGraphState::toggle_edge_gate(uint32_t a, uint32_t b) {
	uint32_t ca = graph.get_val(a);
	uint32_t cb = graph.get_val(b);
	
	apply_gatel(a, HERMITIAN_CONJUGATE_TABLE[ca]);
	apply_gatel(b, HERMITIAN_CONJUGATE_TABLE[cb]);
	cz_gate(a, b);
	apply_gatel(a, ca);
	apply_gatel(b, cb);
}

double QuantumGraphState::entropy(const std::vector<uint32_t> &qubits, uint32_t index) const {
	Graph bipartite_graph = graph.partition(qubits);
	int s = 2*bipartite_graph.num_vertices;
	for (uint32_t i = 0; i < bipartite_graph.num_vertices; i++) {
		if (bipartite_graph.get_val(i)) s--;
	}

	while (bipartite_graph.num_vertices > 0) {
		uint32_t del_node = bipartite_graph.num_vertices - 1;
		uint32_t del_node_degree = bipartite_graph.degree(del_node);
		bool del_node_val = bipartite_graph.get_val(del_node);

		if (del_node_degree == 0) {
			if (!del_node_val) s -= 2;
			else s -= 1;
			bipartite_graph.remove_vertex(del_node);
		} else if (del_node_degree == 1) {
			uint32_t neighbor = bipartite_graph.neighbors(del_node)[0];
			if (del_node_val) {
				std::vector<uint32_t> del_nodes;
				for (auto e : bipartite_graph.neighbors(neighbor)) {
					if (bipartite_graph.degree(e) == 1) del_nodes.push_back(e);
				}

				del_nodes.push_back(neighbor);
				std::sort(del_nodes.begin(), del_nodes.end(), std::greater<>());
				for (auto j : del_nodes) bipartite_graph.remove_vertex(j);

				s -= del_nodes.size();
			} else {
				bipartite_graph.remove_vertex(del_node);
				bipartite_graph.remove_vertex(neighbor);
				
				s -= 2;
			}
		} else {
			bool found_pivot = false;
			uint32_t pivot = 0;
			uint32_t min_degree = INT_MAX;
			for (auto neighbor : bipartite_graph.neighbors(del_node)) {
				uint32_t deg = bipartite_graph.degree(neighbor);

				if ((deg != 1) && (deg < min_degree)) {
					found_pivot = true;
					min_degree = deg;
					pivot = neighbor;
				}
			}

			// there is no valid pivot, so graph is a simple tree; clear it.
			if (!found_pivot) {
				std::vector<uint32_t> neighbors = bipartite_graph.neighbors(del_node);
				std::reverse(neighbors.begin(), neighbors.end());

				bipartite_graph.remove_vertex(del_node);
				for (auto neighbor : neighbors) {
					bipartite_graph.remove_vertex(neighbor);
				}

				if (del_node_val) s -= 2*del_node_degree;
				else s -= 1*del_node_degree;
			} else {
				for (auto neighbor : bipartite_graph.neighbors(del_node)) {
					if (neighbor != pivot) {
						std::vector<uint32_t> pivot_neighbors = bipartite_graph.neighbors(pivot);
						for (uint32_t k = 0; k < min_degree; k++) {
							uint32_t pivot_neighbor = pivot_neighbors[k];
							bipartite_graph.toggle_edge(neighbor, pivot_neighbor);
						}
					}
				}

				// Pivot completed; ready to be deleted on the next iteration
			}
		}
	}

	return static_cast<double>(s);
}

double QuantumGraphState::sparsity() const {
	double s = 0.;
	for (uint32_t i = 0; i < num_qubits; i++) {
		for (uint32_t j = 0; j < num_qubits; j++)
			s += graph.adjacency_matrix(i, j);
	}

	return s/(num_qubits*num_qubits);
}