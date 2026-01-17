# Example Test DLL

This is a simple example DLL you can use to test the injector.

## Building the Example DLL

### Simple MessageBox DLL

Create a file `test_dll.cpp`:

```cpp
#include <Windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        MessageBoxA(NULL, "DLL Injected Successfully!", "Test DLL", MB_OK | MB_ICONINFORMATION);
        break;
    case DLL_PROCESS_DETACH:
        MessageBoxA(NULL, "DLL Unloaded!", "Test DLL", MB_OK | MB_ICONINFORMATION);
        break;
    }
    return TRUE;
}
```

### Build with Visual Studio:

#### x64 DLL
```cmd
cl /LD /D_USRDLL /D_WINDLL test_dll.cpp /Fe:test_dll_x64.dll
```

#### x86 DLL
```cmd
cl /LD /D_USRDLL /D_WINDLL test_dll.cpp /Fe:test_dll_x86.dll
```

### Build with CMake (Advanced)

Create `CMakeLists.txt` for the test DLL:

```cmake
cmake_minimum_required(VERSION 3.15)
project(TestDLL)

add_library(test_dll SHARED test_dll.cpp)

set_target_properties(test_dll PROPERTIES
    OUTPUT_NAME "test_dll"
    PREFIX ""
)
```

Then build:
```cmd
# x64
cmake -S . -B build-x64 -G "Visual Studio 17 2022" -A x64
cmake --build build-x64 --config Release

# x86
cmake -S . -B build-x86 -G "Visual Studio 17 2022" -A Win32
cmake --build build-x86 --config Release
```

## Testing the Injector

1. Build the test DLL (choose x86 or x64 based on your target)
2. Run `notepad.exe`
3. Run the DLL Injector as Administrator
4. Select "Inject DLL"
5. Search for `notepad.exe`
6. Enter the path to your test DLL
7. You should see a message box appear in the notepad process
8. To unload:
   - Select "Unload DLL"
   - Search for `notepad.exe`
   - Enter `test_dll_x64.dll` (or `test_dll_x86.dll`)
   - You should see the unload message box

## Important Notes

- Use the **x64 DLL** with **64-bit notepad** (on 64-bit Windows)
- Use the **x86 DLL** with **32-bit notepad** (if you specifically run the 32-bit version)
- Make sure the DLL architecture matches the target process architecture
- Always test with simple programs like notepad first
