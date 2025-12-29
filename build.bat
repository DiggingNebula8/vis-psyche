@echo off
setlocal EnableDelayedExpansion

echo ============================================
echo   VizPsyche Build Script
echo ============================================
echo.

REM Parse command line arguments
set BUILD_TYPE=Release
set CLEAN_BUILD=0

:parse_args
if "%~1"=="" goto :done_parsing
if /i "%~1"=="debug" set BUILD_TYPE=Debug
if /i "%~1"=="release" set BUILD_TYPE=Release
if /i "%~1"=="clean" set CLEAN_BUILD=1
shift
goto :parse_args
:done_parsing

REM Set the path to the build directory
set BUILD_DIR=build

REM Clean build if requested
if %CLEAN_BUILD%==1 (
    echo Cleaning build directory...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)

REM Create the build directory if it doesn't exist
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Change to the build directory
pushd "%BUILD_DIR%"

REM Auto-detect Visual Studio version using vswhere
set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if exist %VSWHERE% (
    for /f "tokens=1,2 delims=." %%a in ('call %VSWHERE% -latest -property catalog_productLineVersion 2^>nul') do (
        set VS_YEAR=%%a
    )
    
    REM Map year to CMake generator
    if "!VS_YEAR!"=="2026" (
        set CMAKE_GENERATOR=Visual Studio 18 2026
    ) else if "!VS_YEAR!"=="2022" (
        set CMAKE_GENERATOR=Visual Studio 17 2022
    ) else if "!VS_YEAR!"=="2019" (
        set CMAKE_GENERATOR=Visual Studio 16 2019
    ) else (
        echo Warning: Could not detect VS version, trying default generator...
        set CMAKE_GENERATOR=
    )
) else (
    echo Warning: vswhere not found, trying Ninja...
    where ninja >nul 2>&1
    if !errorlevel! equ 0 (
        set CMAKE_GENERATOR=Ninja
    ) else (
        set CMAKE_GENERATOR=
    )
)

echo Configuration: !BUILD_TYPE!
echo Generator: !CMAKE_GENERATOR!
echo.

REM Run CMake to configure the project
if defined CMAKE_GENERATOR (
    cmake -G "!CMAKE_GENERATOR!" -A x64 -DCMAKE_BUILD_TYPE=!BUILD_TYPE! ..
) else (
    cmake -DCMAKE_BUILD_TYPE=!BUILD_TYPE! ..
)

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] CMake configuration failed!
    popd
    pause
    exit /b 1
)

echo.
echo Building !BUILD_TYPE! configuration...
echo.

REM Build the project
cmake --build . --config !BUILD_TYPE!

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Build failed!
    popd
    pause
    exit /b 1
)

REM Return to the original directory
popd

echo.
echo ============================================
echo   Build completed successfully!
echo ============================================
echo.
echo Output: build\bin\!BUILD_TYPE!\
echo   - VizEngine.dll
echo   - Sandbox.exe
echo.
echo To run: build\bin\!BUILD_TYPE!\Sandbox.exe
echo.

endlocal

