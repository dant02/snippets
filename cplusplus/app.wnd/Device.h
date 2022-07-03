#pragma once

#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>

#include "Helper.h"

using namespace System;
using namespace System::Collections::Generic;

namespace App
{
    ref class Device
    {
    public:
        Device(IMFActivate* activate);
        ~Device();
        !Device();

        static List<Device^>^ EnumerateDevices();

        property String^ Name {
            String^ get() {
                wchar_t* ppszName = NULL;
                UINT nameLength = NULL;
                HRESULT hr = imfActivate->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &ppszName, &nameLength);
                Helper::CheckHResult(hr, "Error on GetAllocatedString");
                auto name = gcnew String(ppszName);
                CoTaskMemFree(ppszName);
                return name;
            }
        }

        void Select();
        void Release();

        void ActivateObject(IMFMediaSource** source);

    private:
        IMFActivate* imfActivate; // media foundation device
    };
}
