#include "main_window.h"
#include <Windows.h>

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow
) {
    // Unused parameters
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Create and show main window
    MainWindow mainWindow(hInstance);
    
    if (!mainWindow.Create()) {
        MessageBox(nullptr, L"Failed to create main window!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    mainWindow.Show(nCmdShow);
    mainWindow.MessageLoop();
    
    return 0;
}
