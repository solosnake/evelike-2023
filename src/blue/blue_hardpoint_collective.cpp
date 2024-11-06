#include <algorithm>
#include "solosnake/blue/blue_blueprint.hpp"
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_hardpoint_collective.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_position_info.hpp"
#include "solosnake/blue/blue_hardpoint_angles.hpp"

namespace blue
{
    namespace
    {
        /// Returns true if @a hp is pointing at target in range and weapon
        /// has available cap and is in a firable state.
        bool ready_to_fire( const Turning_hardpoint& hp,
                            const Fixed_angle a,
                            const std::uint16_t cap,
                            const std::uint16_t dist )
        {
            return hp.cooldown_state_ == 0                   // Ready to fire.
                && hp.current_angle_ == a                    // Pointing at target.
                && hp.firing_cap_cost() <= cap               // Cap available.
                && hp.turning_hardpoint_min_range() <= dist  // Within min range.
                && hp.turning_hardpoint_max_range() >= dist; // Within max range.
        }
    }

    Hardpoint_collective::Hardpoint_collective()
        : shots_fired_()
        , hardpoints_()
        , target_angle_( 0 )
        , machine_facing_angle_( 0 )
        , machine_location_( Hex_coord::from_uint16( 0xFFFF ) )
        , target_location_( Hex_coord::from_uint16( 0xFFFF ) )
        , min_range_( 0xFFFF )
        , max_range_( 0 )
        , aiming_( false )
        , cooling_( false )
    {
    }

    Hardpoint_collective::Hardpoint_collective( const Hardware&, const Blueprint& bpo )
        : shots_fired_()
        , hardpoints_()
        , target_angle_( 0 )
        , machine_facing_angle_( 0 )
        , machine_location_( Hex_coord::from_uint16( 0xFFFF ) )
        , target_location_( Hex_coord::from_uint16( 0xFFFF ) )
        , min_range_( 0xFFFF )
        , max_range_( 0 )
        , aiming_( false )
        , cooling_( false )
    {
        const size_t n_hardpoints = bpo.hardpoint_count();

        if( n_hardpoints > 0 )
        {
            shots_fired_.reserve( n_hardpoints );
            hardpoints_.reserve( n_hardpoints );
            hardpoint_indices_by_cap_cost_.reserve( n_hardpoints );

            for( size_t i = 0; i < n_hardpoints; ++i )
            {
                min_range_ = std::min( bpo.hardpoint_at( i ).shooter().min_range(), min_range_ );
                max_range_ = std::max( bpo.hardpoint_at( i ).shooter().max_range(), max_range_ );
                Turning_hardpoint thp( hardpoints_.size(), bpo.hardpoint_angle( i ), bpo.hardpoint_at( i ) );
                hardpoints_.push_back( thp );
                hardpoint_indices_by_cap_cost_.push_back( static_cast<uint8_t>( i ) );
            }

            // Sort by cap cost, lowest first (most firable).
            sort( hardpoint_indices_by_cap_cost_.begin(),
                  hardpoint_indices_by_cap_cost_.end(),
                  [&]( uint8_t lhs, uint8_t rhs )
            {
                return hardpoints_[lhs].shooter_.firing_cap_cost() < hardpoints_[rhs].firing_cap_cost();
            } );
        }
    }

    void Hardpoint_collective::tick_cooldown()
    {
        unsigned int coolingDown = 0;

        const size_t n = hardpoints_.size();
        for( size_t i = 0; i < n; ++i )
        {
            // Cooldown
            if( hardpoints_[i].cooldown_state_ > 0 )
            {
                --hardpoints_[i].cooldown_state_;
                coolingDown += ( hardpoints_[i].cooldown_state_ > 0 ) ? 1 : 0;
            }
        }

        cooling_ = coolingDown > 0;
    }

    void Hardpoint_collective::tick_aim()
    {
        // Count how many hardpoints are trying to aim to target but
        // not yet facing the target.
        unsigned int notAngledTowardsTarget = 0;

        const size_t n = hardpoints_.size();
        for( size_t i = 0; i < n; ++i )
        {
            // Rotation towards target:
            if( target_angle_ != hardpoints_[i].current_angle_
                    && in_angle_range( target_angle_, hardpoints_[i].min_angle_, hardpoints_[i].max_angle_ ) )
            {
                const Fixed_angle turn_speed    = hardpoints_[i].turninghardpoint_rotation_speed();
                const Fixed_angle current_angle = hardpoints_[i].current_angle_;
                const Fixed_angle angle_diff    = abs( target_angle_ - current_angle );

                if( angle_diff <= turn_speed )
                {
                    assert( target_angle_ >= 0 && target_angle_ < BLUE_INT32_TWO_PI );
                    // We are close enough to jump directly to aiming.
                    hardpoints_[i].current_angle_ = target_angle_;
                }
                else
                {
                    const bool targetAngleGT = target_angle_ > current_angle;
                    const bool goOpposite = angle_diff > BLUE_INT32_PI;

                    Fixed_angle delta = targetAngleGT ? turn_speed : -turn_speed;

                    // Turn the opposite way if its less than 180 degrees in
                    // that direction.
                    delta = goOpposite ? -delta : delta;

                    hardpoints_[i].current_angle_ = ( current_angle + BLUE_INT32_TWO_PI + delta )
                                                    % BLUE_INT32_TWO_PI;

                    ++notAngledTowardsTarget;
                }
            }
        }

        // Stop aiming when all hardpoints that can face the target are
        // facing the target.
        aiming_ = notAngledTowardsTarget > 0;
    }

    bool Hardpoint_collective::can_aim_at( Hex_coord xy )
    {
        const uint16_t d = Hex_grid::get_step_distance_between( machine_location_, xy );

        if( d > 0 && d >= min_range_ && d <= max_range_ )
        {
            //if( xy == machine_location_ )
            //{
            //    // Machine is asking if it can aim at itself? Aiming at self command is required
            //    // to fail so we must also fail to reply true to an attempt to query if we can
            //    // aim at ourselves.
            //    return false;
            //}
            //else
            {
                const Fixed_angle a = relative_angle_to_target( machine_facing_angle_, machine_location_, xy );

                const size_t n = hardpoints_.size();

                for( size_t i = 0; i < n; ++i )
                {
                    const bool inAngle = in_angle_range( a, hardpoints_[i].min_angle_, hardpoints_[i].max_angle_ );
                    const bool inRange = (d >= hardpoints_[i].turning_hardpoint_min_range())
                                      && (d <= hardpoints_[i].turning_hardpoint_max_range());

                    if( inAngle && inRange )
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    void Hardpoint_collective::aim_at( Hex_coord xy )
    {
        const uint16_t d = Hex_grid::get_step_distance_between( machine_location_, xy );

        aiming_ = d > 0 && d >= min_range_ && d <= max_range_;

        if( aiming_ )
        {
            target_angle_ = relative_angle_to_target( machine_facing_angle_, machine_location_, xy );
            target_location_ = xy;

            assert( target_angle_ >= 0 && target_angle_ < BLUE_INT32_TWO_PI );
        }
    }

    unsigned int Hardpoint_collective::can_fire_at( const uint16_t cap, const Hex_coord xy ) const
    {
        unsigned int ready_count = 0;

        const uint16_t d = Hex_grid::get_step_distance_between( machine_location_, xy );

        if( d > 0 && d >= min_range_ && d <= max_range_ )
        {
            const Fixed_angle a = relative_angle_to_target( machine_facing_angle_, machine_location_, xy );

            const size_t n = hardpoints_.size();
            for( size_t i = 0; i < n; ++i )
            {
                if( ready_to_fire( hardpoints_[i], a, cap, d ) )
                {
                    ++ready_count;
                }
            }
        }

        return ready_count;
    }

    const std::vector<Hardpoint_shot>&
    Hardpoint_collective::fire_at( Randoms& randoms, uint16_t& cap, Hex_coord xy )
    {
        shots_fired_.clear();

        // Range is used here by Hardpoint firing AI to determine whether to
        // fire or not - however the actual shot that lands may land below the
        // minimum range (if a nearby Machine moves into the firing arc).
        const uint16_t d = Hex_grid::get_step_distance_between( machine_location_, xy );

        if( d > 0 && d >= min_range_ && d <= max_range_ )
        {
            const Fixed_angle a
                = relative_angle_to_target( machine_facing_angle_, machine_location_, xy );

            // Go through the hardpoints in order of increasing cap needs.
            const size_t n = hardpoints_.size();
            for( size_t i = 0; i < n; ++i )
            {
                Turning_hardpoint& hp = hardpoints_[hardpoint_indices_by_cap_cost_[i]];
                if( ready_to_fire( hp, a, cap, d ) )
                {
                    hp.cooldown_state_ = hp.firing_cooldown();
                    cap -= hp.firing_cap_cost();
                    shots_fired_.push_back( hp.make_a_shot( randoms ) );
                }
            }

            cooling_ = !shots_fired_.empty();
        }

        return shots_fired_;
    }
}
