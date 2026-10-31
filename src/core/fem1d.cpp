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

void apply_boundary_conditions(GlobalSystem& sys, double T0, double TL) {
    if (sys.num_nodes < 2) {
        throw std::invalid_argument("apply_boundary_conditions: System must have at least 2 nodes.");
    }
    if (!std::isfinite(T0) || !std::isfinite(TL)) {
        throw std::invalid_argument("apply_boundary_conditions: Boundary temperatures T0 and TL must be finite.");
    }
    const int N_node = sys.num_nodes - 1;

    // 1. Column elimination: subtract column contributions of node 0 and node N_node from interior RHS
    // For node 0: affects node 1
    sys.F[1] -= sys.K[1][0] * T0;
    // For node N_node: affects node N_node - 1
    sys.F[N_node - 1] -= sys.K[N_node - 1][N_node] * TL;

    // 2. Zero off-diagonals in rows and columns 0 and N_node to preserve symmetry
    sys.K[0][1] = 0.0;
    sys.K[1][0] = 0.0;
    sys.K[N_node][N_node - 1] = 0.0;
    sys.K[N_node - 1][N_node] = 0.0;

    // 3. Set diagonal entries to 1.0 and RHS to Dirichlet values
    sys.K[0][0] = 1.0;
    sys.F[0] = T0;

    sys.K[N_node][N_node] = 1.0;
    sys.F[N_node] = TL;
}

std::vector<double> solve(const GlobalSystem& sys) {
    if (sys.num_nodes < 2) {
        throw std::invalid_argument("solve: System must have at least 2 nodes.");
    }
    const int n = sys.num_nodes;
    std::vector<double> T(n, 0.0);

    // Boundary nodes are already decoupled by apply_boundary_conditions
    T[0] = sys.F[0];
    T[n - 1] = sys.F[n - 1];

    const int M = n - 2; // Number of interior unknowns (indices 1 to n-2)
    if (M == 0) {
        return T;
    }

    // Vectors for interior equations:
    // a[i]: subdiagonal (coefficient of x_{i-1}) for i = 1 .. M-1
    // b[i]: main diagonal (coefficient of x_i) for i = 0 .. M-1
    // c[i]: superdiagonal (coefficient of x_{i+1}) for i = 0 .. M-2
    // d[i]: right-hand side for i = 0 .. M-1
    std::vector<double> a(M, 0.0);
    std::vector<double> b(M, 0.0);
    std::vector<double> c(M, 0.0);
    std::vector<double> d(M, 0.0);

    for (int i = 0; i < M; ++i) {
        const int node = i + 1;
        b[i] = sys.K[node][node];
        d[i] = sys.F[node];
        if (i > 0) {
            a[i] = sys.K[node][node - 1];
        }
        if (i < M - 1) {
            c[i] = sys.K[node][node + 1];
        }
    }

    // Thomas algorithm forward sweep
    std::vector<double> c_prime(M, 0.0);
    std::vector<double> d_prime(M, 0.0);

    if (std::abs(b[0]) < 1e-15) {
        throw std::runtime_error("solve: Zero pivot encountered in Thomas forward elimination.");
    }
    c_prime[0] = c[0] / b[0];
    d_prime[0] = d[0] / b[0];

    for (int i = 1; i < M; ++i) {
        const double denom = b[i] - a[i] * c_prime[i - 1];
        if (std::abs(denom) < 1e-15) {
            throw std::runtime_error("solve: Zero pivot encountered in Thomas forward elimination.");
        }
        if (i < M - 1) {
            c_prime[i] = c[i] / denom;
        }
        d_prime[i] = (d[i] - a[i] * d_prime[i - 1]) / denom;
    }

    // Back substitution
    std::vector<double> x(M, 0.0);
    x[M - 1] = d_prime[M - 1];
    for (int i = M - 2; i >= 0; --i) {
        x[i] = d_prime[i] - c_prime[i] * x[i + 1];
    }

    // Map back into full solution vector
    for (int i = 0; i < M; ++i) {
        T[i + 1] = x[i];
    }

    return T;
}

} // namespace thermo
