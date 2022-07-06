#include "App.h"

namespace WndApp
{
    App::~App()
    {
        delete this->selectedDevice;
    }

    void App::StartCapture(Device^ device, ReadSampleCallback^ onReadSample)
    {
        this->selectedDevice = device;
        this->selectedDevice->Select();

        HRESULT hr = S_OK;
        IMFMediaSource* source = NULL;

        Monitor::Enter(syncKey);

        device->ActivateObject(&source);
        device->GetSymbolicLink(&symbolicLink);

        this->capture = new Capture(onReadSample);

        this->reader = OpenMediaSourceAndGetReader(source);
        this->writer = GetWriter();

        ConfigureCapture(this->reader, this->writer);

        isFirstSample = true;
        baseTime = 0;

        // Request the first video frame.
        hr = this->reader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0, NULL, NULL, NULL, NULL);
        Helper::CheckHResult(hr, "Failed to read first sample");

        SafeRelease(&source);
        Monitor::Exit(syncKey);
    }

    void App::ConfigureCapture(IMFSourceReader* reader, IMFSinkWriter* writer)
    {
        DWORD sink_stream = 0;
        IMFMediaType* mediaType = GetMediaType(reader);

        auto hr = ConfigureEncoder(mediaType, writer, &sink_stream);
        Helper::CheckHResult(hr, "Configure encoder");

        hr = MFTRegisterLocalByCLSID(__uuidof(CColorConvertDMO), MFT_CATEGORY_VIDEO_PROCESSOR,
            L"", MFT_ENUM_FLAG_SYNCMFT, 0, NULL, 0, NULL);
        Helper::CheckHResult(hr, "MFTRegisterLocalByCLSID");

        hr = writer->SetInputMediaType(sink_stream, mediaType, NULL);
        Helper::CheckHResult(hr, "Writer SetInputMediaType");

        hr = writer->BeginWriting();
        Helper::CheckHResult(hr, "Writer BeginWriting");

        SafeRelease(&mediaType);
    }

    IMFMediaType* App::GetMediaType(IMFSourceReader* reader)
    {
        IMFMediaType* nativeType = NULL;

        HRESULT nativeTypeErrorCode = S_OK;
        DWORD count = 0;
        UINT32 streamIndex = 0;
        UINT32 requiredWidth = 1280;
        UINT32 requiredheight = 720;
        while (nativeTypeErrorCode == S_OK)
        {
            try {
                nativeTypeErrorCode = reader->GetNativeMediaType(streamIndex, count, &nativeType);
                if (nativeTypeErrorCode != S_OK) continue;

                // get the media type
                GUID nativeGuid = { 0 };
                auto hr = nativeType->GetGUID(MF_MT_SUBTYPE, &nativeGuid);
                Helper::CheckHResult(hr, "Error on IMFMediaType GetGUID");

                UINT32 width, height;
                hr = MFGetAttributeSize(nativeType, MF_MT_FRAME_SIZE, &width, &height);
                Helper::CheckHResult(hr, "Error on MFGetAttributeSize");

                if (/*nativeGuid == MFVideoFormat_YUY2 && */ width == requiredWidth && height == requiredheight)
                {
                    // found native config, set it
                    hr = reader->SetCurrentMediaType(streamIndex, NULL, nativeType);
                    Helper::CheckHResult(hr, "Error on IMFSourceReader SetCurrentMediaType");
                    break;
                }
            }
            catch (Exception^ ex)
            {
                SafeRelease(&nativeType);
                throw;
            }

            SafeRelease(&nativeType);
            count++;
        }

        return nativeType;
    }

    HRESULT App::ConfigureEncoder(IMFMediaType* pType, IMFSinkWriter* pWriter, DWORD* pdwStreamIndex)
    {
        auto subType = MFVideoFormat_H264;
        auto bitrate = 240 * 1000;

        HRESULT hr = S_OK;

        IMFMediaType* pType2 = NULL;

        hr = MFCreateMediaType(&pType2);

        if (SUCCEEDED(hr))
        {
            hr = pType2->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
        }

        if (SUCCEEDED(hr))
        {
            hr = pType2->SetGUID(MF_MT_SUBTYPE, subType);
        }

        if (SUCCEEDED(hr))
        {
            hr = pType2->SetUINT32(MF_MT_AVG_BITRATE, bitrate);
        }

        if (SUCCEEDED(hr))
        {
            hr = CopyAttribute(pType, pType2, MF_MT_FRAME_SIZE);
        }

        if (SUCCEEDED(hr))
        {
            hr = CopyAttribute(pType, pType2, MF_MT_FRAME_RATE);
        }

        if (SUCCEEDED(hr))
        {
            hr = CopyAttribute(pType, pType2, MF_MT_PIXEL_ASPECT_RATIO);
        }

        if (SUCCEEDED(hr))
        {
            hr = CopyAttribute(pType, pType2, MF_MT_INTERLACE_MODE);
        }

        if (SUCCEEDED(hr))
        {
            hr = pWriter->AddStream(pType2, pdwStreamIndex);
        }

        SafeRelease(&pType2);
        return hr;
    }

    IMFSourceReader* App::OpenMediaSourceAndGetReader(IMFMediaSource* source)
    {
        IMFAttributes* attributes = NULL;

        auto hr = MFCreateAttributes(&attributes, 2);
        Helper::CheckHResult(hr, "Error creating attributes");

        hr = attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this->capture);
        Helper::CheckHResult(hr, "Error set MF_SOURCE_READER_ASYNC_CALLBACK");

        IMFSourceReader* reader;
        hr = MFCreateSourceReaderFromMediaSource(source, attributes, &reader);
        Helper::CheckHResult(hr, "Error on MFCreateSourceReaderFromMediaSource");

        SafeRelease(&attributes);
        return reader;
    }

    IMFSinkWriter* App::GetWriter()
    {
        IMFSinkWriter* writer;
        auto hr = MFCreateSinkWriterFromURL(L"capture.mp4", NULL, NULL, &writer);
        Helper::CheckHResult(hr, "Error on MFCreateSinkWriterFromURL");
        return writer;
    }

    void App::StopCapture()
    {
        this->selectedDevice->Release();

        if (this->writer)
        {
            this->writer->Finalize();
        }

        SafeRelease(&this->writer);
        SafeRelease(&this->reader);

        SafeRelease(&this->capture);
        CoTaskMemFree(&this->symbolicLink);
        this->symbolicLink = NULL;
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

    HRESULT App::CopyAttribute(IMFAttributes* pSrc, IMFAttributes* pDest, const GUID& key)
    {
        PROPVARIANT var;
        PropVariantInit(&var);

        HRESULT hr = S_OK;

        hr = pSrc->GetItem(key, &var);
        if (SUCCEEDED(hr))
        {
            hr = pDest->SetItem(key, var);
        }

        PropVariantClear(&var);
        return hr;
    }

    HRESULT App::OnReadSample(HRESULT status, DWORD streamIndex, DWORD streamFlags, __int64 timeStamp, IMFSample* sample)
    {
        if (!this->selectedDevice->IsCapturing)
        {
            return S_OK;
        }

        Monitor::Enter(syncKey);

        if (!this->selectedDevice->IsCapturing)
        {
            Monitor::Exit(syncKey);
            return S_OK;
        }

        HRESULT hr = S_OK;
        try
        {
            if (FAILED(status))
            {
                hr = status;
            }
            Helper::CheckHResult(status, "Error on OnReadSample");

            if (sample)
            {
                if (isFirstSample)
                {
                    baseTime = timeStamp;
                    isFirstSample = false;
                }

                // rebase the time stamp
                timeStamp -= baseTime;
                hr = sample->SetSampleTime(timeStamp);
                Helper::CheckHResult(hr, "Error on SetSampleTime");

                hr = this->writer->WriteSample(0, sample);
                Helper::CheckHResult(hr, "Error on WriteSample");
            }

            // Read another sample.
            hr = this->reader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
                0,
                NULL,   // actual
                NULL,   // flags
                NULL,   // timestamp
                NULL    // sample
            );
        }
        catch (Exception^ ex)
        {
            //NotifyError(hr);
        }
        finally
        {
            Monitor::Exit(syncKey);
        }

        return hr;
    }
}