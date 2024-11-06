#include "solosnake/blue/blue_iscreenview.hpp"
#include "solosnake/throw.hpp"

namespace blue
{
    iscreenview::iscreenview( const std::string& name ) : screenview_name_( name )
    {
        if( name.empty() )
        {
            ss_throw( "View name cannot be empty." );
        }
    }

    iscreenview::~iscreenview()
    {
    }
}
