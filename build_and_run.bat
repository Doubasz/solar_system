@echo off
REM === Build and run the fluid simulation ===

REM Go to the build directory (create it if missing)
if not exist build (
    mkdir build
)
cd build

REM Run CMake configuration
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

REM Build the project (Release mode by default)
cmake --build . --config Release

if errorlevel 1 (
    echo Build failed!
    cd ..
    exit /b 1
)

REM Go back to root
cd ..

REM Run the executable
if exist solar_sys.exe (
    solar_sys.exe
) else (
    echo Executable not found!
)
