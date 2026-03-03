#pragma once

#include "Core/Core.h"
#include "Core/QuestExport.h"
#include <atomic>
#include <cstddef>
#include <type_traits>

namespace Quest
{
    class RefCounted
    {
    public:
        virtual ~RefCounted() = default;

        void AddRef() const noexcept
        {
            ++m_RefCount;
        }

        void Release() const noexcept
        {
            --m_RefCount;
        }

        uint32_t GetRefCount() const noexcept
        {
            return m_RefCount.load();
        }
    private:
        mutable std::atomic<uint32_t> m_RefCount = 0;
    };

    template<typename T>
    class RefCountPtr
    {
    public:
        RefCountPtr() noexcept : m_Ptr(nullptr) {}
        RefCountPtr(std::nullptr_t) noexcept : m_Ptr(nullptr) {}

        RefCountPtr(T* ptr) noexcept
            : m_Ptr(ptr)
        {
            static_assert(std::is_base_of_v<RefCounted, T>, "T must be derived from RefCounted");

            AddRef();
        }

        RefCountPtr(const RefCountPtr<T>& other) noexcept
            : m_Ptr(other.m_Ptr)
        {
            AddRef();
        }


        // Conversion from RefCountPtr<U> where U* -> T* is valid
        template<typename U>
        RefCountPtr(const RefCountPtr<U>& other) noexcept
        {
            m_Ptr = static_cast<T*>(other.m_Ptr);
            AddRef();
        }

        template<typename U>
        RefCountPtr(RefCountPtr<U>&& other) noexcept
        {
            m_Ptr = static_cast<T*>(other.m_Ptr);
            other.m_Ptr = nullptr;
        }

        ~RefCountPtr()
        {
            Release();
        }

        RefCountPtr& operator=(std::nullptr_t)
        {
            Release();
            m_Ptr = nullptr;
            return *this;
        }

        RefCountPtr& operator=(const RefCountPtr& other) noexcept
        {
            if (this == &other)
                return *this;

            other.AddRef();
            Release();
            m_Ptr = other.m_Ptr;

            return *this;
        }

        template<typename U>
        RefCountPtr& operator=(const RefCountPtr<U>& other) noexcept
        {
            other.AddRef();
            Release();

            m_Ptr = static_cast<T*>(other.m_Ptr);
            return *this;
        }

        template<typename U>
        RefCountPtr& operator=(RefCountPtr<U>&& other) noexcept
        {
            Release();

            m_Ptr = static_cast<T*>(other.m_Ptr);
            other.m_Ptr = nullptr;
            return *this;
        }

        T* Get() { return m_Ptr; }
        const T* Get() const { return m_Ptr; }

        T* operator->() noexcept { return m_Ptr; }
        const T* operator->() const noexcept { return m_Ptr; }

        T& operator*() noexcept { return *m_Ptr; }
        const T& operator*() const noexcept { return *m_Ptr; }

        operator bool() noexcept { return m_Ptr != nullptr; }
        operator bool() const noexcept { return m_Ptr != nullptr; }

        bool operator==(const RefCountPtr& other) const noexcept { return m_Ptr == other.m_Ptr; }
        bool operator!=(const RefCountPtr& other) const noexcept { return !(*this == other); }

        bool Equals(const RefCountPtr& other) const noexcept
        {
            if (!m_Ptr || !other.m_Ptr)
                return false;
            return *m_Ptr == *other.m_Ptr;
        }

        void Reset(T* ptr = nullptr) noexcept
        {
            Release();
            m_Ptr = ptr;
        }

        template<typename... Args>
        static RefCountPtr<T> Create(Args&&... args)
        {
            return RefCountPtr<T>(new T(std::forward<Args>(args)...));
        }

        template<typename U>
        RefCountPtr<U> CastTo() const
        {
            return RefCountPtr<U>(dynamic_cast<U*>(m_Ptr));
        }
    private:
        void AddRef() const
        {
            if (m_Ptr)
                m_Ptr->AddRef();
        }

        void Release() const
        {
            if (m_Ptr)
            {
                m_Ptr->Release();

                if (m_Ptr->GetRefCount() == 0)
                {
                    delete m_Ptr;
                    m_Ptr = nullptr;
                }
            }
        }

        template<class U>
        friend class RefCountPtr;
        mutable T* m_Ptr = nullptr;
    };

    template<typename T, typename... Args>
    RefCountPtr<T> MakeRefCounted(Args&&... args)
    {
        return RefCountPtr<T>(new T(std::forward<Args>(args)...));
    }

    template<typename T>
    class WeakRefCountPtr
    {
    public:
        WeakRefCountPtr() noexcept = default;

        WeakRefCountPtr(RefCountPtr<T> ptr) noexcept
        {
            m_Ptr = ptr.Get();
        }

        WeakRefCountPtr(T* ptr)
        {
            m_Ptr = ptr;
        }

        ~WeakRefCountPtr() = default;

        T* operator->() { return m_Ptr; }
        const T* operator->() const { return m_Ptr; }

        T& operator*() { return *m_Ptr;}
        const T& operator*() const { return *m_Ptr; }

        bool IsValid() const { return m_Ptr != nullptr; }
        operator bool() const { return m_Ptr != nullptr; }

        template<typename U>
        WeakRefCountPtr<U> CastTo() const
        {
            return WeakRefCountPtr<U>(dynamic_cast<U*>(m_Ptr));
        }
    private:
        T* m_Ptr = nullptr;
    };
}

namespace std
{
    template<typename T> struct hash<Quest::RefCountPtr<T>>
    {
        std::size_t operator()(const Quest::RefCountPtr<T>& ptr) const noexcept
        {
            std::hash<T*> hash;
            return hash(ptr.Get());
        }
    };
}