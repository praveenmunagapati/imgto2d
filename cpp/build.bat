@echo off
REM imgto2d Qt build script
REM Usage: build.bat [Qt6-prefix] [OpenCV-dir]
REM Example: build.bat "C:\Qt\6.9.0\msvc2022_64" "C:\tools\opencv\build"

SET QT_PREFIX=%~1
SET OPENCV_DIR=%~2

IF "%QT_PREFIX%"=="" (
    SET QT_PREFIX=C:\Qt\6.9.0\msvc2022_64
    echo Using default Qt prefix: %QT_PREFIX%
)

IF "%OPENCV_DIR%"=="" (
    SET OPENCV_DIR=C:\tools\opencv\build
    echo Using default OpenCV dir: %OPENCV_DIR%
)

echo.
echo =============================================
echo  imgto2d C++/Qt6 Build
echo  Qt prefix : %QT_PREFIX%
echo  OpenCV dir: %OPENCV_DIR%
echo =============================================
echo.

cmake -B build ^
  -DCMAKE_PREFIX_PATH="%QT_PREFIX%" ^
  -DOpenCV_DIR="%OPENCV_DIR%" ^
  -DCMAKE_BUILD_TYPE=Release

IF %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] CMake configure failed. Check Qt and OpenCV paths.
    pause
    exit /b 1
)

cmake --build build --config Release --parallel

IF %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo.
echo =============================================
echo  Build complete!
echo  Run: build\Release\imgto2d_qt.exe
echo =============================================
pause
