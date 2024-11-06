#ifndef blue_scene_sun_types_hpp
#define blue_scene_sun_types_hpp

#include <array>
#include "solosnake/ifilefinder.hpp"
#include "solosnake/itextureloader.hpp"
#include "solosnake/blue/blue_isun_src.hpp"

namespace blue
{
    class user_settings;

    //! Details needed to draw different types of sun. Loaded from xml file.
    class scene_sun_types : public isun_src
    {
    public:

        scene_sun_types( const user_settings&,
                         const solosnake::ifilefinder& sunfinder,
                         solosnake::itextureloader& texloader );

        virtual ~scene_sun_types();

        scene_sun get_sun_type_scene_sun( const SunType ) const override;

    private:

        std::array<scene_sun, BLUE_SUNTYPE_COUNT> sun_types_ = {};
    };

    //////////////////////////////////////////////////////////////////////////

    inline scene_sun scene_sun_types::get_sun_type_scene_sun( const SunType t ) const
    {
        return sun_types_[ t ];
    }
}

#endif
