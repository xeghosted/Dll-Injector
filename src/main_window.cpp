#include "main_window.h"
#include "resource.h"
#include <CommCtrl.h>
#include <commdlg.h>
#include <sstream>
#include <algorithm>
#include <dwmapi.h>
#include <uxtheme.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

const wchar_t CLASS_NAME[] = L"DllInjectorMainWindow";

// Dark theme colors (Windows 11 inspired)
#define COLOR_BG_DARK RGB(32, 32, 32)
#define COLOR_BG_LIGHTER RGB(45, 45, 45)
#define COLOR_BG_CONTROL RGB(40, 40, 40)
#define COLOR_TEXT RGB(255, 255, 255)
#define COLOR_TEXT_MUTED RGB(180, 180, 180)
#define COLOR_ACCENT RGB(0, 120, 212)
#define COLOR_ACCENT_HOVER RGB(0, 140, 240)
#define COLOR_BORDER RGB(60, 60, 60)
#define COLOR_HOVER RGB(50, 50, 50)

// Brushes for dark theme
HBRUSH g_hBrushBg = nullptr;
HBRUSH g_hBrushControl = nullptr;
HBRUSH g_hBrushLighter = nullptr;

// Fonts
HFONT g_hFontNormal = nullptr;
HFONT g_hFontBold = nullptr;

void EnableDarkMode(HWND hwnd) {
    // Enable dark mode for window
    BOOL useDarkMode = TRUE;
    DwmSetWindowAttribute(hwnd, 20, &useDarkMode, sizeof(useDarkMode)); // DWMWA_USE_IMMERSIVE_DARK_MODE = 20
    
    // Set dark mode for title bar
    DwmSetWindowAttribute(hwnd, 19, &useDarkMode, sizeof(useDarkMode)); // DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 = 19
}

MainWindow::MainWindow(HINSTANCE hInstance) 
    : m_hInstance(hInstance)
    , m_hWnd(nullptr)
    , m_hProcessList(nullptr)
    , m_hRefreshBtn(nullptr)
    , m_hInjectBtn(nullptr)
    , m_hUnloadBtn(nullptr)
    , m_hBrowseBtn(nullptr)
    , m_hDllPath(nullptr)
    , m_hDllName(nullptr)
    , m_hSearchEdit(nullptr)
    , m_hArchFilter(nullptr)
    , m_hStatusBar(nullptr) {
    
    // Create brushes
    g_hBrushBg = CreateSolidBrush(COLOR_BG_DARK);
    g_hBrushControl = CreateSolidBrush(COLOR_BG_CONTROL);
    g_hBrushLighter = CreateSolidBrush(COLOR_BG_LIGHTER);
    
    // Create fonts (Segoe UI - Windows 11 default)
    g_hFontNormal = CreateFont(
        16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI");
    
    g_hFontBold = CreateFont(
        16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI");
}

MainWindow::~MainWindow() {
    if (g_hBrushBg) DeleteObject(g_hBrushBg);
    if (g_hBrushControl) DeleteObject(g_hBrushControl);
    if (g_hBrushLighter) DeleteObject(g_hBrushLighter);
    if (g_hFontNormal) DeleteObject(g_hFontNormal);
    if (g_hFontBold) DeleteObject(g_hFontBold);
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    MainWindow* pThis = nullptr;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (MainWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        pThis->m_hWnd = hwnd;
    } else {
        pThis = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool MainWindow::Create() {
    // Register window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = g_hBrushBg;
    wc.lpszMenuName = nullptr; // No menu for cleaner dark UI
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        return false;
    }

    // Create window with rounded corners (Windows 11 style)
    m_hWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"DLL Injector GUI - x64/x86 Support",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        980, 700,
        nullptr,
        nullptr,
        m_hInstance,
        this
    );

    if (m_hWnd) {
        // Enable dark mode
        EnableDarkMode(m_hWnd);
        
        // Set rounded corners (Windows 11)
        DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(m_hWnd, 33, &preference, sizeof(preference)); // DWMWA_WINDOW_CORNER_PREFERENCE = 33
    }

    return m_hWnd != nullptr;
}

void MainWindow::Show(int nCmdShow) {
    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);
}

void MainWindow::MessageLoop() {
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        // Handle keyboard shortcuts
        if (msg.message == WM_KEYDOWN) {
            if (msg.wParam == VK_F5) {
                RefreshProcessList();
                continue;
            }
        }
        
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void MainWindow::OnCreate() {
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    CreateControls();
    RefreshProcessList();
}

void MainWindow::CreateControls() {
    int margin = 15;
    int spacing = 10;
    int y = margin;
    
    // Title label
    HWND hTitle = CreateWindow(L"STATIC", L"DLL Injector",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        margin, y, 200, 30, m_hWnd, nullptr, m_hInstance, nullptr);
    SendMessage(hTitle, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);
    
    y += 40;
    
    // Search section
    HWND hSearchLabel = CreateWindow(L"STATIC", L"Search Process:",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        margin, y + 5, 110, 20, m_hWnd, nullptr, m_hInstance, nullptr);
    SendMessage(hSearchLabel, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    m_hSearchEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        margin + 120, y, 250, 28, m_hWnd, (HMENU)IDC_SEARCH_EDIT, m_hInstance, nullptr);
    SendMessage(m_hSearchEdit, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    // Architecture filter
    HWND hFilterLabel = CreateWindow(L"STATIC", L"Filter:",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        margin + 390, y + 5, 50, 20, m_hWnd, nullptr, m_hInstance, nullptr);
    SendMessage(hFilterLabel, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    m_hArchFilter = CreateWindow(L"COMBOBOX", L"",
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        margin + 450, y, 120, 200, m_hWnd, (HMENU)IDC_ARCH_FILTER, m_hInstance, nullptr);
    SendMessage(m_hArchFilter, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    SendMessage(m_hArchFilter, CB_ADDSTRING, 0, (LPARAM)L"All Processes");
    SendMessage(m_hArchFilter, CB_ADDSTRING, 0, (LPARAM)L"x64 Only");
    SendMessage(m_hArchFilter, CB_ADDSTRING, 0, (LPARAM)L"x86 Only");
    SendMessage(m_hArchFilter, CB_SETCURSEL, 0, 0);
    
    // Refresh button
    m_hRefreshBtn = CreateWindow(L"BUTTON", L"Refresh",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
        margin + 590, y, 100, 30, m_hWnd, (HMENU)IDC_REFRESH_BTN, m_hInstance, nullptr);
    SendMessage(m_hRefreshBtn, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    // About button
    HWND hAboutBtn = CreateWindow(L"BUTTON", L"About",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
        margin + 700, y, 100, 30, m_hWnd, (HMENU)IDM_ABOUT, m_hInstance, nullptr);
    SendMessage(hAboutBtn, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    // Exit button
    HWND hExitBtn = CreateWindow(L"BUTTON", L"Exit",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
        margin + 810, y, 100, 30, m_hWnd, (HMENU)IDM_EXIT, m_hInstance, nullptr);
    SendMessage(hExitBtn, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    y += 45;
    
    // Process list
    m_hProcessList = CreateWindowEx(
        0,
        WC_LISTVIEW,
        L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
        margin, y, 940, 380,
        m_hWnd,
        (HMENU)IDC_PROCESS_LIST,
        m_hInstance,
        nullptr
    );
    SendMessage(m_hProcessList, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    // Set up list view columns
    LVCOLUMN lvc = {};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    
    lvc.pszText = (LPWSTR)L"PID";
    lvc.cx = 80;
    ListView_InsertColumn(m_hProcessList, 0, &lvc);
    
    lvc.pszText = (LPWSTR)L"Process Name";
    lvc.cx = 380;
    ListView_InsertColumn(m_hProcessList, 1, &lvc);
    
    lvc.pszText = (LPWSTR)L"Architecture";
    lvc.cx = 110;
    ListView_InsertColumn(m_hProcessList, 2, &lvc);
    
    lvc.pszText = (LPWSTR)L"Status";
    lvc.cx = 320;
    ListView_InsertColumn(m_hProcessList, 3, &lvc);
    
    // Enable full row select and grid lines with dark theme
    ListView_SetExtendedListViewStyle(m_hProcessList, 
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
    
    // Set dark theme colors for list view
    ListView_SetBkColor(m_hProcessList, COLOR_BG_CONTROL);
    ListView_SetTextBkColor(m_hProcessList, COLOR_BG_CONTROL);
    ListView_SetTextColor(m_hProcessList, COLOR_TEXT);
    
    y += 395;
    
    // DLL Path section
    HWND hDllPathLabel = CreateWindow(L"STATIC", L"DLL Path (for injection):",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        margin, y + 5, 220, 20, m_hWnd, nullptr, m_hInstance, nullptr);
    SendMessage(hDllPathLabel, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    m_hDllPath = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        margin + 230, y, 570, 28, m_hWnd, (HMENU)IDC_DLL_PATH, m_hInstance, nullptr);
    SendMessage(m_hDllPath, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    m_hBrowseBtn = CreateWindow(L"BUTTON", L"Browse...",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
        margin + 810, y, 130, 30, m_hWnd, (HMENU)IDC_BROWSE_BTN, m_hInstance, nullptr);
    SendMessage(m_hBrowseBtn, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    y += 40;
    
    // DLL Name section
    HWND hDllNameLabel = CreateWindow(L"STATIC", L"DLL Name (for unloading):",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        margin, y + 5, 220, 20, m_hWnd, nullptr, m_hInstance, nullptr);
    SendMessage(hDllNameLabel, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    m_hDllName = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        margin + 230, y, 710, 28, m_hWnd, (HMENU)IDC_DLL_NAME, m_hInstance, nullptr);
    SendMessage(m_hDllName, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    y += 50;
    
    // Action buttons with spacing
    m_hInjectBtn = CreateWindow(L"BUTTON", L"Inject DLL",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
        margin, y, 220, 38, m_hWnd, (HMENU)IDC_INJECT_BTN, m_hInstance, nullptr);
    SendMessage(m_hInjectBtn, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);
    
    m_hUnloadBtn = CreateWindow(L"BUTTON", L"Unload DLL",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
        margin + 240, y, 220, 38, m_hWnd, (HMENU)IDC_UNLOAD_BTN, m_hInstance, nullptr);
    SendMessage(m_hUnloadBtn, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);
    
    y += 50;
    
    // Status bar
    m_hStatusBar = CreateWindowEx(
        0, STATUSCLASSNAME, nullptr,
        WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0,
        m_hWnd, (HMENU)IDC_STATUS_BAR, m_hInstance, nullptr);
    SendMessage(m_hStatusBar, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
    
    // Set dark theme for status bar
    SetWindowTheme(m_hStatusBar, L"DarkMode_Explorer", NULL);
    
    SetStatusText(L"Ready. Select a process and provide DLL path/name.");
}

void MainWindow::RefreshProcessList() {
    SetStatusText(L"Refreshing process list...");
    
    ListView_DeleteAllItems(m_hProcessList);
    m_processes = m_processManager.GetProcessList();
    
    FilterProcessList();
    
    std::wstringstream ss;
    ss << L"Loaded " << ListView_GetItemCount(m_hProcessList) << L" processes";
    SetStatusText(ss.str());
}

void MainWindow::FilterProcessList() {
    ListView_DeleteAllItems(m_hProcessList);
    
    // Get search text
    wchar_t searchText[256] = {};
    GetWindowText(m_hSearchEdit, searchText, 256);
    std::wstring search = searchText;
    std::transform(search.begin(), search.end(), search.begin(), ::towlower);
    
    // Get architecture filter
    int archFilter = (int)SendMessage(m_hArchFilter, CB_GETCURSEL, 0, 0);
    
    // Check current injector architecture
#ifdef _WIN64
    bool injectorIs64 = true;
#else
    bool injectorIs64 = false;
#endif
    
    int index = 0;
    for (const auto& proc : m_processes) {
        // Apply search filter
        if (!search.empty()) {
            std::wstring procName = proc.name;
            std::transform(procName.begin(), procName.end(), procName.begin(), ::towlower);
            if (procName.find(search) == std::wstring::npos) {
                continue;
            }
        }
        
        // Apply architecture filter
        if (archFilter == 1 && !proc.is64Bit) continue;  // x64 only
        if (archFilter == 2 && proc.is64Bit) continue;   // x86 only
        
        LVITEM lvi = {};
        lvi.mask = LVIF_TEXT | LVIF_PARAM;
        lvi.iItem = index++;
        lvi.lParam = (LPARAM)proc.pid;
        
        // PID
        wchar_t pidStr[32];
        swprintf_s(pidStr, L"%lu", proc.pid);
        lvi.pszText = pidStr;
        ListView_InsertItem(m_hProcessList, &lvi);
        
        // Process name
        ListView_SetItemText(m_hProcessList, lvi.iItem, 1, (LPWSTR)proc.name.c_str());
        
        // Architecture
        ListView_SetItemText(m_hProcessList, lvi.iItem, 2, 
            (LPWSTR)(proc.is64Bit ? L"x64" : L"x86"));
        
        // Status (compatibility check)
        std::wstring status;
        if (proc.is64Bit == injectorIs64) {
            status = L"Compatible";
        } else {
            status = L"Architecture mismatch - use ";
            status += proc.is64Bit ? L"x64" : L"x86";
            status += L" injector";
        }
        ListView_SetItemText(m_hProcessList, lvi.iItem, 3, (LPWSTR)status.c_str());
    }
}

void MainWindow::OnInject() {
    DWORD pid = GetSelectedProcessPID();
    if (pid == 0) {
        ShowError(L"Please select a process from the list.");
        return;
    }
    
    wchar_t dllPath[MAX_PATH];
    GetWindowText(m_hDllPath, dllPath, MAX_PATH);
    
    if (wcslen(dllPath) == 0) {
        ShowError(L"Please provide the DLL path.");
        return;
    }
    
    // Check architecture compatibility
    bool targetIs64 = GetSelectedProcessArch();
#ifdef _WIN64
    bool injectorIs64 = true;
#else
    bool injectorIs64 = false;
#endif
    
    if (targetIs64 != injectorIs64) {
        std::wstring msg = L"Architecture mismatch!\n\nTarget process: ";
        msg += targetIs64 ? L"x64" : L"x86";
        msg += L"\nInjector: ";
        msg += injectorIs64 ? L"x64" : L"x86";
        msg += L"\n\nYou need to use the ";
        msg += targetIs64 ? L"x64" : L"x86";
        msg += L" version of this injector.";
        ShowError(msg);
        return;
    }
    
    SetStatusText(L"Injecting DLL...");
    
    if (m_injector.InjectDll(pid, dllPath)) {
        std::wstringstream ss;
        ss << L"Successfully injected DLL into process " << pid << L" (" << GetSelectedProcessName() << L")";
        ShowInfo(ss.str());
        SetStatusText(L"DLL injected successfully!");
    } else {
        std::wstring error = L"Failed to inject DLL: " + m_injector.GetLastErrorMessage();
        ShowError(error);
        SetStatusText(L"Injection failed!");
    }
}

void MainWindow::OnUnload() {
    DWORD pid = GetSelectedProcessPID();
    if (pid == 0) {
        ShowError(L"Please select a process from the list.");
        return;
    }
    
    wchar_t dllName[MAX_PATH];
    GetWindowText(m_hDllName, dllName, MAX_PATH);
    
    if (wcslen(dllName) == 0) {
        ShowError(L"Please provide the DLL name (e.g., MyDll.dll).");
        return;
    }
    
    // Check architecture compatibility
    bool targetIs64 = GetSelectedProcessArch();
#ifdef _WIN64
    bool injectorIs64 = true;
#else
    bool injectorIs64 = false;
#endif
    
    if (targetIs64 != injectorIs64) {
        std::wstring msg = L"Architecture mismatch!\n\nTarget process: ";
        msg += targetIs64 ? L"x64" : L"x86";
        msg += L"\nInjector: ";
        msg += injectorIs64 ? L"x64" : L"x86";
        msg += L"\n\nYou need to use the ";
        msg += targetIs64 ? L"x64" : L"x86";
        msg += L" version of this injector.";
        ShowError(msg);
        return;
    }
    
    SetStatusText(L"Unloading DLL...");
    
    if (m_injector.UnloadDll(pid, dllName)) {
        std::wstringstream ss;
        ss << L"Successfully unloaded DLL from process " << pid << L" (" << GetSelectedProcessName() << L")";
        ShowInfo(ss.str());
        SetStatusText(L"DLL unloaded successfully!");
    } else {
        std::wstring error = L"Failed to unload DLL: " + m_injector.GetLastErrorMessage();
        ShowError(error);
        SetStatusText(L"Unload failed!");
    }
}

void MainWindow::OnBrowse() {
    OPENFILENAME ofn = {};
    wchar_t fileName[MAX_PATH] = {};
    
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"DLL Files\0*.dll\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle = L"Select DLL to Inject";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileName(&ofn)) {
        SetWindowText(m_hDllPath, fileName);
    }
}

void MainWindow::SetStatusText(const std::wstring& text) {
    SendMessage(m_hStatusBar, SB_SETTEXT, 0, (LPARAM)text.c_str());
}

DWORD MainWindow::GetSelectedProcessPID() {
    int selected = ListView_GetNextItem(m_hProcessList, -1, LVNI_SELECTED);
    if (selected == -1) return 0;
    
    LVITEM lvi = {};
    lvi.mask = LVIF_PARAM;
    lvi.iItem = selected;
    ListView_GetItem(m_hProcessList, &lvi);
    
    return (DWORD)lvi.lParam;
}

std::wstring MainWindow::GetSelectedProcessName() {
    int selected = ListView_GetNextItem(m_hProcessList, -1, LVNI_SELECTED);
    if (selected == -1) return L"";
    
    wchar_t name[256];
    ListView_GetItemText(m_hProcessList, selected, 1, name, 256);
    return name;
}

bool MainWindow::GetSelectedProcessArch() {
    int selected = ListView_GetNextItem(m_hProcessList, -1, LVNI_SELECTED);
    if (selected == -1) return false;
    
    wchar_t arch[16];
    ListView_GetItemText(m_hProcessList, selected, 2, arch, 16);
    return wcscmp(arch, L"x64") == 0;
}

void MainWindow::ShowError(const std::wstring& message) {
    MessageBox(m_hWnd, message.c_str(), L"Error", MB_OK | MB_ICONERROR);
}

void MainWindow::ShowInfo(const std::wstring& message) {
    MessageBox(m_hWnd, message.c_str(), L"Success", MB_OK | MB_ICONINFORMATION);
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            OnCreate();
            return 0;
            
        case WM_COMMAND:
            OnCommand(wParam);
            return 0;
            
        case WM_SIZE:
            OnSize(lParam);
            return 0;
            
        case WM_NOTIFY:
            OnNotify(lParam);
            return 0;
            
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX: {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, COLOR_TEXT);
            SetBkColor(hdcStatic, COLOR_BG_DARK);
            return (INT_PTR)g_hBrushBg;
        }
        
        case WM_CTLCOLORBTN: {
            HDC hdcButton = (HDC)wParam;
            SetTextColor(hdcButton, COLOR_TEXT);
            SetBkColor(hdcButton, COLOR_BG_DARK);
            return (INT_PTR)g_hBrushBg;
        }
        
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(m_hWnd, &rc);
            FillRect(hdc, &rc, g_hBrushBg);
            return 1;
        }
        
        case WM_DRAWITEM: {
            LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
            if (lpDrawItem->CtlType == ODT_BUTTON) {
                // Custom button drawing for flat dark style
                HDC hdc = lpDrawItem->hDC;
                RECT rc = lpDrawItem->rcItem;
                
                // Determine button state
                bool isPressed = (lpDrawItem->itemState & ODS_SELECTED);
                bool isHovered = (lpDrawItem->itemState & ODS_HOTLIGHT) || 
                                (lpDrawItem->itemState & ODS_FOCUS);
                bool isDisabled = (lpDrawItem->itemState & ODS_DISABLED);
                
                // Check if this is an action button (Inject/Unload)
                bool isActionButton = (lpDrawItem->CtlID == IDC_INJECT_BTN || 
                                      lpDrawItem->CtlID == IDC_UNLOAD_BTN);
                
                // Background color based on state and button type
                COLORREF bgColor;
                if (isDisabled) {
                    bgColor = RGB(60, 60, 60);
                } else if (isActionButton) {
                    // Accent color for main action buttons
                    if (isPressed) {
                        bgColor = RGB(0, 100, 180);
                    } else if (isHovered) {
                        bgColor = COLOR_ACCENT_HOVER;
                    } else {
                        bgColor = COLOR_ACCENT;
                    }
                } else {
                    // Regular buttons
                    if (isPressed) {
                        bgColor = RGB(60, 60, 60);
                    } else if (isHovered) {
                        bgColor = COLOR_HOVER;
                    } else {
                        bgColor = COLOR_BG_LIGHTER;
                    }
                }
                
                // Draw background with rounded corners
                HBRUSH hBrush = CreateSolidBrush(bgColor);
                HPEN hPen = CreatePen(PS_SOLID, 1, isActionButton ? COLOR_ACCENT : COLOR_BORDER);
                HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
                
                // Draw rounded rectangle
                RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 6, 6);
                
                SelectObject(hdc, hOldPen);
                SelectObject(hdc, hOldBrush);
                DeleteObject(hPen);
                DeleteObject(hBrush);
                
                // Draw text
                wchar_t text[256];
                GetWindowText(lpDrawItem->hwndItem, text, 256);
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, isDisabled ? RGB(100, 100, 100) : COLOR_TEXT);
                SelectObject(hdc, isActionButton ? g_hFontBold : g_hFontNormal);
                DrawText(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                
                return TRUE;
            }
            break;
        }
        
        case WM_MENUSELECT: {
            // Handle menu highlighting
            return 0;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_hWnd, &ps);
            
            // Fill background
            RECT rc;
            GetClientRect(m_hWnd, &rc);
            FillRect(hdc, &rc, g_hBrushBg);
            
            EndPaint(m_hWnd, &ps);
            return 0;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

void MainWindow::OnCommand(WPARAM wParam) {
    int wmId = LOWORD(wParam);
    int wmEvent = HIWORD(wParam);
    
    switch (wmId) {
        case IDC_REFRESH_BTN:
        case IDM_REFRESH:
            RefreshProcessList();
            break;
            
        case IDC_INJECT_BTN:
            OnInject();
            break;
            
        case IDC_UNLOAD_BTN:
            OnUnload();
            break;
            
        case IDC_BROWSE_BTN:
            OnBrowse();
            break;
            
        case IDC_SEARCH_EDIT:
            if (wmEvent == EN_CHANGE) {
                FilterProcessList();
            }
            break;
            
        case IDC_ARCH_FILTER:
            if (wmEvent == CBN_SELCHANGE) {
                FilterProcessList();
            }
            break;
            
        case IDM_ABOUT:
            MessageBox(m_hWnd, 
                L"DLL Injector GUI v1.0\n\n"
                L"A tool for injecting and unloading DLLs in processes.\n"
                L"Supports both x86 and x64 processes.\n\n"
                L"Built with Windows API and C++",
                L"About", MB_OK | MB_ICONINFORMATION);
            break;
            
        case IDM_EXIT:
            PostQuitMessage(0);
            break;
    }
}

void MainWindow::OnSize(LPARAM lParam) {
    // Resize status bar
    SendMessage(m_hStatusBar, WM_SIZE, 0, 0);
}

void MainWindow::OnNotify(LPARAM lParam) {
    LPNMHDR pnmh = (LPNMHDR)lParam;
    
    if (pnmh->idFrom == IDC_PROCESS_LIST && pnmh->code == NM_DBLCLK) {
        // Double-click on process - auto-fill for injection
        DWORD pid = GetSelectedProcessPID();
        if (pid != 0) {
            std::wstringstream ss;
            ss << L"Selected process: " << GetSelectedProcessName() << L" (PID: " << pid << L")";
            SetStatusText(ss.str());
        }
    }
}
