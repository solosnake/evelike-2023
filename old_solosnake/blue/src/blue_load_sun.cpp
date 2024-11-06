#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/image.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/make_iimg.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/external/xml.hpp"
#include "solosnake/blue/blue_load_sun.hpp"

using namespace std;
using namespace solosnake;

#define BLUE_SUNS_ELEMENT_NAME              "suns"
#define BLUE_SUN_ELEMENT_NAME               "sun"
#define BLUE_SUN_TYPE_ATTRIB                "type"
#define BLUE_SUNS_NOISE_ROTATION_ATTRIB     "dr"
#define BLUE_SUNS_NOISE_MOVE_X_ATTRIB       "dx"
#define BLUE_SUNS_NOISE_MOVE_Y_ATTRIB       "dy"
#define BLUE_SUNS_TEXTURE_ATTRIB            "tex"

namespace blue
{
    array<scene_sun, BLUE_SUNTYPE_COUNT> load_suns_file( const filepath& sunspath,
                                                         itextureloader& loader )
    {
        array<scene_sun, BLUE_SUNTYPE_COUNT> suns;

        TiXmlDocument sunsfile;
        sunsfile.LoadFile( sunspath.string().c_str() );

        const auto& sunsElement = get_child_element( sunsfile, BLUE_SUNS_ELEMENT_NAME );

        const auto* sunElement = sunsElement.FirstChildElement( BLUE_SUN_ELEMENT_NAME );

        while( sunElement )
        {
            int suntype = 0;
            float noise_rotation = 0.0f;
            float noise_move_x   = 0.0f;
            float noise_move_y   = 0.0f;
            texturehandle_t sun_texture = texturehandle_t();

            read_attribute( *sunElement, BLUE_SUN_TYPE_ATTRIB, suntype, false );

            if( ! can_be_a_suntype( suntype ) )
            {
                ss_throw( "'" BLUE_SUN_TYPE_ATTRIB "' is not a valid SunType value." );
            }

            read_attribute( *sunElement, BLUE_SUNS_NOISE_ROTATION_ATTRIB, noise_rotation, false );
            read_attribute( *sunElement, BLUE_SUNS_NOISE_MOVE_X_ATTRIB,   noise_move_x,   false );
            read_attribute( *sunElement, BLUE_SUNS_NOISE_MOVE_Y_ATTRIB,   noise_move_y,   false );

            const auto texname = read_attribute( *sunElement, BLUE_SUNS_TEXTURE_ATTRIB );
            const auto tex     = loader.get_texture( texname );

            suns[ suntype ].sun_noise_.noise_move_x_   = noise_move_x;
            suns[ suntype ].sun_noise_.noise_move_y_   = noise_move_y;
            suns[ suntype ].sun_noise_.noise_rotation_ = noise_rotation;
            suns[ suntype ].sun_texture_               = tex;

            sunElement = sunElement->NextSiblingElement( BLUE_SUN_ELEMENT_NAME );
        }

        return suns;
    }
}
