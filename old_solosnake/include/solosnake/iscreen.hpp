#ifndef solosnake_iscreen_hpp
#define solosnake_iscreen_hpp

#include <memory>

namespace solosnake
{
    class ioswindow;
    class nextscreen;

    //! Base class for application screens. Each screen is to be a
    //! standalone mini app, and it runs and upon exit returns the
    //! name of the next screen to be created and displayed.
    class iscreen
    {
    public:

        virtual ~iscreen();

        //! Display a 'screen' (a mini application) using a window, 
        //! and returns the name of the next screen to display. This 
        //! is not a loop or update call. This call displays the screen
        //! for the lifetime of the screen, and upon returning a new 
        //! screen will be created and shown.
        virtual nextscreen show_screen( const std::shared_ptr<ioswindow>& ) = 0;
    };
}

#endif
