#include "Device.h"
namespace WndApp
{
    Device::Device(IMFActivate* activate)
    {
        this->imfActivate = activate;
    }

    Device::~Device()
    {
        this->!Device();
    }

    Device::!Device()
    {
        if (this->imfActivate != nullptr)
        {
            this->imfActivate->Release();
            this->imfActivate = nullptr;
        }
    }

    List<Device^>^ Device::EnumerateDevices()
    {
        // Ask for source type = video capture devices
        auto captureDevice = MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE;
        auto videoCapture = MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID;

        HRESULT hr = S_OK;
        IMFAttributes* pAttributes = NULL;

        hr = MFCreateAttributes(&pAttributes, 1);
        Helper::CheckHResult(hr, "Error on MFCreateAttributes");

        hr = pAttributes->SetGUID(captureDevice, videoCapture);
        Helper::CheckHResult(hr, "Error on SetGUID");

        // Enumerate devices.
        UINT32        deviceCount;
        IMFActivate** deviceData;
        hr = MFEnumDeviceSources(pAttributes, &deviceData, &deviceCount);
        Helper::CheckHResult(hr, "Error on MFEnumDeviceSources");

        List<Device^>^ devices = gcnew List<Device^>(2);
        for (UINT i = 0; i < deviceCount; i++)
        {
            devices->Add(gcnew Device(deviceData[i]));
        }

        return devices;
    }

    void Device::Select() {
        imfActivate->AddRef();
        isCapturing = true;
    }

    void Device::Release() {
        imfActivate->Release();
        isCapturing = false;
    }

    void Device::ActivateObject(IMFMediaSource** source)
    {
        auto hr = imfActivate->ActivateObject(__uuidof(IMFMediaSource),(void**)source);
        Helper::CheckHResult(hr, "Error on IMFMediaSource::ActivateObject");
    }

    void Device::GetSymbolicLink(wchar_t** symbolicLink)
    {
        UINT symbolicLinkLength = NULL;
        auto hr = imfActivate->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
            symbolicLink, &symbolicLinkLength);
        Helper::CheckHResult(hr, "GetSymbolicLink - error on IMFMediaSource::GetAllocatedString");
    }
}