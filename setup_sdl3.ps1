# Script tự động setup thư viện SDL3 cho MSVC (CrossingGame)
$ErrorActionPreference = "Stop"

$workspace = "d:\Code\CrossingGame"
$tempDir = "$workspace\extern\sdl3_temp"
$destDir = "$workspace\extern\sdl3"

# Tạo các thư mục đích
New-Item -ItemType Directory -Force -Path "$destDir\include" | Out-Null
New-Item -ItemType Directory -Force -Path "$destDir\lib" | Out-Null
New-Item -ItemType Directory -Force -Path "$destDir\bin" | Out-Null
New-Item -ItemType Directory -Force -Path $tempDir | Out-Null

$libs = @(
    @{
        Name = "SDL3"
        Url = "https://github.com/libsdl-org/SDL/releases/download/release-3.4.8/SDL3-devel-3.4.8-VC.zip"
        ZipName = "SDL3-devel-3.4.8-VC.zip"
        UnzipFolder = "SDL3-3.4.8"
    },
    @{
        Name = "SDL3_image"
        Url = "https://github.com/libsdl-org/SDL_image/releases/download/release-3.4.4/SDL3_image-devel-3.4.4-VC.zip"
        ZipName = "SDL3_image-devel-3.4.4-VC.zip"
        UnzipFolder = "SDL3_image-3.4.4"
    },
    @{
        Name = "SDL3_mixer"
        Url = "https://github.com/libsdl-org/SDL_mixer/releases/download/release-3.2.2/SDL3_mixer-devel-3.2.2-VC.zip"
        ZipName = "SDL3_mixer-devel-3.2.2-VC.zip"
        UnzipFolder = "SDL3_mixer-3.2.2"
    }
)

foreach ($lib in $libs) {
    $zipPath = "$tempDir\$($lib.ZipName)"
    $extractPath = "$tempDir\$($lib.Name)_extracted"
    
    # 1. Tải file zip
    Write-Host "Đang tải $($lib.Name)..." -ForegroundColor Cyan
    Invoke-WebRequest -Uri $lib.Url -OutFile $zipPath -UserAgent "Mozilla/5.0"
    Write-Host "Tải thành công $($lib.Name)." -ForegroundColor Green
    
    # 2. Giải nén
    Write-Host "Đang giải nén $($lib.Name)..." -ForegroundColor Cyan
    Expand-Archive -Path $zipPath -DestinationPath $extractPath -Force
    
    # Tìm thư mục thực tế sau khi giải nén (ví dụ SDL3-3.4.8)
    $innerFolder = Get-ChildItem -Path $extractPath -Directory | Select-Object -First 1
    if ($null -eq $innerFolder) {
        $innerFolder = $extractPath
    }
    
    Write-Host "Đang sao chép file của $($lib.Name) vào extern/sdl3/..." -ForegroundColor Cyan
    
    # 3. Sao chép Headers (thư mục include)
    $includeSrc = Join-Path $innerFolder.FullName "include"
    if (Test-Path $includeSrc) {
        # Copy toàn bộ nội dung của include vào extern/sdl3/include
        Copy-Item -Path "$includeSrc\*" -Destination "$destDir\include" -Recurse -Force
    }
    
    # 4. Sao chép Lib (.lib) từ lib/x64
    $libSrc = Join-Path $innerFolder.FullName "lib\x64"
    if (-not (Test-Path $libSrc)) {
        # Thử đường dẫn khác nếu không có x64
        $libSrc = Join-Path $innerFolder.FullName "lib"
    }
    
    if (Test-Path $libSrc) {
        # Copy các file .lib
        Get-ChildItem -Path $libSrc -Filter "*.lib" | ForEach-Object {
            Copy-Item -Path $_.FullName -Destination "$destDir\lib" -Force
        }
        
        # Copy các file .dll sang bin
        Get-ChildItem -Path $libSrc -Filter "*.dll" | ForEach-Object {
            Copy-Item -Path $_.FullName -Destination "$destDir\bin" -Force
        }
    }
    
    Write-Host "Đã cấu trúc xong $($lib.Name)!" -ForegroundColor Green
}

# Dọn dẹp thư mục tạm
Write-Host "Đang dọn dẹp file tạm..." -ForegroundColor Yellow
Remove-Item -Path $tempDir -Recurse -Force
Write-Host "Hoàn thành toàn bộ quá trình setup thư viện!" -ForegroundColor Green
