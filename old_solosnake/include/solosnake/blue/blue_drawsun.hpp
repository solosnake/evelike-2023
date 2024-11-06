#ifndef blue_drawsun_hpp
#define blue_drawsun_hpp

#include "solosnake/inline.hpp"
#include "solosnake/modelnode.hpp"
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/blue/blue_isun_src.hpp"

namespace solosnake
{
    class renderer_cache;
    class deferred_renderer;
    class persistance;
}

namespace blue
{
    class Sun;

    //! One of these is instanced per on-board-Sun. It is responsible
    //! for wrapping the rendering and visual updating needs for each Sun.
    class drawsun
    {
    public:

        //! The scene Sun noise values are interpreted as rates of change of the
        //! scene Sun, not as initial values.
        drawsun(
            const Sun&,
            const scene_sun& );

        void update_animations( const unsigned int dtMs );

        SunStrength sun_strength() const;

        SunType sun_type() const;

        const float* world_xyz() const;

        //! Adds the Sun model and any light it casts onto the board. A Sun only illuminates
        //! the board if it has a strength greater than SunStrength0.
        //! @param tile_spacing This is the distance between two tiles on the board. This
        //!                     value is used to calculate the area of the board this Sun
        //!                     illuminates.
        void draw_into( solosnake::deferred_renderer*, const float tile_spacing );

    private:

        const Sun*                          sun_;
        solosnake::sun_location_and_radius  sun_xyzr_;
        scene_sun                           scene_sun_;
        float                               noise_rotation_rate_;
        float                               noise_move_rate_x_;
        float                               noise_move_rate_y_;
    };

    //////////////////////////////////////////////////////////////////////////

    inline SunStrength drawsun::sun_strength() const
    {
        return sun_->sun_strength();
    }

    inline SunType drawsun::sun_type() const
    {
        return sun_->sun_type();
    }

    inline const float* drawsun::world_xyz() const
    {
        return sun_xyzr_.world_location_;
    }
}

#endif
