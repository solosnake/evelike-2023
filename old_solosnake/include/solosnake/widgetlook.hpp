#ifndef solosnake_widgetlook_hpp
#define solosnake_widgetlook_hpp

#include "solosnake/widgetstate.hpp"
#include "solosnake/widgetvisualstates.hpp"

namespace solosnake
{
    //! Represents the 'look' of a widget. Some widgets that react to mouse events, or which can be enabled/disabled
    //! can be in a visual state which is a combination of them being enabled and their state. This represents a table
    //! of visual states corresponding to the state.
    class widgetlook 
    {
    public:

        widgetlook();

        WidgetVisualStates get_visual_state_for( const WidgetState state ) const;

    private:
        
        WidgetVisualStates looks_[ SOLOSNAKE_WIDGETSTATE_COUNT ];
    };
}

#endif 
