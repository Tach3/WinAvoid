#include <iostream>
#include <ctime>

#include <Windows.h>
#include <winternl.h>
#include "WinAvoid.h"

typedef VOID(NTAPI* PRtlInitAnsiString)(PANSI_STRING DestinationString, PCSTR SourceString);
typedef VOID(NTAPI* PRtlAnsiStringToUnicodeString)(PUNICODE_STRING DestinationString, PCANSI_STRING SourceString, BOOLEAN AllocateDestinationString);
typedef VOID(NTAPI* PRtlFreeUnicodeString)(PUNICODE_STRING UnicodeString);

NtUserFindWindowEx p_NtUserFindWindowEx;
PRtlInitAnsiString p_RtlInitAnsiString;
PRtlAnsiStringToUnicodeString p_RtlAnsiStringToUnicodeString;
PRtlFreeUnicodeString p_RtlFreeUnicodeString;

NTSTATUS STATUS;
HWND hwndTaskBar;
HWND hwndStart;
POINT cursorPos;
RECT taskBarRect;
RECT startRect;
int upperBound;
UINT uTaskbarRestartMsg = 0;

void RefreshTaskbarHandles()
{
    ANSI_STRING ansiStr = { 0 };
    UNICODE_STRING uStr = { 0 };
    p_RtlInitAnsiString(&ansiStr, "Start");
    p_RtlAnsiStringToUnicodeString(&uStr, &ansiStr, TRUE);
    hwndTaskBar = FindWindowA("Shell_TrayWnd", NULL);
    if (hwndTaskBar) {
        //hwndStart = FindWindowExA(hwndTaskBar, NULL, "Start", "Start");
        hwndStart = p_NtUserFindWindowEx(hwndTaskBar, NULL, &uStr, &uStr, 0);
        GetWindowRect(hwndTaskBar, &taskBarRect);
        GetWindowRect(hwndStart, &startRect);
        SetWindowPos(hwndStart, HWND_TOP, 0, 0, 0, 0, (SWP_NOSIZE | SWP_NOZORDER));
        upperBound = taskBarRect.right - startRect.right;
        //p_RtlFreeUnicodeString(&uStr);
    }
    else {
        hwndStart = NULL;
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == uTaskbarRestartMsg) {
        RefreshTaskbarHandles();
        return 0;
    }

    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT WINAPI MouseCallback(int nCode, WPARAM wParam, LPARAM lParam) {

	if (nCode == 0) {
        GetCursorPos(&cursorPos);
	}
    if (cursorPos.x >= startRect.left && cursorPos.x <= startRect.right && cursorPos.y >= startRect.top) {
        int newX = rand() % upperBound;
        SetWindowPos(hwndStart, 0x0, newX, 0, 0, 0, (SWP_NOSIZE or SWP_NOZORDER));
        GetWindowRect(hwndStart, &startRect);
    }
	return CallNextHookEx(0, nCode, wParam, lParam);
}

int APIENTRY WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hInstPrev, _In_ PSTR cmdline, _In_ int cmdshow)
{
    HMODULE hNtDLL = NULL;
    HMODULE hWin32u = NULL;
    WNDCLASSA wc = { 0 };
    HWND hwndExplorerMonitor = NULL;
    HHOOK hook = NULL;
    MSG msg;


    hNtDLL = GetModuleHandleW(L"NTDLL");
    if (hNtDLL == NULL) {
        return FALSE;
    }

    hWin32u = GetModuleHandle(L"WIN32U");
    if (hWin32u == NULL) {
        return FALSE;
    }

    p_NtUserFindWindowEx = (NtUserFindWindowEx)GetProcAddress(hWin32u, "NtUserFindWindowEx");
    p_RtlInitAnsiString = (PRtlInitAnsiString)GetProcAddress(hNtDLL, "RtlInitAnsiString");
    p_RtlAnsiStringToUnicodeString = (PRtlAnsiStringToUnicodeString)GetProcAddress(hNtDLL, "RtlAnsiStringToUnicodeString");
    p_RtlFreeUnicodeString = (PRtlFreeUnicodeString)GetProcAddress(hNtDLL, "RltFreeUnicodeString");
    
    srand((unsigned)time(NULL));
    uTaskbarRestartMsg = RegisterWindowMessageA("TaskbarCreated");

    
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "ExplorerMonitorClass";

    if (!RegisterClassA(&wc)) {
        return 1;
    }

    hwndExplorerMonitor = CreateWindowA("ExplorerMonitorClass",
        "ExplorerMonitor",
        0, 0, 0, 0, 0,
        NULL, NULL,
        GetModuleHandle(NULL),
        NULL);

    if (!hwndExplorerMonitor) {
        return 1;
    }
    RefreshTaskbarHandles();

    hook = SetWindowsHookEx(WH_MOUSE_LL, MouseCallback, 0, 0);
    if (!hook) {
        return 1;
    }

    
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    

    // CLEANUP 

    UnhookWindowsHookEx(hook);
    if (hwndStart) {
        CloseHandle(hwndStart);
    }

    if (hwndTaskBar) {
        CloseHandle(hwndTaskBar);
    }

    if (hwndExplorerMonitor) {
        DestroyWindow(hwndExplorerMonitor);
        CloseHandle(hwndExplorerMonitor);
    }

    return 0;
}
