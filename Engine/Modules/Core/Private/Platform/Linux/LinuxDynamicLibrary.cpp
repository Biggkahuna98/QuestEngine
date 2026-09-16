#include "Core/Platform/PlatformDetection.h"

#if QE_PLATFORM_LINUX

#include "Core/Platform/DynamicLibrary.h"

#include <dlfcn.h>

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
        m_Handle = dlopen(std::string(path).c_str(), RTLD_NOW | RTLD_LOCAL);
        return m_Handle != nullptr;
    }

    void DynamicLibrary::Unload()
    {
        if (m_Handle)
        {
            dlclose(m_Handle);
            m_Handle = nullptr;
        }
    }

    void* DynamicLibrary::GetSymbol(const char* name) const
    {
        if (!m_Handle)
            return nullptr;
        return dlsym(m_Handle, name);
    }
}

#endif
