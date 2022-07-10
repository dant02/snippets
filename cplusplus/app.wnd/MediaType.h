#pragma once

#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <mfreadwrite.h>

#include "Helper.h"

namespace WndApp
{
    ref class MediaType
    {
    public:
        MediaType(IMFMediaType* mediaType, unsigned int streamIndex, GUID guid, unsigned int width, unsigned int height);
        ~MediaType();
        !MediaType();

        void SetOutputMediaType(IMFSourceReader* reader);
        void SetInputMediaType(IMFSinkWriter* writer, DWORD sinkStream);
        void CopyAttribute(IMFMediaType* target, const GUID id);

        property unsigned int Width { unsigned int get() { return this->width; } }
        property unsigned int Height { unsigned int get() { return this->height; } }
        property Guid^ MediaGUID { Guid^ get() { return this->guid; } }

    private:
        IMFMediaType* mediaType;
        unsigned int streamIndex = 0;
        Guid^ guid;
        unsigned int width;
        unsigned int height;
    };
}
