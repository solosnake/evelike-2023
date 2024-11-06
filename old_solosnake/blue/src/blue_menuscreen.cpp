#include "solosnake/blue/blue_luagameoptions.hpp"
#include "solosnake/blue/blue_menuscreen.hpp"
#include "solosnake/blue/blue_settings_objects.hpp"
#include "solosnake/blue/blue_translator.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/blue/blue_xmlscreenviews.hpp"
#include "solosnake/blue/blue_gamelogicconstants.hpp"
#include "solosnake/blue/blue_xmlscreenviewsgameloop.hpp"
#include "solosnake/gui.hpp"
#include "solosnake/hexspacing.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/iscreen_factory.hpp"
#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/run_gameloop.hpp"
#include "solosnake/sound_renderer_to_lua.hpp"
#include "solosnake/waitcursor.hpp"
#include "solosnake/window.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    class menuscreen::screenfactory : public solosnake::iscreen_factory
    {
    public:

        //! Takes window to create deferred_renderer from, and paths.
        screenfactory(
            const shared_ptr<ioswindow>&,
            const shared_ptr<user_settings>&,
            const shared_ptr<translator>& );

    private:

        //! Overloaded factory call.
        unique_ptr<iscreen> do_create_screen( const nextscreen& ) const override;

    private:

        shared_ptr<ioswindow>       window_;
        shared_ptr<user_settings>   userSettings_;
        shared_ptr<translator>      translator_;
    };

    //! Factory ctor grabs and dictates what is needed for construction of menuscreen. The file
    //! name is the name of the XML file to use to expose the gui internals and layout etc.
    menuscreen::screenfactory::screenfactory( const shared_ptr<ioswindow>& wndw,
                                              const shared_ptr<user_settings>& userSettings,
                                              const shared_ptr<translator>& tl8 )
        : window_( wndw )
        , userSettings_( userSettings )
        , translator_( tl8 )
    {
    }

    //! Factory method creates menuscreen.
    unique_ptr<iscreen> menuscreen::screenfactory::do_create_screen( const nextscreen& nxt ) const
    {
        return make_unique<menuscreen>( nxt.screen_name(), userSettings_, translator_ );
    }

    //---------------------------------------------------------------------------------------------

    menuscreen::menuscreen( const string& xmlScreenFile,
                            const shared_ptr<user_settings>& userSettings,
                            const shared_ptr<translator>& tl8 )
        : screen( xmlScreenFile, userSettings, tl8 )
    {
    }

    shared_ptr<iscreen_factory>
    menuscreen::make_factory( const shared_ptr<ioswindow>& w,
                              const shared_ptr<user_settings>& settings,
                              const shared_ptr<translator>& tl8 )
    {
        return make_shared<menuscreen::screenfactory>( w, settings, tl8 );
    }

    nextscreen menuscreen::show_screen( const shared_ptr<ioswindow>& w )
    {

        auto wnd      = window::make_shared( w );
        auto waiting  = waitcursor::make_sentinel();
        auto rndr     = make_rendering_system( wnd, get_user_settings(), get_datapaths() );
        auto wgtrndr  = make_widgetrendingstyles( rndr, get_user_settings(), get_datapaths() );
        auto sounds   = make_sound_system( get_user_settings(), get_datapaths() );
        auto widgets  = make_menu_widgetsfactory( get_user_settings(),
                                                  get_lce(),
                                                  wgtrndr,
                                                  get_translator() );

        // Expose luagameoptions to Lua.
        auto options = luagameoptions::make_gameoptions( get_lce(),
                                                         get_user_settings(),
                                                         get_translator(),
                                                         w );

        expose_soundrenderer_to_lua( sounds.get(), get_lua() );
        expose_hexspacing_function( get_lua() );

        // Now we create the gui, which will have access to all the objects we exposed to lua.
        auto loop = xmlscreensviewsgameloop::make_shared( get_xml_screen_filepath(),
                                                          wnd,
                                                          rndr,
                                                          sounds,
                                                          widgets );

        waiting.reset();

        run_gameloop( *loop, BLUE_FRAME_TICK_DURACTION_MS);

        return w->is_shutdown() ? nextscreen() : get_lce()->get_next_screen();
    }
}
