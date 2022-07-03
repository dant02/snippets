#pragma once

#include <Windows.h>

using namespace System;

namespace App
{
    ref class Helper
    {
    public:
        static void CheckHResult(HRESULT hr, String^ message);
    };
}
