#include "process_manager.h"
#include <iostream>

ProcessManager::ProcessManager() {
}

ProcessManager::~ProcessManager() {
}

bool ProcessManager::IsWow64() {
#ifdef _WIN64
    return false;
#else
    BOOL isWow64 = FALSE;
    typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
        GetModuleHandleW(L"kernel32"), "IsWow64Process");

    if (fnIsWow64Process != NULL) {
        fnIsWow64Process(GetCurrentProcess(), &isWow64);
    }
    return isWow64 != FALSE;
#endif
}

bool ProcessManager::IsProcess64Bit(DWORD pid) {
#ifdef _WIN64
    // If we're 64-bit, we can open any process
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL) {
        return false;
    }

    BOOL isWow64 = FALSE;
    typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
        GetModuleHandleW(L"kernel32"), "IsWow64Process");

    if (fnIsWow64Process != NULL) {
        fnIsWow64Process(hProcess, &isWow64);
    }

    CloseHandle(hProcess);
    // If it's WOW64, it's 32-bit running on 64-bit Windows
    return !isWow64;
#else
    // 32-bit process
    if (!IsWow64()) {
        // We're on 32-bit Windows, all processes are 32-bit
        return false;
    }

    // We're a 32-bit process on 64-bit Windows
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL) {
        return false;
    }

    BOOL isWow64 = FALSE;
    typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
        GetModuleHandleW(L"kernel32"), "IsWow64Process");

    if (fnIsWow64Process != NULL) {
        fnIsWow64Process(hProcess, &isWow64);
    }

    CloseHandle(hProcess);
    // If target is NOT WOW64, it's native 64-bit
    return !isWow64;
#endif
}

std::vector<ProcessInfo> ProcessManager::GetProcessList() {
    std::vector<ProcessInfo> processes;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return processes;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            ProcessInfo info;
            info.pid = pe32.th32ProcessID;
            info.name = pe32.szExeFile;
            info.is64Bit = IsProcess64Bit(pe32.th32ProcessID);
            processes.push_back(info);
        } while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return processes;
}

std::vector<DWORD> ProcessManager::FindProcessByName(const std::wstring& processName) {
    std::vector<DWORD> pids;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return pids;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            std::wstring currentName = pe32.szExeFile;
            // Case-insensitive comparison
            if (_wcsicmp(currentName.c_str(), processName.c_str()) == 0) {
                pids.push_back(pe32.th32ProcessID);
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return pids;
}
