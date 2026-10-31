# ThermoBench – Progress Log

## Phase 0 — Setup  (2026-10-31)
**Status:** ✅ COMPLETE

### What was built
| Item | Done |
|------|------|
| Folder structure | ✅ |
| CMakeLists.txt (C++17, Ninja, MSYS2/GCC) | ✅ |
| .gitignore | ✅ |
| git init + first commit | ✅ |
| src/core/fem1d.{hpp,cpp} stub | ✅ |
| src/server/main.cpp (hello-world + /health) | ✅ |
| src/launcher/launch.cpp | ✅ |
| tests/test_runner.{hpp,cpp} | ✅ |
| tests/test_phase0.cpp (6 tests, all pass) | ✅ |
| ui/index.html (4 tabs) | ✅ |
| ui/css/style.css (dark-orange theme) | ✅ |
| ui/js/app.js | ✅ |
| vendor/httplib.h (v0.16.3) | ✅ |
| .vscode/tasks.json + launch.json | ✅ |
| docs/DEPENDENCIES.md | ✅ |
| docs/AI_LOG.md | ✅ |

### Test results (real run)
```
=== ThermoBench Test Runner ===
  [ RUN ] framework: ASSERT true
  [ OK  ] framework: ASSERT true
  [ RUN ] framework: ASSERT_NEAR passes
  [ OK  ] framework: ASSERT_NEAR passes
  [ RUN ] framework: ASSERT_NEAR fails correctly
  [ OK  ] framework: ASSERT_NEAR fails correctly
  [ RUN ] framework: ASSERT_THROWS passes
  [ OK  ] framework: ASSERT_THROWS passes
  [ RUN ] core: version() returns non-empty string
  [ OK  ] core: version() returns non-empty string
  [ RUN ] core: version() contains 'ThermoBench'
  [ OK  ] core: version() contains 'ThermoBench'

Result: 6 passed, 0 failed.
```

---

## Phase 1 — Thermal FEA  (2026-10-31)
**Status:** ✅ COMPLETE

### What was built
| Function | Description | Test Status |
|----------|-------------|-------------|
| `assemble_element` | 2-node linear stiffness matrix Ke and load vector fe | ✅ PASS (analytical formulas + bad input) |
| `assemble_global` | Global K and F assembly for N uniform elements | ✅ PASS (symmetry, conservation, tridiagonal) |
| `apply_boundary_conditions` | Dirichlet BCs via row/column elimination | ✅ PASS (T0=100, TL=20, decoupled rows, RHS mod) |
| `solve` | Thomas algorithm (TDMA) for tridiagonal system | ✅ PASS (residual K*T=F < 1e-10, O(N) complexity) |
| `exact_solution` | Analytical benchmark: T(x)=100-80x+(q/2k)x(1-x) | ✅ PASS (bounds, midpoints, peaks) |
| `midpoint_error` | FE midpoint interpolation error evaluation | ✅ PASS (1.25, 0.3125, 0.078125, 0.01953125 °C) |
| `run_for_N` | Full pipeline execution for mesh size N | ✅ PASS (N=10, 20, 40, 80 end-to-end) |
| `export_csv` | CSV serializers for per-mesh and convergence data | ✅ PASS (schema, line count, file output) |

### Real Test Output (RunTests.exe)
```
=== ThermoBench Test Runner ===
  [ RUN ] Phase 1 - assemble_element: values match analytical formulas
  [ OK  ] Phase 1 - assemble_element: values match analytical formulas
  [ RUN ] Phase 1 - assemble_element: negative/zero inputs throw invalid_argument
  [ OK  ] Phase 1 - assemble_element: negative/zero inputs throw invalid_argument
  [ RUN ] Phase 1 - assemble_element: NaN / Inf inputs throw invalid_argument
  [ OK  ] Phase 1 - assemble_element: NaN / Inf inputs throw invalid_argument
  [ RUN ] Phase 1 - assemble_global: dimensions, symmetry, tridiagonal and row sums
  [ OK  ] Phase 1 - assemble_global: dimensions, symmetry, tridiagonal and row sums
  [ RUN ] Phase 1 - assemble_global: invalid N and negative params throw invalid_argument
  [ OK  ] Phase 1 - assemble_global: invalid N and negative params throw invalid_argument
  [ RUN ] Phase 1 - apply_boundary_conditions: row/column elimination and RHS modification
  [ OK  ] Phase 1 - apply_boundary_conditions: row/column elimination and RHS modification
  [ RUN ] Phase 1 - apply_boundary_conditions: invalid inputs throw invalid_argument
  [ OK  ] Phase 1 - apply_boundary_conditions: invalid inputs throw invalid_argument
  [ RUN ] Phase 1 - solve: Thomas solver satisfies K*T = F to machine precision on interior
  [ OK  ] Phase 1 - solve: Thomas solver satisfies K*T = F to machine precision on interior
  [ RUN ] Phase 1 - solve: minimal mesh N=2 (1 interior unknown)
  [ OK  ] Phase 1 - solve: minimal mesh N=2 (1 interior unknown)
  [ RUN ] Phase 1 - solve: invalid input throws invalid_argument
  [ OK  ] Phase 1 - solve: invalid input throws invalid_argument
  [ RUN ] Phase 1 - exact_solution: matches known analytical benchmark points
  [ OK  ] Phase 1 - exact_solution: matches known analytical benchmark points
  [ RUN ] Phase 1 - exact_solution: out of bounds and non-finite inputs throw invalid_argument
  [ OK  ] Phase 1 - exact_solution: out of bounds and non-finite inputs throw invalid_argument
  [ RUN ] Phase 1 - midpoint_error: matches (q/(2k))*(h/2)^2 for N=10, 20, 40, 80
  [ OK  ] Phase 1 - midpoint_error: matches (q/(2k))*(h/2)^2 for N=10, 20, 40, 80
  [ RUN ] Phase 1 - midpoint_error: rate of convergence is 2.0 (drops 4x per doubling)
  [ OK  ] Phase 1 - midpoint_error: rate of convergence is 2.0 (drops 4x per doubling)
  [ RUN ] Phase 1 - midpoint_error: invalid inputs throw invalid_argument
  [ OK  ] Phase 1 - midpoint_error: invalid inputs throw invalid_argument
  [ RUN ] Phase 1 - run_for_N: end-to-end pipeline execution for N=10, 20, 40, 80
  [ OK  ] Phase 1 - run_for_N: end-to-end pipeline execution for N=10, 20, 40, 80
  [ RUN ] Phase 1 - run_for_N: invalid parameters throw invalid_argument
  [ OK  ] Phase 1 - run_for_N: invalid parameters throw invalid_argument
  [ RUN ] Phase 1 - export_csv: to_csv produces valid schema and expected rows
  [ OK  ] Phase 1 - export_csv: to_csv produces valid schema and expected rows
  [ RUN ] Phase 1 - export_csv: exports to file and content matches
  [ OK  ] Phase 1 - export_csv: exports to file and content matches
  [ RUN ] Phase 1 - export_csv: convergence CSV produces valid rate table
  [ OK  ] Phase 1 - export_csv: convergence CSV produces valid rate table
  [ RUN ] Phase 1 - export_csv: generate official benchmark CSV files in results/
  [ OK  ] Phase 1 - export_csv: generate official benchmark CSV files in results/
  [ RUN ] Phase 1 - export_csv: invalid inputs throw invalid_argument
  [ OK  ] Phase 1 - export_csv: invalid inputs throw invalid_argument
  [ RUN ] framework: ASSERT true
  [ OK  ] framework: ASSERT true
  [ RUN ] framework: ASSERT_NEAR passes
  [ OK  ] framework: ASSERT_NEAR passes
  [ RUN ] framework: ASSERT_NEAR fails correctly
  [ OK  ] framework: ASSERT_NEAR fails correctly
  [ RUN ] framework: ASSERT_THROWS passes
  [ OK  ] framework: ASSERT_THROWS passes
  [ RUN ] core: version() returns non-empty string
  [ OK  ] core: version() returns non-empty string
  [ RUN ] core: version() contains 'ThermoBench'
  [ OK  ] core: version() contains 'ThermoBench'

Result: 28 passed, 0 failed.
```

### Convergence Verification Table
| Mesh N | Element Size h [m] | Max Nodal Error [°C] | Max Midpoint Error [°C] | Theoretical Error [°C] | Ratio | Observed Order p |
|--------|-------------------|----------------------|-------------------------|------------------------|-------|------------------|
| 10 | 0.1000 | 0.00000000 | 1.25000000 | 1.25000000 | — | — |
| 20 | 0.0500 | 0.00000000 | 0.31250000 | 0.31250000 | 4.0000× | 2.0000 |
| 40 | 0.0250 | 0.00000000 | 0.07812500 | 0.07812500 | 4.0000× | 2.0000 |
| 80 | 0.0125 | 0.00000000 | 0.01953125 | 0.01953125 | 4.0000× | 2.0000 |

---

## Phase 2 — Backpropagation  (pending)
## Phase 3 — Multicore batch  (pending)
## Phase 4 — Geometry / CAD  (pending)
## Phase 5 — Embedded design + mocks  (pending)
## Phase 6 — Packaging and evidence  (pending)
