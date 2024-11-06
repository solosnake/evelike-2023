#ifndef solosnake_fxtriangles_hpp
#define solosnake_fxtriangles_hpp

#include "solosnake/unaliased.hpp"

namespace solosnake
{
    struct fxtriangle;

    //! A 'baffle' is the name I am giving to the classic sfx transparency
    //! object where 3 intersecting rectangles are used to create the illusion
    //! of a solid volume e.g. an explosion.
    //! This method will construct a baffle with UVs in the 0,0 to 1,1 range
    //! centred at xyz and with each side being 2 x sfx_size long.
    //! @note fx must point to at least 6 triangles.
    void initialise_baffle( fxtriangle* fx6, float const xz[2], float y, float sfx_size );

    //! Creates a single flat plane at xyz of side length 2 x sfx_size with
    //! UVs 0,0 to 1,1.
    void initialise_plane( fxtriangle* fx2, float const xz[2], float y, float x_scale, float y_scale );

    //! Fills the two triangles @a fx2 with a scaled square billboard centred at
    //! x y z and scaled in x and y (so that the result may not be a square).
    void initialise_billboard( fxtriangle* unaliased fx2,
                               float x,
                               float y,
                               float z,
                               float x_scale,
                               float y_scale,
                               const float* unaliased invView4x4 );

    //! Fills the two triangles @a fx2 with a scaled square billboard centred at
    //! x y z and scaled in x and y (so that the result may not be a square). The
    //! billboard is offset in the eye plane by @a z_offset.
    void initialise_offset_billboard( fxtriangle* unaliased fx2,
                                      float x,
                                      float y,
                                      float z,
                                      float x_scale,
                                      float y_scale,
                                      float z_offset,
                                      const float* unaliased invView4x4 );

    //! Creates a beam width/length at a point between fromXZ and toXZ. The
    //! end of the beam will arrive at toXZ when timeline is 1.0. The head of
    //! the beam will be at fromXZ when timeline is 0.0.
    //! fx4[0] vertices 0 and 1 are guaranteed to be the leading edge vertices
    //! of the beam.
    void initialise_beam( fxtriangle* unaliased fx4,
                          const float* unaliased fromXZ,
                          const float* unaliased toXZ,
                          float timeline,
                          float y,
                          float beamwidth,
                          float beamlength );
}

#endif
