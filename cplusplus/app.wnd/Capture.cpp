#include "Capture.h"

namespace WndApp
{
    Capture::Capture(ReadSampleCallback^ onReadSample)
    {
        this->onReadSample = onReadSample;
    }

    STDMETHODIMP Capture::QueryInterface(REFIID iid, void** ppv)
    {
        static const QITAB qit[] =
        {
            QITABENT(Capture, IMFSourceReaderCallback),
            { 0 },
        };
        return QISearch(this, qit, iid, ppv);
    }
    
    STDMETHODIMP_(ULONG) Capture::AddRef()
    {
        return InterlockedIncrement(&referenceCount);
    }
    
    STDMETHODIMP_(ULONG) Capture::Release()
    {
        ULONG uCount = InterlockedDecrement(&referenceCount);
        if (uCount == 0)
        {
            delete this;
        }
        return uCount;
    }

    STDMETHODIMP Capture::OnEvent(DWORD, IMFMediaEvent*)
    {
        return S_OK;
    }

    STDMETHODIMP Capture::OnFlush(DWORD)
    {
        return S_OK;
    }

    STDMETHODIMP Capture::OnReadSample(HRESULT status, DWORD streamIndex, DWORD streamFlags, __int64 timeStamp, IMFSample* sample)
    {
        return onReadSample.get()(status, streamIndex, streamFlags, timeStamp, sample);
    }
}