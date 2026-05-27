$presetsDir = "C:\Users\sir\Desktop\imgto2d\cli\presets"
$outDir = "C:\Users\sir\Desktop\imgto2d\validation_results"
$testImage = "C:\Users\sir\Desktop\imgto2d\cli\1192.jpg"
$cliExe = "C:\Users\sir\Desktop\imgto2d\dbv3_cli\build\dbv3_cli.exe"

if (!(Test-Path $outDir)) {
    New-Item -ItemType Directory -Path $outDir | Out-Null
}

$presets = Get-ChildItem -Path $presetsDir -Filter *.dbv3

$total = $presets.Count
$current = 0

foreach ($preset in $presets) {
    $current++
    $outPath = Join-Path $outDir ($preset.BaseName + ".svg")
    
    Write-Host "[$current/$total] Processing $($preset.Name)..."
    
    # Run the CLI
    $pinfo = New-Object System.Diagnostics.ProcessStartInfo
    $pinfo.FileName = $cliExe
    $pinfo.Arguments = "-c `"$($preset.FullName)`" -i `"$testImage`" -o `"$outPath`""
    $pinfo.EnvironmentVariables["PATH"] = "C:\msys64\mingw64\bin;" + [Environment]::GetEnvironmentVariable("PATH", "Process")
    $pinfo.RedirectStandardOutput = $true
    $pinfo.RedirectStandardError = $true
    $pinfo.UseShellExecute = $false
    $pinfo.CreateNoWindow = $true
    
    $p = New-Object System.Diagnostics.Process
    $p.StartInfo = $pinfo
    $p.Start() | Out-Null
    $p.WaitForExit()
    
    if ($p.ExitCode -ne 0) {
        Write-Host "ERROR on $($preset.Name): $($p.StandardError.ReadToEnd())" -ForegroundColor Red
    } else {
        if (Test-Path $outPath) {
            Write-Host "SUCCESS: Generated $(Split-Path $outPath -Leaf)" -ForegroundColor Green
        } else {
            Write-Host "FAILED to generate output for $($preset.Name)" -ForegroundColor Yellow
        }
    }
}

Write-Host "Validation complete."
