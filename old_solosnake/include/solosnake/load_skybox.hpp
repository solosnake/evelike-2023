#ifndef solosnake_load_skybox_hpp
#define solosnake_load_skybox_hpp

#include "solosnake/filepath.hpp"

namespace solosnake
{
    struct cubemap;
    class ifilefinder;

    //! Loads the skybox file as it's textures into a cubemap, or throws.
    cubemap load_skybox( 
      const filepath& skyboxpath, 
      const ifilefinder& texfinder );
}

#endif
