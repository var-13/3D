# ThermoBench – Engineering Workbench

> **Windows 11 x64 Engineering Workbench for Thermal FEA, Backpropagation, Multicore Benchmarking, 3-D Geometry, and Hardware Simulation.**  
> Built in modern C++17 with a zero-CDN, offline-ready web frontend.

---

### 🎬 Demonstration Video Walkthrough
> 📺 **Video Link:** [**▶ Watch 3–5 Min Demonstration Video on Google Drive**](https://drive.google.com/file/d/1oRRoOZ5wvf7-aGRu-NaxsGebJjBp9HAy/view?usp=sharing)  
> *(Demonstrates automated build, test pass, 3D CAD/FEA viewport, thermal convergence, multicore benchmark, and embedded fault recovery).*

---

## 🌐 Live Deployments & Key Links

- 📁 **Google Drive Submission File:** [Google Drive Submission Link](https://drive.google.com/file/d/18Jt6KvRIcyc4wG4TsBWUNSlS6LfzkoJ6/view?usp=sharing)
- 🎬 **Demonstration Video:** [Google Drive Video Link](https://drive.google.com/file/d/1oRRoOZ5wvf7-aGRu-NaxsGebJjBp9HAy/view?usp=sharing)
- 🌐 **Live Web App (Vercel):** [https://3d-varsha.vercel.app/](https://3d-varsha.vercel.app/)
- 💻 **GitHub Repository:** [https://github.com/var-13/3D](https://github.com/var-13/3D)
- ⚙️ **CI / Automated Tests:** [GitHub Actions Workflow](https://github.com/var-13/3D/actions) (61/61 tests passing)

---

## 🚀 One-Click Quick Start (Windows)

No manual environment setup or PATH modification required. Open a PowerShell prompt in the project root:

```powershell
# 1. Configure, build all targets, and run the test suite (61/61 passing)
.\build.ps1

# 2. Start the local server
.\start.ps1
```

Then visit **`http://localhost:8080`** in any web browser.

Alternatively, double-click **`Launch.exe`** (or `.\build\bin\Launch.exe`), which starts the backend server, verifies `/health`, and automatically opens your default browser.

---

## 📋 Prerequisites

| Tool | Minimum Version | Notes |
|------|-----------------|-------|
| **C++ Compiler** | GCC ≥ 11 / Clang ≥ 13 / MSVC 2022 | C++17 support required |
| **CMake** | ≥ 3.20 | Build system generator |
| **Ninja** or **Make** | Any recent | High-speed parallel builds |
| **Browser** | Modern Evergreen Browser | Chrome, Edge, Firefox (WebGL enabled) |

On Windows, MSYS2 MinGW-w64 (`mingw-w64-x86_64-gcc`, `mingw-w64-x86_64-cmake`, `mingw-w64-x86_64-ninja`) is detected automatically by `build.ps1`.

---

## 🏗 System Architecture

ThermoBench is structured into decoupled numerical core libraries, a lightweight server layer, and an offline-first visualizer:

```
ThermoBench/
├── src/
│   ├── core/                  # Pure numerical C++17 engines (zero UI/network deps)
│   │   ├── fem1d.{hpp,cpp}        # 1-D Steady-state Heat Conduction (Thomas algorithm, O(N))
│   │   ├── backprop.{hpp,cpp}     # Gradient-descent backpropagation neural network
│   │   ├── batch_solver.{hpp,cpp} # Threaded multicore batch solver with parity checks
│   │   ├── geometry.{hpp,cpp}     # ASCII STL & ANSYS CSV nodal parsers + bounding box
│   │   └── embedded_mock.{hpp,cpp}# MCU, FPGA, and JTAG telemetry simulation & fault injection
│   ├── server/                # Lightweight HTTP REST server (cpp-httplib)
│   │   └── main.cpp               # Endpoint routing, JSON serialization, static file serving
│   └── launcher/              # Native Windows launch utility
│       └── launch.cpp             # Process supervisor, /health polling, browser launcher
├── ui/                        # Clean, responsive single-page web UI (100% offline)
│   ├── index.html                 # 4-tab interface: Geometry, Thermal, Performance, Hardware
│   ├── css/style.css              # Custom responsive dark-mode styling
│   ├── js/
│   │   ├── app.js                 # Thermal solver UI, batch benchmarks, hardware telemetry
│   │   └── viewer3d.js            # Three.js 3-D WebGL CAD & thermal point cloud viewport
│   └── vendor/                    # Local vendor libraries (zero CDN reliance)
│       ├── chart.umd.min.js       # Chart.js 4.4.3
│       └── three.module.min.js    # Three.js r167
├── tests/                     # 0-dependency native test suite
│   ├── test_runner.{hpp,cpp}      # Colorized test harness with timing and assertions
│   └── test_phase{0..5}.cpp       # 61 unit tests covering all numerical and boundary cases
├── data/                      # Synthetic benchmark inputs + provenance records
│   ├── sample.stl                 # 12-facet ASCII STL heatsink geometry
│   ├── sample_ansys.csv           # 65-node ANSYS thermal FEA nodal export
│   └── PROVENANCE.md              # Provenance & generation methodology documentation
├── results/                   # Benchmark CSV outputs & labelled SVG plots
│   ├── thermal_N{10,20,40,80}.csv # Thermal FEA profiles across mesh densities
│   ├── thermal_convergence.csv    # Spatial convergence grid error data
│   ├── benchmark_multicore.csv    # Multicore scaling speedup & efficiency numbers
│   └── backprop_loss.svg          # Labelled training loss curve
├── docs/                      # Comprehensive technical documentation & logs
│   ├── REPORT.md                  # Comprehensive Technical Report (design, FEA, embedded)
│   ├── AI_LOG.md                  # Detailed AI interaction log with validation & mistakes
│   ├── WALKTHROUGH.md             # Timestamped feature walkthrough & logs
│   ├── EMBEDDED.md                # Embedded mock system specifications & fault models
│   ├── PROGRESS.md                # Development phase-by-phase implementation log
│   └── DEPENDENCIES.md            # Third-party dependency manifest & license locks
├── .vscode/                   # Editor configuration (relative paths only)
│   ├── tasks.json                 # Configure, Build, Test, and Launch tasks
│   └── launch.json                # GDB debugger launch configurations
├── CMakeLists.txt             # Root CMake build configuration
├── vercel.json                # Vercel deployment configuration
├── build.ps1                  # One-click portable build script
└── start.ps1                  # One-click portable server runner
```

---

## 📑 Feature Modules & Tabs

| Tab / Module | Feature Description | Core Implementation |
|---|---|---|
| **📐 3-D Geometry** | Interactive WebGL viewport rendering synthetic CAD surface geometry (ASCII STL) alongside 3-D ANSYS nodal thermal cloud with interactive temperature inspector. | [`geometry.cpp`](src/core/geometry.cpp), [`viewer3d.js`](ui/js/viewer3d.js) |
| **🔥 Thermal Analysis** | 1-D Steady-state Heat Conduction solver with Dirichlet boundary conditions, analytical exact solution verification, and second-order ($O(h^2)$) spatial convergence analysis. | [`fem1d.cpp`](src/core/fem1d.cpp), [`app.js`](ui/js/app.js) |
| **⚡ Performance** | Multicore parallel batch solver benchmark across 1, 2, 4+ threads with speedup/efficiency KPIs, parity checks, and backpropagation loss training. | [`batch_solver.cpp`](src/core/batch_solver.cpp), [`backprop.cpp`](src/core/backprop.cpp) |
| **🖥 Hardware** | Embedded subsystem monitor simulating MCU, FPGA, and JTAG devices with live telemetry streaming, timeout faults, and recovery routines. | [`embedded_mock.cpp`](src/core/embedded_mock.cpp), [`main.cpp`](src/server/main.cpp) |

---

## 📂 Supported File Formats

1. **ASCII STL (`.stl`)**:
   - Triangle mesh surface representation.
   - Strict validation: rejects non-finite / NaN values, verifies face normals and winding order, computes total bounding box and surface area.
2. **ANSYS FEA CSV (`.csv`)**:
   - Columns: `node_id, x_m, y_m, z_m, temperature_C`.
   - Validates node integrity, computes spatial bounds and global temperature extrema for thermal colormapping.

---

## 🧪 Test Suite & Verification Outcomes

ThermoBench includes 61 unit tests with zero external test framework dependencies. Run all tests via:

```powershell
.\build\bin\RunTests.exe
```

### Test Summary:
```
======================================================================
  ThermoBench Test Suite Summary
======================================================================
  Phase 0: Build & Launcher Setup       :  3 /  3 PASSED
  Phase 1: 1-D Thermal FEA Solver       : 22 / 22 PASSED
  Phase 2: Backpropagation Engine       : 10 / 10 PASSED
  Phase 3: Multicore Batch Solver       : 12 / 12 PASSED
  Phase 4: STL & ANSYS Geometry Parsers :  8 /  8 PASSED
  Phase 5: Embedded Mock & Telemetry    :  6 /  6 PASSED
----------------------------------------------------------------------
  Total: 61 / 61 tests PASSED (0 failures, 0 warnings)
======================================================================
```

### Key Mathematical Verifications:
- **Spatial Convergence**: Midpoint errors on meshes $N=10, 20, 40, 80$ follow exact quadratic convergence $e_{\text{mid}} = 125 \cdot h^2$ (slope = 2.0000 on log-log plot).
- **Superconvergence**: Nodal solution matches analytical closed-form solution within numerical precision ($\max |T_{\text{FE}} - T_{\text{exact}}| < 10^{-11}\ ^\circ\text{C}$).
- **Thread Parity**: Multicore batch solver output matches single-threaded reference to zero discrepancy ($\Delta T < 10^{-12}\ ^\circ\text{C}$).

---

## ⚠️ Known Limitations

1. **Binary STL**: Currently supports standard ASCII STL; binary STL format is not yet supported.
2. **Plain HTTP**: The embedded server is designed for local / internal evaluation; it does not implement TLS/HTTPS encryption.
3. **Hardware Simulation**: Telemetry streams and faults are generated via a high-fidelity synthetic mock rather than physical bus transceivers (CAN/I2C/SPI).
4. **1-D Conduction Model**: The thermal solver models 1-D axial conduction with uniform internal heat generation.

---

## 🤖 AI-Assisted Development Log

In accordance with academic integrity and assessment guidelines, all interactions with AI coding tools (Claude Sonnet, Gemini Flash) are documented in [`docs/AI_LOG.md`](docs/AI_LOG.md).
- **Prompt Log & Model Tracking**: Exact prompts, model names, and generated code summaries.
- **Validation Evidence**: Commands executed, compilation outputs, and test pass logs.
- **Identified Mistakes & Corrections**: Documents real issues caught during development (e.g. toolchain environment mixing between MSYS2 UCRT64 vs MinGW64, missing hardware server routes, process handle management in Windows).

---

## 📜 Ownership & Provenance Statement

- **Original Work**:
  - All numerical core algorithms (`src/core/fem1d.cpp`, `backprop.cpp`, `batch_solver.cpp`, `geometry.cpp`, `embedded_mock.cpp`).
  - Server integration and route handlers (`src/server/main.cpp`).
  - Native launcher and supervisor utility (`src/launcher/launch.cpp`).
  - Custom user interface, CSS design system, and Three.js 3-D viewport logic (`ui/index.html`, `ui/css/style.css`, `ui/js/app.js`, `ui/js/viewer3d.js`).
  - Entire 61-case unit test suite (`tests/`).
  - Build automation and packaging scripts (`CMakeLists.txt`, `build.ps1`, `start.ps1`, `package.ps1`).
  - Synthetic test data (`data/sample.stl`, `data/sample_ansys.csv`).
- **Reused Third-Party Libraries** (permissible permissive open-source licenses, see [`docs/DEPENDENCIES.md`](docs/DEPENDENCIES.md)):
  - `cpp-httplib` v0.16.3 (MIT License) – single-header C++ HTTP library.
  - `Chart.js` v4.4.3 (MIT License) – client-side chart rendering.
  - `Three.js` r167 (MIT License) – client-side WebGL 3-D renderer.

---

## 📦 Submission Archive Checklist (`CandidateID_Workbench.zip`)

To package this workspace for final submission, run:
```powershell
.\package.ps1 -CandidateID "YOUR_CANDIDATE_ID"
```
This generates `CandidateID_Workbench.zip` containing:
- [x] `Launch.exe` and packaged application binaries
- [x] `src/` (complete C++ backend and frontend logic)
- [x] `CMakeLists.txt` and dependency documentation (`docs/DEPENDENCIES.md`)
- [x] `.vscode/` editor run configurations (`tasks.json`, `launch.json`) using relative paths
- [x] `data/` with provenance record (`data/PROVENANCE.md`)
- [x] `tests/` with all 61 tests
- [x] `results/` with raw CSVs and labelled plots
- [x] `docs/` (`REPORT.md`, `AI_LOG.md`, `WALKTHROUGH.md`, `EMBEDDED.md`, `PROGRESS.md`)
- [x] `README.md`
- [x] Git bundle / version-control history (`repo.bundle`)
- [x] Strict compliance: relative paths only, no credentials, no machine-specific absolute paths.
