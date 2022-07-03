// https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program

#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <tchar.h>

#include "Device.h"
#include "SelectedDevice.h"

using namespace System::Collections::Generic;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

            EndPaint(hwnd, &ps);
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// application entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{

    WNDCLASS windowClass = { };
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = L"SmplWndCls";

    RegisterClass(&windowClass);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                           // Optional window styles.
        windowClass.lpszClassName,   // Window class
        L"Learn to Program Windows", // Window text
        WS_OVERLAPPEDWINDOW,         // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,                  // Parent window
        NULL,                  // Menu
        windowClass.hInstance, // Instance handle
        NULL                   // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    auto devices = App::Device::EnumerateDevices();
    auto selectedDevice = gcnew App::SelectedDevice(devices[0]);

    selectedDevice->StartCapture();

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete selectedDevice;

    return 0;
}