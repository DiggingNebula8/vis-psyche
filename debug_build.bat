@echo off
setlocal

REM Set the path to the build directory
set BUILD_DIR=build

REM Create the build directory if it doesn't exist
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Change to the build directory
pushd "%BUILD_DIR%"

REM Run CMake to configure the project for Visual Studio 2022
cmake -G "Visual Studio 17 2022" -A x64 ..

REM Build the project using the Debug configuration
cmake --build . --config Debug

REM Return to the original directory
popd

echo Build completed.
endlocal
