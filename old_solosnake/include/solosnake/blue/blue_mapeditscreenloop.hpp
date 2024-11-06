#ifndef blue_mapeditscreenloop_hpp
#define blue_mapeditscreenloop_hpp

#include <memory>
#include "solosnake/blue/blue_boardcamera.hpp"
#include "solosnake/blue/blue_drawboard.hpp"
#include "solosnake/blue/blue_game_hex_grid.hpp"
#include "solosnake/blue/blue_xmlscreenviewsgameloop.hpp"

namespace blue
{
    class mapedit;
    class user_settings;
    class widgetsfactory;

    class mapeditscreenloop : public xmlscreensviewsgameloop
    {
    public:

        static std::shared_ptr<mapeditscreenloop> make_shared(
            const solosnake::filepath& xmlScreenFile,
            const std::shared_ptr<mapedit>& sharedmap,
            const std::shared_ptr<user_settings>& settings,
            const std::shared_ptr<solosnake::window>&,
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>&,
            const std::shared_ptr<widgetsfactory>& );

        mapeditscreenloop(
            const solosnake::filepath& xmlScreenFile,
            const std::shared_ptr<mapedit>& sharedmap,
            const std::shared_ptr<user_settings>& settings,
            const std::shared_ptr<solosnake::window>&,
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>&,
            const std::shared_ptr<widgetsfactory>&,
            const solosnake::screenloop::HeapOnly& );

        virtual ~mapeditscreenloop();

        void start_scene( const unsigned long ms ) override;

        void render_scene( const unsigned long ms ) override;

        void end_scene( const unsigned long ms ) override;

    private:

        std::shared_ptr<mapedit>    map_;
    };
}

#endif
