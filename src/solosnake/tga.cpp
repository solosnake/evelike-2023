#include "solosnake/tga.hpp"
#include <fstream>
#include <sstream>

namespace solosnake
{
    void save_bgr_tga(const std::int16_t w,
                      const std::int16_t h,
                      const void* rgb,
                      bool flip_vertically,
                      const char* filename)
    {
        std::ofstream file(filename, std::ios::binary | std::ios::out);

        if(file.is_open())
        {
            // TGA header data.
            const std::uint8_t id_size    = 0;    // Size of following ID field
            const std::uint8_t map_type   = 0;    // Color map type 0 = none
            const std::uint8_t image_type = 2;    // Image type 2 = rgb
            const std::int16_t map_start  = 0;    // First entry in palette
            const std::int16_t map_size   = 0;    // Number of entries in palette
            const std::uint8_t map_bpp    = 0;    // Number of bits per palette entry
            const std::int16_t x_origin   = 0;    // X origin
            const std::int16_t y_origin   = 0;    // Y origin
            const std::int16_t width      = w;    // Width in pixels
            const std::int16_t height     = h;    // Height in pixels
            const std::uint8_t bpp        = 24;   // Bits per pixel
            const std::uint8_t descriptor = 0;    // Descriptor bits

            file.write( reinterpret_cast<const char*>(&id_size), sizeof(id_size) );
            file.write( reinterpret_cast<const char*>(&map_type), sizeof(map_type) );
            file.write( reinterpret_cast<const char*>(&image_type), sizeof(image_type) );
            file.write( reinterpret_cast<const char*>(&map_start), sizeof(map_start) );
            file.write( reinterpret_cast<const char*>(&map_size), sizeof(map_size) );
            file.write( reinterpret_cast<const char*>(&map_bpp), sizeof(map_bpp) );
            file.write( reinterpret_cast<const char*>(&x_origin), sizeof(x_origin) );
            file.write( reinterpret_cast<const char*>(&y_origin), sizeof(y_origin) );
            file.write( reinterpret_cast<const char*>(&width), sizeof(width) );
            file.write( reinterpret_cast<const char*>(&height), sizeof(height) );
            file.write( reinterpret_cast<const char*>(&bpp), sizeof(bpp) );
            file.write( reinterpret_cast<const char*>(&descriptor), sizeof(descriptor) );

            if(flip_vertically)
            {
                // TGA seems to be upside-down by default.
                file.write( reinterpret_cast<const char*>(rgb), sizeof(std::uint8_t) * 3u * w * h );
            }
            else
            {
                // Reverse the order of the rows of pixels, top to bottom.
                const auto row_bytes = sizeof(std::uint8_t) * 3u * w;
                for(auto i=0u; i < static_cast<unsigned>(h); ++i)
                {
                    auto row = reinterpret_cast<const char*>(rgb) + ((h - (i+1u)) * row_bytes);
                    file.write( row, row_bytes );
                }
            }
        }
        else
        {
            std::stringstream ss;
            ss << "Failed to create screenshot '" << filename << "'.";
            throw std::runtime_error(ss.str());
        }
    }
}