#include "fem1d.hpp"
#include <cmath>
#include <stdexcept>
#include <sstream>

namespace thermo {

std::string version() {
    return "ThermoBench v0.1.0 (Phase 1: Thermal FEA)";
}

ElementMatrices assemble_element(double k, double A, double h, double q) {
    if (!std::isfinite(k) || !std::isfinite(A) || !std::isfinite(h) || !std::isfinite(q)) {
        throw std::invalid_argument("assemble_element: All parameters (k, A, h, q) must be finite real numbers.");
    }
    if (k <= 0.0) {
        std::ostringstream oss;
        oss << "assemble_element: Thermal conductivity k must be positive (> 0), got " << k;
        throw std::invalid_argument(oss.str());
    }
    if (A <= 0.0) {
        std::ostringstream oss;
        oss << "assemble_element: Area A must be positive (> 0), got " << A;
        throw std::invalid_argument(oss.str());
    }
    if (h <= 0.0) {
        std::ostringstream oss;
        oss << "assemble_element: Element length h must be positive (> 0), got " << h;
        throw std::invalid_argument(oss.str());
    }

    ElementMatrices em;
    const double stiff = (k * A) / h;
    em.Ke[0][0] =  stiff;
    em.Ke[0][1] = -stiff;
    em.Ke[1][0] = -stiff;
    em.Ke[1][1] =  stiff;

    const double load = (q * A * h) / 2.0;
    em.fe[0] = load;
    em.fe[1] = load;

    return em;
}

GlobalSystem assemble_global(double L, double A, double k, double q, int N) {
    if (N <= 0) {
        std::ostringstream oss;
        oss << "assemble_global: Number of elements N must be positive (> 0), got " << N;
        throw std::invalid_argument(oss.str());
    }
    if (!std::isfinite(L) || !std::isfinite(A) || !std::isfinite(k) || !std::isfinite(q)) {
        throw std::invalid_argument("assemble_global: All physical parameters (L, A, k, q) must be finite real numbers.");
    }
    if (L <= 0.0) {
        std::ostringstream oss;
        oss << "assemble_global: Domain length L must be positive (> 0), got " << L;
        throw std::invalid_argument(oss.str());
    }
    if (A <= 0.0) {
        std::ostringstream oss;
        oss << "assemble_global: Area A must be positive (> 0), got " << A;
        throw std::invalid_argument(oss.str());
    }
    if (k <= 0.0) {
        std::ostringstream oss;
        oss << "assemble_global: Thermal conductivity k must be positive (> 0), got " << k;
        throw std::invalid_argument(oss.str());
    }

    const double h = L / static_cast<double>(N);
    ElementMatrices em = assemble_element(k, A, h, q);

    const int num_nodes = N + 1;
    GlobalSystem sys;
    sys.num_nodes = num_nodes;
    sys.K.assign(num_nodes, std::vector<double>(num_nodes, 0.0));
    sys.F.assign(num_nodes, 0.0);

    for (int e = 0; e < N; ++e) {
        const int n0 = e;
        const int n1 = e + 1;

        sys.K[n0][n0] += em.Ke[0][0];
        sys.K[n0][n1] += em.Ke[0][1];
        sys.K[n1][n0] += em.Ke[1][0];
        sys.K[n1][n1] += em.Ke[1][1];

        sys.F[n0] += em.fe[0];
        sys.F[n1] += em.fe[1];
    }

    return sys;
}

} // namespace thermo
