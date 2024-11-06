#include <exception>
#include "solosnake/iscreen.hpp"
#include "solosnake/iscreen_factory.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/nextscreen.hpp"
#include "solosnake/show_screens.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    //! Creates and displays the named screen
    int show_screens( const nextscreen& firstScreen,
                      const iscreen_factory& screenMaker,
                      std::shared_ptr<ioswindow> window )
    {
        int result = 0;

        try
        {
            nextscreen currentScreen = firstScreen;

            while( currentScreen )
            {
                ss_log( "Creating screen '", currentScreen.screen_name(), '\'' );

                auto activeScreen = screenMaker.create_screen( currentScreen );

                if( activeScreen.get() )
                {
                    ss_log( "Displaying screen '", currentScreen.screen_name(), '\'' );
                    currentScreen = activeScreen->show_screen( window );
                }
                else
                {
                    ss_throw( "User screen factory returned null screen." );
                }
            }
        }
        catch( const std::exception& e )
        {
            ss_err( e.what() );
            result = -1;
        }
        catch( ... )
        {
            result = -2;
        }

        return result;
    }
}
