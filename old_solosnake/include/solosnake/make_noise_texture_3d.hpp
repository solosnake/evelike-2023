#ifndef SOLOSNAKE_MAKE_NOISE_TEXTURE_3D_HPP
#define SOLOSNAKE_MAKE_NOISE_TEXTURE_3D_HPP

#include <vector>
#include <cstdint>
#include "solosnake/power_of_2.hpp"

namespace solosnake
{
    //! Returns an array with size x size x size x 4 elements suitable for use
    //! as an OpenGL 3D noise texture.
    //!
    //! This should be considered a 3 dimensional texture with each
    //! texel containing a four byte element, and each of the 4 elements
    //! representing a different octave of Perlin noise.
    //!
    //! Recommended settings of size 2^7 (128) and frequency 4 creates an
    //! 8MB noise cube as suggested by the OpenGL red and orange books.
    std::vector<std::uint8_t> make_noise_texture_3d( power_of_2 size, unsigned int frequency );
}

#endif
