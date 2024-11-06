#include "solosnake/blue/blue_luagameoptions.hpp"
#include "solosnake/blue/blue_mapedit.hpp"
#include "solosnake/blue/blue_mapeditscreen.hpp"
#include "solosnake/blue/blue_mapeditscreenloop.hpp"
#include "solosnake/blue/blue_mapedit_widgetfactory.hpp"
#include "solosnake/blue/blue_settings_objects.hpp"
#include "solosnake/blue/blue_translator.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/blue/blue_xmlscreenviews.hpp"
#include "solosnake/blue/blue_xmlscreenviewsgameloop.hpp"
#include "solosnake/blue/blue_gamelogicconstants.hpp"
#include "solosnake/gui.hpp"
#include "solosnake/hexspacing.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/iscreen_factory.hpp"
#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/run_gameloop.hpp"
#include "solosnake/sound_renderer_to_lua.hpp"
#include "solosnake/window.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    class mapeditscreen::screenfactory : public iscreen_factory
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
        shared_ptr<user_settings>   user_settings_;
        shared_ptr<translator>      translator_;
    };

    //! Factory ctor grabs and dictates what is needed for construction of mapeditscreen. The file
    //! name is the name of the XML file to use to expose the gui internals and layout etc.
    mapeditscreen::screenfactory::screenfactory( const shared_ptr<ioswindow>& wndw,
                                                 const shared_ptr<user_settings>& settings,
                                                 const shared_ptr<translator>& tl8 )
        : window_( wndw )
        , user_settings_( settings )
        , translator_( tl8 )
    {
    }

    //! Factory method creates mapeditscreen.
    unique_ptr<iscreen> mapeditscreen::screenfactory::do_create_screen( const nextscreen& nxt ) const
    {
        return make_unique<mapeditscreen>( nxt.screen_name(), user_settings_, translator_ );
    }

    //---------------------------------------------------------------------------------------------

    mapeditscreen::mapeditscreen( const string& xmlScreenFile,
                                  const shared_ptr<user_settings>& userSettings,
                                  const shared_ptr<translator>& tl8 )
        : screen( xmlScreenFile, userSettings, tl8 )
    {
    }

    shared_ptr<iscreen_factory>
    mapeditscreen::make_factory( const shared_ptr<ioswindow>& w,
                                 const shared_ptr<user_settings>& settings,
                                 const shared_ptr<translator>& tl8 )
    {
        return make_shared<mapeditscreen::screenfactory>( w, settings, tl8 );
    }

    nextscreen mapeditscreen::show_screen( const shared_ptr<ioswindow>& w )
    {
        auto wnd          = window::make_shared( w );
        auto rndr         = make_rendering_system( wnd, get_user_settings(), get_datapaths() );
        auto wgtrndr      = make_widgetrendingstyles( rndr, get_user_settings(), get_datapaths() );
        auto sounds       = make_sound_system( get_user_settings(), get_datapaths() );
        auto sharedmap    = make_shared<mapedit>( *get_user_settings(), rndr );

        auto widgets      = make_menu_widgetsfactory( get_user_settings(),
                                                      get_lce(),
                                                      wgtrndr,
                                                      get_translator() );

        auto mapwidgets   = make_shared<mapedit_widgetfactory>( sharedmap,
                                                                get_user_settings(),
                                                                get_lce(),
                                                                wgtrndr,
                                                                get_translator() );

        auto screenwidgets = make_shared<mapeditscreen_widgetsfactory>( mapwidgets,
                                                                        widgets,
                                                                        get_lce() );

        // Expose luagameoptions to Lua.
        auto options = luagameoptions::make_gameoptions( get_lce(),
                                                         get_user_settings(),
                                                         get_translator(),
                                                         w );

        expose_soundrenderer_to_lua( sounds.get(), get_lua() );

        // Now we create the gui, which will have access to all the objects we exposed to lua.
        auto loop = mapeditscreenloop::make_shared( get_xml_screen_filepath(),
                                                    sharedmap,
                                                    get_user_settings(),
                                                    wnd,
                                                    rndr,
                                                    sounds,
                                                    screenwidgets );

        run_gameloop( *loop, BLUE_FRAME_TICK_DURACTION_MS );

        return w->is_shutdown() ? nextscreen() : get_lce()->get_next_screen();
    }
}
