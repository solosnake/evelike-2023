#include "solosnake/blue/blue_create_gamecamera.hpp"
#include "solosnake/blue/blue_hexcontentflags.hpp"
#include "solosnake/blue/blue_mapedit.hpp"
#include "solosnake/blue/blue_mapeditscreenloop.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/blue/blue_xmlscreenviews.hpp"

using namespace std;
using namespace solosnake;

namespace blue
{
    namespace
    {
        shared_ptr<xmlscreenviews> make_views( const filepath& xmlScreenFile,
                                               const shared_ptr<window>& wndw,
                                               const shared_ptr<widgetsfactory>& widgets )
        {
            auto views = xmlscreenviews::make_shared( xmlScreenFile,
                                                      wndw->get_window_dimensions(),
                                                      widgets );

            return views;
        }

        solosnake::dynbufferRGBA color0()
        {
            solosnake::dynbufferRGBA c = { 0xFFu, 0x0u, 0x0u, 0x10u };
            return c;
        }

        solosnake::dynbufferRGBA color1()
        {
            solosnake::dynbufferRGBA c = { 0xFFu, 0xFFu, 0x0u, 0x10u };
            return c;
        }

        solosnake::dynbufferRGBA color2()
        {
            solosnake::dynbufferRGBA c = { 0xFFu, 0x0u, 0xFFu, 0x10u };
            return c;
        }


    }


    std::shared_ptr<mapeditscreenloop>
    mapeditscreenloop::make_shared( const solosnake::filepath& xmlScreenFile,
                                    const std::shared_ptr<mapedit>& sharedmap,
                                    const shared_ptr<user_settings>& settings,
                                    const shared_ptr<window>& wndw,
                                    const shared_ptr<rendering_system>& r,
                                    const shared_ptr<sound_renderer>& s,
                                    const shared_ptr<widgetsfactory>& widgets )
    {
        auto msl = std::make_shared<mapeditscreenloop>( xmlScreenFile,
                                                        sharedmap,
                                                        settings,
                                                        wndw,
                                                        r,
                                                        s,
                                                        widgets,
                                                        solosnake::screenloop::HeapOnly() );

        xmlscreensviewsgameloop::add_as_observer_of_window( msl, wndw );

        return msl;
    }

    mapeditscreenloop::mapeditscreenloop( const solosnake::filepath& xmlScreenFile,
                                          const std::shared_ptr<mapedit>& sharedmap,
                                          const shared_ptr<user_settings>& settings,
                                          const shared_ptr<window>& wndw,
                                          const shared_ptr<rendering_system>& r,
                                          const shared_ptr<sound_renderer>& s,
                                          const shared_ptr<widgetsfactory>& widgets,
                                          const solosnake::screenloop::HeapOnly& ho )
        : xmlscreensviewsgameloop( wndw, make_views( xmlScreenFile, wndw, widgets ), r, s, ho )
        , map_( sharedmap )
    {
    }

    mapeditscreenloop::~mapeditscreenloop()
    {
    }

    void mapeditscreenloop::start_scene( const unsigned long ms )
    {
        assert( map_ );
        map_->setup_scene_view_and_proj_matrices();
        xmlscreensviewsgameloop::start_scene( ms );
    }

    void mapeditscreenloop::render_scene( const unsigned long ms )
    {
        xmlscreensviewsgameloop::render_scene( ms );
    }

    void mapeditscreenloop::end_scene( const unsigned long ms )
    {
        xmlscreensviewsgameloop::end_scene( ms );
    }
}
