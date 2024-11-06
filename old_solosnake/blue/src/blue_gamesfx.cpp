#include <cassert>
#include "solosnake/blue/blue_ifx.hpp"
#include "solosnake/blue/blue_ifxrenderer.hpp"
#include "solosnake/blue/blue_gamesfx.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_machine_events.hpp"
#include "solosnake/blue/blue_fxmachineexplosion.hpp"
#include "solosnake/blue/blue_fxsense.hpp"
#include "solosnake/blue/blue_fxfiring.hpp"
#include "solosnake/blue/blue_hardpointtype.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_secondaryevent.hpp"

#define BLUE_DEFAULT_SFX_RESERVED   (64)

using namespace std;

namespace blue
{
    gamesfx::gamesfx(
        const shared_ptr<solosnake::rendering_system>& r,
        const shared_ptr<solosnake::sound_renderer>& sr,
        const solosnake::persistance& settings )
    {
        active_effects_.reserve( BLUE_DEFAULT_SFX_RESERVED );

        inactive_effects_.resize( BLUE_EFFECT_TYPES_COUNT );
        for( size_t i = 0; i < inactive_effects_.size(); ++i )
        {
            inactive_effects_[i].reserve( BLUE_DEFAULT_SFX_RESERVED );
        }

        effects_store_.reserve( BLUE_EFFECT_TYPES_COUNT * BLUE_DEFAULT_SFX_RESERVED );

        fxrenderers_.resize( BLUE_EFFECT_TYPES_COUNT );
        fxrenderers_.at( SensedEffect )          = fxsense::make_fxrenderer( r, sr, settings );
        fxrenderers_.at( MachineExplodedEffect ) = fxmachineexplosion::make_fxrenderer( r, sr, settings );
        fxrenderers_.at( WeaponFiringEffect )    = fxweaponfiring::make_fxrenderer( r, sr, settings );
        // fxrenderers_.at( CapDrainFiringEffect )  = fxcapdrainfiring::make_fxrenderer( r, sr, settings );
        // fxrenderers_.at( CapXFerFiringEffect )   = fxcapxferfiring::make_fxrenderer( r, sr, settings );
    }

    gamesfx::~gamesfx()
    {
    }

    void gamesfx::update( bool advanceAnimations, const unsigned int dtms )
    {
        if( advanceAnimations )
        {
            for( size_t i = 0; i < active_effects_.size(); ++i )
            {
                ifx* fx = active_effects_[i];

                if( fx->update( advanceAnimations, dtms ) )
                {
                    pending_active_effects_.push_back( fx );
                }
                else
                {
                    inactive_effects_[ fx->effect_type() ].push_back( fx );
                }
            }

            pending_active_effects_.swap( active_effects_ );
            pending_active_effects_.clear();
        }
    }

    void gamesfx::draw_fx()
    {
        for( size_t i = 0; i < active_effects_.size(); ++i )
        {
            active_effects_[i]->draw_sfx();
        }
    }

    void gamesfx::add_sense_fx( float y, Sensed s )
    {
        if( inactive_effects_[ SensedEffect ].empty() )
        {
            effects_store_.emplace_back( make_unique<fxsense>( y, *fxrenderers_[ SensedEffect ].get() ) );
            active_effects_.push_back( effects_store_.back().get() );
        }
        else
        {
            active_effects_.push_back( inactive_effects_[ SensedEffect ].back() );
            inactive_effects_[ SensedEffect ].pop_back();
        }

        reinterpret_cast<fxsense*>( active_effects_.back() )->restart( s );
    }

    void gamesfx::add_machine_exploded_fx( float y, Machine_exploded hx )
    {
        if( inactive_effects_[ MachineExplodedEffect ].empty() )
        {
            effects_store_.emplace_back( make_unique<fxmachineexplosion>( y, *fxrenderers_[ MachineExplodedEffect ].get() ) );
            active_effects_.push_back( effects_store_.back().get() );
        }
        else
        {
            active_effects_.push_back( inactive_effects_[  MachineExplodedEffect  ].back() );
            inactive_effects_[ MachineExplodedEffect ].pop_back();
        }

        reinterpret_cast<fxmachineexplosion*>( active_effects_.back() )->restart( hx );
    }

    void gamesfx::add_machine_weapon_firing_fx( float y, const Shot_fired& f )
    {
        assert( f.shot_fired_.shot_.hardpoint_type_ == WeaponHardpoint );

        if( inactive_effects_[ WeaponFiringEffect ].empty() )
        {
            effects_store_.emplace_back( make_unique<fxweaponfiring>( y, *fxrenderers_[ WeaponFiringEffect ].get() ) );
            active_effects_.push_back( effects_store_.back().get() );
        }
        else
        {
            active_effects_.push_back( inactive_effects_[ WeaponFiringEffect ].back() );
            inactive_effects_[ WeaponFiringEffect ].pop_back();
        }

        reinterpret_cast<fxweaponfiring*>( active_effects_.back() )->restart( f );
    }

    void gamesfx::add_machine_capdrain_firing_fx( float y, const Shot_fired& )
    {
    }

    void gamesfx::add_machine_capxfer_firing_fx( float y, const Shot_fired& )
    {
    }

    void gamesfx::add_machine_repper_firing_fx( float y, const Shot_fired& )
    {
    }

    void gamesfx::add_machine_miner_firing_fx( float y, const Shot_fired& )
    {
    }

}
