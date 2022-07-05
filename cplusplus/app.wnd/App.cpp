#include "App.h"

namespace WndApp
{
    App::~App()
    {
        delete this->selectedDevice;
    }

    void App::StartCapture(Device^ device)
    {
        this->selectedDevice = device;
        this->selectedDevice->Select();

        HRESULT hr = S_OK;
        IMFMediaSource* source = NULL;

        Monitor::Enter(syncKey);

        device->ActivateObject(&source);

        device->GetSymbolicLink(&symbolicLink);

        // open media source

        if (source != NULL)
        {
            source->Release();
            source = NULL;
        }

        Monitor::Exit(syncKey);
    }

    void App::StopCapture()
    {
        this->selectedDevice->Release();

        CoTaskMemFree(symbolicLink);
        symbolicLink = NULL;
    }

    void App::OnDeviceChange(HWND hDlg, WPARAM reason, DEV_BROADCAST_HDR* pHdr)
    {
        if (reason == DBT_DEVNODES_CHANGED || reason == DBT_DEVICEARRIVAL)
        {
            // Check for added/removed devices, regardless of whether
            // the application is capturing video at this time.

            //UpdateDeviceList(hDlg);
            //UpdateUI(hDlg);
        }

        // Now check if the current video capture device was lost.

        if (pHdr == NULL)
        {
            return;
        }
        if (pHdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE)
        {
            return;
        }

        HRESULT hr = S_OK;

        if (selectedDevice.get() != nullptr && selectedDevice->IsCapturing)
        {
            auto deviceLost = CheckDeviceLost(pHdr);

            if (deviceLost)
            {
                StopCapture();

                MessageBox(hDlg, L"The capture device was removed or lost.", L"Lost Device", MB_OK);
            }
        }
    }

    bool App::CheckDeviceLost(DEV_BROADCAST_HDR* pHdr)
    {
        bool result = false;
        Monitor::Enter(syncKey);
        try
        {
            if (!selectedDevice->IsCapturing ||
                pHdr == NULL ||
                pHdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE ||
                !this->symbolicLink)
            {
                // TODO invert condition
            }
            else
            {
                // Compare the device name with the symbolic link.
                auto pDi = (DEV_BROADCAST_DEVICEINTERFACE*)pHdr;
                if (_wcsicmp(this->symbolicLink, pDi->dbcc_name) == 0)
                {
                    result = true;
                }
            }
        }
        finally
        {
            Monitor::Exit(syncKey);
        }

        return result;
    }
}