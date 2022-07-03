#pragma once

#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>

using namespace System;

class DeviceList
{
public:
    HRESULT EnumerateDevices();
};

