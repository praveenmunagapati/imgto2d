param(
    [string]$Python = "python",
    [string]$RuntimeDir = "runtime\python"
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$runtimePath = Join-Path $repoRoot $RuntimeDir
$pythonExe = Join-Path $runtimePath "Scripts\python.exe"
$vpypeExe = Join-Path $runtimePath "Scripts\vpype.exe"

if (-not (Test-Path $pythonExe)) {
    New-Item -ItemType Directory -Force -Path (Split-Path $runtimePath) | Out-Null
    & $Python -m venv $runtimePath
}

& $pythonExe -m pip install --upgrade pip
& $pythonExe -m pip install vpype
& $vpypeExe --version

Write-Host "Portable vpype runtime ready at $runtimePath"
