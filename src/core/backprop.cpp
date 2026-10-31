// src/core/backprop.cpp
// Implementation of pure C++17 Backpropagation network (no autodiff)

#include "backprop.hpp"
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <fstream>

namespace thermo {

ForwardResult forward(double x, double y, const NetworkParams& p) {
    if (!std::isfinite(x) || !std::isfinite(y) ||
        !std::isfinite(p.w) || !std::isfinite(p.b) ||
        !std::isfinite(p.v) || !std::isfinite(p.c)) {
        throw std::invalid_argument("forward: All inputs and parameters must be finite numbers.");
    }

    ForwardResult res;
    res.z = p.w * x + p.b;
    res.h = std::tanh(res.z);
    res.y_hat = p.v * res.h + p.c;
    const double diff = res.y_hat - y;
    res.loss = 0.5 * diff * diff;

    return res;
}

Gradients gradients(double x, double y, const NetworkParams& p) {
    ForwardResult fwd = forward(x, y, p);

    const double dJ_dyhat = fwd.y_hat - y;

    Gradients g;
    // dJ/dc = dJ/dy_hat * dy_hat/dc = (y_hat - y) * 1
    g.dc = dJ_dyhat;

    // dJ/dv = dJ/dy_hat * dy_hat/dv = (y_hat - y) * h
    g.dv = dJ_dyhat * fwd.h;

    // dJ/dh = dJ/dy_hat * dy_hat/dh = (y_hat - y) * v
    const double dJ_dh = dJ_dyhat * p.v;

    // dh/dz = 1 - tanh(z)^2 = 1 - h^2
    const double dh_dz = 1.0 - fwd.h * fwd.h;

    // delta_z = dJ/dz = dJ/dh * dh/dz
    const double delta_z = dJ_dh * dh_dz;

    // dJ/dw = delta_z * dz/dw = delta_z * x
    g.dw = delta_z * x;

    // dJ/db = delta_z * dz/db = delta_z * 1
    g.db = delta_z;

    return g;
}

double finite_diff_check(double x, double y, const NetworkParams& p, double eps) {
    (void)x; (void)y; (void)p; (void)eps;
    return 0.0; // Stub for func 3
}

NetworkParams update_step(const NetworkParams& p, const Gradients& g, double lr) {
    (void)p; (void)g; (void)lr;
    return {}; // Stub for func 4
}

std::vector<StepRecord> train_steps(double x, double y, NetworkParams p, double lr, int num_steps) {
    (void)x; (void)y; (void)p; (void)lr; (void)num_steps;
    return {}; // Stub for func 5
}

std::string loss_to_csv(const std::vector<StepRecord>& history) {
    (void)history;
    return {}; // Stub for func 6
}

void export_loss_csv(const std::vector<StepRecord>& history, const std::string& filepath) {
    (void)history; (void)filepath;
}

} // namespace thermo
