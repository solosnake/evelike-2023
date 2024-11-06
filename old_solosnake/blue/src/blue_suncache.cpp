#include <cassert>
#include "solosnake/blue/blue_load_sun.hpp"
#include "solosnake/blue/blue_suncache.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/deferred_renderer_types.hpp"

using namespace std;
using namespace solosnake;

namespace blue
{
    namespace
    {
        std::string get_suns_filename( const user_settings& settings )
        {
            // TODO: LOAD FROM SETTINGS.
            return "default.xml";
        }
    }

    scene_sun_types::scene_sun_types( const user_settings& settings,
                                      const ifilefinder& sunfinder,
                                      itextureloader& texloader )
        : sun_types_( load_suns_file( sunfinder.get_file( get_suns_filename( settings ) ),
                                      texloader ) )
    {
    }

    scene_sun_types::~scene_sun_types()
    {
    }
}
