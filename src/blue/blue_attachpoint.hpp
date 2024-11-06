#ifndef blue_attachpoint_hpp
#define blue_attachpoint_hpp

#include "solosnake/blue/blue_point2d.hpp"
#include "solosnake/blue/blue_point3d.hpp"
#include "solosnake/blue/blue_throw.hpp"

/// This is not exposed. It is a variable related to how many attach-points
/// a Machine is permitted to have and how many can fit inside a unit
/// square.
#   define BLUE_ATTACHPOINT_SCALE   (0.14f)

namespace blue
{
    //                     Occupies 10 x 10 grid
    //
    //                +--+--+--+--+--+--+--+--+--+--+
    //                |  |  |  |  |  |  |  |  |  |  |
    //                +--+--+--+--+--+--+--+--+--+--+
    //                |  |  |  |  |  |  |  |  |  |  |
    //                +--+--+--+--+--+--+--+--+--+--+
    //                |  |  |  |  |  |  |  |  |  |  |
    //                +--+--+--+--+--+--+--+--+--+--+
    //                |  |  |  |  |  |  |  |  |  |  |
    //                +--+--+--+--+--+--+--+--+--+--+
    //                |  |  |  |  |  |  |  |  |  |  |
    //                +--+--+--+--+--+--+--+--+--+--+
    //                |  |  |  |  |  |  |  |  |  |  |
    //                +--+--+--+--+--+--+--+--+--+--+
    //                |  |  |  |  |  |  |  |  |  |  |
    //                +--+--+--+--+--+--+--+--+--+--+
    //                |  |  |  |  |  |  |  |  |  |  |
    //                +--+--+--+--+--+--+--+--+--+--+
    //                |  |  |  |  |  |  |  |  |  |  |
    //                +--+--+--+--+--+--+--+--+--+--+
    //                |  |  |  |  |  |  |  |  |  |  |
    //                +--+--+--+--+--+--+--+--+--+--+
    //
    //
    //                 MAX POSSIBLE MACHINE SIZE (23)
    //
    //                   +-----+-----+-----+-----+
    //                   |     |     |     |     |
    //                   |     |     |     |     |
    //                   |     |     |     |     |
    //                +--+--+--+--+--+--+--+--+--+--+
    //                |     |     |     |     |     |
    //                |     |     |     |     |     |
    //                |     |     |     |     |     |
    //                +-----+-----+-----+-----+-----+
    //                |     |     |     |     |     |
    //                |     |     |     |     |     |
    //                |     |     |     |     |     |
    //                +-----+-----+-----+-----+-----+
    //                |     |     |     |     |     |
    //                |     |     |     |     |     |
    //                |     |     |     |     |     |
    //                +-----+-----+-----+-----+-----+
    //                   |     |     |     |     |
    //                   |     |     |     |     |
    //                   |     |     |     |     |
    //                   +-----+-----+-----+-----+
    //
    //
    //                        Attachpoints (70)
    //
    //                    -- -- -- -- -- -- -- --
    //                   |                       |
    //                 --   *--*--*--*--*--*--*   --     1-7
    //                |     |  |  |  |  |  |  |     |
    //                |  *--*--*--*--*--*--*--*--*  |   10-18
    //                |  |  |  |  |  |  |  |  |  |  |
    //                |  *--*--*--*--*--*--*--*--*  |   20-28
    //                |  |  |  |  |  |  |  |  |  |  |
    //                |  *--*--*--*--*--*--*--*--*  |   30-38
    //                |  |  |  |  |  |  |  |  |  |  |
    //                |  *--*--*--*--*--*--*--*--*  |   40-48
    //                |  |  |  |  |  |  |  |  |  |  |
    //                |  *--*--*--*--*--*--*--*--*  |   50-58
    //                |  |  |  |  |  |  |  |  |  |  |
    //                |  *--*--*--*--*--*--*--*--*  |   60-68
    //                |  |  |  |  |  |  |  |  |  |  |
    //                |  *--*--*--*--*--*--*--*--*  |   70-78
    //                |     |  |  |  |  |  |  |     |
    //                 --   *--*--*--*--*--*--*   --    81-87
    //                   |                       |
    //                    -- -- -- -- -- -- -- --
    //
    /// Represents a valid location number where a Component can be
    /// attached, to form a ship. Ships (bots) are built using blueprints,
    /// and blueprints are formed from collections of components at
    /// attachpoints.
    class AttachPoint
    {
    public:

        /// Constructs an AttachPoint with the first possible
        /// valid location value.
        AttachPoint() = default;

        /// Constructs an AttachPoint with the location value @a n.
        /// Throws if this is not a permitted location value.
        AttachPoint( const unsigned short n );

        explicit operator unsigned short() const noexcept
        {
            return location_number_;
        }

        unsigned short location_number() const noexcept
        {
            return location_number_;
        }

        /// Returns true if a Thruster can be placed here.
        bool is_thruster_attachpoint() const noexcept;

        /// Returns true if placing a Component on @a other would cause it
        /// to overlap with a Component placed on this AttachPoint.
        bool conflicts_with( const AttachPoint& other ) const noexcept;

        /// Returns the location of the centre of this AttachPoint
        /// as if sitting on the board. The board is considered to be
        /// at Y=0 the x-z plane.
        Point3d get_3d_hex_location() const noexcept;

        /// Returns the unscaled 2D location of the centre of this AttachPoint
        /// within the unscaled hex.
        Point2d get_2d_hex_location() const noexcept;

        bool operator < (const AttachPoint&) const noexcept;
        bool operator == (const AttachPoint&) const noexcept;
        bool operator != (const AttachPoint&) const noexcept;

    private:

        unsigned short location_number_ = 1u;
    };

    /// Returns true if this is a valid location number.
    bool is_location_number( const unsigned int ) noexcept;

    /// Returns true if this is a valid location number.
    bool is_location_number( const int ) noexcept;
}

#endif
