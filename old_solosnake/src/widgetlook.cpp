#include "solosnake/widgetlook.hpp"

namespace solosnake
{
    widgetlook::widgetlook()
    {
        looks_[ WidgetDisabled                  ] = LooksDisabled;
        looks_[ WidgetNormal                    ] = LooksNormal;
        looks_[ WidgetHighlighted               ] = LooksActivatable;
        looks_[ WidgetActivatedAndCursorInside  ] = LooksActivated;
        looks_[ WidgetActivatedAndCursorOutside ] = LooksActivated;
    }

    WidgetVisualStates widgetlook::get_visual_state_for( const WidgetState state ) const
    {
        return looks_[ state ];
    }
}
