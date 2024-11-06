#include <cstdint>
#include <list>
#include <memory>
#include "solosnake/blue/blue_gamelogicconstants.hpp"
#include "solosnake/blue/blue_gamescreen.hpp"
#include "solosnake/blue/blue_gameloop.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_settings_objects.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/picturebutton_widgetfactory.hpp"
#include "solosnake/colour.hpp"
#include "solosnake/external/xml.hpp"
#include "solosnake/gui.hpp"
#include "solosnake/guicommandexecutor.hpp"
#include "solosnake/ixmlelementreader.hpp"
#include "solosnake/iwidget.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/run_gameloop.hpp"
#include "solosnake/iscreen_factory.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/timer.hpp"
#include "solosnake/xmlwidgetsmaker.hpp"

using solosnake::iscreen;
using solosnake::iscreen_factory;
using solosnake::ioswindow;
using solosnake::nextscreen;

using namespace std;

namespace blue
{
    //! Factory class private to gamescreen.
    class gamescreen::screenfactory : public iscreen_factory
    {
    public:

        screenfactory(
            const shared_ptr<ioswindow>&,
            const shared_ptr<user_settings>&,
            const shared_ptr<translator>& );

    private:

        unique_ptr<iscreen> do_create_screen( const nextscreen& ) const override;

    private:

        shared_ptr<ioswindow>       window_;
        shared_ptr<user_settings>   userSettings_;
        shared_ptr<translator>      translator_;
    };

    //! Factory ctor grabs and dictates what is needed for construction of gamescreen.
    //! The file name is the name of the XML file to use to expose the gui internals
    //! and layout etc.
    gamescreen::screenfactory::screenfactory( const shared_ptr<ioswindow>& wndw,
                                              const shared_ptr<user_settings>& userSettings,
                                              const shared_ptr<translator>& tl8 )
        : window_( wndw )
        , userSettings_( userSettings )
        , translator_( tl8 )
    {
    }

    //! Factory method creates gamescreen.
    unique_ptr<iscreen> gamescreen::screenfactory::do_create_screen( const nextscreen& nxt ) const
    {
        return make_unique<gamescreen>( nxt.screen_name(), userSettings_, translator_ );
    }

    //-------------------------------------------------------------------------

    gamescreen::gamescreen( const string& xmlScreenFile,
                            const shared_ptr<user_settings>& userSettings,
                            const shared_ptr<translator>& tl8 )
        : screen( xmlScreenFile, userSettings, tl8 )
    {
        assert( userSettings.get() != nullptr );
    }

    shared_ptr<solosnake::iscreen_factory>
    gamescreen::make_factory( const shared_ptr<ioswindow>& wnd,
                              const shared_ptr<user_settings>& settings,
                              const shared_ptr<translator>& tl8 )
    {
        return make_shared<gamescreen::screenfactory>( wnd, settings, tl8 );
    }

    nextscreen gamescreen::run_game_as_host( const std::shared_ptr<ioswindow>& wnd,
                                             const unsigned long tickLengthMs,
                                             const unsigned short timeout,
                                             const unsigned int n_participants,
                                             const unsigned short host_port ) const
    {
        auto startupargs = make_game_startup_args( get_user_settings() );

        auto loop = gameloop::make_shared_host( get_xml_screen_filepath(),
                                                startupargs,
                                                n_participants,
                                                get_user_settings(),
                                                wnd,
                                                get_datapaths(),
                                                get_translator() );

        // Create host. This ctor does not return until the game has completed.
        // REMOVED DUE TO BOOST auto server = make_unique<lockstepserver>( loop, n_participants, host_port, tickLengthMs, timeout );
        run_gameloop( *loop, BLUE_FRAME_TICK_DURACTION_MS );
        
        return loop->get_next_screen();
    }

    nextscreen gamescreen::run_game_as_client( const shared_ptr<ioswindow>& wnd,
                                               const unsigned long tickLengthMs,
                                               const unsigned short timeout,
                                               const unsigned int n_participants,
                                               const unsigned short host_port,
                                               const string& host_ip ) const
    {
        auto loop = gameloop::make_shared_client( get_xml_screen_filepath(),
                                                  n_participants,
                                                  get_user_settings(),
                                                  wnd,
                                                  get_datapaths(),
                                                  get_translator() );

        // Create client. This ctor does not return until the game has completed.
        // REMOVED DUE TO BOOST  auto server = make_unique<lockstepserver>( loop, host_ip, host_port, tickLengthMs, timeout );

        run_gameloop( *loop, BLUE_FRAME_TICK_DURACTION_MS );
        
        return loop->get_next_screen();
    }

    //! This is the point at which the screen creates its contents and enters
    //! its internal loop.
    nextscreen gamescreen::show_screen( const shared_ptr<ioswindow>& wnd )
    {
        auto t = solosnake::make_timer();
        ss_log( "Made timer              ", t->delta() );

        auto usersettings = get_user_settings();

        const string host_ip              = usersettings->value( BLUE_HOST_IP );
        const unsigned int n_participants = usersettings->value( BLUE_NUM_PLAYERS ).as_uint();
        const unsigned short host_port    = usersettings->value( BLUE_HOST_PORT ).as_ushort();
        const unsigned short timeout      = 5 * 60;
        const unsigned long tickLengthMs  = BLUE_FRAME_TICK_DURACTION_MS;

        nextscreen next;
        if( host_ip.empty() )
        {
            next = run_game_as_host( wnd, tickLengthMs, timeout, n_participants, host_port );
        }
        else
        {
            next = run_game_as_client( wnd, tickLengthMs, timeout, n_participants, host_port, host_ip );
        }

        return wnd->is_shutdown() ? nextscreen() : next;
    }
}
