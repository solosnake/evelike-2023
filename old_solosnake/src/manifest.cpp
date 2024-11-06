#include "solosnake/manifest.hpp"
#include <algorithm>

namespace solosnake
{
    manifest::manifest() : ixmlelementreader( "manifest" )
    {
    }

    bool manifest::can_read_version( const std::string& v ) const
    {
        return v.compare( "1.0" ) == 0;
    }

    bool manifest::read_from_element( const TiXmlElement& elem )
    {
        if( elem.ValueStr() == "item" )
        {
            const std::string* item = elem.Attribute( std::string( "name" ) );

            if( item )
            {
                contents_.push_back( *item );
            }

            return item != nullptr;
        }

        return false;
    }

    bool manifest::contains( const std::string& s ) const
    {
        return contents_.cend() != std::find( contents_.cbegin(), contents_.cend(), s );
    }
}
