#include "SelectedDevice.h"

namespace App
{
    SelectedDevice::SelectedDevice(Device^ device)
    {
        this->device = device;
        this->device->Select();
    }

    SelectedDevice::~SelectedDevice()
    {
        this->!SelectedDevice();
    }

    SelectedDevice::!SelectedDevice()
    {
        if (this->device != nullptr)
        {
            this->device->Release();
        }
    }

    void SelectedDevice::StartCapture()
    {
        HRESULT hr = S_OK;
        IMFMediaSource* source = NULL;

        device->ActivateObject(&source);

        if (source != NULL)
        {
            source->Release();
            source = NULL;
        }
    }
}