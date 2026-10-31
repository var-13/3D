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

// ── Function 2: assemble_global ──────────────────────────────────────────────

TEST("Phase 1 - assemble_global: dimensions, symmetry, tridiagonal and row sums") {
    const double L = 1.0;
    const double A = 1e-4;
    const double k = 10.0;
    const double q = 10000.0;
    const int N = 10;

    thermo::GlobalSystem sys = thermo::assemble_global(L, A, k, q, N);

    ASSERT(sys.num_nodes == N + 1);
    ASSERT(static_cast<int>(sys.K.size()) == N + 1);
    ASSERT(static_cast<int>(sys.F.size()) == N + 1);

    // Physical conservation 1: Total load sum = q * A * L
    double total_F = 0.0;
    for (int i = 0; i <= N; ++i) {
        total_F += sys.F[i];
    }
    ASSERT_NEAR(total_F, q * A * L, 1e-12);

    // Boundary load vs interior load:
    // Node 0 and Node N each receive half-element load: q*A*h / 2
    const double h = L / N;
    ASSERT_NEAR(sys.F[0], (q * A * h) / 2.0, 1e-12);
    ASSERT_NEAR(sys.F[N], (q * A * h) / 2.0, 1e-12);
    // Interior nodes receive 2 * (q*A*h / 2) = q*A*h
    for (int i = 1; i < N; ++i) {
        ASSERT_NEAR(sys.F[i], q * A * h, 1e-12);
    }

    // Physical conservation 2: Unconstrained stiffness has zero row sums (rigid body mode)
    for (int i = 0; i <= N; ++i) {
        double row_sum = 0.0;
        for (int j = 0; j <= N; ++j) {
            // Symmetry check
            ASSERT_NEAR(sys.K[i][j], sys.K[j][i], 1e-12);

            // Tridiagonal check: zero if |i - j| > 1
            if (std::abs(i - j) > 1) {
                ASSERT_NEAR(sys.K[i][j], 0.0, 1e-12);
            }
            row_sum += sys.K[i][j];
        }
        ASSERT_NEAR(row_sum, 0.0, 1e-12);
    }
}

TEST("Phase 1 - assemble_global: invalid N and negative params throw invalid_argument") {
    // N = 0
    ASSERT_THROWS(thermo::assemble_global(1.0, 1e-4, 10.0, 10000.0, 0));
    // N < 0
    ASSERT_THROWS(thermo::assemble_global(1.0, 1e-4, 10.0, 10000.0, -5));
    // Negative L
    ASSERT_THROWS(thermo::assemble_global(-1.0, 1e-4, 10.0, 10000.0, 10));
    // Negative k
    ASSERT_THROWS(thermo::assemble_global(1.0, 1e-4, -10.0, 10000.0, 10));
    // NaN input
    const double nan_val = std::numeric_limits<double>::quiet_NaN();
    ASSERT_THROWS(thermo::assemble_global(nan_val, 1e-4, 10.0, 10000.0, 10));
}

// ── Function 3: apply_boundary_conditions ────────────────────────────────────

TEST("Phase 1 - apply_boundary_conditions: row/column elimination and RHS modification") {
    const double L = 1.0;
    const double A = 1e-4;
    const double k = 10.0;
    const double q = 10000.0;
    const int N = 10;
    const double T0 = 100.0;
    const double TL = 20.0;

    thermo::GlobalSystem sys = thermo::assemble_global(L, A, k, q, N);

    const double orig_K10 = sys.K[1][0]; // -0.01
    const double orig_F1  = sys.F[1];    // 0.1
    const double orig_KNminus1_N = sys.K[N-1][N]; // -0.01
    const double orig_FNminus1  = sys.F[N-1];     // 0.1

    thermo::apply_boundary_conditions(sys, T0, TL);

    // Boundary rows decoupled
    ASSERT_NEAR(sys.K[0][0], 1.0, 1e-12);
    ASSERT_NEAR(sys.K[0][1], 0.0, 1e-12);
    ASSERT_NEAR(sys.K[1][0], 0.0, 1e-12);
    ASSERT_NEAR(sys.F[0], T0, 1e-12);

    ASSERT_NEAR(sys.K[N][N], 1.0, 1e-12);
    ASSERT_NEAR(sys.K[N][N-1], 0.0, 1e-12);
    ASSERT_NEAR(sys.K[N-1][N], 0.0, 1e-12);
    ASSERT_NEAR(sys.F[N], TL, 1e-12);

    // Interior RHS modified by column elimination
    ASSERT_NEAR(sys.F[1], orig_F1 - orig_K10 * T0, 1e-12);
    ASSERT_NEAR(sys.F[N-1], orig_FNminus1 - orig_KNminus1_N * TL, 1e-12);

    // Other interior nodes untouched
    for (int i = 2; i < N - 1; ++i) {
        ASSERT_NEAR(sys.F[i], q * A * (L / N), 1e-12);
    }
}

TEST("Phase 1 - apply_boundary_conditions: invalid inputs throw invalid_argument") {
    thermo::GlobalSystem empty_sys;
    empty_sys.num_nodes = 1;
    ASSERT_THROWS(thermo::apply_boundary_conditions(empty_sys, 100.0, 20.0));

    thermo::GlobalSystem valid_sys = thermo::assemble_global(1.0, 1e-4, 10.0, 10000.0, 10);
    const double nan_val = std::numeric_limits<double>::quiet_NaN();
    ASSERT_THROWS(thermo::apply_boundary_conditions(valid_sys, nan_val, 20.0));
    ASSERT_THROWS(thermo::apply_boundary_conditions(valid_sys, 100.0, nan_val));
}

// ── Function 4: solve ────────────────────────────────────────────────────────

TEST("Phase 1 - solve: Thomas solver satisfies K*T = F to machine precision on interior") {
    const double L = 1.0;
    const double A = 1e-4;
    const double k = 10.0;
    const double q = 10000.0;
    const int N = 10;
    const double T0 = 100.0;
    const double TL = 20.0;

    // Keep a copy of original unconstrained system to verify residuals
    thermo::GlobalSystem orig_sys = thermo::assemble_global(L, A, k, q, N);
    thermo::GlobalSystem sys = orig_sys;

    thermo::apply_boundary_conditions(sys, T0, TL);
    std::vector<double> T = thermo::solve(sys);

    ASSERT(static_cast<int>(T.size()) == N + 1);
    ASSERT_NEAR(T[0], T0, 1e-12);
    ASSERT_NEAR(T[N], TL, 1e-12);

    // Verify residual on all interior rows: sum_j K_ij * T_j == F_i
    for (int i = 1; i < N; ++i) {
        double row_dot = 0.0;
        for (int j = 0; j <= N; ++j) {
            row_dot += orig_sys.K[i][j] * T[j];
        }
        ASSERT_NEAR(row_dot, orig_sys.F[i], 1e-10);
    }
}

TEST("Phase 1 - solve: minimal mesh N=2 (1 interior unknown)") {
    thermo::GlobalSystem sys = thermo::assemble_global(1.0, 1e-4, 10.0, 10000.0, 2);
    thermo::apply_boundary_conditions(sys, 100.0, 20.0);
    std::vector<double> T = thermo::solve(sys);

    ASSERT(T.size() == 3);
    ASSERT_NEAR(T[0], 100.0, 1e-12);
    ASSERT_NEAR(T[2], 20.0, 1e-12);
    // Interior node x = 0.5: Exact is 100 - 80*0.5 + 500*0.5*0.5 = 60 + 125 = 185
    ASSERT_NEAR(T[1], 185.0, 1e-10);
}

TEST("Phase 1 - solve: invalid input throws invalid_argument") {
    thermo::GlobalSystem invalid_sys;
    invalid_sys.num_nodes = 1;
    ASSERT_THROWS(thermo::solve(invalid_sys));
}

// ── Function 5: exact_solution ───────────────────────────────────────────────

TEST("Phase 1 - exact_solution: matches known analytical benchmark points") {
    const double L = 1.0;
    const double k = 10.0;
    const double q = 10000.0;
    const double T0 = 100.0;
    const double TL = 20.0;

    // Boundaries
    ASSERT_NEAR(thermo::exact_solution(0.0, L, k, q, T0, TL), 100.0, 1e-12);
    ASSERT_NEAR(thermo::exact_solution(1.0, L, k, q, T0, TL), 20.0, 1e-12);

    // Midpoint: 100 - 80(0.5) + 500(0.5)(0.5) = 60 + 125 = 185.0
    ASSERT_NEAR(thermo::exact_solution(0.5, L, k, q, T0, TL), 185.0, 1e-12);

    // Peak at x = 0.42: 100 - 80(0.42) + 500(0.42)(0.58) = 188.2
    ASSERT_NEAR(thermo::exact_solution(0.42, L, k, q, T0, TL), 188.2, 1e-12);
}

TEST("Phase 1 - exact_solution: out of bounds and non-finite inputs throw invalid_argument") {
    const double L = 1.0;
    const double k = 10.0;
    const double q = 10000.0;

    // Out of domain bounds
    ASSERT_THROWS(thermo::exact_solution(-0.1, L, k, q, 100.0, 20.0));
    ASSERT_THROWS(thermo::exact_solution(1.1, L, k, q, 100.0, 20.0));

    // Negative conductivity
    ASSERT_THROWS(thermo::exact_solution(0.5, L, -10.0, q, 100.0, 20.0));

    // Negative length
    ASSERT_THROWS(thermo::exact_solution(0.5, -1.0, k, q, 100.0, 20.0));

    // NaN input
    const double nan_val = std::numeric_limits<double>::quiet_NaN();
    ASSERT_THROWS(thermo::exact_solution(nan_val, L, k, q, 100.0, 20.0));
}
