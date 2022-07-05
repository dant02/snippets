#pragma once

#include <Windows.h>
#include <tchar.h>
#include <Dbt.h>
#include <Wmcodecdsp.h>

#include <msclr\auto_gcroot.h>

#include "Capture.h"
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
        bool     isFirstSample;
        __int64  baseTime;

        Capture* capture;

        msclr::auto_gcroot<Device^> selectedDevice;
        msclr::auto_gcroot<Object^> syncKey = gcnew Object();

        bool CheckDeviceLost(DEV_BROADCAST_HDR* pHdr);

        IMFSourceReader* OpenMediaSourceAndGetReader(IMFMediaSource* source);
        IMFSinkWriter* GetWriter();

        void ConfigureCapture(IMFSourceReader* reader, IMFSinkWriter* writer);
        HRESULT ConfigureEncoder(IMFMediaType* pType, IMFSinkWriter* pWriter, DWORD* pdwStreamIndex);

        IMFMediaType* GetMediaType(IMFSourceReader* reader);

        HRESULT CopyAttribute(IMFAttributes* pSrc, IMFAttributes* pDest, const GUID& key);
    };
}
