#include <algorithm>
#include <climits>
#include "solosnake/blue/blue_tradables.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{

    Amount::Amount( const std::array<std::int16_t, BLUE_TRADABLE_TYPES_COUNT>& v )
        : amounts_(v)
    {
        for( auto a : v )
        {
            if( a < 0 )
            {
                ss_throw( "An amount cannot be negative." );
            }
        }
    }

    Amount::Amount( TradableTypes t, const int16_t units )
        : amounts_ { 0, 0, 0, 0, 0, 0, 0, 0 }
    {
        static_assert( BLUE_TRADABLE_TYPES_COUNT == 8, "Tradables expected to be 8" );

        if( units < 0 )
        {
            ss_throw( "An amount of Tradables cannot be negative." );
        }

        amounts_[t] = units;
    }

    Amount::Amount( OreTypes t, const int16_t units )
        : amounts_ { 0, 0, 0, 0, 0, 0, 0, 0 }
    {
        static_assert( BLUE_TRADABLE_TYPES_COUNT == 8, "Tradables expected to be 8" );

        if( units < 0 )
        {
            ss_throw( "An amount of OreTypes cannot be negative." );
        }

        amounts_[t] = units;
    }

    Amount Amount::operator+( const Amount& rhs ) const
    {
        Amount result;

        for( size_t i = 0; i < BLUE_TRADABLE_TYPES_COUNT; ++i )
        {
            const int sum = amounts_[i] + rhs.amounts_[i];
            assert( sum <= SHRT_MAX );

            // Clamp to max.
            result.amounts_[i] = static_cast<int16_t>( std::min<int>( SHRT_MAX, sum ) );
        }

        return result;
    }

    Amount& Amount::operator+=( const Amount& rhs )
    {
        for( size_t i = 0; i < BLUE_TRADABLE_TYPES_COUNT; ++i )
        {
            const int sum = amounts_[i] + rhs.amounts_[i];
            assert( sum <= SHRT_MAX );

            // Clamp to max.
            amounts_[i] = static_cast<int16_t>( std::min<int>( SHRT_MAX, sum ) );
        }

        return *this;
    }

    //-------------------------------------------------------------------------

    Tradables::Tradables( const Amount& a )
    {
        for( size_t i = 0; i < BLUE_TRADABLE_TYPES_COUNT; ++i )
        {
            amounts_[i] = a[i];
        }
    }

    /// Never returns more than 0xFFFF.
    uint16_t Tradables::volume_of( const std::array<std::int16_t, BLUE_TRADABLE_TYPES_COUNT>& v )
    {
        unsigned int total_volume = 0;

        uint16_t typeVolumes[BLUE_TRADABLE_TYPES_COUNT];

        typeVolumes[ Panguite   ] = VolumePanguite;
        typeVolumes[ Kamacite   ] = VolumeKamacite;
        typeVolumes[ Ataxite    ] = VolumeAtaxite;
        typeVolumes[ Chondrite  ] = VolumeChondrite;
        typeVolumes[ Metals     ] = VolumeMetals;
        typeVolumes[ NonMetals  ] = VolumeNonMetals;
        typeVolumes[ SemiMetals ] = VolumeSemiMetals;
        typeVolumes[ Alkalis    ] = VolumeAlkalis;

        for( size_t i = 0u; i < BLUE_TRADABLE_TYPES_COUNT; ++i )
        {
            total_volume += static_cast<unsigned int>(v[i]) * typeVolumes[i];
        }

        assert( total_volume <= USHRT_MAX );

        return static_cast<uint16_t>( std::min<unsigned int>( total_volume, USHRT_MAX ) );
    }

    /// Caps the contents at 0 and never wraps.
    void Tradables::debit( const Amount& a )
    {
        assert( can_afford( a ) );

        for( size_t i = 0u; i < BLUE_TRADABLE_TYPES_COUNT; ++i )
        {
            // Although we are asserting it can never happen, the
            // code is written to be always safe and never wrap.
            assert( amounts_[i] >= a[i] );
            amounts_[i] = amounts_[i] > a[i] ? amounts_[i] - a[i] : 0;
        }
    }

    /// Caps the contents at max unsigned int and never wraps.
    void Tradables::credit( const Amount& a )
    {
        for( size_t i = 0u; i < BLUE_TRADABLE_TYPES_COUNT; ++i )
        {
            const int sum = amounts_[i] + a[i];
            amounts_[i] = ( sum > SHRT_MAX ) ? SHRT_MAX : static_cast<int16_t>( sum );
        }
    }

    bool Tradables::can_afford( const Amount& cost ) const
    {
        bool affordable = true;

        for( size_t i = 0; i < BLUE_TRADABLE_TYPES_COUNT; ++i )
        {
            if( cost[i] > amounts_[i] )
            {
                affordable = false;
            }
        }

        return affordable;
    }
}
