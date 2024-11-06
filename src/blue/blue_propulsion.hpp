#ifndef blue_propulsion_hpp
#define blue_propulsion_hpp

#include <cstdint>
#include <cassert>
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_turning_system.hpp"

namespace blue
{
    class Hardware;
    class Movement_grid_view;
    class Machine_grid_view;
    struct Position_info;

    /// The system that causes a Machine to turn and accelerate and which
    /// changes its position.
    class Propulsion
    {
    public:

        Propulsion();

        explicit Propulsion( const Hardware& );

        Position_info       get_position_info() const noexcept;

        Hex_coord           gridXY() const noexcept;

        Fixed_angle         angle() const noexcept;

        HexFacingName       facing() const noexcept;

        bool                is_advancing() const noexcept;

        bool                is_turning() const noexcept;

        bool                is_not_turning() const noexcept;

        bool                is_turning_cw() const noexcept;

        bool                is_turning_ccw() const noexcept;

        bool                is_moving() const noexcept;

        bool                is_not_moving() const noexcept;

        bool                is_accelerating() const noexcept;

        int                 max_speed() const noexcept;

        int                 max_turn_speed() const noexcept;

        float               speed_level() const noexcept;

        float               turn_speed_level() const noexcept;

        Radians             angle_as_radians() const noexcept;

        std::uint16_t       number_of_tiles_advanced() const noexcept;

        void                offset_after_n_frames( const float frames, float* unaliased xy ) const;

        HexFacingName       facing_after_n_turns( bool turn_ccw, std::uint16_t n ) const;

        void                tick( Movement_grid_view& );

        void                reset( Hex_coord pos, HexFacingName facing );

        void                start_n_face_turns( bool turn_ccw, std::uint16_t n );

        void                start_n_tile_move( Machine_grid_view& grid, std::uint16_t n );

    private:

        void tick_turning();

        void tick_advancing( Movement_grid_view& );

        void decelerate_to_centre();

        struct advancing_variables
        {
            advancing_variables();

            std::uint16_t       numTilesWanted_;
            std::uint16_t       numTilesAdvanced_;
            std::int16_t        currentddXYdtdt_;
            Hex_coord           nextTile_;
            bool                wantStopAtCentre_;
        };

    private:

        bool                  is_advancing_;
        advancing_variables   advancing_;
        Turning_system        turning_system_;
        Hex_coord             gridXY_;    // x, y; 0 to 255
        std::int32_t          distance_;  // How far the world position is from the board x/y.
        std::int32_t          dXYdt_;     // Speed per frame; 0 to BLUE_MAX_SPEED.
        std::int32_t          ddXYdtdt_;
        std::int32_t          max_speed_;
    };
}

#include "solosnake/blue/blue_propulsion.inl"
#endif
