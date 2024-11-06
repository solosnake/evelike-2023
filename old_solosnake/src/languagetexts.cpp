#include "solosnake/languagetexts.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    utf8text languagetexts::text_for( const utf8text& id ) const
    {
        auto i = texts_.find( id );
        return ( i == texts_.cend() ) ? id : i->second;
    }


    void languagetexts::add_translation( const utf8text& from, const utf8text& to )
    {
        if( from.is_empty() && ! to.is_empty() )
        {
            ss_throw( "Empty text can only translate to empty text." );
        }
        else
        {
            texts_[ from ] = to;
        }
    }
}
