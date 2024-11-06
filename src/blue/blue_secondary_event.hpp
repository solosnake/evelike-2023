#ifndef blue_secondaryevent_hpp
#define blue_secondaryevent_hpp

#include <cassert>
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_machine_events.hpp"

namespace blue
{
    enum SecondaryEventId
    {
        SEWeaponFiredAndHitMachine,
        SEWeaponFiredAndHitAsteroid,
        SEWeaponFiredAndMissed,

        SECapDrainFiredAndHitMachine,
        SECapDrainFiredAndHitAsteroid,
        SECapDrainFiredAndMissed,

        SECapXferFiredAndHitMachine,
        SECapXferFiredAndHitAsteroid,
        SECapXferFiredAndMissed,

        SEArmourRepFiredAndHitMachine,
        SEArmourRepFiredAndHitAsteroid,
        SEArmourRepFiredAndMissed,

        SEMiningFiredAndHitMachine,
        SEMiningFiredAndHitAsteroid,
        SEMiningFiredAndMissed,

        SEAsteroidDepleted,
        SECashTransferred
    };

    struct Shot_fired
    {
        Fired        shot_fired_;
        Hex_coord    hit_xy_;

        static Shot_fired make_shot_that_missed( const Fired& f );
        static Shot_fired make_shot_that_hit( const Fired& f, Hex_coord );

        bool shot_missed() const { return hit_xy_.is_invalid(); }
    };


    /// Secondary events are board events that are not directly generated
    /// by the machines: they are reactions etc such as hitting or missing
    /// shots on other machines etc. Machines can only indirectly create
    /// secondary events.
    class Secondary_event
    {
    private:

        std::uint32_t   event_id_;

    public:

        std::uint32_t   event_id() const  { return event_id_; }

        static Secondary_event   make_shotfiredandhitmachine( const Shot_fired& );
        static Secondary_event   make_shotfiredandhitasteroid( const Shot_fired& );
        static Secondary_event   make_shotfiredandmissed( const Shot_fired& );

        static Secondary_event   make_capdrainfiredandhitmachine( const Shot_fired& );
        static Secondary_event   make_capdrainfiredandhitasteroid( const Shot_fired& );
        static Secondary_event   make_capdrainfiredandmissed( const Shot_fired& );

        static Secondary_event   make_capxferfiredandhitmachine( const Shot_fired& );
        static Secondary_event   make_capxferfiredandhitasteroid( const Shot_fired& );
        static Secondary_event   make_capxferfiredandmissed( const Shot_fired& );

        static Secondary_event   make_armourrepfiredandhitmachine( const Shot_fired& );
        static Secondary_event   make_armourrepfiredandhitasteroid( const Shot_fired& );
        static Secondary_event   make_armourrepfiredandmissed( const Shot_fired& );

        static Secondary_event   make_miningfiredandhitmachine( const Shot_fired& );
        static Secondary_event   make_miningfiredandhitasteroid( const Shot_fired& );
        static Secondary_event   make_miningfiredandmissed( const Shot_fired& );

        static Secondary_event   make_asteroid_depleted();

        static Secondary_event   make_cash_transferred( Hex_coord from, Hex_coord to, std::uint16_t amount );

        /// A union of possible events, only one is ever valid,
        /// matching the EventType.
        union EventDatum
        {
            Shot_fired  shotfired_;
        } datum_;

        bool operator == (const Secondary_event& rhs) const noexcept;

    private:

        Secondary_event( SecondaryEventId e ) : event_id_( e )
        {
        }
    };
}

#include "solosnake/blue/blue_secondary_event.inl"

#endif
