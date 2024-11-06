#ifndef blue_softpoint_hpp
#define blue_softpoint_hpp

#include <vector>
#include "solosnake/blue/blue_component.hpp"
#include "solosnake/blue/blue_hardware_modifier.hpp"

namespace blue
{
    /// A Softpoint is a non functional component of a Machine that augments it
    /// in some way - for example CPU speed modifiers etc.
    class Softpoint final
    {
    public:

        Softpoint(const Component&, const Hardware_modifier& );

        const Hardware_modifier& modifier() const noexcept;

        const Component& details() const noexcept;

        bool operator == (const Softpoint& ) const noexcept;
        bool operator != (const Softpoint& ) const noexcept;

    private:

        Component         component_;
        Hardware_modifier modifier_;
    };

    //-------------------------------------------------------------------------

    inline const Hardware_modifier& Softpoint::modifier() const noexcept
    {
        return modifier_;
    }

    inline const Component& Softpoint::details() const noexcept
    {
        return component_;
    }
}

#endif
