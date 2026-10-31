# ThermoBench Submission Packaging Script
# Generates CandidateID_Workbench.zip adhering to all submission requirements.
# Usage: .\package.ps1 -CandidateID "YourID"

param (
    [string]$CandidateID = "CandidateID"
)

$ErrorActionPreference = "Stop"

Write-Host "=================================================================" -ForegroundColor Cyan
Write-Host "  ThermoBench Packaging: ${CandidateID}_Workbench.zip" -ForegroundColor Cyan
Write-Host "=================================================================" -ForegroundColor Cyan

# 1. Resolve compiler / cmake
$cmakeCmd = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmakeCmd) {
    if (Test-Path "C:\msys64\mingw64\bin\cmake.exe") {
        $env:PATH = "C:\msys64\mingw64\bin;" + $env:PATH
    }
}

# 2. Re-build all targets to guarantee fresh binaries
Write-Host "`n[1/5] Compiling fresh binaries..." -ForegroundColor Yellow
if (-not (Test-Path "build\CMakeCache.txt")) {
    cmake -S . -B build -G Ninja
}
cmake --build build --parallel
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed! Please resolve build errors first." -ForegroundColor Red
    exit 1
}

# 3. Verify all 61 tests pass
Write-Host "`n[2/5] Verifying 61/61 unit tests..." -ForegroundColor Yellow
& ".\build\bin\RunTests.exe"
if ($LASTEXITCODE -ne 0) {
    Write-Host "Tests failed! Aborting packaging." -ForegroundColor Red
    exit 1
}

# 4. Generate Git Bundle as standalone version-control evidence
Write-Host "`n[3/5] Exporting Git version-control evidence (repo.bundle)..." -ForegroundColor Yellow
git bundle create repo.bundle --all
if ($LASTEXITCODE -ne 0) {
    Write-Host "Git bundle creation failed." -ForegroundColor Yellow
}

# 5. Prepare clean staging directory
$stageDir = ".\staging_${CandidateID}"
if (Test-Path $stageDir) { Remove-Item -Recurse -Force $stageDir }
New-Item -ItemType Directory -Path $stageDir | Out-Null
New-Item -ItemType Directory -Path "$stageDir\bin" | Out-Null

Write-Host "`n[4/5] Staging files with relative paths (no credentials/absolute paths)..." -ForegroundColor Yellow

# Copy binaries
Copy-Item ".\build\bin\Launch.exe" -Destination "$stageDir\Launch.exe"
Copy-Item ".\build\bin\Launch.exe" -Destination "$stageDir\bin\Launch.exe"
Copy-Item ".\build\bin\ThermoBenchServer.exe" -Destination "$stageDir\bin\ThermoBenchServer.exe"
Copy-Item ".\build\bin\RunTests.exe" -Destination "$stageDir\bin\RunTests.exe"

# Copy directories
Copy-Item -Recurse ".\src" -Destination "$stageDir\src"
Copy-Item -Recurse ".\ui" -Destination "$stageDir\ui"
Copy-Item -Recurse ".\data" -Destination "$stageDir\data"
Copy-Item -Recurse ".\tests" -Destination "$stageDir\tests"
Copy-Item -Recurse ".\results" -Destination "$stageDir\results"
Copy-Item -Recurse ".\docs" -Destination "$stageDir\docs"
Copy-Item -Recurse ".\.vscode" -Destination "$stageDir\.vscode"

# Copy root configuration and scripts
Copy-Item ".\CMakeLists.txt" -Destination "$stageDir\CMakeLists.txt"
Copy-Item ".\README.md" -Destination "$stageDir\README.md"
Copy-Item ".\build.ps1" -Destination "$stageDir\build.ps1"
Copy-Item ".\start.ps1" -Destination "$stageDir\start.ps1"
Copy-Item ".\vercel.json" -Destination "$stageDir\vercel.json"
Copy-Item ".\.gitignore" -Destination "$stageDir\.gitignore"

if (Test-Path ".\repo.bundle") {
    Copy-Item ".\repo.bundle" -Destination "$stageDir\repo.bundle"
}

# 6. Compress into ZIP archive
$zipName = "${CandidateID}_Workbench.zip"
Write-Host "`n[5/5] Compressing into $zipName..." -ForegroundColor Yellow
if (Test-Path $zipName) { Remove-Item -Force $zipName }
Compress-Archive -Path "$stageDir\*" -DestinationPath $zipName -Force

# Clean staging directory
Remove-Item -Recurse -Force $stageDir

$zipItem = Get-Item $zipName
Write-Host "`n=================================================================" -ForegroundColor Green
Write-Host "  Packaging Complete: $zipName" -ForegroundColor Green
Write-Host "  Archive Size: $([math]::Round($zipItem.Length / 1MB, 2)) MB" -ForegroundColor Green
Write-Host "  Location    : $($zipItem.FullName)" -ForegroundColor Green
Write-Host "=================================================================" -ForegroundColor Green
