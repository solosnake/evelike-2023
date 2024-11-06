#ifndef blue_drawasteroid_hpp
#define blue_drawasteroid_hpp

#include "solosnake/inline.hpp"
#include "solosnake/modelnode.hpp"

namespace solosnake
{
    class renderer_cache;
}

namespace blue
{
    class Asteroid;

    //! One of these is instanced per on-board-Asteroid. It is responsible
    //! for wrapping the rendering and visual updating needs for each Asteroid.
    class drawasteroid
    {
    public:

        drawasteroid(
            const Asteroid&,
            solosnake::modelnode&& );

        void update_animations( const unsigned int dtMs );

        SS_INLINE void draw_into( solosnake::modelscene& s ) const
        {
            node_.draw_into( s, transform4x4_ );
        }

        SS_INLINE const float* location_4x4() const
        {
            return transform4x4_;
        }

        SS_INLINE float* location_4x4()
        {
            return transform4x4_;
        }

        SS_INLINE const Asteroid* get_asteroid() const
        {
            return asteroid_;
        }

        SS_INLINE const float* worldXYZ() const
        {
            return worldXYZ_;
        }

    private:
        const Asteroid*         asteroid_;
        float                   transform4x4_[16];
        float                   worldXYZ_[3];
        float                   rotation_angle_;
        float                   rotation_rate_;
        solosnake::modelnode    node_;
    };
}

#endif
