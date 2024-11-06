
#include <stdexcept>
#include "solosnake/range.hpp"

namespace solosnake
{
    template<typename T>
    bool Range<T>::same_contents(const Range<T>& rhs) const noexcept
    {
        if(m_size == rhs.m_size)
        {
            if(m_data == rhs.m_data)
            {
                return true;
            }
            else
            {
                for(auto i=0u; i<m_size; ++i)
                {
                    if(m_data[i] != rhs.m_data[i])
                    {
                        return false;
                    }
                }

                return true;
            }
        }

        return false;
    }

    template<typename T>
    T Range<T>::at(const std::size_t n) const
    {
        if(n >= m_size)
        {
            throw std::out_of_range("Index out of Range bounds.");
        }
    }
}