#ifndef blue_drawmachine_hpp
#define blue_drawmachine_hpp

#include "solosnake/bgr.hpp"
#include "solosnake/point.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/inline.hpp"
#include "solosnake/unaliased.hpp"
#include "solosnake/modelnode.hpp"
#include "solosnake/blue/blue_emissivechannels.hpp"
#include "solosnake/blue/blue_machine_events.hpp"
#include "solosnake/blue/blue_machinereadouts.hpp"

namespace solosnake
{
    class renderer_cache;
}

namespace blue
{
    class Machine;

    //! One of these is instanced per on-board-machine. It is responsible
    //! for wrapping the rendering and visual updating needs for each machine.
    //! The drawmachine object owns an unmanaged pointer to its machine, and requires
    //! that the machine object exists.
    class drawmachine
    {
    public:

        //! The address of the machine will be taken here.
        drawmachine(
            const Machine&,
            solosnake::renderer_cache&,
            float boardY );

        //! frameFraction is how much of the current frame has elapsed. This
        //! can exceed 1.0.
        //! Returns the position to use for the machine on the board for this
        //! frame.
        solosnake::point2d update_animations( float frameFraction );

        void update_location( const Positional_update& posInfoEvent );

        void draw_into( solosnake::modelscene& s ) const;

        const float* location_4x4() const;

        float* location_4x4();

        const Machine* get_machine() const;

    private:

        solosnake::point2d interpolate_position( const float frameFraction );

        void update_emissive_colours();

        void orient_hardpoints();

        struct visiblemachine
        {
            MachineReadout  emissive_intensity_src( unsigned int i, unsigned int j ) const;

            solosnake::bgra emissive_colour( unsigned int i ) const;

            solosnake::modelnode    node_;
            Emissive_channels        channels_;
        };

    private:

        const Machine*      machine_;
        Position_info       previous_position_;
        Position_info       current_position_;
        float               transform4x4_[16];
        float               position_interpolation_;
        float               boardY_;
        visiblemachine      visiblemachine_;
        solosnake::bgra     engine_glow_;
    };
}

#include "solosnake/blue/blue_drawmachine.inl"

#endif
