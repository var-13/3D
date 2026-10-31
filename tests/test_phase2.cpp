// tests/test_phase2.cpp
// Tests for Phase 2: Backpropagation Network

#include "test_runner.hpp"
#include "core/backprop.hpp"
#include <cmath>
#include <limits>

// ── Function 1: forward ──────────────────────────────────────────────────────

TEST("Phase 2 - forward: verifies initial state against analytical values") {
    const double x = 1.0;
    const double y = 2.0;
    thermo::NetworkParams p;
    p.w = 0.5;
    p.b = 0.0;
    p.v = 1.0;
    p.c = 0.0;

    thermo::ForwardResult res = thermo::forward(x, y, p);

    // z = w*x + b = 0.5*1 + 0 = 0.5
    ASSERT_NEAR(res.z, 0.5, 1e-12);

    // h = tanh(0.5) ~ 0.46211715726
    const double expected_h = std::tanh(0.5);
    ASSERT_NEAR(res.h, expected_h, 1e-12);
    ASSERT_NEAR(res.h, 0.4621, 1e-3); // Brief check

    // y_hat = v*h + c = 1.0*h + 0 = h
    ASSERT_NEAR(res.y_hat, expected_h, 1e-12);
    ASSERT_NEAR(res.y_hat, 0.4621, 1e-3); // Brief check

    // J = 0.5 * (y_hat - y)^2 = 0.5 * (0.462117 - 2)^2 ~ 1.1825418
    const double diff = expected_h - 2.0;
    const double expected_J = 0.5 * diff * diff;
    ASSERT_NEAR(res.loss, expected_J, 1e-12);
    ASSERT_NEAR(res.loss, 1.1825, 1e-3); // Brief check
}

TEST("Phase 2 - forward: non-finite inputs throw invalid_argument") {
    const double nan_val = std::numeric_limits<double>::quiet_NaN();
    thermo::NetworkParams p;

    ASSERT_THROWS(thermo::forward(nan_val, 2.0, p));
    ASSERT_THROWS(thermo::forward(1.0, nan_val, p));

    thermo::NetworkParams bad_p = p;
    bad_p.w = nan_val;
    ASSERT_THROWS(thermo::forward(1.0, 2.0, bad_p));
}

// ── Function 2: gradients ────────────────────────────────────────────────────

TEST("Phase 2 - gradients: analytical chain rule matches brief values") {
    const double x = 1.0;
    const double y = 2.0;
    thermo::NetworkParams p;
    p.w = 0.5;
    p.b = 0.0;
    p.v = 1.0;
    p.c = 0.0;

    thermo::Gradients g = thermo::gradients(x, y, p);

    // Approximate checks from brief:
    // dc ~ -1.5379, dv ~ -0.7107, dw = db ~ -1.2095
    ASSERT_NEAR(g.dc, -1.5379, 1e-3);
    ASSERT_NEAR(g.dv, -0.7107, 1e-3);
    ASSERT_NEAR(g.dw, -1.2095, 1e-3);
    ASSERT_NEAR(g.db, -1.2095, 1e-3);

    // Precise analytical check:
    const double h = std::tanh(0.5);
    const double dyhat = h - 2.0;
    const double expected_dc = dyhat;
    const double expected_dv = dyhat * h;
    const double expected_dz = dyhat * 1.0 * (1.0 - h * h);
    const double expected_dw = expected_dz * 1.0;
    const double expected_db = expected_dz;

    ASSERT_NEAR(g.dc, expected_dc, 1e-12);
    ASSERT_NEAR(g.dv, expected_dv, 1e-12);
    ASSERT_NEAR(g.dw, expected_dw, 1e-12);
    ASSERT_NEAR(g.db, expected_db, 1e-12);

    // Since x = 1.0, dw must equal db exactly
    ASSERT_NEAR(g.dw, g.db, 1e-12);
}

TEST("Phase 2 - gradients: zero error produces zero gradients") {
    thermo::NetworkParams p;
    // Set target y equal to forward prediction
    thermo::ForwardResult fwd = thermo::forward(1.0, 0.0, p);
    thermo::Gradients g = thermo::gradients(1.0, fwd.y_hat, p);

    ASSERT_NEAR(g.dw, 0.0, 1e-12);
    ASSERT_NEAR(g.db, 0.0, 1e-12);
    ASSERT_NEAR(g.dv, 0.0, 1e-12);
    ASSERT_NEAR(g.dc, 0.0, 1e-12);
}

TEST("Phase 2 - gradients: non-finite inputs throw invalid_argument") {
    const double nan_val = std::numeric_limits<double>::quiet_NaN();
    thermo::NetworkParams p;
    ASSERT_THROWS(thermo::gradients(nan_val, 2.0, p));
}
