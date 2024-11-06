#ifndef blue_turningsystem_hpp
#define blue_turningsystem_hpp

#include <cassert>
#include <cstdint>
#include <cmath>
#include "solosnake/blue/blue_fixed_angle.hpp"

namespace blue
{
    /// A system which will turn through n faces, accelerating to a max turn
    /// speed before decelerating to reach its intended facing angle. This can
    /// be used for any turning mechanical item in blue, machines and turrets.
    class Turning_system
    {
    public:

        Turning_system();

        /// @param turningPower - The rate of change of the rotation speed per
        /// frame, in 1/100 of a degree.
        Turning_system( Fixed_angle turningPower, Fixed_angle maxTurnSpeed );

        /// Resets turning to motionless and sets facing.
        void reset( HexFacingName f );

        /// Returns true if the system is turning and has not yet reached its
        /// desired facing direction.
        bool is_turning() const noexcept;

        /// Returns true if the system is not currently turning. The desired
        /// facing and the facing should be identical.
        bool is_not_turning() const noexcept;

        /// Returns true if the system is turning clockwise.
        bool is_turning_cw() const noexcept;

        /// Returns true if the system is turning counter-clockwise.
        bool is_turning_ccw() const noexcept;

        /// Sets the turning system to be facing @a startingAngle and begins it
        /// turnings @a turn_ccw by @a n faces. is_turning will return false
        /// when it has completed turning. If n > 0 then is_turning always
        /// returns false after this call, and requires at least one call to
        /// advance_one_frame to reach destination and for is_turning to return
        /// false again.
        void start_n_face_turns( Fixed_angle startingAngle, bool turn_ccw, std::uint16_t n );

        /// Returns the expected facing after n turns in the given direction
        /// (ccw = true).
        HexFacingName facing_after_n_turns( bool turn_ccw, std::uint16_t n ) const;

        /// Moves the system forward one frame's worth.
        void advance_one_frame();

        /// Returns the angle the system is facing, as it turns through the
        /// circle towards its intended facing. This angle might not be one
        /// of the 6 possible facing angles during the turn.
        Fixed_angle angle() const noexcept;

        /// Returns the direction (one of six) the system is considered to be
        /// facing. Throws if the system has not yet come to rest at an
        /// agreed facing angle. The result of this is undefined whilst the
        /// system is turning.
        /// @pre System is not turning.
        HexFacingName get_facing() const;

        /// The facing the system will come to rest facing towards.
        HexFacingName facing_wanted() const noexcept;

        /// Returns max turn speed.
        int max_turn_speed() const noexcept;

        /// Returns a measure of how fast the system is turning, 0.0 to 1.0
        float turn_speed_level() const noexcept;

    private:

        unsigned int    degrees_wanted_;        // Constant during turn.
        unsigned int    degrees_turned_through_;
        unsigned int    deceleration_marker_;
        HexFacingName   facingWanted_;          // Constant during turn.
        Fixed_angle     angle_;                 // Angle faced during turn.
        Fixed_angle     dAdt_;                  // Turning speed per frame; -BLUE_INT32_TWO_PI to +BLUE_INT32_TWO_PI
        Fixed_angle     ddAdtdt_;               // Constant during turn.
        Fixed_angle     max_turn_speed_;        // Constant during turn.
    };
}

#include "solosnake/blue/blue_turning_system.inl"
#endif
