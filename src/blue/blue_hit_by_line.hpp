#ifndef blue_idetecthit_hpp
#define blue_idetecthit_hpp

#include <cstdint>

namespace blue
{
    class Line_hit;
    struct Hex_coord;

    /// Returns the first object hit in the line of sight between from and
    /// target, and not further than maxSteps away.
    class Hit_by_line
    {
    public:

        virtual ~Hit_by_line() = default;

        virtual Line_hit find_first_hit_by_line( Hex_coord from,
                                                 Hex_coord target,
                                                 std::uint16_t maxSteps ) const = 0;
    };
}

#endif
