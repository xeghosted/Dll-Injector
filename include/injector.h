#pragma once

#include <Windows.h>
#include <string>

class DllInjector {
public:
    DllInjector();
    ~DllInjector();

    // Inject DLL into process
    bool InjectDll(DWORD pid, const std::wstring& dllPath);

    // Unload DLL from process by name
    bool UnloadDll(DWORD pid, const std::wstring& dllName);

    // Get last error message
    std::wstring GetLastErrorMessage() const;

private:
    std::wstring lastError;

    // Helper to get full DLL path
    std::wstring GetFullDllPath(const std::wstring& dllPath);

    // Helper to find module base address in remote process
    HMODULE GetRemoteModuleHandle(HANDLE hProcess, const std::wstring& moduleName);

    // Set error message
    void SetError(const std::wstring& error);
};
