#ifndef blue_sun_hpp
#define blue_sun_hpp

#include <cstdint>
#include <string>
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_sunstrength.hpp"
#include "solosnake/blue/blue_suntype.hpp"

namespace blue
{
    /// A sun is a non tile object which can have a strength from 0 .. 7.
    /// The strength of the sun determines how much solar energy a tile
    /// can yield.
    class Sun
    {
    public:

        Sun( const SunStrength, const SunType, const Hex_coord );

        float       radius() const;

        Hex_coord    board_xy() const;

        SunStrength sun_strength() const;

        SunType     sun_type() const;

        bool operator == (const Sun& ) const;
        bool operator != (const Sun& ) const;

    private:

        Hex_coord       xy_;
        std::uint16_t   strength_;
        SunType         type_;
    };
}

#include "solosnake/blue/blue_sun.inl"

#endif
