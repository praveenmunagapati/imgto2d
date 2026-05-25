param(
    [string]$RuntimeDir = "runtime\python"
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$runtimePath = Join-Path $repoRoot $RuntimeDir
$pythonExe = Join-Path $runtimePath "python.exe"
$vpypeExe = Join-Path $runtimePath "Scripts\vpype.exe"

$zipUrl = "https://www.python.org/ftp/python/3.13.3/python-3.13.3-embed-amd64.zip"
$zipFile = Join-Path $repoRoot "python-3.13.3-embed-amd64.zip"
$getPipUrl = "https://bootstrap.pypa.io/get-pip.py"
$getPipScript = Join-Path $runtimePath "get-pip.py"

Write-Host "Setting up Python 3.13.3 embed-amd64..."

if (-not (Test-Path $pythonExe)) {
    if (Test-Path $runtimePath) {
        Write-Host "Cleaning up existing runtime directory..."
        Remove-Item -Recurse -Force $runtimePath
    }
    New-Item -ItemType Directory -Force -Path $runtimePath | Out-Null

    Write-Host "Downloading Python embeddable zip..."
    Invoke-WebRequest -Uri $zipUrl -OutFile $zipFile

    Write-Host "Extracting Python embeddable zip..."
    Expand-Archive -Path $zipFile -DestinationPath $runtimePath

    # Clean up zip
    Remove-Item $zipFile -Force
}

# Find pythonXX._pth and enable site-packages
$pthFile = Get-ChildItem -Path $runtimePath -Filter "python*._pth" | Select-Object -First 1
if ($pthFile) {
    Write-Host "Enabling site-packages in $($pthFile.Name)..."
    $content = Get-Content $pthFile.FullName
    $content = $content -replace '#\s*import site', 'import site'
    $content | Set-Content $pthFile.FullName
}

# Download get-pip.py and install pip
if (-not (Test-Path (Join-Path $runtimePath "Scripts\pip.exe"))) {
    Write-Host "Downloading get-pip.py..."
    Invoke-WebRequest -Uri $getPipUrl -OutFile $getPipScript

    Write-Host "Installing pip..."
    & $pythonExe -s $getPipScript --no-user

    # Clean up get-pip.py
    Remove-Item $getPipScript -Force
}

# Install vpype
if (-not (Test-Path $vpypeExe)) {
    Write-Host "Installing vpype..."
    & $pythonExe -s -m pip install --no-user vpype
}

Write-Host "Verifying vpype installation..."
& $vpypeExe --version

Write-Host "Portable vpype runtime ready at $runtimePath"
