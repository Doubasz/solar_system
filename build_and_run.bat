@echo off

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 goto :fail

cmake --build build
if errorlevel 1 goto :fail

REM Shaders are loaded relative to the working directory, so run from build/
pushd build
solar_sys.exe
popd
exit /b 0

:fail
echo Build failed!
exit /b 1