# ThermoBench

> Windows 11 x64 engineering workbench — thermal FEA, backpropagation, multicore benchmarking, hardware simulation.  
> **Internship project. All data is synthetic. No credentials stored.**

---

## Quick Start (one action)

1. **Open VS Code** in the project root.
2. Press `Ctrl+Shift+B` → choose **"CMake Build"** (first time only — ~2 min).
3. Press `F5` → choose **"Launch ThermoBench (server + browser)"**.

The browser opens automatically at `http://localhost:8080`.

---

## Prerequisites

| Tool | Version | Where to get |
|------|---------|--------------|
| MSYS2 / MinGW-w64 | GCC ≥ 13 | https://www.msys2.org |
| CMake | ≥ 3.20 | bundled with MSYS2 |
| Ninja | any | bundled with MSYS2 |
| VS Code | any | https://code.visualstudio.com |
| VS Code C/C++ extension | any | ms-vscode.cpptools |

Add `C:\msys64\mingw64\bin` to your user `PATH`.

---

## Manual Build & Test

```powershell
# From project root (PowerShell with mingw64\bin in PATH)
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
.\build\bin\RunTests.exe        # run all tests
.\build\bin\ThermoBenchServer.exe ui 8080   # start server manually
```

---

## Architecture

```
ThermoBench
├── src/core/       Pure C++17 numerics (FEA, backprop, batch, parsers, mocks)
├── src/server/     Thin HTTP server (cpp-httplib). Routes only – no maths.
├── src/launcher/   Launch.exe – starts server, opens browser, handles shutdown.
├── ui/             Plain HTML/CSS/JS (one page, 4 tabs). No build step needed.
│   └── vendor/     Chart.js 4.4.3, Three.js r167 – saved locally, no CDN.
├── tests/          Tiny test runner + per-phase test files. No external deps.
├── data/           Synthetic sample STL, CSV, PROVENANCE.md.
├── results/        Generated CSVs and plots (git-ignored).
└── docs/           Report, block diagram, AI_LOG, PROGRESS, DEPENDENCIES.
```

---

## Supported Formats

| File type | Tab | Description |
|-----------|-----|-------------|
| ASCII STL | Geometry | Triangle mesh – validates structure, rejects NaN/Inf |
| CSV (nodes) | Geometry | Columns: node_id, x_m, y_m, z_m, temperature_C |

---

## Known Limitations (Phase 0)

- Phases 1–6 not yet implemented; all tabs show placeholder text.
- Launcher opens browser but polls TCP only (does not verify HTTP 200 yet).
- No pre-built binaries — must build from source.
- Binary STL not supported (ASCII only).

---

## Licence

All original code: MIT.  
Third-party libraries: see [docs/DEPENDENCIES.md](docs/DEPENDENCIES.md).
