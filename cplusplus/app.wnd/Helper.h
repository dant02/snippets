#pragma once

#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>

using namespace System;

namespace WndApp
{
    ref class Helper
    {
    public:
        static void CheckHResult(HRESULT hr, String^ message);

        static Guid^ ToGuid(GUID guid);

        static HRESULT CopyAttribute(IMFAttributes* pSrc, IMFAttributes* pDest, const GUID& key);
    };

    template <class T> void SafeRelease(T** ppT)
    {
        if (*ppT)
        {
            (*ppT)->Release();
            *ppT = NULL;
        }
    }
}
