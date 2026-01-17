# DLL Injector/Unloader

A powerful DLL injection and unloading tool that supports both x86 and x64 processes on Windows.

## Features

- ✅ **Process Listing**: View all running processes with their PID, name, and architecture (x86/x64)
- ✅ **DLL Injection**: Inject DLLs into running processes using LoadLibrary technique
- ✅ **DLL Unloading**: Unload DLLs from running processes by name using FreeLibrary
- ✅ **Architecture Support**: Separate builds for x86 and x64 processes
- ✅ **Process Search**: Find processes by name or PID
- ✅ **Architecture Detection**: Automatically detects if target process is 32-bit or 64-bit
- ✅ **Interactive Menu**: User-friendly command-line interface

## Architecture Requirements

**Important**: You must use the correct architecture build for your target process:
- Use the **x64 build** to inject into 64-bit processes
- Use the **x86 build** to inject into 32-bit processes

The tool will warn you if there's an architecture mismatch.

## Building

### Prerequisites

- Windows 10/11
- Visual Studio 2019 or 2022 (with C++ desktop development workload)
- CMake 3.15 or higher
- VSCode (optional, but configured)

### Build from Command Line

**Note:** The project is configured for Visual Studio 2019. If you have VS2022, change `"Visual Studio 16 2019"` to `"Visual Studio 17 2022"` in the commands below.

#### x64 Build
```cmd
# Configure
cmake -S . -B build-x64 -G "Visual Studio 16 2019" -A x64

# Build Debug
cmake --build build-x64 --config Debug

# Build Release
cmake --build build-x64 --config Release
```

#### x86 Build
```cmd
# Configure
cmake -S . -B build-x86 -G "Visual Studio 16 2019" -A Win32

# Build Debug
cmake --build build-x86 --config Debug

# Build Release
cmake --build build-x86 --config Release
```

### Build from VSCode

The project is fully configured for VSCode:

1. Open the project folder in VSCode
2. Press `Ctrl+Shift+P` and select:
   - `Tasks: Run Build Task` to build (default: x64 Release)
   - Or select specific tasks:
     - `Build x64 Debug`
     - `Build x64 Release`
     - `Build x86 Debug`
     - `Build x86 Release`

### Running from VSCode

Press `F5` and select:
- `Debug x64` - Run x64 debug build
- `Debug x86` - Run x86 debug build
- `Release x64` - Run x64 release build
- `Release x86` - Run x86 release build

## Usage

### Running the Application

**Important**: The injector requires **Administrator privileges** to inject into other processes.

1. Run the executable as Administrator:
   - Right-click `DllInjector.exe` → "Run as administrator"

2. The main menu will appear:
   ```
   Menu:
   1. List all processes
   2. Inject DLL
   3. Unload DLL
   4. Exit
   ```

### Injecting a DLL

1. Select option `2` from the menu
2. Enter the target process PID (or `0` to search by name)
3. If searching by name:
   - Enter the process name (e.g., `notepad.exe`)
   - Select from found processes if multiple exist
4. Enter the full path to your DLL file
   - Example: `C:\MyDlls\MyMod.dll`
   - You can drag-and-drop the DLL file into the console
5. The tool will inject the DLL and report success/failure

### Unloading a DLL

1. Select option `3` from the menu
2. Enter the target process PID (or `0` to search by name)
3. If searching by name:
   - Enter the process name (e.g., `notepad.exe`)
   - Select from found processes if multiple exist
4. Enter the DLL name to unload (e.g., `MyMod.dll`)
   - This is just the filename, not the full path
5. The tool will unload the DLL and report success/failure

### Listing Processes

1. Select option `1` from the menu
2. View all running processes with:
   - Process ID (PID)
   - Process name
   - Architecture (x86 or x64)

## Project Structure

```
DllInjector/
├── CMakeLists.txt          # Main CMake configuration
├── .vscode/                # VSCode configuration
│   ├── tasks.json          # Build tasks
│   ├── launch.json         # Debug configurations
│   ├── c_cpp_properties.json
│   └── settings.json
├── include/                # Header files
│   ├── process_manager.h
│   └── injector.h
├── src/                    # Source files
│   ├── main.cpp
│   ├── process_manager.cpp
│   └── injector.cpp
├── build-x64/             # x64 build output (generated)
└── build-x86/             # x86 build output (generated)
```

## Technical Details

### Injection Method

The injector uses the classic **LoadLibrary** technique:
1. Opens the target process with appropriate permissions
2. Allocates memory in the target process
3. Writes the DLL path to the allocated memory
4. Creates a remote thread that calls `LoadLibraryW`
5. Waits for the thread to complete
6. Cleans up allocated resources

### Unloading Method

The unloader uses the **FreeLibrary** technique:
1. Opens the target process
2. Searches for the module in the target process's module list
3. Gets the module's base address (HMODULE)
4. Creates a remote thread that calls `FreeLibrary` with the module handle
5. Waits for completion

### Architecture Detection

The tool uses Windows API functions to detect process architecture:
- `IsWow64Process` - Determines if a process is running under WOW64 (32-bit on 64-bit Windows)
- Processes are marked as x64 if they're native 64-bit
- Processes are marked as x86 if they're 32-bit

## Common Issues

### "Failed to open process"
- Make sure you're running as Administrator
- The target process may have higher privileges
- Some system processes cannot be injected

### "Architecture mismatch"
- You're trying to inject an x86 DLL into an x64 process (or vice versa)
- Use the correct build of the injector (x64 for x64 processes, x86 for x86 processes)

### "DLL file does not exist"
- Check that the DLL path is correct
- Use absolute paths (e.g., `C:\path\to\your.dll`)
- Ensure the DLL file exists and is accessible

### "Failed to inject DLL" / "LoadLibraryW failed"
- The DLL may have dependencies that aren't available in the target process
- The DLL may not be compatible with the target process
- Check that the DLL architecture matches the target process

### "DLL not found in target process"
- The DLL name must match exactly (case-insensitive)
- Include the `.dll` extension
- The DLL may have already been unloaded

## Security Notes

- This tool requires Administrator privileges
- Only inject DLLs into processes you own or have permission to modify
- Be careful with system processes
- Some antivirus software may flag DLL injectors as potentially unwanted

## License

This project is provided as-is for educational and legitimate development purposes.

## Disclaimer

This tool is intended for legitimate purposes such as:
- Debugging your own applications
- Modding games you own
- Development and testing
- Security research in controlled environments

Do not use this tool for malicious purposes or to inject into software you don't have permission to modify.
