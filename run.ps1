# ====================================================================
# SCRIPT CHAY GAME BANG 1 LENH DUY NHAT (PowerShell)
# ====================================================================

$vsPath = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath 2>$null
if (!$vsPath) {
    $vsPath = "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools"
}

$cmakeExe = Get-ChildItem -Path $vsPath -Recurse -Filter "cmake.exe" -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty FullName
$msbuildExe = Get-ChildItem -Path $vsPath -Recurse -Filter "MSBuild.exe" -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty FullName

if (!$cmakeExe -or !$msbuildExe) {
    Write-Host "[ERROR] Khong tim thay CMake hoac MSBuild!" -ForegroundColor Red
    exit 1
}

Write-Host "[INFO] Found CMake: $cmakeExe" -ForegroundColor Cyan
Write-Host "[INFO] Found MSBuild: $msbuildExe" -ForegroundColor Cyan

if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Write-Host "[INFO] Configuring project..." -ForegroundColor Cyan
& $cmakeExe -B build -S .

Write-Host "[INFO] Compiling source code..." -ForegroundColor Cyan
& $msbuildExe "build\CrossingGame.sln" /p:Configuration=Debug /v:minimal

if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Compile failed!" -ForegroundColor Red
    exit 1
}

Write-Host "[SUCCESS] Compile successful!" -ForegroundColor Green

# Kill old process if running
taskkill /F /IM CrossingGame.exe 2>$null
Write-Host "[PLAY] Starting game..." -ForegroundColor Green
Start-Process "build\bin\Debug\CrossingGame.exe"
