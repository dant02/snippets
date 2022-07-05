#pragma once

#include <mfidl.h>
#include <mfreadwrite.h>
#include <shlwapi.h>

namespace WndApp
{
    class Capture : public IMFSourceReaderCallback
    {
    public:

        // IUnknown methods
        STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();

        // IMFSourceReaderCallback methods
        STDMETHODIMP OnReadSample(HRESULT status, DWORD streamIndex, DWORD streamFlags, __int64 timestamp, IMFSample* sample);

        STDMETHODIMP OnEvent(DWORD, IMFMediaEvent*)
        {
            return S_OK;
        }

        STDMETHODIMP OnFlush(DWORD)
        {
            return S_OK;
        }

    private:
        long referenceCount;
    };
}
