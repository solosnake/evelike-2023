#include <cassert>
#include <cstring>
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_game_hex_grid.hpp"
#include "solosnake/blue/blue_hardware.hpp"
#include "solosnake/blue/blue_logging.hpp"
#include "solosnake/blue/blue_propulsion.hpp"
#include "solosnake/blue/blue_machine_grid_view.hpp"
#include "solosnake/blue/blue_movement_grid_view.hpp"

static_assert( BLUE_MAX_MACHINE_MOVE_SPEED < BLUE_INT32_TILE_WIDTH,
               "Machine might move across > 1 tile in a tick." );

namespace blue
{
    namespace
    {
        // Makes a Machine's turning system and checks constraints.
        Turning_system make_machine_turning_system( Fixed_angle turningPower, Fixed_angle maxTurnSpeed )
        {
            assert( maxTurnSpeed <= BLUE_MAX_MACHINE_TURN_SPEED && maxTurnSpeed
                    >= BLUE_MIN_MACHINE_TURN_SPEED );

            if( maxTurnSpeed > BLUE_MAX_MACHINE_TURN_SPEED || maxTurnSpeed < BLUE_MIN_MACHINE_TURN_SPEED )
            {
                ss_wrn( "Propulsion max turn speed is outside range and will be clamped." );
                maxTurnSpeed = maxTurnSpeed > BLUE_MAX_MACHINE_TURN_SPEED ? BLUE_MAX_MACHINE_TURN_SPEED
                               : maxTurnSpeed;
                maxTurnSpeed = maxTurnSpeed < BLUE_MIN_MACHINE_TURN_SPEED ? BLUE_MIN_MACHINE_TURN_SPEED
                               : maxTurnSpeed;
            }

            turningPower = turningPower <= 0 ? BLUE_MIN_MACHINE_TURN_SPEED : turningPower;
            turningPower = turningPower > maxTurnSpeed ? maxTurnSpeed : turningPower;

            return Turning_system( turningPower, maxTurnSpeed );
        }
    }

    Propulsion::advancing_variables::advancing_variables()
        : numTilesWanted_( 0u )
        , numTilesAdvanced_( 0u )
        , currentddXYdtdt_( 0 )
        , nextTile_()
        , wantStopAtCentre_( false )
    {
    }

    Propulsion::Propulsion()
      : is_advancing_( false )
      , turning_system_()
      , ddXYdtdt_( 0 )
      , max_speed_( 0 )
    {
        gridXY_.x = 0;
        gridXY_.y = 0;
        distance_ = 0;
        dXYdt_    = 0;
    }

    /// @param turnPower The rate of change of the rotation speed, in 1/100 of a degree.
    /// @param thrustPower The rate of change of the movement speed, in 1/10000 of a tile width.
    Propulsion::Propulsion( const Hardware& hw )
        : is_advancing_( false )
        , turning_system_( make_machine_turning_system( hw.turn_power(), hw.max_possible_turn_speed() ) )
        , ddXYdtdt_( hw.thrust_power() )
        , max_speed_( hw.max_possible_speed() )
    {
        assert( max_speed_ <= BLUE_MAX_MACHINE_MOVE_SPEED && max_speed_ >= BLUE_MIN_MACHINE_MOVE_SPEED );

        if( max_speed_ > BLUE_MAX_MACHINE_MOVE_SPEED || max_speed_ < BLUE_MIN_MACHINE_MOVE_SPEED )
        {
            ss_wrn( "Propulsion max move speed is outside range and will be clamped." );
            max_speed_ = max_speed_ > BLUE_MAX_MACHINE_MOVE_SPEED ? BLUE_MAX_MACHINE_MOVE_SPEED : max_speed_;
            max_speed_ = max_speed_ < BLUE_MIN_MACHINE_MOVE_SPEED ? BLUE_MIN_MACHINE_MOVE_SPEED : max_speed_;
        }

        ddXYdtdt_ = ddXYdtdt_ <= 0 ? BLUE_MIN_MACHINE_MOVE_SPEED : ddXYdtdt_;
        ddXYdtdt_ = ddXYdtdt_ > max_speed_ ? max_speed_ : ddXYdtdt_;

        gridXY_.x = 0;
        gridXY_.y = 0;
        distance_ = 0;
        dXYdt_    = 0;
    }

    void Propulsion::reset( Hex_coord xy, HexFacingName facing )
    {
        assert( !is_moving() );
        assert( dXYdt_ == 0 );
        assert( !is_turning() );

        is_advancing_ = false;
        turning_system_.reset( facing );
        gridXY_ = xy;
        distance_ = 0;
        dXYdt_ = 0;
    }

    void Propulsion::decelerate_to_centre()
    {
        advancing_.wantStopAtCentre_ = true;

        assert( ( dXYdt_ * dXYdt_ / ( 2 * distance_ ) ) <= std::numeric_limits<int16_t>::max() );

        // Calculate deceleration needed:
        advancing_.currentddXYdtdt_ = static_cast<int16_t>( dXYdt_ * dXYdt_ / ( 2 * distance_ ) );
    }

    void Propulsion::tick_advancing( Movement_grid_view& grid )
    {
        assert( !is_turning() );
        assert( dXYdt_ != 0 );
        assert( dXYdt_ > 0 );
        assert( dXYdt_ <= max_speed_ );
        assert( max_speed_ >= BLUE_MIN_MACHINE_MOVE_SPEED );

        auto oldDistance = distance_;
        distance_ += dXYdt_;

        dXYdt_ += advancing_.currentddXYdtdt_;
        dXYdt_ = dXYdt_ < BLUE_MIN_MACHINE_MOVE_SPEED ? BLUE_MIN_MACHINE_MOVE_SPEED : dXYdt_;
        dXYdt_ = dXYdt_ > max_speed_ ? max_speed_ : dXYdt_;

        if( oldDistance < 0 && distance_ > 0 )
        {
            // We are at the centre.
            if( advancing_.wantStopAtCentre_ )
            {
                // This is where the STOP happens:
                distance_ = 0;
                dXYdt_ = 0;
                is_advancing_ = false;
                assert( grid.is_on_board( gridXY_ ) );
            }
        }
        else if( distance_ > BLUE_INT32_TILE_WIDTH )
        {
            // New tile entered:
            // The Machine changes its tile-of-reference
            // and is now moving towards the centre of this new tile, until
            // it passes through its centre.
            assert( grid.is_on_board( gridXY_ ) );
            assert( grid.is_on_board( advancing_.nextTile_ ) );

            if( !grid.is_tile_reserved( advancing_.nextTile_ ) )
            {
                assert( grid.is_tile_reserved( advancing_.nextTile_ ) );
            }

            // Move bot on board. Note that this call is re-entrant : it
            // will use gridXY_ so we need to setup this before calling.
            Hex_coord oldXY = gridXY_;
            gridXY_ = advancing_.nextTile_;

            grid.move_bot_from_tile_to_tile( oldXY, gridXY_ );

            distance_ = distance_ - ( BLUE_INT32_TILE_WIDTH * 2 );

            if( ++advancing_.numTilesAdvanced_ == advancing_.numTilesWanted_ )
            {
                decelerate_to_centre();
            }
            else
            {
                // Move across this tile to next one if possible:
                if( grid.try_get_neighbour_tile( gridXY_, facing(), advancing_.nextTile_ ) )
                {
                    if( grid.is_tile_empty_and_unreserved( advancing_.nextTile_ ) )
                    {
                        grid.reserve_empty_tile( advancing_.nextTile_ );
                    }
                    else
                    {
                        decelerate_to_centre();
                    }
                }
                else
                {
                    decelerate_to_centre();
                }
            }
        }
    }

    void Propulsion::tick_turning()
    {
        assert( !is_advancing_ );

        // Only called when we are turning.
        assert( is_turning() );

        turning_system_.advance_one_frame();
    }

    void Propulsion::tick( Movement_grid_view& grid )
    {
        assert( dXYdt_ <= max_speed_ );

        if( is_turning() )
        {
            tick_turning();
        }
        else if( is_advancing_ )
        {
            tick_advancing( grid );
        }
    }

    void Propulsion::start_n_tile_move( Machine_grid_view& grid, std::uint16_t n )
    {
        assert( is_not_turning() );
        assert( max_speed() > 0 );
        assert( blue::is_facing_angle( turning_system_.angle() ) );
        assert( distance_ == 0 );

        advancing_.numTilesWanted_ = n;
        advancing_.numTilesAdvanced_ = 0;

        // try_get_neighbour_tile fills in advancing_.nextTile_.
        if( n > 0 && grid.try_get_neighbour_tile( gridXY_, turning_system_.get_facing(), advancing_.nextTile_ )
                && grid.is_tile_empty_and_unreserved( advancing_.nextTile_ ) )
        {
            grid.reserve_empty_tile( advancing_.nextTile_ );
            assert( !grid.is_tile_empty_and_unreserved( advancing_.nextTile_ ) );
            assert( ddXYdtdt_ <= std::numeric_limits<int16_t>::max() );

            advancing_.currentddXYdtdt_ = static_cast<int16_t>( ddXYdtdt_ );
            advancing_.wantStopAtCentre_ = false;

            is_advancing_ = true;
            dXYdt_ = advancing_.currentddXYdtdt_;
        }
    }

    Position_info Propulsion::get_position_info() const noexcept
    {
        Position_info p;
        p.distance = distance_;
        p.dXYdt = dXYdt_;

        if (turning_system_.is_turning())
        {
            p.turning = turning_system_.is_turning_cw() ? BLUE_POSITION_INFO_CW : BLUE_POSITION_INFO_CCW;
        }
        else
        {
            p.turning = BLUE_POSITION_INFO_NOT_TURNING;
        }

        p.angle = turning_system_.angle();
        p.gridXY = gridXY_;

        return p;
    }
}