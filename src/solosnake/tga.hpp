#ifndef SOLOSNAKE_TGA_HPP
#define SOLOSNAKE_TGA_HPP

#include <cstdint>

namespace solosnake
{
    /// @Saves an BGR set of pixels (w * h pixels) to a Truevision Graphics
    /// Adapter format file (https://en.wikipedia.org/wiki/Truevision_TGA)
    /// @param flip_vertically Flip the image upside down.
    void save_bgr_tga(std::int16_t w,
                      std::int16_t h,
                      const void* rgb,
                      bool flip_vertically,
                      const char* filename);
}

#endif
