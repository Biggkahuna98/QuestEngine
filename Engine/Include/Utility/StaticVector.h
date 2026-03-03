#pragma once
#include <cstdint>
#include <array>
#include "Core/Core.h"

namespace Quest
{
    template<typename T, uint32_t max_elements>
    class StaticVector
    {
    public:
        using ArrayType = std::array<T, max_elements>;
        using iterator = typename ArrayType::iterator;
        using const_iterator = typename ArrayType::const_iterator;

        StaticVector() = default;
        StaticVector(std::initializer_list<T> list)
        {
            for (auto& element : list)
            {
                push_back(element);
            }
        }

        T& operator[](uint32_t index)
        {
            QE_ASSERT(index < max_elements);
            return m_Data[index];
        }
        const T& operator[](uint32_t index) const
        {
            QE_ASSERT(index < max_elements);
            return m_Data[index];
        }

        T& at(uint32_t index)
        {
            QE_ASSERT(index < max_elements);
            return m_Data[index];
        }

        const T& at(uint32_t index) const
        {
            QE_ASSERT(index < max_elements);
            return m_Data[index];
        }

        void push_back(const T& element)
        {
            QE_ASSERT(m_CurrentSize < max_elements);
            m_Data[m_CurrentSize++] = element;
        }

        T& front()
        {
            QE_ASSERT(m_CurrentSize > 0);
            return m_Data[0];
        }

        const T& front() const
        {
            QE_ASSERT(m_CurrentSize > 0);
            return m_Data[0];
        }

        T& back()
        {
            QE_ASSERT(m_CurrentSize > 0);
            return m_Data[m_CurrentSize - 1];
        }

        const T& back() const
        {
            QE_ASSERT(m_CurrentSize > 0);
            return m_Data[m_CurrentSize - 1];
        }

        void fill(const T& value)
        {
            for (uint32_t i = 0; i < max_elements; i++)
                m_Data[i] = value;
            m_CurrentSize = max_elements;
        }

        T* data()
        {
            return m_Data.data();
        }

        const T* data() const
        {
            return m_Data.data();
        }

        iterator begin() { return m_Data.begin(); }
        const_iterator begin() const { return m_Data.begin(); }
        const_iterator cbegin() const { return m_Data.cbegin(); }

        iterator end() { return iterator(begin() + m_CurrentSize); }
        const_iterator end() const { return cend(); }
        const_iterator cend() const { return const_iterator(cbegin() + m_CurrentSize); }

        bool empty() const { return m_CurrentSize == 0; }
        uint32_t size() const { return m_CurrentSize; }
        uint32_t max_size() const { return max_elements; }

        ArrayType& GetUnderlyingArray() { return m_Data; }
    private:
        ArrayType m_Data;
        uint32_t m_CurrentSize = 0;
    };
}