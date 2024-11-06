#include <algorithm>
#include <set>
#include <string>
#include "solosnake/iwidgetcollection.hpp"
#include "solosnake/iwidget.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    namespace
    {
        bool not_all_names_are_unique( const std::list<std::shared_ptr<iwidget>>& w )
        {
            std::set<std::string> names;
            for( auto it = w.cbegin(); it != w.cend(); ++it )
            {
                if( names.find( ( *it )->name().str() ) != names.cend() )
                {
                    ss_dbg( "Duplicated name : ", ( *it )->name().str() );
                    return true;
                }
                else
                {
                    names.insert( ( *it )->name().str() );
                }
            }

            return false;
        }
    }

    iwidgetcollection::iwidgetcollection( std::list<std::shared_ptr<iwidget>>&& w )
    {
        if( not_all_names_are_unique( w ) )
        {
            ss_throw( "Two or more widgets with same name found." );
        }

        widgets_ = std::move( w );

        // Sort them by z order, so they render back to front.
        widgets_.sort( 
            [&]( const std::shared_ptr<iwidget>& lhs, const std::shared_ptr<iwidget>& rhs )
            { return lhs->zlayer() < rhs->zlayer(); } );
    }
}
