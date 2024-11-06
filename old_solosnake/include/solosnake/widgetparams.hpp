#ifndef solosnake_widgetparams_hpp
#define solosnake_widgetparams_hpp

#include <string>
#include "solosnake/rect.hpp"

namespace solosnake
{
    //! Params for the iwidget base class.
    struct widgetparams
    {
        //! Constructs default invalid params.
        widgetparams();

        widgetparams(
            const std::string& id,
            const Rect& area,
            const int z,
            const int tab);

        //! The name uniquely identifying a widget in a widget collection.
        std::string         name;

        //! The area of the screen belonging to the widget. This is the area that accepts
        //! mouse events.
        Rect                active_area;

        //! The Z ordering of a widget. When widgets overlap each other the Z order determines
        //! which widget receives input. Higher Z layer values receive input before lower Z values.
        //! The default Z layer values is typically -1.
        int                 zlayer;

        //! When tabbing through widgets on screen, this determines the ordering.
        int                 tab_order_value;
    };

    //-------------------------------------------------------------------------

    inline widgetparams::widgetparams()
        : name()
        , active_area()
        , zlayer(-1)
        , tab_order_value(-1)
    {
    }

    inline widgetparams::widgetparams(
        const std::string& id,
        const Rect& area,
        const int z,
        const int tab)
        : name(id)
        , active_area(area)
        , zlayer(z)
        , tab_order_value(tab)
    {
    }
}

#endif
