#include "solosnake/filepath.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/nextscreen.hpp"
#include "solosnake/show_screens.hpp"
#include "solosnake/utf8_to_unicode.hpp"
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_datapathfinder.hpp"
#include "solosnake/blue/blue_start.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_screenfactory.hpp"
#include "solosnake/blue/blue_translator.hpp"
#include "solosnake/blue/blue_user_settings.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    namespace
    {
        //! Creates the initial window based up the settings of the
        //! application and the users preferences.
        shared_ptr<ioswindow> create_window( const user_settings& userSettings )
        {
            // Use safe defaults to begin with.
            const string title        = userSettings[AO_APP_TITLE];
            const unsigned int width  = userSettings[UO_WINDOW_WIDTH].as_uint();
            const unsigned int height = userSettings[UO_WINDOW_HEIGHT].as_uint();
            const bool fullscreen     = userSettings[UO_FULLSCREEN].is_true();
            const string windowtype   = userSettings[UO_WINDOW_TYPE];

            const bool allowResizing = true;

            assert( ! windowtype.empty() );
            assert( string::npos != windowtype.find( "opengl 3.3" ) );

            return ioswindow::make_shared( width,
                                           height,
                                           allowResizing, 
                                           title, 
                                           fullscreen,
                                           BLUE_WINDOW_CLASSNAME, 
                                           windowtype );
        }
    }

    //! This is the application entry point. Here the options parsed from the users
    //! command line are used (partially) to create the initial window (maybe)
    //! and start the application running.
    int start( const shared_ptr<user_settings>& userSettings )
    {
        userSettings->set( UO_CLEAN_RUN, "false,", UO_CLEAN_RUN_COMMENT );
        userSettings->save();

        auto dataPaths = make_shared<datapaths>( userSettings->data_dirs() );
        auto language  = make_shared<translator>( userSettings, dataPaths );

        shared_ptr<ioswindow> wndw = create_window( *userSettings );

        const int run = show_screens(
                            nextscreen( string( userSettings->value( AO_FIRST_SCREEN ) ) ),
                            screenfactory( wndw, userSettings, language, dataPaths ),
                            wndw );

        if( 0 == run )
        {
            // Record a clean run if one occurred.
            userSettings->set( UO_CLEAN_RUN, "true", UO_CLEAN_RUN_COMMENT );
        }

        // Save final settings, recording clean run or not.
        userSettings->save();

        return run;
    }
}
