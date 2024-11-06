#ifndef solosnake_buttonpressoutcome_hpp
#define solosnake_buttonpressoutcome_hpp

namespace solosnake
{
    enum ButtonPressOutcome
    {
        // Gui will take no action.
        IgnoreButtonPress,

        // Gui will make this widget the exclusive actioned widget
        // if there it not already one set.
        ExclusiveButtonPress,

        // Gui will make this widget the exclusive actioned widget
        // and begin a dragging action.
        BeginDraggingButtonPress
    };
}

#endif
