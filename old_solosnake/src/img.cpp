#include "solosnake/img.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    img::img( const filepath& url ) : img_( url, false )
    {
        if( img_.format() != solosnake::image::format_bgra )
        {
            ss_log( url.string(), " is not in RGBA format." );
            ss_throw( "Non RGBA img loaded." );
        }
    }
}
