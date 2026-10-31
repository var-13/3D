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

} // namespace thermo
