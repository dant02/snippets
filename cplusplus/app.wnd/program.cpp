// https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program

#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <tchar.h>

#include "App.h"
#include "Device.h"

using namespace System::Collections::Generic;
using namespace WndApp;

App* app = new App();

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DEVICECHANGE:
            app->OnDeviceChange(hwnd, wParam, (PDEV_BROADCAST_HDR)lParam);
            return TRUE;

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

HRESULT OnReadSample(HRESULT status, DWORD streamIndex, DWORD streamFlags, __int64 timeStamp, IMFSample* sample)
{
    return app->OnReadSample(status, streamIndex, streamFlags, timeStamp, sample);
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

    auto devices = Device::EnumerateDevices();
    app->StartCapture(devices[0], gcnew ReadSampleCallback(OnReadSample));

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    app->StopCapture();
    delete app;

    return 0;
}