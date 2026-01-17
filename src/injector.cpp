#include "injector.h"
#include <TlHelp32.h>
#include <filesystem>
#include <sstream>

DllInjector::DllInjector() : lastError(L"") {
}

DllInjector::~DllInjector() {
}

void DllInjector::SetError(const std::wstring& error) {
    lastError = error;
}

std::wstring DllInjector::GetLastErrorMessage() const {
    return lastError;
}

std::wstring DllInjector::GetFullDllPath(const std::wstring& dllPath) {
    wchar_t fullPath[MAX_PATH];
    DWORD result = GetFullPathNameW(dllPath.c_str(), MAX_PATH, fullPath, NULL);
    
    if (result == 0 || result >= MAX_PATH) {
        return dllPath;
    }
    
    return std::wstring(fullPath);
}

HMODULE DllInjector::GetRemoteModuleHandle(HANDLE hProcess, const std::wstring& moduleName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(hProcess));
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    MODULEENTRY32W me32;
    me32.dwSize = sizeof(MODULEENTRY32W);

    HMODULE hModule = NULL;
    if (Module32FirstW(hSnapshot, &me32)) {
        do {
            std::wstring currentModule = me32.szModule;
            if (_wcsicmp(currentModule.c_str(), moduleName.c_str()) == 0) {
                hModule = me32.hModule;
                break;
            }
        } while (Module32NextW(hSnapshot, &me32));
    }

    CloseHandle(hSnapshot);
    return hModule;
}

bool DllInjector::InjectDll(DWORD pid, const std::wstring& dllPath) {
    // Get full path to DLL
    std::wstring fullDllPath = GetFullDllPath(dllPath);

    // Check if DLL exists
    if (!std::filesystem::exists(fullDllPath)) {
        SetError(L"DLL file does not exist: " + fullDllPath);
        return false;
    }

    // Open target process
    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | 
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, pid);

    if (hProcess == NULL) {
        std::wstringstream ss;
        ss << L"Failed to open process (PID: " << pid << L"). Error code: " << GetLastError();
        SetError(ss.str());
        return false;
    }

    // Allocate memory in target process for DLL path
    size_t pathSize = (fullDllPath.length() + 1) * sizeof(wchar_t);
    LPVOID pRemotePath = VirtualAllocEx(hProcess, NULL, pathSize, 
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (pRemotePath == NULL) {
        SetError(L"Failed to allocate memory in target process");
        CloseHandle(hProcess);
        return false;
    }

    // Write DLL path to target process
    SIZE_T bytesWritten;
    if (!WriteProcessMemory(hProcess, pRemotePath, fullDllPath.c_str(), 
        pathSize, &bytesWritten)) {
        SetError(L"Failed to write DLL path to target process");
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Get address of LoadLibraryW
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (hKernel32 == NULL) {
        SetError(L"Failed to get kernel32.dll handle");
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    LPTHREAD_START_ROUTINE pLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(
        hKernel32, "LoadLibraryW");

    if (pLoadLibrary == NULL) {
        SetError(L"Failed to get LoadLibraryW address");
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Create remote thread to call LoadLibraryW
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, 
        pLoadLibrary, pRemotePath, 0, NULL);

    if (hThread == NULL) {
        SetError(L"Failed to create remote thread");
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Wait for thread to finish
    WaitForSingleObject(hThread, INFINITE);

    // Get thread exit code (module handle)
    DWORD exitCode;
    GetExitCodeThread(hThread, &exitCode);

    // Clean up
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    if (exitCode == 0) {
        SetError(L"LoadLibraryW failed in target process");
        return false;
    }

    SetError(L"");
    return true;
}

bool DllInjector::UnloadDll(DWORD pid, const std::wstring& dllName) {
    // Open target process
    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, pid);

    if (hProcess == NULL) {
        std::wstringstream ss;
        ss << L"Failed to open process (PID: " << pid << L"). Error code: " << GetLastError();
        SetError(ss.str());
        return false;
    }

    // Find module in target process
    HMODULE hModule = GetRemoteModuleHandle(hProcess, dllName);
    if (hModule == NULL) {
        SetError(L"DLL not found in target process: " + dllName);
        CloseHandle(hProcess);
        return false;
    }

    // Get address of FreeLibrary
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (hKernel32 == NULL) {
        SetError(L"Failed to get kernel32.dll handle");
        CloseHandle(hProcess);
        return false;
    }

    LPTHREAD_START_ROUTINE pFreeLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(
        hKernel32, "FreeLibrary");

    if (pFreeLibrary == NULL) {
        SetError(L"Failed to get FreeLibrary address");
        CloseHandle(hProcess);
        return false;
    }

    // Create remote thread to call FreeLibrary
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
        pFreeLibrary, hModule, 0, NULL);

    if (hThread == NULL) {
        SetError(L"Failed to create remote thread");
        CloseHandle(hProcess);
        return false;
    }

    // Wait for thread to finish
    WaitForSingleObject(hThread, INFINITE);

    // Get thread exit code
    DWORD exitCode;
    GetExitCodeThread(hThread, &exitCode);

    // Clean up
    CloseHandle(hThread);
    CloseHandle(hProcess);

    if (exitCode == 0) {
        SetError(L"FreeLibrary failed in target process");
        return false;
    }

    SetError(L"");
    return true;
}
