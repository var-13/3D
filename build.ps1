# ThermoBench build + test script
# Run from project root: .\build.ps1
# Supports existing system PATH as well as standard MSYS2/MinGW toolchain locations.

$cmakeCmd = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmakeCmd) {
    $candidates = @(
        "C:\msys64\mingw64\bin",
        "C:\msys64\ucrt64\bin"
    )
    foreach ($c in $candidates) {
        if (Test-Path "$c\cmake.exe") {
            $env:PATH = "$c;" + $env:PATH
            break
        }
    }
}

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "ERROR: CMake / MinGW toolchain not found in PATH or standard MSYS2 paths." -ForegroundColor Red
    Write-Host "Please install MSYS2 (https://www.msys2.org) or ensure cmake & g++ are on your PATH." -ForegroundColor Yellow
    exit 1
}

Write-Host ""
Write-Host "=== ThermoBench Build ===" -ForegroundColor Cyan
Write-Host "Compiler : $(g++ --version | Select-Object -First 1)"
Write-Host "CMake    : $(cmake --version | Select-Object -First 1)"
Write-Host ""

# Configure (only if build/ does not have a cache already)
if (-not (Test-Path "build\CMakeCache.txt")) {
    Write-Host "Configuring CMake (Ninja generator)..." -ForegroundColor Yellow
    cmake -S . -B build -G Ninja
    if ($LASTEXITCODE -ne 0) { 
        Write-Host "Falling back to default CMake generator..." -ForegroundColor Yellow
        cmake -S . -B build
    }
    if ($LASTEXITCODE -ne 0) { Write-Host "Configure FAILED" -ForegroundColor Red; exit 1 }
}

# Build
Write-Host "Building targets..." -ForegroundColor Yellow
cmake --build build
if ($LASTEXITCODE -ne 0) { Write-Host "Build FAILED" -ForegroundColor Red; exit 1 }

Write-Host ""
Write-Host "=== Running Tests (61/61) ===" -ForegroundColor Cyan
& ".\build\bin\RunTests.exe"
if ($LASTEXITCODE -ne 0) { Write-Host "Tests FAILED" -ForegroundColor Red; exit 1 }

Write-Host ""
Write-Host "All targets built and verified! Start the application with:" -ForegroundColor Green
Write-Host "  .\start.ps1  or  .\build\bin\Launch.exe" -ForegroundColor White
