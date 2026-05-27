$maxConcurrentJobs = [Environment]::ProcessorCount
$presets = Get-ChildItem -Path presets -Filter *.dbv3

$runningJobs = @()
Write-Host "Starting parallel execution with max $maxConcurrentJobs concurrent jobs..."

foreach ($preset in $presets) {
    $outName = "testing\$($preset.BaseName).svg"
    $presetFullName = $preset.FullName

    $scriptBlock = {
        param($exePath, $presetFile, $outFile, $imgFile, $workDir)
        Set-Location $workDir
        $env:PATH = "C:\msys64\mingw64\bin;" + $env:PATH
        & $exePath $presetFile $outFile $imgFile
    }

    $job = Start-Job -ScriptBlock $scriptBlock -ArgumentList ".\build\imgto2d_standalone_cli.exe", $presetFullName, $outName, "0.jpg", (Get-Location).Path
    $runningJobs += $job
    Write-Host "Started Job $($job.Id) for $($preset.Name)"

    # Throttle
    while (($runningJobs | Where-Object State -eq 'Running').Count -ge $maxConcurrentJobs) {
        Start-Sleep -Seconds 2
        # Clean up finished jobs to save memory
        $finished = $runningJobs | Where-Object State -ne 'Running'
        foreach ($f in $finished) {
            Receive-Job -Job $f -AutoRemoveJob -Wait
            $runningJobs = $runningJobs | Where-Object Id -ne $f.Id
        }
    }
}

Write-Host "All jobs started. Waiting for completion..."
while (($runningJobs | Where-Object State -eq 'Running').Count -gt 0) {
    Start-Sleep -Seconds 5
    $finished = $runningJobs | Where-Object State -ne 'Running'
    foreach ($f in $finished) {
        Receive-Job -Job $f -AutoRemoveJob -Wait
        $runningJobs = $runningJobs | Where-Object Id -ne $f.Id
    }
}

Write-Host "All tests completed!"
