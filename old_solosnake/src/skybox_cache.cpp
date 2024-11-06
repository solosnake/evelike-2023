#include <cassert>
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/load_skybox.hpp"
#include "solosnake/skybox_cache.hpp"

using namespace std;

namespace solosnake
{
    skybox_cache::skybox_cache( shared_ptr<ifilefinder> skyboxfinder, shared_ptr<ifilefinder> texfinder )
        : skyboxfinder_( skyboxfinder ), texfinder_( texfinder )
    {
        assert( skyboxfinder );
        assert( texfinder );
    }

    skybox_cache::~skybox_cache()
    {
    }

    void skybox_cache::compact_cache()
    {
        knownskyboxes_.clear();
    }

    void skybox_cache::load_skybox_file( const string& name )
    {
        auto url = skyboxfinder_->get_file( name );
        auto sky = make_shared<cubemap>( solosnake::load_skybox( url, *texfinder_ ) );
        knownskyboxes_.insert( make_pair( name, sky ) );
    }

    shared_ptr<cubemap> skybox_cache::get_skybox( const string& name )
    {
        if( knownskyboxes_.find( name ) == knownskyboxes_.end() )
        {
            load_skybox_file( name );
        }

        return knownskyboxes_.find( name )->second;
    }
}
