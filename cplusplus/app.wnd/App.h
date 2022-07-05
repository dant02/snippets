#pragma once

#include <Windows.h>
#include <tchar.h>
#include <Dbt.h>

#include <msclr\auto_gcroot.h>

#include "Device.h"

using namespace System::Threading;

namespace WndApp
{
    class App
    {
    public:
        ~App();

        void OnDeviceChange(HWND hDlg, WPARAM reason, DEV_BROADCAST_HDR* pHdr);
        void StartCapture(Device^ device);
        void StopCapture();

    private:
        wchar_t* symbolicLink; // for handling device loss

        msclr::auto_gcroot<Device^> selectedDevice;
        msclr::auto_gcroot<Object^> syncKey = gcnew Object();

        bool CheckDeviceLost(DEV_BROADCAST_HDR* pHdr);
    };
}
