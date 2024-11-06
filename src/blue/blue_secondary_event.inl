#include "solosnake/blue/blue_secondary_event.hpp"

namespace blue
{
    inline Secondary_event Secondary_event::make_shotfiredandhitmachine( const Shot_fired& s )
    {
        Secondary_event e( SEWeaponFiredAndHitMachine );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_shotfiredandhitasteroid( const Shot_fired& s )
    {
        Secondary_event e( SEWeaponFiredAndHitAsteroid );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_shotfiredandmissed( const Shot_fired& s )
    {
        Secondary_event e( SEWeaponFiredAndMissed );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_capdrainfiredandhitmachine( const Shot_fired& s )
    {
        Secondary_event e( SECapDrainFiredAndHitMachine );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_capdrainfiredandhitasteroid( const Shot_fired& s )
    {
        Secondary_event e( SECapDrainFiredAndHitAsteroid );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_capdrainfiredandmissed( const Shot_fired& s )
    {
        Secondary_event e( SECapDrainFiredAndMissed );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_capxferfiredandhitmachine( const Shot_fired& s )
    {
        Secondary_event e( SECapXferFiredAndHitMachine );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_capxferfiredandhitasteroid( const Shot_fired& s )
    {
        Secondary_event e( SECapXferFiredAndHitAsteroid );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_capxferfiredandmissed( const Shot_fired& s )
    {
        Secondary_event e( SECapXferFiredAndMissed );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_armourrepfiredandhitmachine( const Shot_fired& s )
    {
        Secondary_event e( SEArmourRepFiredAndHitMachine );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_armourrepfiredandhitasteroid( const Shot_fired& s )
    {
        Secondary_event e( SEArmourRepFiredAndHitAsteroid );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_armourrepfiredandmissed( const Shot_fired& s )
    {
        Secondary_event e( SEArmourRepFiredAndMissed );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_miningfiredandhitmachine( const Shot_fired& s )
    {
        Secondary_event e( SEMiningFiredAndHitMachine );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_miningfiredandhitasteroid( const Shot_fired& s )
    {
        Secondary_event e( SEMiningFiredAndHitAsteroid );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_miningfiredandmissed( const Shot_fired& s )
    {
        Secondary_event e( SEMiningFiredAndMissed );
        e.datum_.shotfired_ = s;
        return e;
    }

    inline Secondary_event Secondary_event::make_asteroid_depleted()
    {
        Secondary_event e( SEAsteroidDepleted );

        return e;
    }

    inline Secondary_event Secondary_event::make_cash_transferred( Hex_coord /*from*/, Hex_coord /*to*/, std::uint16_t /*amount*/ )
    {
        Secondary_event e( SECashTransferred );

        return e;
    }

    inline Shot_fired Shot_fired::make_shot_that_missed( const Fired& f )
    {
        Shot_fired s;
        s.shot_fired_ = f;
        s.hit_xy_     = Hex_coord::make_invalid_coord();
        return s;
    }

    inline Shot_fired Shot_fired::make_shot_that_hit( const Fired& f, Hex_coord xy )
    {
        assert( ! xy.is_invalid() );

        Shot_fired s;
        s.shot_fired_ = f;
        s.hit_xy_     = xy;
        return s;
    }
}
