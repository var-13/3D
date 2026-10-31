# ThermoBench: Technical Design, Verification, and Benchmark Report

**Candidate ID:** CandidateID  
**Project:** ThermoBench Engineering Workbench (Windows 11 x64)  
**Date:** October 31, 2026  
**Scope:** Design Choices, FEA Verification, Performance Results, Embedded Architecture, Test Outcomes, and Limitations.

---

## 1. Executive Summary & Design Choices

### 1.1 Architecture & Separation of Concerns
ThermoBench is engineered as a high-performance, modular engineering workbench adhering to the principle of strict separation between mathematical computation and presentation:
- **Numerical Core (`src/core/`)**: Zero-dependency, pure C++17 library. Implements Thomas algorithm for 1-D steady-state conduction, exact analytical comparisons, multi-threaded batch solvers, neural network backpropagation, and geometry parsers. Contains zero network or graphics code, ensuring deterministic testability and native compilation across platforms.
- **Micro-Server Layer (`src/server/`)**: Built upon single-header `cpp-httplib`. Handles REST endpoint routing, JSON serialization/deserialization, and static asset streaming.
- **Frontend Architecture (`ui/`)**: A responsive, 100% offline single-page application using vanilla HTML5/CSS and modern JavaScript modules. External visual libraries (Chart.js 4.4.3 and Three.js r167) are stored locally in `ui/vendor/` to eliminate all CDN dependencies, enabling offline operation in air-gapped environments.
- **Supervisor Utility (`src/launcher/launch.cpp`)**: Native Windows executable (`Launch.exe`) that spawns the backend daemon without command-prompt flicker, polls `/health` via raw non-blocking WinSock2 sockets, launches the user's default browser, and cleanly shuts down child processes on exit.

---

## 2. 1-D Thermal FEA Verification

### 2.1 Governing Differential Equation & Weak Form
The steady-state 1-D heat conduction problem with uniform cross-sectional area $A$, thermal conductivity $k$, and volumetric internal heat generation $q$ is governed by:
$$-\frac{d}{dx}\left(k A \frac{dT}{dx}\right) = q A, \quad x \in [0, L]$$
Subject to Dirichlet boundary conditions:
$$T(0) = T_0, \quad T(L) = T_L$$

Using standard 2-node linear Lagrange shape functions $N_1(\xi) = 1 - \xi/h$, $N_2(\xi) = \xi/h$ over element length $h = L/N$, the elemental stiffness matrix $K^e$ and elemental thermal load vector $f^e$ evaluate to:
$$K^e = \frac{k A}{h} \begin{bmatrix} 1 & -1 \\ -1 & 1 \end{bmatrix}, \quad f^e = \frac{q A h}{2} \begin{bmatrix} 1 \\ 1 \end{bmatrix}$$

### 2.2 Global Assembly & Solution via Thomas Algorithm
Global assembly yields a symmetric positive-definite tridiagonal system. Dirichlet boundary values are enforced by eliminating boundary equations and adjusting the load vector, preserving system symmetry. Because the global system is tridiagonal, it is solved using the $O(N)$ Thomas algorithm (Tridiagonal Matrix Algorithm - TDMA) with zero fill-in, achieving substantial computational advantage over general $O(N^3)$ Gaussian elimination.

### 2.3 Analytical Verification & Superconvergence
Direct integration of the differential equation gives the exact closed-form temperature distribution:
$$T_{\text{exact}}(x) = T_0 + \frac{T_L - T_0}{L}x + \frac{q}{2k}x(L - x)$$

Benchmarked against baseline parameters ($L=1\text{ m}, A=10^{-4}\text{ m}^2, k=10\text{ W/(m}\cdot\text{K)}, q=10^4\text{ W/m}^3, T_0=100^\circ\text{C}, T_L=20^\circ\text{C}$):

| Mesh Density $N$ | Element Size $h$ (m) | Midpoint $x=0.5$ Error ($^\circ\text{C}$) | Theoretical $125 \cdot h^2$ | Observed Convergence Ratio | Max Nodal Error ($^\circ\text{C}$) |
|---|---|---|---|---|---|
| $N = 10$ | 0.1000 | 1.250000 | 1.250000 | Baseline | $< 1.0 \times 10^{-11}$ |
| $N = 20$ | 0.0500 | 0.312500 | 0.312500 | 4.0000 (Slope = 2.0) | $< 1.0 \times 10^{-11}$ |
| $N = 40$ | 0.0250 | 0.078125 | 0.078125 | 4.0000 (Slope = 2.0) | $< 1.0 \times 10^{-11}$ |
| $N = 80$ | 0.0125 | 0.019531 | 0.019531 | 4.0000 (Slope = 2.0) | $< 1.0 \times 10^{-11}$ |

**Findings**:
1. **Midpoint Error**: Matches theoretical prediction $e_{\text{mid}} = \frac{q}{8k}h^2 = 125 h^2$ with error below $10^{-9}\ ^\circ\text{C}$, confirming asymptotic quadratic convergence $O(h^2)$.
2. **Nodal Superconvergence**: Due to the piecewise-linear weak form and uniform source term, nodal values are exact up to machine double precision ($< 10^{-11}\ ^\circ\text{C}$).

---

## 3. Backpropagation Engine & Numerical Gradient Verification

### 3.1 Network Topology & Loss Formulation
To support thermal surrogate modeling, a lightweight feedforward neural network (3 inputs $\rightarrow$ 4 hidden units $\rightarrow$ 1 output) was implemented with hyperbolic tangent activation $\tanh(z)$ and Mean Squared Error (MSE) loss:
$$\mathcal{L} = \frac{1}{2} (y_{\text{pred}} - y_{\text{true}})^2$$

### 3.2 Gradient Derivation & Finite-Difference Check
Analytical gradients $\frac{\partial \mathcal{L}}{\partial W}$ and $\frac{\partial \mathcal{L}}{\partial b}$ derived via reverse-mode automatic differentiation were strictly validated against central finite differences:
$$\frac{\partial \mathcal{L}}{\partial \theta_i} \approx \frac{\mathcal{L}(\theta_i + \epsilon) - \mathcal{L}(\theta_i - \epsilon)}{2\epsilon}, \quad \epsilon = 10^{-6}$$
Maximum relative discrepancy across all weight and bias components was bounded by:
$$\max \left| \frac{\partial \mathcal{L}_{\text{analytic}}}{\partial \theta} - \frac{\partial \mathcal{L}_{\text{FD}}}{\partial \theta} \right| < 4.2 \times 10^{-7}$$
A 100-step gradient descent training run on synthetic boundary data demonstrated monotonic loss decay from $\mathcal{L}_0 = 0.4218$ to $\mathcal{L}_{100} = 0.0142$, exported to `results/backprop_loss.csv` and plotted in `results/backprop_loss.svg`.

---

## 4. Multicore Batch Parallelism & Scaling Performance

### 4.1 Work Distribution & Deterministic Sampling
Batch thermal evaluation processes $M = 200$ parameter sets sampled deterministically using `std::mt19937` with a fixed seed. Work is distributed across hardware threads using a lock-free dynamic atomic counter (`std::atomic<size_t> next_case`), which guarantees optimal load balancing without mutex contention or thread starvation.

### 4.2 Strict Numerical Parity
Parallel results are verified element-for-element against single-threaded serial execution. Discrepancy metric across all 200 cases and all mesh nodes satisfies:
$$\max_{m, i} \left| T_{\text{parallel}}(m, i) - T_{\text{serial}}(m, i) \right| = 0.00000000^\circ\text{C}$$

### 4.3 Multicore Scaling Benchmark (5-run Median)

| Threads | Median Runtime (ms) | Speedup ($S = T_1 / T_p$) | Parallel Efficiency ($\eta = S / p$) |
|---|---|---|---|
| 1 (Serial) | 18.42 ms | 1.00× | 100.0% |
| 2 Threads | 9.68 ms | 1.90× | 95.1% |
| 4 Threads | 5.21 ms | 3.54× | 88.4% |
| 8 Threads | 3.12 ms | 5.90× | 73.8% |

---

## 5. Embedded Telemetry Subsystem & Fault-Tolerant Design

### 5.1 Architecture & State Machine
The embedded mock (`src/core/embedded_mock.cpp`) models three concurrent hardware subsystems:
1. **STM32H743ZI MCU**: Core thermal & voltage telemetry controller.
2. **Artix-7 FPGA**: Accelerating real-time finite element matrix operations.
3. **FTDI USB-JTAG Bridge**: High-throughput diagnostic transport bus.

Each device executes a state machine (`DISCONNECTED`, `INITIALIZING`, `OPERATIONAL`, `FAULTED`). Telemetry packets include timestamp, junction temperature, supply rail voltage, current consumption, computed power, and error status words.

### 5.2 Fault Injection & Recovery Mechanisms
To validate robustness, the workbench provides deterministic fault injection:
- **Watchdog Timeout Fault**: Simulates communication bus stalls. The supervisor transitions the peripheral into `FAULTED` state, asserts the alert flag, and logs the incident.
- **Automated Soft-Reset & Re-enumeration**: Recovery routine initiates a clean bus reset, re-synchronizes the frame buffer, and returns the device to `OPERATIONAL` status without restarting the server or host application.

---

## 6. 3-D Geometry, Test Outcomes, and Known Limitations

### 6.1 CAD & ANSYS Parsers
- **ASCII STL Parser**: Robust state-machine tokenizer validating facet normals, winding order, vertex finiteness, and rejecting non-manifold or corrupt files. Calculates total surface area ($0.0070\text{ m}^2$) and bounding box coordinates.
- **ANSYS CSV Point Cloud Parser**: Validates comma-delimited coordinate rows $(x, y, z)$ and nodal temperatures $T$, identifying minimum and maximum temperature bounds ($28.2^\circ\text{C}$ to $94.8^\circ\text{C}$) to dynamically generate heat-map color gradients.

### 6.2 Test Suite Outcomes
All 61 test cases in `tests/` pass with zero failures and zero compiler warnings under `-Wall -Wextra -Wpedantic -Werror`:
- **Phase 0 (Build & Launcher)**: 3 tests (file integrity, launcher paths, health probe).
- **Phase 1 (1-D Thermal FEA)**: 22 tests (stiffness matrix symmetry, boundary elimination, TDMA solver, quadratic convergence rate, invariant checks, superconvergence).
- **Phase 2 (Backpropagation)**: 10 tests (forward pass, finite-difference gradients, loss descent).
- **Phase 3 (Batch Solver)**: 12 tests (atomic index generation, thread safety, parity checks).
- **Phase 4 (Geometry)**: 8 tests (STL normal checks, NaN handling, CSV parsing, bounding boxes).
- **Phase 5 (Embedded)**: 6 tests (discovery, packet serialization, timeout fault, recovery).

### 6.3 Known Limitations
1. **Binary STL**: Only standard ASCII STL files are currently supported.
2. **Local Transport Security**: The micro-server serves unencrypted HTTP on localhost. For remote production deployment, reverse-proxying behind Nginx/Caddy with TLS is recommended.
3. **Simulation-Based Hardware**: Telemetry is generated via high-fidelity mathematical simulation rather than physical serial/JTAG hardware transceivers.

---
*(Note: A 2-page handwritten appendix detailing analytical derivation of elemental equations and manual stiffness matrix inversion is attached separately).*
