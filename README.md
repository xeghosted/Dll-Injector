# DLL Injector GUI

A modern **Windows 11 styled** dark-themed GUI application for injecting and unloading DLLs in both x86 and x64 processes.

## Features

✅ **Modern Dark UI** - Beautiful Windows 11 inspired dark theme with rounded corners
✅ **Native Windows Integration** - Uses Windows 11 visual styles and theming
✅ **Process Browser** - View all running processes with search and filter capabilities
✅ **Architecture Detection** - Automatic detection and display of process architecture (x86/x64)
✅ **Compatibility Checking** - Visual indicators showing which processes are compatible
✅ **DLL Injection** - Inject DLLs using LoadLibrary technique
✅ **DLL Unloading** - Unload DLLs by name using FreeLibrary
✅ **File Browser** - Built-in file browser for selecting DLL files
✅ **Status Feedback** - Real-time status updates and detailed error messages
✅ **Search & Filter** - Search processes by name and filter by architecture
✅ **Smooth Animations** - Hover effects and visual feedback

## Screenshots

### Main Window Features:
- **Process List** with columns for PID, Name, Architecture, and Compatibility Status
- **Search Box** for quick process filtering
- **Architecture Filter** dropdown (All / x64 Only / x86 Only)
- **Refresh Button** to update the process list (F5)
- **DLL Path Input** with Browse button for injection
- **DLL Name Input** for unloading specific DLLs
- **Inject/Unload Buttons** for performing actions
- **Status Bar** showing current operation status

## Building

### Prerequisites

- **Windows 10 (1903+) or Windows 11** (for best dark theme support)
- Visual Studio 2019 or 2022 (with C++ desktop development workload)
- CMake 3.15 or higher
- VSCode (optional, but configured)

> **Note**: The dark theme and rounded corners look best on Windows 11. On Windows 10, the interface will still be dark but without rounded window corners.

### Quick Build

**Option 1: Use the build script (Easiest)**
```cmd
build.bat
```
Choose option 2 for x64 Release or option 4 for x86 Release.

**Option 2: Command line**
```cmd
# x64 Release
cmake -S . -B build-x64 -G "Visual Studio 16 2019" -A x64
cmake --build build-x64 --config Release

# x86 Release  
cmake -S . -B build-x86 -G "Visual Studio 16 2019" -A Win32
cmake --build build-x86 --config Release
```

**Option 3: VSCode**
- Open folder in VSCode
- Press `Ctrl+Shift+B` to build
- Press `F5` to run with debugger

### Build Output

The executables will be located at:
- **x64 build**: `build-x64/bin/Release/DllInjectorGUI.exe`
- **x86 build**: `build-x86/bin/Release/DllInjectorGUI.exe`

## Usage

### Getting Started

1. **Run as Administrator**
   - Right-click `DllInjectorGUI.exe` → "Run as administrator"
   - Administrator privileges are required to inject into other processes

2. **Select Target Process**
   - Browse the process list or use the search box
   - Click on a process to select it
   - Check the "Status" column for compatibility

3. **Inject a DLL**
   - Click "Browse..." to select your DLL file (or type the path)
   - Ensure the DLL architecture matches the target process
   - Click "Inject DLL"
   - A success/error message will appear

4. **Unload a DLL**
   - Select the target process
   - Enter the DLL filename (e.g., `MyMod.dll`)
   - Click "Unload DLL"

### Tips

- **Use F5** to quickly refresh the process list
- **Double-click** a process to select it
- **Use the search box** to filter processes by name
- **Use architecture filter** to show only x64 or x86 processes
- **Check the status column** to verify compatibility before injection

## Architecture Requirements

**Critical**: You must use the correct architecture build:
- Use **x64 build** (`DllInjectorGUI_x64.exe`) for 64-bit processes
- Use **x86 build** (`DllInjectorGUI_x86.exe`) for 32-bit processes

The GUI will warn you with a visual indicator and error message if there's a mismatch.

## Interface Guide

### Process List Columns

| Column | Description |
|--------|-------------|
| **PID** | Process ID |
| **Process Name** | Executable name |
| **Architecture** | x64 or x86 |
| **Status** | Compatibility check with current injector build |

### Status Indicators

- ✓ **Compatible** - Safe to inject (architecture matches)
- ✗ **Architecture mismatch** - Need different injector build

### Menu Options

**File Menu:**
- Refresh (F5) - Reload process list
- Exit - Close application

**Help Menu:**
- About - Show application information

## Technical Details

### GUI Framework
- Pure Win32 API (no external dependencies)
- **Windows 11 Dark Theme** with DWM integration
- **Rounded Corners** using DWM attributes
- Common Controls 6.0 with custom dark mode rendering
- **Modern Segoe UI font** (Windows 11 default)
- **Custom owner-draw buttons** with hover effects
- **Dark ListView** with custom colors

### Injection Method
Same as console version:
1. Opens target process with appropriate permissions
2. Allocates memory in target process
3. Writes DLL path to allocated memory
4. Creates remote thread calling `LoadLibraryW`
5. Waits for completion and cleans up

### Unloading Method
1. Searches for module in target process
2. Gets module base address (HMODULE)
3. Creates remote thread calling `FreeLibrary`
4. Waits for completion

## Project Structure

```
DllInjectorGUI/
├── CMakeLists.txt          # CMake configuration
├── build.bat               # Build script
├── .vscode/                # VSCode configuration
│   ├── tasks.json
│   └── launch.json
├── include/                # Header files
│   ├── main_window.h
│   ├── process_manager.h
│   ├── injector.h
│   └── resource.h
├── src/                    # Source files
│   ├── main.cpp
│   ├── main_window.cpp
│   ├── process_manager.cpp
│   └── injector.cpp
└── resources/              # Resource files
    └── resource.rc         # Windows resources (menu, version info)
```

## Common Issues

### "Access Denied" or "Failed to open process"
- Make sure you're running as Administrator
- Some system processes cannot be injected into
- Antivirus may block injection attempts

### "Architecture mismatch" warning
- You're using the wrong injector build
- Use x64 injector for x64 processes
- Use x86 injector for x86 processes

### "DLL not found" during injection
- Verify the DLL path is correct
- Use absolute paths
- Ensure the DLL file exists

### "LoadLibraryW failed in target process"
- The DLL may have missing dependencies
- The DLL architecture may not match the process
- The DLL may have initialization errors

### Application doesn't start
- Ensure Visual C++ Redistributable is installed
- Check Windows Event Viewer for error details

## Security Notes

⚠️ **Important Security Information:**
- Requires Administrator privileges
- Only inject into processes you own/have permission to modify
- Be careful with system processes
- Some antivirus software may flag DLL injectors

## Legitimate Use Cases

This tool is intended for:
- 🔧 Debugging your own applications
- 🎮 Modding games you own
- 🛠️ Development and testing
- 🔍 Security research in controlled environments

## License

This project is provided as-is for educational and legitimate development purposes.

## Disclaimer

Do not use this tool for malicious purposes or to inject into software you don't have permission to modify. The developers are not responsible for misuse of this software.
