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
}