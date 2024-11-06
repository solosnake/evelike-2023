#ifndef blue_suntype_hpp
#define blue_suntype_hpp

#include <cstdint>

namespace blue
{
    /// There are four types of Suns. These types are purely arbitrary and decorative:
    /// the actual energy output and its range depends only the size of the Sun.
    enum SunType : std::uint16_t
    {
        SunType0,
        SunType1,
        SunType2,
        SunType3
    };

    constexpr std::uint16_t BLUE_SUNTYPE_COUNT  {4u};

    /// Returns true if @a s in the range to be a value SunType enum.
    inline bool can_be_a_suntype( const std::uint16_t s )
    {
        return ( s == SunType0 ) || ( s == SunType1 ) || ( s == SunType2 ) || ( s == SunType3 );
    }
}

#endif
