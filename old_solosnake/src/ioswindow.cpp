#include "solosnake/ioswindow.hpp"
#include "solosnake/iwindow.hpp"

#ifdef SS_OS_WINDOWS
#include "./win32/ioswindow.cpp"
#else
#ifdef SS_OS_LINUX
#include "./x11/ioswindow.cpp"
#endif
#endif

namespace solosnake
{
    ioswindow::ioswindow() : input_events_(), window_()
    {
    }

    ioswindow::~ioswindow()
    {
    }

    void ioswindow::attach( std::shared_ptr<iwindow> newwindow )
    {
        std::shared_ptr<iwindow> oldwindow = window_.lock();

        if( newwindow != oldwindow )
        {
            window_ = newwindow;

            on_attached( oldwindow, newwindow );
        }
    }
}
