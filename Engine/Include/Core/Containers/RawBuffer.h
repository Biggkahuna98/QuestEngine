#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace QE
{
    struct QUEST_API RawBuffer
    {
        RawBuffer() = default;
        ~RawBuffer()
        {
            if (Data)
                Release();
        }

        RawBuffer(uint64_t size)
            : Size(size)
        {
            Allocate(size);
        }

        RawBuffer(const void* data, uint64_t dataCount, uint64_t size)
            : Data((uint8_t*)data), DataCount(dataCount), Size(size)
        {
        }

        RawBuffer(const RawBuffer&) = default;

        void Allocate(uint64_t size)
        {
            // Clear buffer before allocation
            Release();

            Data = (uint8_t*)malloc(size);
            Size = size;
        }

        void Release()
        {
            free(Data);

            Data = nullptr;
            Size = 0;
            DataCount = 0;
        }

        static RawBuffer Copy(RawBuffer buff)
        {
            RawBuffer newBuff(buff.Size);
            memcpy(newBuff.Data, buff.Data, buff.Size);
            return newBuff;
        }

        template<typename T>
        T* Cast()
        {
            return (T*)Data;
        }

        operator bool() const
        {
            return (bool)Data;
        }

        uint8_t* Data = nullptr;
        // Size (in bytes) of the buffer
        uint64_t Size = 0;
        uint64_t DataCount = 0;
    };
}