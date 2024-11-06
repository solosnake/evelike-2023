#include "solosnake/nextscreen.hpp"

namespace solosnake
{
    //! Default ctor constructs empty (negative) nextscreen.
    nextscreen::nextscreen()
    {
    }

    nextscreen::nextscreen( const std::string& name ) : screen_name_( name )
    {
    }

    //! Returns true if the name of the next screen is not the empty string.
    nextscreen::operator bool() const
    {
        return false == screen_name_.empty();
    }

    const std::string& nextscreen::screen_name() const
    {
        return screen_name_;
    }
}
