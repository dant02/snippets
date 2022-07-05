#pragma once

#include <Windows.h>

using namespace System;

namespace WndApp
{
    ref class Helper
    {
    public:
        static void CheckHResult(HRESULT hr, String^ message);
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
