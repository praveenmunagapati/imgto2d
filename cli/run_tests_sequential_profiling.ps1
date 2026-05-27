$presets = Get-ChildItem -Path presets -Filter *.dbv3
$results = @()
$testImage = "0.jpg"
$outDir = "testing"

if (!(Test-Path $outDir)) {
    New-Item -ItemType Directory -Force -Path $outDir
}

$env:PATH = "C:\msys64\mingw64\bin;" + $env:PATH

Write-Host "Starting sequential profiling of $(@($presets).Count) algorithms..."

foreach ($preset in $presets) {
    $outName = "$outDir\$($preset.BaseName).svg"
    Write-Host "Profiling $($preset.Name)..."
    
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    & .\build\imgto2d_standalone_cli.exe $preset.FullName $outName $testImage
    $sw.Stop()
    
    $fileSizeMB = 0
    if (Test-Path $outName) {
        $fileSizeMB = [math]::Round((Get-Item $outName).Length / 1MB, 2)
    }
    
    $results += [PSCustomObject]@{
        Algorithm = $preset.BaseName
        TimeSeconds = [math]::Round($sw.Elapsed.TotalSeconds, 2)
        SizeMB = $fileSizeMB
    }
    Write-Host "-> Finished in $($results[-1].TimeSeconds)s. Size: $($fileSizeMB)MB"
    
    # Save incrementally in case it crashes or is stopped
    $results | Export-Csv -Path testing_profile_results.csv -NoTypeInformation
}

Write-Host "Profiling complete! Results saved to testing_profile_results.csv."
