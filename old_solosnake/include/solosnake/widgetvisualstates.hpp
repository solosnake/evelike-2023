#ifndef solosnake_widgetvisualstates_hpp
#define solosnake_widgetvisualstates_hpp

namespace solosnake
{
    enum WidgetVisualStates
    {
        //! Widget is disabled.
        LooksDisabled,

        //! Widget is enabled but not being interacted with.
        LooksNormal,

        //! Widget is enabled, and is showing it is activatable; the mouse
        //! might be over it etc.
        LooksActivatable,

        //! Widget is enabled and in activated state; the mouse may be pressed
        //! down inside it.
        LooksActivated,

        //! Widget is enabled but not being interacted with, and also has
        //! Keyboard focus.
        LooksNormalAndHasKeyboardFocus,

        //! Widget is enabled, and is showing it is activatable; the mouse
        //! might be over it etc, and also has Keyboard focus.
        LooksActivatableAndHasKeyboardFocus,

        //! Widget is enabled and in activated state; the mouse may be pressed
        //! down inside it, and also has Keyboard focus.
        LooksActivatedAndHasKeyboardFocus
    };


#define SOLOSNAKE_WIDGET_VISUALSTATES_COUNT (7)

    // Names of the states when read in from XML.
#define SOLOSNAKE_VISUALSTATES_DISABLED_XMLNAME     "disabled"
#define SOLOSNAKE_VISUALSTATES_NORMAL_XMLNAME       "normal"
#define SOLOSNAKE_VISUALSTATES_HIGHLIGHTED_XMLNAME  "highlighted"
#define SOLOSNAKE_VISUALSTATES_ACTIVATED_XMLNAME    "activated"
}
#endif
