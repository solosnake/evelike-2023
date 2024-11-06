#ifndef solosnake_show_screens_hpp
#define solosnake_show_screens_hpp
#include <memory>

namespace solosnake
{
    class nextscreen;
    class iscreen_factory;
    class ioswindow;

    //! Runs the screens in a loop, with no two screens existing at the same
    //! time, until a false nextscreen object is returned.
    int show_screens( const nextscreen& firstScreen,
                      const iscreen_factory&,
                      std::shared_ptr<ioswindow> );

}

#endif
