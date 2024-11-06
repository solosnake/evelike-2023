#ifndef SOLOSNAKE_CONTAINS_HPP
#define SOLOSNAKE_CONTAINS_HPP
#include <algorithm>

#ifdef _MSC_VER
#ifndef INLINE
#define INLINE __forceinline
#endif
#else
#ifndef INLINE
#define INLINE inline
#endif
#endif

namespace solosnake
{
    template <typename T> INLINE bool contains( const T& t, const typename T::value_type& v )
    {
        return std::find( t.cbegin(), t.cend(), v ) != t.end();
    }
}

#endif
