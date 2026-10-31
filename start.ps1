# ThermoBench server launcher
# Run from project root: .\start.ps1

$serverPath = ".\build\bin\ThermoBenchServer.exe"

if (-not (Test-Path $serverPath)) {
    Write-Host "Server binary not found at $serverPath." -ForegroundColor Red
    Write-Host "Running build first..." -ForegroundColor Yellow
    .\build.ps1
    if ($LASTEXITCODE -ne 0) { exit 1 }
}

Write-Host "Starting ThermoBench server on http://localhost:8080 ..." -ForegroundColor Cyan
Write-Host "Serving UI from ./ui" -ForegroundColor Gray
Write-Host "Press Ctrl+C to stop." -ForegroundColor Gray
Write-Host ""
& $serverPath ui 8080
