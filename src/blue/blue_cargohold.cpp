#include <cassert>
#include "solosnake/blue/blue_cargohold.hpp"
#include "solosnake/blue/blue_tradabletypes.hpp"

namespace blue
{
    void Cargohold::create_cargo( const Amount& a )
    {
        // Check we have the space.
        assert( a.volume() < total_volume() );

        // Check we are not destroying anything.
        assert( contents_.volume() == 0 );

        contents_ = Tradables( a );
    }

    bool Cargohold::try_add_cargo( const Amount& a )
    {
        if( has_capacity_for( a ) )
        {
            contents_.credit( a );
            return true;
        }

        return false;
    }

    void Cargohold::destroy_cargo( const Amount& a )
    {
        assert( contents_.can_afford( a ) );
        contents_.debit( a );
    }

    void Cargohold::transfer_cargo_to( const Amount& a, Cargohold& other )
    {
        assert( contents_.can_afford( a ) );
        assert( other.free_volume() >= a.volume() );
        contents_.debit( a );
        other.contents_.credit( a );
    }
}
