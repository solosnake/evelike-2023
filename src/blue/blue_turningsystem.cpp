#include "solosnake/blue/blue_turning_system.hpp"
#include "solosnake/blue/blue_position_info.hpp"

namespace blue
{
    Turning_system::Turning_system()
        : degrees_wanted_( 0 )
        , degrees_turned_through_( 0 )
        , deceleration_marker_( 0 )
        , facingWanted_( FacingTile0 )
        , angle_( blue::fixedAngle_from_facing( FacingTile0 ) )
        , dAdt_( 0 )
        , ddAdtdt_( 0 )
        , max_turn_speed_( 0 )
    {
    }

    Turning_system::Turning_system( Fixed_angle turningPower, Fixed_angle maxTurnSpeed )
        : degrees_wanted_( 0 )
        , degrees_turned_through_( 0 )
        , deceleration_marker_( 0 )
        , facingWanted_( FacingTile0 )
        , angle_( blue::fixedAngle_from_facing( FacingTile0 ) )
        , dAdt_( 0 )
        , ddAdtdt_( turningPower )
        , max_turn_speed_( maxTurnSpeed )
    {
    }

    void Turning_system::reset( HexFacingName f )
    {
        degrees_wanted_ = 0;
        degrees_turned_through_ = 0;
        deceleration_marker_ = 0;
        facingWanted_ = f;
        angle_ = blue::fixedAngle_from_facing( f );
        dAdt_ = 0;
    }

    HexFacingName Turning_system::facing_after_n_turns( bool turn_ccw, std::uint16_t n ) const
    {
        if( turn_ccw )
        {
            return get_facing() + std::int32_t{ 6 - ( n % 6 ) };
        }
        else
        {
            return get_facing() + std::int32_t{ n % 6 };
        }
    }

    void Turning_system::start_n_face_turns( Fixed_angle startingAngle, bool turn_ccw, std::uint16_t n )
    {
        assert( is_not_turning() );
        assert( max_turn_speed() > 0 );

        angle_ = startingAngle;

        if( n > 0 )
        {
            degrees_wanted_ = n * BLUE_INT32_THIRD_PI;
            degrees_turned_through_ = 0;
            deceleration_marker_ = n * BLUE_INT32_THIRD_PI;

            if( turn_ccw )
            {
                auto turns    = ( 6 - ( n % 6 ) );
                facingWanted_ = blue::facing_from_fixedAngle( startingAngle ) + turns;
                dAdt_ = -ddAdtdt_;
            }
            else
            {
                auto turns    = ( n % 6 );
                facingWanted_ = blue::facing_from_fixedAngle( startingAngle ) + turns;
                dAdt_ = ddAdtdt_;
            }
        }
    }

    void Turning_system::advance_one_frame()
    {
        if( is_turning() )
        {
            assert( dAdt_ != 0 );
            assert( dAdt_ > -BLUE_INT32_TWO_PI && dAdt_ < BLUE_INT32_TWO_PI );
            assert( ( angle_ + dAdt_ + BLUE_INT32_TWO_PI ) >= 0 );
            assert( dAdt_ <= max_turn_speed_ );

            // Count the 'degrees' turned through.
            degrees_turned_through_ += ( dAdt_ > 0 ) ? dAdt_ : -dAdt_;

            angle_ = ( angle_ + dAdt_ + BLUE_INT32_TWO_PI ) % BLUE_INT32_TWO_PI;
            assert( angle_ >= 0 && angle_ < BLUE_INT32_TWO_PI );

            if( degrees_turned_through_ >= degrees_wanted_ )
            {
                // We are here at the desired position.
                angle_ = blue::fixedAngle_from_facing( cast_to_facing_name( facingWanted_ ) );
                dAdt_ = 0;
            }
            else if( ( degrees_turned_through_ >= deceleration_marker_ )
                     || ( ( deceleration_marker_ == degrees_wanted_ )
                          && ( degrees_turned_through_ >= degrees_wanted_ / 2 ) ) )
            {
                if( deceleration_marker_ == degrees_wanted_ )
                {
                    deceleration_marker_ = degrees_wanted_ / 2;
                }
                else
                {
                    // Deceleration Phase
                    if( dAdt_ > 0 )
                    {
                        // Positive turning, positive deceleration.
                        // Minimum deceleration is never less than turning power.
                        dAdt_ -= ddAdtdt_;
                        dAdt_ = dAdt_ <= 0 ? ddAdtdt_ : dAdt_;
                    }
                    else
                    {
                        // Negative turning, negative deceleration.
                        // Minimum deceleration is never less than turning power.
                        dAdt_ += ddAdtdt_;
                        dAdt_ = dAdt_ >= 0 ? -ddAdtdt_ : dAdt_;
                    }
                }
            }
            else
            {
                // Acceleration Phase

                const int dAdtOld = dAdt_;

                if( dAdt_ > 0 )
                {
                    // Positive turning, positive acceleration.
                    dAdt_ += ddAdtdt_;
                    dAdt_ = dAdt_ > max_turn_speed_ ? max_turn_speed_ : dAdt_;

                    if( dAdtOld < max_turn_speed_ && dAdt_ == max_turn_speed_ )
                    {
                        // We just reached max speed. Calculate the deceleration
                        // marker based on this position.
                        deceleration_marker_ = degrees_wanted_ - degrees_turned_through_;
                        assert( deceleration_marker_ > degrees_turned_through_ );
                    }
                }
                else
                {
                    // Negative turning, negative acceleration.
                    dAdt_ -= ddAdtdt_;
                    dAdt_ = dAdt_ < -max_turn_speed_ ? -max_turn_speed_ : dAdt_;

                    if( dAdtOld > -max_turn_speed_ && dAdt_ == -max_turn_speed_ )
                    {
                        // We just reached max speed. Calculate the deceleration
                        // marker based on this position.
                        deceleration_marker_ = degrees_wanted_ - degrees_turned_through_;
                        assert( deceleration_marker_ > degrees_turned_through_ );
                    }
                }
            }
        }
    }
}
