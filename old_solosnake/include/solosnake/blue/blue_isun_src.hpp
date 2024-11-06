#ifndef solosnake_isun_src_hpp
#define solosnake_isun_src_hpp

#include <string>
#include <memory>
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/texturehandle.hpp"
#include "solosnake/blue/blue_suntype.hpp"

namespace blue
{
    //! Details used by the renderer when drawing a sun.
    struct scene_sun
    {
        scene_sun();

        solosnake::sun_noise        sun_noise_;
        solosnake::texturehandle_t  sun_texture_;
    };

    //! An interface used by something which wishes load details about a sun type but which 
    //! does not really care where the sun data really comes from.
    class isun_src
    {
    public:

        virtual ~isun_src();

        virtual scene_sun get_sun_type_scene_sun( const SunType ) const = 0;
    };
}

#endif
