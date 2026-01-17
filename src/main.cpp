#include "process_manager.h"
#include "injector.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <io.h>
#include <fcntl.h>

void PrintBanner() {
    std::wcout << L"========================================" << std::endl;
    std::wcout << L"     DLL Injector/Unloader v1.0        " << std::endl;
    std::wcout << L"     Supports x86 and x64 processes    " << std::endl;
    std::wcout << L"========================================" << std::endl;
    std::wcout << std::endl;
}

void PrintMenu() {
    std::wcout << L"\nMenu:" << std::endl;
    std::wcout << L"1. List all processes" << std::endl;
    std::wcout << L"2. Inject DLL" << std::endl;
    std::wcout << L"3. Unload DLL" << std::endl;
    std::wcout << L"4. Exit" << std::endl;
    std::wcout << L"\nChoice: ";
}

void ListProcesses(ProcessManager& pm) {
    std::wcout << L"\nScanning processes..." << std::endl;
    auto processes = pm.GetProcessList();

    if (processes.empty()) {
        std::wcout << L"No processes found!" << std::endl;
        return;
    }

    std::wcout << L"\n" << std::setw(8) << std::left << L"PID"
               << std::setw(40) << std::left << L"Process Name"
               << std::setw(10) << std::left << L"Arch" << std::endl;
    std::wcout << std::wstring(60, L'-') << std::endl;

    for (const auto& proc : processes) {
        std::wcout << std::setw(8) << std::left << proc.pid
                   << std::setw(40) << std::left << proc.name
                   << std::setw(10) << std::left << (proc.is64Bit ? L"x64" : L"x86")
                   << std::endl;
    }

    std::wcout << L"\nTotal processes: " << processes.size() << std::endl;
}

void InjectDllMenu(ProcessManager& pm, DllInjector& injector) {
    std::wcout << L"\n=== Inject DLL ===" << std::endl;

    // Get PID
    std::wcout << L"Enter target process PID (or 0 to search by name): ";
    DWORD pid;
    std::wcin >> pid;
    std::wcin.ignore();

    if (pid == 0) {
        std::wcout << L"Enter process name (e.g., notepad.exe): ";
        std::wstring processName;
        std::getline(std::wcin, processName);

        auto pids = pm.FindProcessByName(processName);
        if (pids.empty()) {
            std::wcout << L"Process not found!" << std::endl;
            return;
        }

        std::wcout << L"\nFound " << pids.size() << L" process(es):" << std::endl;
        for (size_t i = 0; i < pids.size(); i++) {
            bool is64 = ProcessManager::IsProcess64Bit(pids[i]);
            std::wcout << L"  [" << i << L"] PID: " << pids[i] 
                       << L" (" << (is64 ? L"x64" : L"x86") << L")" << std::endl;
        }

        if (pids.size() > 1) {
            std::wcout << L"Select index: ";
            size_t index;
            std::wcin >> index;
            std::wcin.ignore();

            if (index >= pids.size()) {
                std::wcout << L"Invalid index!" << std::endl;
                return;
            }
            pid = pids[index];
        } else {
            pid = pids[0];
        }
    }

    // Check process architecture
    bool targetIs64 = ProcessManager::IsProcess64Bit(pid);
    std::wcout << L"\nTarget process: PID " << pid << L" (" 
               << (targetIs64 ? L"x64" : L"x86") << L")" << std::endl;

#ifdef _WIN64
    bool injectorIs64 = true;
#else
    bool injectorIs64 = false;
#endif

    if (targetIs64 != injectorIs64) {
        std::wcout << L"\nWARNING: Architecture mismatch!" << std::endl;
        std::wcout << L"  Injector: " << (injectorIs64 ? L"x64" : L"x86") << std::endl;
        std::wcout << L"  Target:   " << (targetIs64 ? L"x64" : L"x86") << std::endl;
        std::wcout << L"\nYou need to use the " << (targetIs64 ? L"x64" : L"x86") 
                   << L" version of this injector!" << std::endl;
        return;
    }

    // Get DLL path
    std::wcout << L"Enter full path to DLL: ";
    std::wstring dllPath;
    std::getline(std::wcin, dllPath);

    // Remove quotes if present
    if (!dllPath.empty() && dllPath.front() == L'"' && dllPath.back() == L'"') {
        dllPath = dllPath.substr(1, dllPath.length() - 2);
    }

    // Inject
    std::wcout << L"\nInjecting..." << std::endl;
    if (injector.InjectDll(pid, dllPath)) {
        std::wcout << L"Successfully injected DLL!" << std::endl;
    } else {
        std::wcout << L"Failed to inject DLL!" << std::endl;
        std::wcout << L"Error: " << injector.GetLastErrorMessage() << std::endl;
    }
}

void UnloadDllMenu(ProcessManager& pm, DllInjector& injector) {
    std::wcout << L"\n=== Unload DLL ===" << std::endl;

    // Get PID
    std::wcout << L"Enter target process PID (or 0 to search by name): ";
    DWORD pid;
    std::wcin >> pid;
    std::wcin.ignore();

    if (pid == 0) {
        std::wcout << L"Enter process name (e.g., notepad.exe): ";
        std::wstring processName;
        std::getline(std::wcin, processName);

        auto pids = pm.FindProcessByName(processName);
        if (pids.empty()) {
            std::wcout << L"Process not found!" << std::endl;
            return;
        }

        std::wcout << L"\nFound " << pids.size() << L" process(es):" << std::endl;
        for (size_t i = 0; i < pids.size(); i++) {
            bool is64 = ProcessManager::IsProcess64Bit(pids[i]);
            std::wcout << L"  [" << i << L"] PID: " << pids[i]
                       << L" (" << (is64 ? L"x64" : L"x86") << L")" << std::endl;
        }

        if (pids.size() > 1) {
            std::wcout << L"Select index: ";
            size_t index;
            std::wcin >> index;
            std::wcin.ignore();

            if (index >= pids.size()) {
                std::wcout << L"Invalid index!" << std::endl;
                return;
            }
            pid = pids[index];
        } else {
            pid = pids[0];
        }
    }

    // Check process architecture
    bool targetIs64 = ProcessManager::IsProcess64Bit(pid);
    std::wcout << L"\nTarget process: PID " << pid << L" ("
               << (targetIs64 ? L"x64" : L"x86") << L")" << std::endl;

#ifdef _WIN64
    bool injectorIs64 = true;
#else
    bool injectorIs64 = false;
#endif

    if (targetIs64 != injectorIs64) {
        std::wcout << L"\nWARNING: Architecture mismatch!" << std::endl;
        std::wcout << L"  Injector: " << (injectorIs64 ? L"x64" : L"x86") << std::endl;
        std::wcout << L"  Target:   " << (targetIs64 ? L"x64" : L"x86") << std::endl;
        std::wcout << L"\nYou need to use the " << (targetIs64 ? L"x64" : L"x86")
                   << L" version of this injector!" << std::endl;
        return;
    }

    // Get DLL name
    std::wcout << L"Enter DLL name (e.g., MyDll.dll): ";
    std::wstring dllName;
    std::getline(std::wcin, dllName);

    // Unload
    std::wcout << L"\nUnloading..." << std::endl;
    if (injector.UnloadDll(pid, dllName)) {
        std::wcout << L"Successfully unloaded DLL!" << std::endl;
    } else {
        std::wcout << L"Failed to unload DLL!" << std::endl;
        std::wcout << L"Error: " << injector.GetLastErrorMessage() << std::endl;
    }
}

int wmain(int argc, wchar_t* argv[]) {

    PrintBanner();

#ifdef _WIN64
    std::wcout << L"Running as: x64" << std::endl;
#else
    std::wcout << L"Running as: x86" << std::endl;
#endif

    std::wcout << L"Note: To inject into x64 processes, use the x64 build." << std::endl;
    std::wcout << L"      To inject into x86 processes, use the x86 build." << std::endl;

    ProcessManager pm;
    DllInjector injector;

    while (true) {
        PrintMenu();

        int choice;
        std::wcin >> choice;
        std::wcin.ignore();

        switch (choice) {
            case 1:
                ListProcesses(pm);
                break;
            case 2:
                InjectDllMenu(pm, injector);
                break;
            case 3:
                UnloadDllMenu(pm, injector);
                break;
            case 4:
                std::wcout << L"\nExiting..." << std::endl;
                return 0;
            default:
                std::wcout << L"\nInvalid choice!" << std::endl;
                break;
        }
    }

    return 0;
}
