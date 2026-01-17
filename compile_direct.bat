@echo off
echo ========================================
echo   Direct Compilation (No CMake)
echo ========================================
echo.
echo This script compiles directly using g++ (MinGW)
echo.

if not exist bin mkdir bin

echo Compiling x64 version...
g++ -std=c++17 ^
    -I include ^
    src/main.cpp src/process_manager.cpp src/injector.cpp ^
    -o bin/DllInjector_x64.exe ^
    -static-libgcc -static-libstdc++ ^
    -DUNICODE -D_UNICODE ^
    -O2

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✓ x64 build successful: bin\DllInjector_x64.exe
) else (
    echo.
    echo ✗ x64 build failed!
)

echo.
echo Compiling x86 version...
g++ -std=c++17 -m32 ^
    -I include ^
    src/main.cpp src/process_manager.cpp src/injector.cpp ^
    -o bin/DllInjector_x86.exe ^
    -static-libgcc -static-libstdc++ ^
    -DUNICODE -D_UNICODE ^
    -O2

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✓ x86 build successful: bin\DllInjector_x86.exe
) else (
    echo.
    echo ✗ x86 build failed!
    echo Note: x86 build requires multilib support
    echo Install with: mingw-get install gcc-core-multilib g++-multilib
)

echo.
echo Build complete!
pause
