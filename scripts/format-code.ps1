# Format all C++ source files with clang-format
# Run this before committing to avoid CI failures

$ErrorActionPreference = "Stop"

# Check if clang-format is installed
$clangFormat = Get-Command clang-format -ErrorAction SilentlyContinue
if (-not $clangFormat) {
    Write-Host "clang-format not found. Installing via winget..." -ForegroundColor Yellow
    winget install LLVM.LLVM
    Write-Host "Please restart PowerShell and run this script again." -ForegroundColor Yellow
    exit 1
}

# Find all source files
$srcPath = Join-Path $PSScriptRoot "..\src"
$files = Get-ChildItem -Path $srcPath -Recurse -Include "*.h", "*.c", "*.cpp" |
    Where-Object { 
        $_.FullName -notmatch "spatialmedia" -and 
        $_.Name -ne "defaultlayouts.h" -and
        $_.FullName -notmatch "_autogen"
    }

Write-Host "Formatting $($files.Count) files..." -ForegroundColor Cyan

foreach ($file in $files) {
    clang-format -style=file -i $file.FullName
}

Write-Host "Done! All files formatted." -ForegroundColor Green
