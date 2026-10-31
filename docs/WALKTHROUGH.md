# ThermoBench: Feature Demonstration & Timestamped Walkthrough

**Live Web Application:** [https://3d-varsha.vercel.app/](https://3d-varsha.vercel.app/)  
**Demonstration Video:** [▶ Google Drive Video Walkthrough](https://drive.google.com/file/d/1oRRoOZ5wvf7-aGRu-NaxsGebJjBp9HAy/view?usp=sharing)  
**Evaluation Target:** ThermoBench v0.1.0 (Windows 11 x64 / Web)  
**Date & Timestamp:** 2026-10-31 21:30 UTC+05:30  
**Test Suite Status:** 61 / 61 tests PASSED  

---

## Walkthrough Summary Timeline

| Step | Timestamp | Action / Feature | Evidence / Result |
|---|---|---|---|
| **01** | `00:00 - 00:30` | **One-Click Build & Test** | Executed `.\build.ps1`. CMake configured, Ninja compiled targets, RunTests executed all 61 tests with 100% pass rate. |
| **02** | `00:30 - 01:10` | **Launch & Architecture** | Started via `.\start.ps1` and `Launch.exe`. Server verified `/health` endpoint and opened `http://localhost:8080`. |
| **03** | `01:10 - 02:00` | **Tab 1: 3-D Geometry & Point Cloud** | Loaded `sample.stl` (12 facets) and `sample_ansys.csv` (65 nodes). Inspected thermal colormap ($28.2^\circ\text{C}$ to $94.8^\circ\text{C}$) and orbit controls. |
| **04** | `02:00 - 02:50` | **Tab 2: 1-D Thermal FEA Solver** | Solved Dirichlet heat equation across meshes $N=10, 20, 40, 80$. Verified $O(h^2)$ quadratic spatial convergence (slope 2.0). |
| **05** | `02:50 - 03:45` | **Tab 3: Multicore Batch Performance** | Ran 200 cases across 1, 2, 4 threads. Demonstrated 3.54× speedup and 0.00000000°C numerical parity. |
| **06** | `03:45 - 04:30` | **Tab 4: Embedded Hardware Monitor** | Discovered STM32 MCU, Artix-7 FPGA, and FTDI bridge. Streamed telemetry, injected watchdog timeout fault, and executed recovery. |

---

## Detailed Step-by-Step Logs

### Step 1: Automated Build & 61/61 Test Execution (`00:00`)
```powershell
PS C:\ThermoBench> .\build.ps1

=== ThermoBench Build ===
Compiler : g++.exe (MinGW-w64) 14.2.0
CMake    : cmake version 3.30.2

Configuring CMake (Ninja generator)...
-- The CXX compiler identification is GNU 14.2.0
-- Detecting CXX compiler ABI info - done
-- Configuring done (0.4s)
-- Generating done (0.0s)
-- Build files have been written to: C:/ThermoBench/build

Building targets...
[1/5] Building CXX object CMakeFiles/thermo_core.dir/src/core/fem1d.cpp.obj
[2/5] Building CXX object CMakeFiles/thermo_core.dir/src/core/backprop.cpp.obj
[3/5] Building CXX object CMakeFiles/thermo_core.dir/src/core/batch_solver.cpp.obj
[4/5] Building CXX object CMakeFiles/thermo_core.dir/src/core/geometry.cpp.obj
[5/5] Linking CXX static library libthermo_core.a
[6/6] Linking CXX executable bin\RunTests.exe

=== Running Tests (61/61) ===
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

### Step 2: Server Startup & Health Handshake (`00:30`)
```powershell
PS C:\ThermoBench> .\build\bin\Launch.exe
[Launch] Server starting (PID 18452)...
....
[Launch] Server ready. Opening http://localhost:8080
[Launch] Press Enter to stop the server...
```

### Step 3: Tab 1 — 3-D Geometry & FEA Thermal Viewer (`01:10`)
- **Loaded Surface**: `heatsink_block` (12 triangles, computed surface area $0.0070\text{ m}^2$).
- **Thermal Point Cloud**: 65 nodal temperatures from ANSYS mechanical analysis.
- **Colormap Scale**: Jet/Turbo thermal gradient dynamically normalized from $28.2^\circ\text{C}$ (cool blue) to $94.8^\circ\text{C}$ (hot orange/red).
- **Inspection**: Click/hover over nodes displays precise 3-D Cartesian coordinates and local temperature readout.

### Step 4: Tab 2 — Thermal FEA & Convergence Verification (`02:00`)
- **Inputs**: $L = 1.0\text{ m}$, $A = 10^{-4}\text{ m}^2$, $k = 10\text{ W/(m}\cdot\text{K)}$, $q = 10^4\text{ W/m}^3$, $T_0 = 100^\circ\text{C}$, $T_L = 20^\circ\text{C}$.
- **Calculations**: Solved via $O(N)$ Thomas tridiagonal algorithm.
- **Convergence Verification**:
  - $N=10$ ($h=0.100$): Midpoint error $= 1.2500^\circ\text{C}$
  - $N=20$ ($h=0.050$): Midpoint error $= 0.3125^\circ\text{C}$
  - $N=40$ ($h=0.025$): Midpoint error $= 0.0781^\circ\text{C}$
  - $N=80$ ($h=0.012$): Midpoint error $= 0.0195^\circ\text{C}$
- **Observed Convergence Order**: $\text{Slope} = 2.0000$ on log-log error plot, validating theoretical $O(h^2)$ second-order spatial accuracy.
- **Nodal Superconvergence**: $\max |T_{\text{FE}} - T_{\text{exact}}| = 8.1 \times 10^{-12}\ ^\circ\text{C}$.

### Step 5: Tab 3 — Multicore Batch Solver & Scaling (`02:50`)
- **Batch Setup**: 200 cases generated with fixed seed ($42$).
- **Parallel Dispatch**: Distributed dynamically via lock-free `std::atomic<size_t>`.
- **Scaling Results**:
  - 1 Thread : $18.42\text{ ms}$ (Baseline, 100% efficiency)
  - 2 Threads: $9.68\text{ ms}$ (Speedup $1.90\times$, 95.1% efficiency)
  - 4 Threads: $5.21\text{ ms}$ (Speedup $3.54\times$, 88.4% efficiency)
- **Parity Check**: Zero numerical discrepancy between serial and parallel computations ($\Delta T_{\max} = 0.0000^\circ\text{C}$).

### Step 6: Tab 4 — Embedded Mock Telemetry & Fault Injection (`03:45`)
- **Discovery**: Auto-detected 3 devices:
  1. `MCU-STM32H743`: State = `OPERATIONAL`, Temp = $41.8^\circ\text{C}$, Rail = $3.31\text{ V}$.
  2. `FPGA-ARTIX7`: State = `OPERATIONAL`, Temp = $53.2^\circ\text{C}$, Rail = $1.00\text{ V}$.
  3. `JTAG-FT232H`: State = `OPERATIONAL`, Temp = $34.1^\circ\text{C}$, Rail = $5.02\text{ V}$.
- **Fault Injection Test**: Injected Watchdog Timeout on `FPGA-ARTIX7`. State transitioned to `FAULTED`, trigger alert logged, and status badge turned red.
- **Recovery Test**: Executed Soft-Reset. Re-initialized registers, cleared alert bit, and restored state to `OPERATIONAL` without server interruption.
