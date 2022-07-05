#include "Capture.h"

namespace WndApp
{
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

    HRESULT Capture::OnReadSample(HRESULT status, DWORD streamIndex, DWORD streamFlags, __int64 timeStamp, IMFSample* sample)
    {
        


        return S_OK;
    }
}