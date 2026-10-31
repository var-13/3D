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

} // namespace thermo
