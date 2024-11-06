#include <cassert>
#include "solosnake/iwidgetname.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    namespace
    {
        //! Checks to see if the name is a name not beginning with _ or a numeral,
        //! and is composed of only the letters A-Z or a-z and underscore.
        bool is_safe_name( const std::string& name )
        {
            const std::string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "0123456789_";

            static_assert( '0' < '9', "Bug in code" );

            return ( false == name.empty() ) && ( std::string::npos == name.find_first_not_of( letters ) )
                   && ( name[0] != '_' ) && ( name[0] < '0' || name[0] > '9' );
        }
    }

    iwidgetname::iwidgetname( const std::string& name ) : name_( name )
    {
        if( name_.empty() )
        {
            ss_throw( "Attempt to create a widget with an empty name." );
        }

        if( false == is_safe_name( name_ ) )
        {
            ss_throw( "Attempt to create Widget with name which contains illegal characters." );
        }
    }
}
