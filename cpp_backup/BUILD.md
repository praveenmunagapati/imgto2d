# imgto2d — C++/Qt6 Build Instructions

## Prerequisites

### 1. Qt6 (C++ dev headers required)

The Python `PyQt6` in your venv only has runtime DLLs, **not** the C++ headers.

**Option A — Qt Online Installer (recommended, free for open source):**
1. Download from: https://www.qt.io/download-qt-installer
2. Install **Qt 6.9** → **MSVC 2019 64-bit** component
3. Note the install path (e.g. `C:\Qt\6.9.0\msvc2022_64`)

**Option B — Chocolatey (command line):**
```powershell
choco install qt6-default -y
```

**Option C — winget:**
```powershell
winget install Qt.Qt.6.9.0
```

### 2. OpenCV (C++ dev headers required)

The Python `cv2` in your venv only has Python bindings, **not** the C++ headers.

**Option A — Chocolatey:**
```powershell
choco install opencv -y
# Sets OPENCV_DIR automatically
```

**Option B — vcpkg:**
```powershell
vcpkg install opencv4:x64-windows
```

**Option C — Manual:** Download from https://opencv.org/releases/ and extract.

### 3. Visual Studio 2019/2022 Build Tools
Already installed (MSVC 19.29 detected). ✅

---

## Build

```powershell
# From the cpp/ directory:
cmake -B build ^
  -DCMAKE_PREFIX_PATH="C:\Qt\6.9.0\msvc2022_64" ^
  -DOpenCV_DIR="C:\tools\opencv\build"

cmake --build build --config Release
```

### Quick build script
Run `build.bat` (in this folder) which prompts for Qt path.

---

## Run

```powershell
.\build\Release\imgto2d_qt.exe
```
