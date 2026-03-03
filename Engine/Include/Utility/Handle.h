#pragma once

#include <spdlog/fmt/fmt.h>
#include <functional>

namespace Quest
{
    // Template parameter for the underlying handle type
    // Note: to print with fmt, you must call handle.GetID() otherwise it will not compile
    // Note: it is up to the provider of the handles to set and manage the IDs to ensure uniqueness
    template<typename T, typename U = uint32_t>
    class Handle_t
    {
    public:
        using UnderlyingType = U;

        Handle_t() {}
        Handle_t(UnderlyingType id) : m_ID(id) {}
        ~Handle_t() = default;

        explicit operator UnderlyingType() const { return m_ID; }
        UnderlyingType GetID() const { return m_ID; }
        // Should only use this if you know what you are doing
        void SetID(UnderlyingType id) { m_ID = id; }

        bool operator==(const Handle_t& other) const { return m_ID == other.m_ID; }
        bool operator!=(const Handle_t& other) const { return m_ID != other.m_ID; }
    private:
        UnderlyingType m_ID = 0; // 0 is invalid handle
    };

    template<class T>
    concept Integral = std::is_integral_v<T>;

    // A simple provided handle ID manager
    // Not the most type safe, you could use the wrong assigner for a handle type... but it works
    template<Integral T>
    class HandleAssigner
    {
    public:
        HandleAssigner() = default;
        ~HandleAssigner() = default;

        T GetNextID() { return ++m_ID; }
        T GetCurrentID() { return m_ID; } // no increment
        void Reset() { m_ID = 0; }
    private:
        T m_ID = 0;
    };
}

namespace std
{
    template<typename T, typename UnderlyingType>
    struct hash<Quest::Handle_t<T, UnderlyingType>>
    {
        size_t operator()(const Quest::Handle_t<T, UnderlyingType>& handle) const noexcept
        {
            return std::hash<UnderlyingType>{}(handle.GetID());
        }
    };
}

// for spdlog
template<typename T, typename UnderlyingType>
struct fmt::formatter<Quest::Handle_t<T, UnderlyingType>>
{
    // Allow normal {} formatting (no custom specifiers yet)
    constexpr auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const Quest::Handle_t<T, UnderlyingType>& handle,
                FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", handle.GetID());
    }
};