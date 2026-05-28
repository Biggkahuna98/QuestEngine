#include "RHI/QRHI.h"

namespace qrhi
{
    BufferRange BufferRange::Resolve(const BufferDesc& desc) const
    {
        BufferRange result;
        result.offsetInBytes = std::min(offsetInBytes, desc.sizeInBytes);
        if (sizeInBytes == 0)
            result.sizeInBytes = desc.sizeInBytes - result.offsetInBytes;
        else
            result.sizeInBytes = std::min(sizeInBytes, desc.sizeInBytes - result.offsetInBytes);
        return result;
    }
}
