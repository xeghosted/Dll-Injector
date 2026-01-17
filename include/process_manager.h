#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>

struct ProcessInfo {
    DWORD pid;
    std::wstring name;
    bool is64Bit;
};

class ProcessManager {
public:
    ProcessManager();
    ~ProcessManager();

    // Get list of all running processes
    std::vector<ProcessInfo> GetProcessList();

    // Check if a process is 64-bit
    static bool IsProcess64Bit(DWORD pid);

    // Find process by name
    std::vector<DWORD> FindProcessByName(const std::wstring& processName);

private:
    // Check if current process is 64-bit
    static bool IsWow64();
};
