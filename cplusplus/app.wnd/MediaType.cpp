#include "MediaType.h"

namespace WndApp
{
    MediaType::MediaType(IMFMediaType* mediaType, unsigned int streamIndex, GUID guid, unsigned int width, unsigned int height)
    {
        this->mediaType = mediaType;
        this->streamIndex = streamIndex;
        this->guid = Helper::ToGuid(guid);
        this->width = width;
        this->height = height;
    }

    MediaType::~MediaType()
    {
        this->!MediaType();
    }

    MediaType::!MediaType()
    {
        if (this->mediaType != nullptr)
        {
            this->mediaType->Release();
            this->mediaType = nullptr;
        }
    }

    void MediaType::SetOutputMediaType(IMFSourceReader* reader)
    {
        auto hr = reader->SetCurrentMediaType(streamIndex, NULL, this->mediaType);
        Helper::CheckHResult(hr, "Error on IMFSourceReader SetCurrentMediaType");
    }

    void MediaType::SetInputMediaType(IMFSinkWriter* writer, DWORD sinkStream)
    {
        auto hr = writer->SetInputMediaType(sinkStream, mediaType, NULL);
        Helper::CheckHResult(hr, "Writer SetInputMediaType");
    }

    void MediaType::CopyAttribute(IMFMediaType* target, const GUID id)
    {
        auto hr = Helper::CopyAttribute(this->mediaType, target, id);
        Helper::CheckHResult(hr, "Error on copy attribure.");
    }
}
