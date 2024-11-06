#include "solosnake/blue/blue_propulsion.hpp"
#include "solosnake/blue/blue_position_info.hpp"

namespace blue
{
    inline Hex_coord Propulsion::gridXY() const noexcept
    {
        return gridXY_;
    }

    inline Fixed_angle Propulsion::angle() const noexcept
    {
        return turning_system_.angle();
    }

    inline HexFacingName Propulsion::facing() const noexcept
    {
        return turning_system_.get_facing();
    }

    inline bool Propulsion::is_advancing() const noexcept
    {
        return is_advancing_;
    }

    inline bool Propulsion::is_turning() const noexcept
    {
        return turning_system_.is_turning();
    }

    inline bool Propulsion::is_not_turning() const noexcept
    {
        return ! turning_system_.is_turning();
    }

    inline bool Propulsion::is_turning_cw() const noexcept
    {
        return turning_system_.is_turning_cw();
    }

    inline bool Propulsion::is_turning_ccw() const noexcept
    {
        return turning_system_.is_turning_ccw();
    }

    inline bool Propulsion::is_moving() const noexcept
    {
        return is_advancing_ || is_turning();
    }

    inline bool Propulsion::is_not_moving() const noexcept
    {
        return ! is_moving();
    }

    inline bool Propulsion::is_accelerating() const noexcept
    {
        return is_advancing_ && !advancing_.wantStopAtCentre_;
    }

    inline int Propulsion::max_speed() const noexcept
    {
        return max_speed_;
    }

    inline int Propulsion::max_turn_speed() const noexcept
    {
        return turning_system_.max_turn_speed();
    }

    inline float Propulsion::speed_level() const noexcept
    {
        return max_speed_ ? static_cast<float>(dXYdt_) / max_speed_ : 0.0f;
    }

    inline float Propulsion::turn_speed_level() const noexcept
    {
        return turning_system_.turn_speed_level();
    }

    inline Radians Propulsion::angle_as_radians() const noexcept
    {
        return blue::fixed_angle_to_radians(turning_system_.angle());
    }

    inline std::uint16_t Propulsion::number_of_tiles_advanced() const noexcept
    {
        return is_advancing_ ? advancing_.numTilesAdvanced_ : 0;
    }

    inline void Propulsion::offset_after_n_frames(const float frames, float *unaliased xy) const
    {
        const auto a = blue::fixed_angle_to_radians(turning_system_.angle());
        xy[0] = (a.cos() * (distance_ + (dXYdt_ * frames))) / BLUE_FLOAT32_TILE_WIDTH;
        xy[1] = (a.sin() * (distance_ + (dXYdt_ * frames))) / BLUE_FLOAT32_TILE_WIDTH;
    }

    inline HexFacingName Propulsion::facing_after_n_turns(bool turn_ccw, std::uint16_t n) const
    {
        return turning_system_.facing_after_n_turns(turn_ccw, n);
    }

    inline void Propulsion::start_n_face_turns(bool turn_ccw, std::uint16_t n)
    {
        assert( is_not_moving() );
        assert( blue::is_facing_angle(turning_system_.angle()) );
        turning_system_.start_n_face_turns(turning_system_.angle(), turn_ccw, n);
    }

}
