#pragma once

#include "Core/CoreExport.h"

#include <string_view>

namespace Quest
{
    // Loads a shared library (.dll / .so / .dylib) and resolves symbols from it.
    // Compile-time platform abstraction: exactly one platform .cpp implements the
    // out-of-line members, so there is no virtual dispatch and no exposed OS types.
    class CORE_API DynamicLibrary
    {
    public:
        DynamicLibrary() = default;
        explicit DynamicLibrary(std::string_view path) { Load(path); }
        ~DynamicLibrary() { Unload(); }

        DynamicLibrary(DynamicLibrary&& other) noexcept;
        DynamicLibrary& operator=(DynamicLibrary&& other) noexcept;
        DynamicLibrary(const DynamicLibrary&) = delete;
        DynamicLibrary& operator=(const DynamicLibrary&) = delete;

        bool Load(std::string_view path);
        void Unload();
        bool IsLoaded() const { return m_Handle != nullptr; }

        // Returns the address of an exported symbol, or nullptr if not found.
        // The address is untyped: the caller casts it to the symbol's real type.
        // This is the only thing each platform .cpp implements for lookup.
        void* GetSymbol(const char* name) const;

        // Typed convenience for function symbols.
        template<typename Fn>
        Fn GetFunction(const char* name) const
        {
            return reinterpret_cast<Fn>(GetSymbol(name));
        }

        // Typed convenience for exported data/object symbols (returns its address).
        template<typename T>
        T* GetVariable(const char* name) const
        {
            return static_cast<T*>(GetSymbol(name));
        }

    private:
        void* m_Handle = nullptr;
    };
}
