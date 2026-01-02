#pragma once

#include "Core/Core.h"
#include "Core/QuestExport.h"
#include <atomic>
#include <cstddef>
#include <type_traits>

namespace Quest
{
    // Interfaces inherit this interface to make them compatible with the intrusive RefCountPtr and RefCounted.
    class IRefCountable
    {
    protected:
        IRefCountable() = default;
        virtual ~IRefCountable() = default;

    public:
        virtual unsigned long AddRef() = 0;
        virtual unsigned long Release() = 0;
        virtual unsigned long GetRefCount() = 0;

        // Returns a native object or interface, for example ID3D11Device*, or nullptr if the requested interface is unavailable.
        // Does *not* AddRef the returned interface.
        //virtual Object getNativeObject(ObjectType objectType) { (void)objectType; return nullptr; }

        // Non-copyable and non-movable
        IRefCountable(const IRefCountable&) = delete;
        IRefCountable(const IRefCountable&&) = delete;
        IRefCountable& operator=(const IRefCountable&) = delete;
        IRefCountable& operator=(const IRefCountable&&) = delete;
    };

    // RefCountPtr
    // Mostly a copy of Microsoft::WRL::ComPtr<T>
    // Also yoinked from NVRHI
    template <typename T>
    class RefCountPtr
    {
    public:
        typedef T InterfaceType;

        template <bool b, typename U = void>
        struct EnableIf
        {
        };

        template <typename U>
        struct EnableIf<true, U>
        {
            typedef U type;
        };

    protected:
        InterfaceType *ptr_;
        template<class U> friend class RefCountPtr;

        void InternalAddRef() const noexcept
        {
            if (ptr_ != nullptr)
            {
                ptr_->AddRef();
            }
        }

        unsigned long InternalRelease() noexcept
        {
            unsigned long ref = 0;
            T* temp = ptr_;

            if (temp != nullptr)
            {
                ptr_ = nullptr;
                ref = temp->Release();
            }

            return ref;
        }

    public:

        RefCountPtr() noexcept : ptr_(nullptr)
        {
        }

        RefCountPtr(std::nullptr_t) noexcept : ptr_(nullptr)
        {
        }

        template<class U>
        RefCountPtr(U *other) noexcept : ptr_(other)
        {
            InternalAddRef();
        }

        RefCountPtr(const RefCountPtr& other) noexcept : ptr_(other.ptr_)
        {
            InternalAddRef();
        }

        // copy ctor that allows to instanatiate class when U* is convertible to T*
        template<class U>
        RefCountPtr(const RefCountPtr<U> &other, typename std::enable_if<std::is_convertible<U*, T*>::value, void *>::type * = nullptr) noexcept :
            ptr_(other.ptr_)

        {
            InternalAddRef();
        }

        RefCountPtr(RefCountPtr &&other) noexcept : ptr_(nullptr)
        {
            if (this != reinterpret_cast<RefCountPtr*>(&reinterpret_cast<unsigned char&>(other)))
            {
                Swap(other);
            }
        }

        // Move ctor that allows instantiation of a class when U* is convertible to T*
        template<class U>
        RefCountPtr(RefCountPtr<U>&& other, typename std::enable_if<std::is_convertible<U*, T*>::value, void *>::type * = nullptr) noexcept :
            ptr_(other.ptr_)
        {
            other.ptr_ = nullptr;
        }

        ~RefCountPtr() noexcept
        {
            InternalRelease();
        }

        RefCountPtr& operator=(std::nullptr_t) noexcept
        {
            InternalRelease();
            return *this;
        }

        RefCountPtr& operator=(T *other) noexcept
        {
            if (ptr_ != other)
            {
                RefCountPtr(other).Swap(*this);
            }
            return *this;
        }

        template <typename U>
        RefCountPtr& operator=(U *other) noexcept
        {
            RefCountPtr(other).Swap(*this);
            return *this;
        }

        RefCountPtr& operator=(const RefCountPtr &other) noexcept  // NOLINT(bugprone-unhandled-self-assignment)
        {
            if (ptr_ != other.ptr_)
            {
                RefCountPtr(other).Swap(*this);
            }
            return *this;
        }

        template<class U>
        RefCountPtr& operator=(const RefCountPtr<U>& other) noexcept
        {
            RefCountPtr(other).Swap(*this);
            return *this;
        }

        RefCountPtr& operator=(RefCountPtr &&other) noexcept
        {
            RefCountPtr(static_cast<RefCountPtr&&>(other)).Swap(*this);
            return *this;
        }

        template<class U>
        RefCountPtr& operator=(RefCountPtr<U>&& other) noexcept
        {
            RefCountPtr(static_cast<RefCountPtr<U>&&>(other)).Swap(*this);
            return *this;
        }

        void Swap(RefCountPtr&& r) noexcept
        {
            T* tmp = ptr_;
            ptr_ = r.ptr_;
            r.ptr_ = tmp;
        }

        void Swap(RefCountPtr& r) noexcept
        {
            T* tmp = ptr_;
            ptr_ = r.ptr_;
            r.ptr_ = tmp;
        }

        [[nodiscard]] T* Get() const noexcept
        {
            return ptr_;
        }

        operator T*() const
        {
            return ptr_;
        }

        InterfaceType* operator->() const noexcept
        {
            return ptr_;
        }

        T** operator&()   // NOLINT(google-runtime-operator)
        {
            return &ptr_;
        }

        [[nodiscard]] T* const* GetAddressOf() const noexcept
        {
            return &ptr_;
        }

        [[nodiscard]] T** GetAddressOf() noexcept
        {
            return &ptr_;
        }

        [[nodiscard]] T** ReleaseAndGetAddressOf() noexcept
        {
            InternalRelease();
            return &ptr_;
        }

        T* Detach() noexcept
        {
            T* ptr = ptr_;
            ptr_ = nullptr;
            return ptr;
        }

        // Set the pointer while keeping the object's reference count unchanged
        void Attach(InterfaceType* other)
        {
            if (ptr_ != nullptr)
            {
                auto ref = ptr_->Release();
                (void)ref;

                // Attaching to the same object only works if duplicate references are being coalesced. Otherwise
                // re-attaching will cause the pointer to be released and may cause a crash on a subsequent dereference.
                //assert(ref != 0 || ptr_ != other);
            }

            ptr_ = other;
        }

        // Create a wrapper around a raw object while keeping the object's reference count unchanged
        static RefCountPtr<T> Create(T* other)
        {
            RefCountPtr<T> Ptr;
            Ptr.Attach(other);
            return Ptr;
        }

        unsigned long Reset()
        {
            return InternalRelease();
        }
    };

    using ResourceHandle = RefCountPtr<IRefCountable>;

    // RefCounted<T>
    // A class that implements reference counting in a way compatible with RefCountPtr.
    // Intended usage is to use it as a base class for interface implementations, like so:
    // class Texture : public RefCounted<Interface> { ... }
    template<class T>
    class RefCounted : public T
    {
    private:
        // start at 1 for COM, think i will start at 0 for now
        std::atomic<unsigned long> m_RefCount = 0;
    public:
        virtual unsigned long AddRef() override
        {
            return ++m_RefCount;
        }

        virtual unsigned long Release() override
        {
            unsigned long result = --m_RefCount;
            if (result == 0) {
                delete this;
            }
            return result;
        }

        virtual unsigned long GetRefCount() override
        {
            return m_RefCount;
        }
    };

    template<typename T>
    class WeakRefCountPtr
    {
    public:
        WeakRefCountPtr() = default;

        WeakRefCountPtr(RefCountPtr<T> ref)
        {
            m_Ptr = ref.GetRawPtr();
        }

        WeakRefCountPtr(T* ptr)
        {
            m_Ptr = ptr;
        }

        T* operator->() { return m_Ptr; }
        const T* operator->() const { return m_Ptr; }

        T& operator*() { return *m_Ptr;}
        const T& operator*() const { return *m_Ptr; }

        bool IsValid() const { return m_Ptr != nullptr; }
        operator bool() const { return m_Ptr != nullptr; }

        template<typename T2>
        WeakRefCountPtr<T2> CastTo() const
        {
            return WeakRefCountPtr<T2>(dynamic_cast<T2*>(m_Ptr));
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
            return hash(ptr.GetRawPtr());
        }
    };
}