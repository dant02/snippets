#pragma once

#include <mfidl.h>
#include <mfreadwrite.h>
#include <shlwapi.h>
#include <msclr\auto_gcroot.h>

namespace WndApp
{
    delegate HRESULT ReadSampleCallback(HRESULT status, DWORD streamIndex, DWORD streamFlags, __int64 timestamp, IMFSample* sample);

    class Capture : public IMFSourceReaderCallback
    {
    public:
        Capture(ReadSampleCallback^ f);

        // IUnknown methods
        STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();

        // IMFSourceReaderCallback methods
        STDMETHODIMP OnEvent(DWORD, IMFMediaEvent*);
        STDMETHODIMP OnFlush(DWORD);
        STDMETHODIMP OnReadSample(HRESULT status, DWORD streamIndex, DWORD streamFlags, __int64 timestamp, IMFSample* sample);

    private:
        long referenceCount;

        msclr::auto_gcroot<ReadSampleCallback^> onReadSample;
    };
}
