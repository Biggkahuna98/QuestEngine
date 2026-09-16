#include "Core/Platform/PlatformDetection.h"

#if QE_PLATFORM_WINDOWS

#include "Core/Platform/DynamicLibrary.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <string>

namespace Quest
{
    DynamicLibrary::DynamicLibrary(DynamicLibrary&& other) noexcept
        : m_Handle(other.m_Handle)
    {
        other.m_Handle = nullptr;
    }

    DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& other) noexcept
    {
        if (this != &other)
        {
            Unload();
            m_Handle = other.m_Handle;
            other.m_Handle = nullptr;
        }
        return *this;
    }

    bool DynamicLibrary::Load(std::string_view path)
    {
        Unload();
        m_Handle = LoadLibraryA(std::string(path).c_str());
        return m_Handle != nullptr;
    }

    void DynamicLibrary::Unload()
    {
        if (m_Handle)
        {
            FreeLibrary(static_cast<HMODULE>(m_Handle));
            m_Handle = nullptr;
        }
    }

    void* DynamicLibrary::GetSymbol(const char* name) const
    {
        if (!m_Handle)
            return nullptr;
        return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(m_Handle), name));
    }
}

#endif
