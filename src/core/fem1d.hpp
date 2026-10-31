// src/core/fem1d.hpp
// Phase 0 stub – just enough to compile and link.
// Real implementation added in Phase 1.
#pragma once
#include <string>
#include <vector>

namespace thermo {

// Returns a human-readable version string (used by the hello-world server).
std::string version();

// ── Phase 1: 1D Steady-State Thermal Finite Element Analysis ─────────────────
// Governing equation: -d/dx(k*A * dT/dx) = q*A on x in [0, L]
// Using 2-node linear elements with shape functions N1(x) = 1 - s, N2(x) = s (s = x/h)

struct ElementMatrices {
    double Ke[2][2]; // 2x2 local conductance matrix [W/K]
    double fe[2];    // 2x1 local thermal load vector [W]
};

// Assembles the 2x2 stiffness (conductance) matrix and 2x1 load vector for one element.
// Ke = (k*A/h) * [[ 1, -1], [-1,  1]]
// fe = (q*A*h / 2) * [1, 1]
// Throws std::invalid_argument if h <= 0, k <= 0, A <= 0, or any value is non-finite (NaN/Inf).
ElementMatrices assemble_element(double k, double A, double h, double q);

struct GlobalSystem {
    int num_nodes = 0;
    std::vector<std::vector<double>> K; // (num_nodes x num_nodes) global conductance matrix
    std::vector<double> F;              // (num_nodes) global thermal load vector
};

// Assembles global stiffness matrix K and load vector F for N uniform elements on [0, L].
// Node 0 is at x = 0; Node N is at x = L.
// Element e connects node e and node e+1, with h = L / N.
// Throws std::invalid_argument if N <= 0, L <= 0, A <= 0, k <= 0, or any input is non-finite.
GlobalSystem assemble_global(double L, double A, double k, double q, int N);

// Imposes Dirichlet boundary conditions T(0) = T0 and T(L) = TL by row and column elimination:
// 1. Column elimination: Known boundary values are moved to the right-hand side:
//    F_1 -= K_{1,0} * T0;  F_{N-1} -= K_{N-1, N} * TL
// 2. Off-diagonal boundary entries are zeroed to preserve matrix symmetry:
//    K_{0,1} = K_{1,0} = 0;  K_{N, N-1} = K_{N-1, N} = 0
// 3. Diagonal boundary entries set to 1.0, and RHS set to prescribed values:
//    K_{0,0} = 1.0, F_0 = T0;  K_{N,N} = 1.0, F_N = TL
// Throws std::invalid_argument if sys.num_nodes < 2 or boundary temperatures are non-finite.
void apply_boundary_conditions(GlobalSystem& sys, double T0, double TL);

} // namespace thermo
