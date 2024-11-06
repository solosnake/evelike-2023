#ifndef blue_isfx_hpp
#define blue_isfx_hpp

#include "solosnake/blue/blue_gamesfx.hpp"

namespace blue
{
    enum GameEffectType
    {
        SensedEffect,
        MachineExplodedEffect,
        WeaponFiringEffect,
        CapDrainFiringEffect,
        CapXFerFiringEffect
    };

    enum SfxState
    {
        EffectIsFinished,
        EffectIsActive
    };

#   define BLUE_EFFECT_TYPES_COUNT (5)

    class ifxrenderer;

    //! A base class for game sfx. sfx can be gui, rendering or sound effects
    //! that have no game-play interaction, but are just audio or visual
    //! feedback to the user of game events.
    class ifx
    {
    public:

        ifx( GameEffectType t, ifxrenderer& );

        virtual ~ifx();

        GameEffectType effect_type() const
        {
            return effect_type_;
        }

        //! Called once per frame, when advance animations is true any internal
        //! time dependent state should be advanced by dtms (milliseconds).
        //! When advance animations is false then the game might be paused and 
        //! the effect can be displayed but in a frozen state if possible.
        virtual SfxState update( bool advanceAnimations, const unsigned int dtms );

        //! The renderer for an effect can request custom event data from the
        //! instance of the effect being renderer. By default this returns 
        //! nullptr.
        virtual const void* event_data() const;

        //! Here 'draw' generically means render or update audio or perform
        //! any feedback effect associated with the sfx. Calls the abstract
        //! sfx renderer to render this instance. The first call to draw_sfx
        //! after restart or construction will start the fx.
        void draw_sfx();

        //! Some aspects of an fx play only once, such as sounds, and so need
        // !to know about when an instance of an effect is starting.
        bool is_started() const
        {
            return started_;
        }

        void restart( const float xz[2], unsigned int duration );

    private:
        ifxrenderer*    renderer_;
        GameEffectType  effect_type_;
        unsigned int    duration_ms_;
        unsigned int    current_ms_;
        float           worldXZ_[2];
        bool            started_;
    };
}

#endif
