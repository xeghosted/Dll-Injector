#pragma once

#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <vector>
#include "process_manager.h"
#include "injector.h"

class MainWindow {
public:
    MainWindow(HINSTANCE hInstance);
    ~MainWindow();

    bool Create();
    void Show(int nCmdShow);
    void MessageLoop();

private:
    HINSTANCE m_hInstance;
    HWND m_hWnd;
    HWND m_hProcessList;
    HWND m_hRefreshBtn;
    HWND m_hInjectBtn;
    HWND m_hUnloadBtn;
    HWND m_hBrowseBtn;
    HWND m_hDllPath;
    HWND m_hDllName;
    HWND m_hSearchEdit;
    HWND m_hArchFilter;
    HWND m_hStatusBar;

    ProcessManager m_processManager;
    DllInjector m_injector;
    std::vector<ProcessInfo> m_processes;

    // Window procedure
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Event handlers
    void OnCreate();
    void OnCommand(WPARAM wParam);
    void OnSize(LPARAM lParam);
    void OnNotify(LPARAM lParam);

    // UI functions
    void CreateControls();
    void RefreshProcessList();
    void FilterProcessList();
    void OnInject();
    void OnUnload();
    void OnBrowse();
    void SetStatusText(const std::wstring& text);
    DWORD GetSelectedProcessPID();
    std::wstring GetSelectedProcessName();
    bool GetSelectedProcessArch();

    // Helper functions
    void ShowError(const std::wstring& message);
    void ShowInfo(const std::wstring& message);
};
