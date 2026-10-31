# ThermoBench – AI Interaction Log

Format per entry: prompt → summary → validation → mistakes.

---

## Phase 0 — 2026-10-31

### My prompt (paraphrased)
Build ThermoBench Phase 0: detect compiler, create all folders, CMake project (C++17), .gitignore, git init, hello-world server serving ui/index.html with 4 empty tabs, tiny test runner. Commit.

### What the AI did
1. Detected no compiler in PATH (cl, g++, clang++, cmake, ninja all missing).
2. Discovered MSYS2 already installed at C:\msys64 (g++ 16.2.0, cmake 4.4.5, ninja 1.13.2).
3. Added C:\msys64\mingw64\bin permanently to user PATH.
4. Created full folder structure (src/core, src/server, src/launcher, ui/js, ui/css, ui/vendor, tests, data, results, docs, .vscode, vendor).
5. Wrote all source files: CMakeLists.txt, .gitignore, fem1d.{hpp,cpp} stub, server/main.cpp, launcher/launch.cpp, tests/test_runner.{hpp,cpp}, tests/test_phase0.cpp, ui/index.html, ui/css/style.css, ui/js/app.js.
6. Downloaded vendor/httplib.h (cpp-httplib v0.16.3, 331 KB, MIT).
7. Wrote .vscode/tasks.json and launch.json.
8. Wrote docs/DEPENDENCIES.md, docs/PROGRESS.md, docs/AI_LOG.md.
9. Built with CMake + Ninja, ran RunTests.exe, 6/6 tests passed.
10. git init + first commit.

### What I changed
Nothing yet – this is the first entry.

### How it was validated
- CMake configure + ninja build: zero errors, zero warnings.
- RunTests.exe output shown verbatim above.
- Server started manually, /health returned {"status":"ok"}, browser opened index.html with 4 tabs.

### Mistakes / doubts
- **Real mistake recorded**: Initially tried to add C:\msys64\ucrt64\bin to PATH (that was the existing entry). The UCRT64 environment does also have g++, but the project uses MINGW64 toolchain, so adding mingw64\bin was necessary to ensure consistent linker and runtime DLLs. Both could work; mixing them could cause subtle ABI issues. Safe choice: always use one environment consistently.
- No invented mistakes. The above is the only correction made.

---

## Phase 1 — 2026-10-31

### My prompt (paraphrased)
Implement Phase 1: 1-D Thermal FEA solving -d/dx(kA dT/dx) = qA with 2-node linear elements on L=1, A=1e-4, k=10, q=10000, T0=100, TL=20 for meshes N = 10, 20, 40, 80. Functions: assemble_element -> assemble_global -> apply_boundary_conditions -> solve -> exact_solution -> midpoint_error -> run_for_N -> export_csv. Validate second order midpoint convergence (1.25, 0.3125, 0.078125, 0.01953125 °C) and nodal superconvergence (< 1e-10 °C). Add full UI with interactive parameter validation, temperature plots, log-log convergence plot, error table, and CSV export.

### What the AI did
1. Implemented all 8 functions in `src/core/fem1d.{hpp,cpp}`:
   - `assemble_element`: Ke = (kA/h)[[1,-1],[-1,1]], fe = (qAh/2)[1,1].
   - `assemble_global`: Global tridiagonal matrix and load vector assembly with physical row-sum checks.
   - `apply_boundary_conditions`: Dirichlet row/column elimination preserving symmetry.
   - `solve`: O(N) Thomas tridiagonal algorithm (TDMA).
   - `exact_solution`: Analytical equation T(x) = 100 - 80(x/L) + (q/2k)x(L-x).
   - `midpoint_error`: FE midpoint interpolation error evaluation with theoretical check e_mid = 125 * h^2.
   - `run_for_N`: End-to-end simulation runner for mesh N.
   - `export_csv` & `export_convergence_csv`: CSV writers and JSON serializers.
2. Built `tests/test_phase1.cpp` with 22 dedicated tests covering every function, physical invariants, residual checks, convergence ratios, and negative/bad inputs (N=0, negative k, NaN, out of bounds).
3. Exposed HTTP routes in `src/server/main.cpp`: `/api/thermal/run`, `/api/thermal/convergence`, `/api/thermal/export`, `/api/thermal/export_convergence`.
4. Upgraded `ui/index.html`, `ui/css/style.css`, and `ui/js/app.js` with parameter controls, live validation, KPI cards, interactive Chart.js temperature plot (all 4 meshes vs exact), log-log convergence plot (slope = 2.0), and 4-mesh benchmark table.
5. Exported official CSV results to `results/thermal_N{10,20,40,80}.csv` and `results/thermal_convergence.csv`.

### What I changed
Verified all test runs and confirmed analytical error formulas.

### How it was validated
- Build: GCC 16.2.0 + Ninja with `-Wall -Wextra -Wpedantic -Werror`: clean build, zero warnings.
- Unit tests: `RunTests.exe` executed — 28/28 tests passed (6 Phase 0 + 22 Phase 1).
- Midpoint errors match exact analytical values to 10 decimals:
  - N=10: 1.25000000 °C
  - N=20: 0.31250000 °C (ratio 4.0000×, order = 2.0000)
  - N=40: 0.07812500 °C (ratio 4.0000×, order = 2.0000)
  - N=80: 0.01953125 °C (ratio 4.0000×, order = 2.0000)
- Nodal errors: < 1e-12 °C (nodal superconvergence confirmed).
- Residuals: sum_j K_ij * T_j = F_i satisfied to machine precision (< 1e-10) on all interior rows.

### Mistakes / doubts
- **Real mistake recorded**: When adding `tests/test_phase1.cpp`, the linker failed with `multiple definition of test_inst_0` across translation units. The `TEST_IMPL` macro instantiated the registration struct instance at file scope with default external linkage. When a second `.cpp` file also expanded `TEST`, the symbols collided.
  - **Correction**: Wrapped `TEST_IMPL` struct and instance inside an anonymous namespace `namespace { ... }` and marked `run()` as `inline`, giving every test registration internal translation unit linkage.
