#include "solosnake/blue/blue_softpoint.hpp"
#include "solosnake/blue/blue_throw.hpp"
#include <memory>

#define SS_SOFTPOINTS_XML_NAME      "softpoint"
#define SS_SOFTPOINTS_XML_VERSION   "1.0"

namespace blue
{
    Softpoint::Softpoint( const Component& c, const Hardware_modifier& m )
        : component_(c)
        , modifier_(m)
    {
    }

    bool Softpoint::operator == (const Softpoint& rhs ) const noexcept
    {
        return component_ == rhs.component_
            && modifier_  == rhs.modifier_;
    }

    bool Softpoint::operator != (const Softpoint& rhs ) const noexcept
    {
        return ! ((*this) == rhs);
    }
}