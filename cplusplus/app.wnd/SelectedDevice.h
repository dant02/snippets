#pragma once

#include "Device.h"

namespace App
{
    ref class SelectedDevice
    {
    public:
        SelectedDevice(Device^ device);
        ~SelectedDevice();
        !SelectedDevice();

        void StartCapture();

    private:
        Device^ device;
    };
}
