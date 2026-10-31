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
