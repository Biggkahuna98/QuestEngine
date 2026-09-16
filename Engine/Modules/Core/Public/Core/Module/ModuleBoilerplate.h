#pragma once

#include "Core/Memory/Memory.h"
#include <new>

// Always, ALWAYS, put this in exactly one .cpp file in each module
#define OVERRIDE_NEW_DELETE \
    void* operator new(std::size_t size) { return Quest::Memory::Malloc(size); } \
    void* operator new[](std::size_t size) { return Quest::Memory::Malloc(size); } \
    void* operator new(std::size_t size, const std::nothrow_t&) noexcept { return Quest::Memory::Malloc(size); } \
    void* operator new[](std::size_t size, const std::nothrow_t&) noexcept { return Quest::Memory::Malloc(size); } \
    void* operator new(std::size_t size, std::align_val_t alignment) { return Quest::Memory::Malloc(size, (std::size_t)alignment); } \
    void* operator new[](std::size_t size, std::align_val_t alignment) { return Quest::Memory::Malloc(size, (std::size_t)alignment); } \
    void* operator new(std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept { return Quest::Memory::Malloc(size, (std::size_t)alignment); } \
    void* operator new[](std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept { return Quest::Memory::Malloc(size, (std::size_t)alignment); } \
    void operator delete(void* ptr) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete[](void* ptr) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete(void* ptr, const std::nothrow_t&) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete[](void* ptr, const std::nothrow_t&) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete(void* ptr, std::size_t size) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete[](void* ptr, std::size_t size) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete(void* ptr, std::size_t size, const std::nothrow_t&) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete[](void* ptr, std::size_t size, const std::nothrow_t&) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete(void* ptr, std::align_val_t alignment) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete[](void* ptr, std::align_val_t alignment) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete(void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete[](void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete(void* ptr, std::size_t size, std::align_val_t alignment) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete[](void* ptr, std::size_t size, std::align_val_t alignment) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete(void* ptr, std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept { Quest::Memory::Free(ptr); } \
    void operator delete[](void* ptr, std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept { Quest::Memory::Free(ptr); } \

