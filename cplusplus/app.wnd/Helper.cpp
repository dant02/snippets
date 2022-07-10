#include "Helper.h"

namespace WndApp
{
    void Helper::CheckHResult(HRESULT result, String^ message)
    {
        if (SUCCEEDED(result))
        {
            return;
        }

        throw gcnew Exception(message);
    }

    Guid^ Helper::ToGuid(GUID guid)
    {
        return Guid(guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1],
            guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5],
            guid.Data4[6], guid.Data4[7]);
    }

    HRESULT Helper::CopyAttribute(IMFAttributes* pSrc, IMFAttributes* pDest, const GUID& key)
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
}