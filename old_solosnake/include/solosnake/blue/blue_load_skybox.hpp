#ifndef blue_load_skybox_hpp
#define blue_load_skybox_hpp

#include <memory>
#include <string>

namespace solosnake
{
    struct cubemap;
}

namespace blue
{
    class datapaths;

    solosnake::cubemap load_skybox( 
        const std::string& skyboxname, 
        const std::shared_ptr<datapaths>& );
}

#endif
