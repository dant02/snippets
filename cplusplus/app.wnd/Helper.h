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
}
