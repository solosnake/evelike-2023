#ifndef solosnake_itextureloader_hpp
#define solosnake_itextureloader_hpp

#include <string>
#include <memory>
#include "solosnake/texturehandle.hpp"

namespace solosnake
{
    //! Interface for loading a texture as a texture handle from its name.
    class itextureloader
    {
    public:

        virtual ~itextureloader();

        virtual texturehandle_t get_texture( const std::string& ) = 0;
    };
}

#endif
