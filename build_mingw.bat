@echo off
setlocal enabledelayedexpansion

echo ========================================
echo   DLL Injector Build Script (MinGW)
echo ========================================
echo.

:menu
echo Select build configuration:
echo 1. Build x64 Debug (MinGW)
echo 2. Build x64 Release (MinGW)
echo 3. Build x86 Debug (MinGW)
echo 4. Build x86 Release (MinGW)
echo 5. Clean x64
echo 6. Clean x86
echo 7. Exit
echo.
set /p choice="Enter choice (1-7): "

if "%choice%"=="1" goto build_x64_debug
if "%choice%"=="2" goto build_x64_release
if "%choice%"=="3" goto build_x86_debug
if "%choice%"=="4" goto build_x86_release
if "%choice%"=="5" goto clean_x64
if "%choice%"=="6" goto clean_x86
if "%choice%"=="7" goto end
echo Invalid choice!
goto menu

:build_x64_debug
echo.
echo Building x64 Debug with MinGW...
cmake -S . -B build-x64 -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build-x64
echo.
echo x64 Debug build complete!
echo Output: build-x64\bin\DllInjector.exe
pause
goto menu

:build_x64_release
echo.
echo Building x64 Release with MinGW...
cmake -S . -B build-x64 -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build-x64
echo.
echo x64 Release build complete!
echo Output: build-x64\bin\DllInjector.exe
pause
goto menu

:build_x86_debug
echo.
echo Building x86 Debug with MinGW...
cmake -S . -B build-x86 -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-m32" -DCMAKE_CXX_FLAGS="-m32"
cmake --build build-x86
echo.
echo x86 Debug build complete!
echo Output: build-x86\bin\DllInjector.exe
pause
goto menu

:build_x86_release
echo.
echo Building x86 Release with MinGW...
cmake -S . -B build-x86 -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-m32" -DCMAKE_CXX_FLAGS="-m32"
cmake --build build-x86
echo.
echo x86 Release build complete!
echo Output: build-x86\bin\DllInjector.exe
pause
goto menu

:clean_x64
echo.
echo Cleaning x64 build...
if exist build-x64 (
    rmdir /s /q build-x64
    echo x64 build cleaned!
) else (
    echo No x64 build directory found.
)
pause
goto menu

:clean_x86
echo.
echo Cleaning x86 build...
if exist build-x86 (
    rmdir /s /q build-x86
    echo x86 build cleaned!
) else (
    echo No x86 build directory found.
)
pause
goto menu

:end
echo.
echo Exiting...
exit /b 0
