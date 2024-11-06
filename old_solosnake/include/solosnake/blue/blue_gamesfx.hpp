#ifndef blue_gamesfx_hpp
#define blue_gamesfx_hpp

#include <memory>
#include <vector>
#include "solosnake/rendering_system_fwd.hpp"
#include "solosnake/blue/blue_ifx.hpp"
#include "solosnake/blue/blue_ifxrenderer.hpp"

namespace solosnake
{
    class persistance;
    class sound_renderer;
}

namespace blue
{
    struct Sensed;
    struct Machine_exploded;
    struct Shot_fired;

    //! The factory class through which the game can activate and advance fx.
    class gamesfx
    {
    public:

        gamesfx(
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>&,
            const solosnake::persistance& );

        ~gamesfx();

        //! Called once per frame to update the states of any active fx.
        //! When advance animations is true any internal time dependent states
        //! should be advanced by dtms (milliseconds). When advance animations
        //! is false then the game might be paused and the effects can be
        //! displayed but in a frozen state if possible.
        void update( bool advanceAnimations, const unsigned int dtms );

        //! Here 'draw' generically means render or update audio or perform
        //! any feedback effect associated with the fx.
        void draw_fx();

        void add_sense_fx( float y, Sensed );

        void add_machine_exploded_fx( float y, Machine_exploded );

        void add_machine_weapon_firing_fx( float y, const Shot_fired& );

        void add_machine_capdrain_firing_fx( float y, const Shot_fired& );

        void add_machine_capxfer_firing_fx( float y, const Shot_fired& );

        void add_machine_repper_firing_fx( float y, const Shot_fired& );

        void add_machine_miner_firing_fx( float y, const Shot_fired& );

    private:

        std::vector<ifx*>                           active_effects_;
        std::vector<ifx*>                           pending_active_effects_;
        std::vector<std::vector<ifx*>>              inactive_effects_;
        std::vector<std::unique_ptr<ifx>>           effects_store_;
        std::vector<std::unique_ptr<ifxrenderer>>   fxrenderers_;
    };

}

#endif
