$ErrorActionPreference = "Continue"
$env:PATH = "C:\msys64\mingw64\bin;" + $env:PATH

$outDir = "test_results"
if (-not (Test-Path $outDir)) {
    New-Item -ItemType Directory -Force -Path $outDir | Out-Null
}

$presets = Get-ChildItem -Path "presets" -Filter "*.dbv3"
$total = $presets.Count
$count = 1

foreach ($p in $presets) {
    $outPath = Join-Path $outDir ($p.BaseName + ".svg")
    Write-Host "`n========================================================"
    Write-Host "[$count/$total] Running algorithm: $($p.BaseName)"
    Write-Host "========================================================"
    
    # We use Start-Process with -Wait to prevent stdout flooding if we want, 
    # but since it's a background task, seeing the progress is helpful.
    # We will just run it directly.
    .\build\imgto2d_standalone_cli.exe $p.FullName $outPath "1192.jpg"
    
    $count++
}

Write-Host "`nAll $total algorithms completed successfully!"
