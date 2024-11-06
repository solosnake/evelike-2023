#ifndef SOLOSNAKE_RANGE_HPP
#define SOLOSNAKE_RANGE_HPP

#include <array>
#include <vector>

namespace solosnake
{
    template<typename T>
    class Range
    {
    public:

        Range() noexcept = default;

        explicit Range(const T* p, std::size_t n) noexcept
        : m_data(p), m_size(n)
        {
        }

        Range(const std::vector<T>& v) noexcept
        : m_data(v.data()), m_size(v.size())
        {
        }

        template <std::size_t N>
        Range(const std::array<T,N>& a) noexcept
        : m_data(a.data()), m_size(N)
        {
        }

        const T* begin() const noexcept
        {
            return m_data;
        }

        const T* end() const noexcept
        {
            return m_data + m_size;
        }

        std::size_t byte_size() const noexcept
        {
            return sizeof(T) * m_size;
        }

        std::size_t size() const noexcept
        {
            return m_size;
        }

        const T* data() const noexcept
        {
            return m_data;
        }

        T operator [] (std::size_t n) const noexcept
        {
            return m_data[n];
        }

        T at(std::size_t n) const;

        bool operator == (const Range<T>& rhs) const noexcept
        {
            return (m_size == rhs.m_size) && same_contents(rhs);
        }

        bool operator != (const Range<T>& rhs) const noexcept
        {
            return not (*this == rhs);
        }

    private:

        bool same_contents(const Range<T>& rhs) const noexcept;

    private:
        const T*    m_data{nullptr};
        std::size_t m_size{0u};
    };
}

#include "solosnake/range.inl"
#endif
