#include "Helper.h"

namespace App
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