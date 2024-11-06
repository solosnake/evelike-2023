#include <cassert>
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_hardpoint.hpp"
#include "solosnake/blue/blue_turning_hardpoint.hpp"

namespace blue
{
    Turning_hardpoint::Turning_hardpoint( const size_t indexOnMachine,
                                          const Fixed_angle angleOnMachine,
                                          const Hardpoint& hp )
        : shooter_( hp.shooter() )
        , current_angle_( ( hp.get_shooter_mid_angle() + angleOnMachine ) % BLUE_INT32_TWO_PI )
        , min_angle_( ( hp.shooter().min_angle() + angleOnMachine ) % BLUE_INT32_TWO_PI )
        , max_angle_( ( hp.shooter().max_angle() + angleOnMachine ) % BLUE_INT32_TWO_PI )
        , cooldown_state_( 0 )
    {
        shooter_.set_index( indexOnMachine );
    }
}
