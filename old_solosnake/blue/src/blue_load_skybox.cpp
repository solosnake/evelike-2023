#include "solosnake/blue/blue_load_skybox.hpp"
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_datapathfinder.hpp"
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/load_skybox.hpp"

using namespace std;

namespace blue
{
    solosnake::cubemap load_skybox( const string& skyboxname,
                                    const shared_ptr<datapaths>& paths )
    {
        auto url       = paths->get_skyboxes_filepath( skyboxname );
        auto texfinder = make_shared<datapathfinder>( paths, &datapaths::get_textures_filepath );
        return solosnake::load_skybox( url, *texfinder );
    }
}
