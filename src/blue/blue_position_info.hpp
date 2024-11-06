#ifndef blue_position_info_hpp
#define blue_position_info_hpp

#include <cmath>
#include <cstdint>
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_game_logic_constants.hpp"
#include "solosnake/blue/blue_unaliased.hpp"

namespace blue
{
    constexpr std::int16_t BLUE_POSITION_INFO_CW          = -1;
    constexpr std::int16_t BLUE_POSITION_INFO_CCW         = +1;
    constexpr std::int16_t BLUE_POSITION_INFO_NOT_TURNING = 0;

    struct Position_info
    {
        std::int32_t        distance;  // How far the world position is from the board x/y.
        std::int32_t        dXYdt;     // Speed per frame, its magnitude is related to BLUE_FLOAT32_TILE_WIDTH; 0 to BLUE_MAX_SPEED.
        Fixed_angle         angle;     // Rotation expressed as a fixed point (int32).
        std::int16_t        turning;   // -1 when CW, +1 when CCW, 0 when not turning.
        Hex_coord           gridXY;    // x, y; 0 to 255

        /// @param frames is a floating point number of frames, whole or partial
        /// that have elapsed *since last tick*.
        /// @param Returns in xy (a 2D float) the BOARD location interpolated
        /// forwards by this number of frames. Scale this by the tile-width to
        /// and apply the world xy offset to get the correct WORLD location.
        /// This does not correctly calculate position across a complete move,
        /// due to acceleration, but it gives an approximate moving update
        /// based on last known position and last known speed.
        void board_offset_after_n_frames( float frames, float* unaliased xy ) const;

        Radians angle_as_radians() const;
    };
}

#include "solosnake/blue/blue_position_info.inl"
#endif
