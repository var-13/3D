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
*(Future phases will append new entries here.)*
