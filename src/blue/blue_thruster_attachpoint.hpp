#ifndef blue_thruster_attachpoint_hpp
#define blue_thruster_attachpoint_hpp

#include "solosnake/blue/blue_attachpoint.hpp"

namespace blue
{
    /// Represents a valid location number where a thruster can be
    /// attached, to form a ship. Ships (bots) are built using blueprints,
    /// and blueprints are formed from collections of components at
    /// attach points.
    /// Some attach points can be thruster attachpoints.
    class Thruster_attachpoint final : public AttachPoint
    {
    public:

        /// Constructs a Thruster_attachpoint at the first permitted
        /// valid attachpoint for a thruster.
        Thruster_attachpoint();

        /// Tries to construct a Thruster_attachpoint at attachpoint @a n.
        /// Throws if @a n is not an attachpoint that is permitted to have
        /// a thruster there - only a limited set of attach points permit
        /// thrusters.
        explicit Thruster_attachpoint( unsigned short n );

        bool operator < (const Thruster_attachpoint&) const noexcept;
        bool operator == (const Thruster_attachpoint&) const noexcept;
        bool operator != (const Thruster_attachpoint&) const noexcept;
    };
}

#endif
