#include <cassert>
#include <string>
#include "solosnake/blue/blue_hardpoint_shot.hpp"
#include "solosnake/blue/blue_hardpoint_shooter.hpp"
#include "solosnake/blue/blue_throw.hpp"
#include "solosnake/blue/blue_unreachable.hpp"

namespace blue
{
    constexpr std::string_view SS_HARDPOINT_WEAPONTYPE_NAME     = "gun";
    constexpr std::string_view SS_HARDPOINT_CAPDRAINTYPE_NAME   = "drain";
    constexpr std::string_view SS_HARDPOINT_CAPXFERTYPE_NAME    = "cap";
    constexpr std::string_view SS_HARDPOINT_ARMOURREPTYPE_NAME  = "rep";
    constexpr std::string_view SS_HARDPOINT_MINERTYPE_NAME      = "miner";

    namespace
    {
        // Generated using "solosnake::normaldistribution(128,0.5f,0.5f/3.0f);"
        const float normalDistributionTable[]
            = { 0.05145f, 0.09663f, 0.11174f, 0.12142f, 0.13605f, 0.17105f, 0.17258f, 0.21530f,
                0.23635f, 0.23639f, 0.23904f, 0.24757f, 0.24980f, 0.24990f, 0.25790f, 0.26612f,
                0.26901f, 0.27228f, 0.27448f, 0.27559f, 0.28557f, 0.30922f, 0.30927f, 0.31713f,
                0.32236f, 0.33123f, 0.34497f, 0.34826f, 0.35015f, 0.35721f, 0.36043f, 0.36845f,
                0.37570f, 0.37651f, 0.37730f, 0.38103f, 0.38347f, 0.38358f, 0.38484f, 0.38530f,
                0.38532f, 0.38780f, 0.39186f, 0.39258f, 0.40487f, 0.40520f, 0.40527f, 0.40847f,
                0.40848f, 0.41209f, 0.42415f, 0.42703f, 0.42871f, 0.43386f, 0.43608f, 0.44135f,
                0.45615f, 0.45931f, 0.46713f, 0.46910f, 0.47121f, 0.47193f, 0.47625f, 0.47652f,
                0.48158f, 0.48515f, 0.48932f, 0.49506f, 0.49913f, 0.50990f, 0.51050f, 0.51157f,
                0.51221f, 0.51680f, 0.51992f, 0.52243f, 0.53382f, 0.53642f, 0.54355f, 0.55121f,
                0.55626f, 0.56045f, 0.56506f, 0.56605f, 0.56606f, 0.57277f, 0.57465f, 0.57908f,
                0.58343f, 0.58985f, 0.59067f, 0.61786f, 0.61924f, 0.62512f, 0.62933f, 0.63372f,
                0.65184f, 0.65558f, 0.65710f, 0.65928f, 0.66245f, 0.66927f, 0.67744f, 0.68314f,
                0.68956f, 0.69014f, 0.69789f, 0.71099f, 0.71988f, 0.73045f, 0.74423f, 0.74634f,
                0.76216f, 0.76461f, 0.77973f, 0.78143f, 0.78193f, 0.78853f, 0.82277f, 0.82796f,
                0.84038f, 0.84198f, 0.84552f, 0.84775f, 0.85794f, 0.88356f, 0.89753f, 1.00000f
              };

        std::uint8_t hardpoint_type_from_string( const std::string& t )
        {
            if( t == SS_HARDPOINT_WEAPONTYPE_NAME )
            {
                return WeaponHardpoint;
            }

            if( t == SS_HARDPOINT_CAPDRAINTYPE_NAME )
            {
                return CapDrainHardpoint;
            }

            if( t == SS_HARDPOINT_CAPXFERTYPE_NAME )
            {
                return CapXFerHardpoint;
            }

            if( t == SS_HARDPOINT_ARMOURREPTYPE_NAME )
            {
                return ArmourRepairerHardpoint;
            }

            if( t == SS_HARDPOINT_MINERTYPE_NAME )
            {
                return MinerHardpoint;
            }

            ss_err( "Unknown Hardpoint effect name ", t );
            ss_throw( "Unknown Hardpoint type name" );
        }

        std::string_view hardpoint_type_to_string( std::uint8_t t )
        {
            switch( t )
            {
                case WeaponHardpoint:
                    return SS_HARDPOINT_WEAPONTYPE_NAME;
                case CapDrainHardpoint:
                    return SS_HARDPOINT_CAPDRAINTYPE_NAME;
                case CapXFerHardpoint:
                    return SS_HARDPOINT_CAPXFERTYPE_NAME;
                case ArmourRepairerHardpoint:
                    return SS_HARDPOINT_ARMOURREPTYPE_NAME;
                case MinerHardpoint:
                    return SS_HARDPOINT_MINERTYPE_NAME;
            }

            ss_err( "Unknown Hardpoint effect ", static_cast<unsigned int>( t ) );
            ss_throw( "Unknown Hardpoint type" );
        }

        /// High reliability means less influence by rand. reliability is
        /// a percentage.
        float effectiveness_factor( Randoms& r, const float reliability )
        {
            assert( reliability >= 0.0f && reliability <= 100.0f );
            const size_t index = static_cast<size_t>( r.urand() % ( sizeof( normalDistributionTable ) / sizeof( float ) ) );
            return 0.01f * ( reliability + ( ( 100.0f - reliability ) * normalDistributionTable[index] ) );
        }
    }

    Hardpoint_shooter::Hardpoint_shooter(
            const Hardpoint_shot& hardpoint_shot,
            const Fixed_angle     rotation_speed,  // Per frame
            const std::uint16_t   firing_cap_cost,
            const std::uint16_t   firing_cooldown,  // In frames.
            const std::int16_t    min_angle_degrees,
            const std::int16_t    max_angle_degrees,
            const std::uint16_t   min_range,
            const std::uint16_t   max_range_percent_effect,  // 0-100+ %, factor applied to damage at max range.
            const std::uint8_t    reliability // 0-100%
        )
        : hardpoint_shot_(hardpoint_shot)
        , rotation_speed_(rotation_speed)
        , firing_cap_cost_(firing_cap_cost)
        , firing_cooldown_(firing_cooldown)
        , min_angle_degrees_(min_angle_degrees)
        , max_angle_degrees_(max_angle_degrees)
        , min_range_(min_range)
        , max_range_percent_effect_(max_range_percent_effect)
        , reliability_(reliability)
    {
        validate();
    }

    /// Returns the max potential shot effect that should be applied if this
    /// shot hits its target. No range compensation is applied as at this stage
    /// the final range to whatever target is struck is unknown - it is not
    /// resolved until all machines turns ends.
    /// Note that this can also be effects like cap drained or ore mined.
    Hardpoint_shot Hardpoint_shooter::make_a_shot( Randoms& r ) const noexcept
    {
        const float e = effectiveness_factor( r, reliability_ );

        Hardpoint_shot shot = hardpoint_shot_;

        if( shot.hardpoint_type_ == WeaponHardpoint )
        {
            shot.shot_effect_.weapon.damage *= e;
        }
        else
        {
            // repairs, cap_xfer and mining are all the same union layout.
            shot.shot_effect_.repairs.armour_repaired = static_cast<std::uint16_t>( e * shot.shot_effect_.repairs.armour_repaired );
            assert( shot.shot_effect_.repairs.armour_repaired == shot.shot_effect_.cap_drain.cap_drained );
            assert( shot.shot_effect_.repairs.armour_repaired == shot.shot_effect_.cap_xfer.cap_transferred );
            assert( shot.shot_effect_.repairs.armour_repaired == shot.shot_effect_.mining.volume_mined );
        }

        return shot;
    }

    /// Weapons do no damage beyond their max ranges. Min range or less returns
    /// 1.0f, beyond max range and beyond returns 0.0f Ranges
    /// are inclusive. A weapon's damage needs to take into account its
    /// falloff setting. A weapons damage is its damage at min range.
    /// Weapons that do more damage at max range will have a  falloff which
    /// is > 100%, meaning their minimum range damage is amplified at max range.
    /// Weapons commonly do less damage at max range: these will have a falloff
    /// less than 100%, and the damage will range between 100% at min range
    /// to falloff% at max range.
    float Hardpoint_shooter::calculate_falloff( const std::uint16_t target_range,
                                                const std::uint16_t min_range,
                                                const std::uint16_t max_range,
                                                const std::uint16_t max_range_effect ) noexcept
    {
        assert( max_range >= min_range );

        if( target_range <= min_range )
        {
            // Catches the target_range == 0 case.
            return 1.0f;
        }

        if( target_range > max_range )
        {
            return 0.0f;
        }

        assert( target_range > 0U );
        assert( ( max_range - min_range ) > 0 );

        const float range_factor = static_cast<float>(target_range - min_range) / ( max_range - min_range );
        const float delta = (0.01f * max_range_effect) - 1.0f;
        auto result = (range_factor * delta) + 1.0f;

        return result;
    }

    void Hardpoint_shooter::validate() const
    {
        if(min_angle_degrees_ < 0 || min_angle_degrees_ > 360)
        {
            ss_throw("Hardpoint_shooter min-angle-degrees out of bounds.");
        }

        if(max_angle_degrees_ < -360 || max_angle_degrees_ > 360)
        {
            ss_throw("Hardpoint_shooter max-angle-degrees out of [-360,+360]]bounds.");
        }

        if(hardpoint_shot_.max_range_ < min_range_)
        {
            ss_throw("Hardpoint_shooter bad min/max weapon range.");
        }

        if(rotation_speed_ <= 0u)
        {
            ss_throw("Hardpoint_shooter has illegal rotation speed.");
        }

        if(reliability_ > 100u)
        {
            ss_throw("Hardpoint_shooter reliability is beyond 100%.");
        }

        // Check enum really is in range.
        switch (hardpoint_shot_.hardpoint_type_)
        {
        case WeaponHardpoint:
        case CapDrainHardpoint:
        case CapXFerHardpoint:
        case ArmourRepairerHardpoint:
        case MinerHardpoint:
            break;

        default:
            ss_throw("Hardpoint_shooter bad hardpoint_type enum value.");
            break;
        }

        // Check index is not some mad number: 87 is max possible location
        // index. A Machine could not have 87+ hardpoints.
        if(hardpoint_shot_.hardpoint_index_ > 87)
        {
            ss_throw("Hardpoint_shooter bad Hardpoint index.");
        }
    }

    void Hardpoint_shooter::set_index( const size_t index )
    {
        assert( index <= 0xFF );
        hardpoint_shot_.hardpoint_index_ = static_cast<std::uint8_t>( index );
    }

    Fixed_angle Hardpoint_shooter::get_mid_angle() const noexcept
    {
        const Fixed_angle range = ( max_angle() + BLUE_INT32_TWO_PI - min_angle() ) % BLUE_INT32_TWO_PI;
        const Fixed_angle half = range / 2;
        const Fixed_angle mid = ( min_angle() + half ) % BLUE_INT32_TWO_PI;
        return mid;
    }

    bool Hardpoint_shooter::operator == ( const Hardpoint_shooter& rhs) const noexcept
    {
        return (hardpoint_shot_             == rhs.hardpoint_shot_)
            && (rotation_speed_             == rhs.rotation_speed_)
            && (firing_cap_cost_            == rhs.firing_cap_cost_)
            && (firing_cooldown_            == rhs.firing_cooldown_)
            && (min_range_                  == rhs.min_range_)
            && (min_angle_degrees_          == rhs.min_angle_degrees_)
            && (max_angle_degrees_          == rhs.max_angle_degrees_)
            && (max_range_percent_effect_   == rhs.max_range_percent_effect_)
            && (reliability_                == rhs.reliability_)
            ;
    }

    bool Hardpoint_shooter::operator != ( const Hardpoint_shooter& rhs) const noexcept
    {
        return !((*this) == rhs);
    }
}
