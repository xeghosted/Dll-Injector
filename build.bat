@echo off
setlocal enabledelayedexpansion

echo ========================================
echo   DLL Injector GUI Build Script
echo ========================================
echo.

:menu
echo Select build configuration:
echo 1. Build x64 Debug
echo 2. Build x64 Release
echo 3. Build x86 Debug
echo 4. Build x86 Release
echo 5. Build All
echo 6. Clean x64
echo 7. Clean x86
echo 8. Clean All
echo 9. Exit
echo.
set /p choice="Enter choice (1-9): "

if "%choice%"=="1" goto build_x64_debug
if "%choice%"=="2" goto build_x64_release
if "%choice%"=="3" goto build_x86_debug
if "%choice%"=="4" goto build_x86_release
if "%choice%"=="5" goto build_all
if "%choice%"=="6" goto clean_x64
if "%choice%"=="7" goto clean_x86
if "%choice%"=="8" goto clean_all
if "%choice%"=="9" goto end
echo Invalid choice!
goto menu

:build_x64_debug
echo.
echo Building x64 Debug...
cmake -S . -B build-x64 -G "Visual Studio 16 2019" -A x64
cmake --build build-x64 --config Debug
echo.
echo x64 Debug build complete!
echo Output: build-x64\bin\Debug\DllInjectorGUI.exe
pause
goto menu

:build_x64_release
echo.
echo Building x64 Release...
cmake -S . -B build-x64 -G "Visual Studio 16 2019" -A x64
cmake --build build-x64 --config Release
echo.
echo x64 Release build complete!
echo Output: build-x64\bin\Release\DllInjectorGUI.exe
pause
goto menu

:build_x86_debug
echo.
echo Building x86 Debug...
cmake -S . -B build-x86 -G "Visual Studio 16 2019" -A Win32
cmake --build build-x86 --config Debug
echo.
echo x86 Debug build complete!
echo Output: build-x86\bin\Debug\DllInjectorGUI.exe
pause
goto menu

:build_x86_release
echo.
echo Building x86 Release...
cmake -S . -B build-x86 -G "Visual Studio 16 2019" -A Win32
cmake --build build-x86 --config Release
echo.
echo x86 Release build complete!
echo Output: build-x86\bin\Release\DllInjectorGUI.exe
pause
goto menu

:build_all
echo.
echo Building all configurations...
echo.
echo [1/4] x64 Debug...
cmake -S . -B build-x64 -G "Visual Studio 16 2019" -A x64
cmake --build build-x64 --config Debug
echo.
echo [2/4] x64 Release...
cmake --build build-x64 --config Release
echo.
echo [3/4] x86 Debug...
cmake -S . -B build-x86 -G "Visual Studio 16 2019" -A Win32
cmake --build build-x86 --config Debug
echo.
echo [4/4] x86 Release...
cmake --build build-x86 --config Release
echo.
echo All builds complete!
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

:clean_all
echo.
echo Cleaning all builds...
if exist build-x64 (
    rmdir /s /q build-x64
    echo x64 build cleaned!
)
if exist build-x86 (
    rmdir /s /q build-x86
    echo x86 build cleaned!
)
echo All builds cleaned!
pause
goto menu

:end
echo.
echo Exiting...
exit /b 0
