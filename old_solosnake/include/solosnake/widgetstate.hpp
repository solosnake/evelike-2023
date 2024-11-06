#ifndef solosnake_widgetstate_hpp
#define solosnake_widgetstate_hpp

namespace solosnake
{
    // In addition to these the widget can also have keyboard focus, giving
    // a total of 7 different states.
    enum WidgetState
    {
        // Always the state when is_disabled is true.
        WidgetDisabled,

        // State when not disable and not any other. Default state.
        WidgetNormal,

        // Mouse is over the widget - indicates to user widget can be
        // interacted with. Typically the widget is clickable.
        WidgetHighlighted,

        // Mouse is pressed down on it and inside it.
        WidgetActivatedAndCursorInside,

        // Mouse was pressed down inside but then left the active area.
        WidgetActivatedAndCursorOutside
    };

#define SOLOSNAKE_WIDGETSTATE_COUNT (5)
}
#endif
