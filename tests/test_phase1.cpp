// tests/test_phase1.cpp
// Tests for Phase 1: 1D Thermal FEA

#include "test_runner.hpp"
#include "core/fem1d.hpp"
#include <cmath>
#include <limits>

// ── Function 1: assemble_element ─────────────────────────────────────────────

TEST("Phase 1 - assemble_element: values match analytical formulas") {
    const double k = 10.0;     // W/(m K)
    const double A = 1e-4;     // m^2
    const double h = 0.1;      // m (for N = 10 on L = 1.0)
    const double q = 10000.0;  // W/m^3

    thermo::ElementMatrices em = thermo::assemble_element(k, A, h, q);

    const double expected_stiff = (k * A) / h; // 0.01 W/K
    const double expected_load  = (q * A * h) / 2.0; // 0.05 W

    ASSERT_NEAR(em.Ke[0][0],  expected_stiff, 1e-12);
    ASSERT_NEAR(em.Ke[0][1], -expected_stiff, 1e-12);
    ASSERT_NEAR(em.Ke[1][0], -expected_stiff, 1e-12);
    ASSERT_NEAR(em.Ke[1][1],  expected_stiff, 1e-12);

    ASSERT_NEAR(em.fe[0], expected_load, 1e-12);
    ASSERT_NEAR(em.fe[1], expected_load, 1e-12);

    // Matrix properties: symmetry and zero row sum (rigid body mode / constant T)
    ASSERT_NEAR(em.Ke[0][1], em.Ke[1][0], 1e-12);
    ASSERT_NEAR(em.Ke[0][0] + em.Ke[0][1], 0.0, 1e-12);
}

TEST("Phase 1 - assemble_element: negative/zero inputs throw invalid_argument") {
    // Negative k
    ASSERT_THROWS(thermo::assemble_element(-10.0, 1e-4, 0.1, 10000.0));
    // Zero k
    ASSERT_THROWS(thermo::assemble_element(0.0, 1e-4, 0.1, 10000.0));
    // Negative A
    ASSERT_THROWS(thermo::assemble_element(10.0, -1e-4, 0.1, 10000.0));
    // Zero h
    ASSERT_THROWS(thermo::assemble_element(10.0, 1e-4, 0.0, 10000.0));
    // Negative h
    ASSERT_THROWS(thermo::assemble_element(10.0, 1e-4, -0.1, 10000.0));
}

TEST("Phase 1 - assemble_element: NaN / Inf inputs throw invalid_argument") {
    const double nan_val = std::numeric_limits<double>::quiet_NaN();
    const double inf_val = std::numeric_limits<double>::infinity();

    ASSERT_THROWS(thermo::assemble_element(nan_val, 1e-4, 0.1, 10000.0));
    ASSERT_THROWS(thermo::assemble_element(10.0, nan_val, 0.1, 10000.0));
    ASSERT_THROWS(thermo::assemble_element(10.0, 1e-4, nan_val, 10000.0));
    ASSERT_THROWS(thermo::assemble_element(10.0, 1e-4, 0.1, nan_val));
    ASSERT_THROWS(thermo::assemble_element(inf_val, 1e-4, 0.1, 10000.0));
}
