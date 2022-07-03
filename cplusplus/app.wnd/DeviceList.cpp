#include "DeviceList.h"
HRESULT DeviceList::EnumerateDevices()
{
    // Ask for source type = video capture devices
    auto captureDevice = MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE;
    auto videoCapture = MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID;

    HRESULT result = S_OK;
    IMFAttributes* pAttributes = NULL;

    result = MFCreateAttributes(&pAttributes, 1);

    if (SUCCEEDED(result))
    {
        result = pAttributes->SetGUID(captureDevice, videoCapture);
    }

    // Enumerate devices.
    UINT32      m_cDevices;
    IMFActivate** m_ppDevices;

    if (SUCCEEDED(result))
    {
        result = MFEnumDeviceSources(pAttributes, &m_ppDevices, &m_cDevices);
    }

    return result;
}