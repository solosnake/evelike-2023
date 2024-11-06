#ifndef blue_game_hpp
#define blue_game_hpp

#include <memory>
#include <string>
#include <vector>
#include "solosnake/byte.hpp"
#include "solosnake/bytebuffer.hpp"
#include "solosnake/dimension.hpp"
#include "solosnake/fullscreen.hpp"
#include "solosnake/loopresult.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/minimised.hpp"
#include "solosnake/radians.hpp"
#include "solosnake/windowobserver.hpp"
#include "solosnake/blue/blue_boardcamera.hpp"
#include "solosnake/blue/blue_igameview.hpp"

namespace solosnake
{
    class iinput_events;
    class ilanguagetext;
    class luaguicommandexecutor;
    class nextscreen;
    class rendering_system;
    class screenxy;
    class sound_renderer;
    class windowchange;
    struct point3d;
}

namespace blue
{
    class user_settings;
    class Board_state;
    class drawgame;
    class datapaths;
    class gameaction;
    class gamestate;
    class gamestartupargs;
    class Game_hex_grid;
    class iboardcamera;
    class Machine;
    class player;
    class pendinginstructions;
    class xmlscreenviews;

    struct Hex_coord;
    struct pickresult;

    //! The class inside the game loop that contains the views etc.
    class game :
        public igameview,
        public std::enable_shared_from_this<game>,
        public solosnake::windowobserver
    {
    protected:

        struct HeapOnly
        {
        };

    public:

        static std::shared_ptr<game> make_shared_game(
            const solosnake::filepath& xmlScreenFile,
            const std::shared_ptr<solosnake::window>& wnd,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<datapaths>&,
            const std::shared_ptr<solosnake::ilanguagetext>&,
            const std::shared_ptr<solosnake::luaguicommandexecutor>& );

        //! Cannot be publicly called.
        game(
            const solosnake::filepath& xmlScreenFile,
            const std::shared_ptr<solosnake::window>& wnd,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<datapaths>&,
            const std::shared_ptr<solosnake::ilanguagetext>&,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const HeapOnly& );

        ~game();

        bool try_load_game( const std::shared_ptr<gamestartupargs>& );

        void begin_playing();

        void advance_one_frame();

        void update_frame( const unsigned int ms );

        void render_game( const unsigned int ms );

        void end_playing();

        // Return true to continue looping.
        solosnake::LoopResult handle_inputs( const solosnake::iinput_events& );

        // Process externally generated message as well as internal.
        // msg is guaranteed to be never null and msgSize never zero.
        void process_gameactions_message( const solosnake::network::byte* msg, const size_t msgSize );

        void get_frame_message( solosnake::network::bytebuffer& );

        solosnake::nextscreen get_next_screen() const;

        pickresult get_boardpick( solosnake::screenxy xy ) const override;

        solosnake::point3d get_boardplanepick( solosnake::screenxy xy ) const override;

        void move_camera_forwards( float units ) override;

        solosnake::point3d camera_location() const override;

        solosnake::point3d camera_lookedat() const override;

        void translate_camera( float x, float z ) override;

        void rotate_camera( solosnake::radians ) override;

        void instructions_sent_highlight( Hex_coord ) override;

        const Machine* get_machine_at( Hex_coord ) const override;

        void render_with_loaded_game( const unsigned long dtMs ) const;

    private:

        void on_event( const solosnake::windowchange&, const solosnake::window& ) override;

        void render_without_loaded_game( const unsigned long dtMs ) const;

        void set_screensize( const solosnake::dimension2d<unsigned int>& );

        void move_listener_to_camera_location();

        void handle_screensize_changed(
            const solosnake::dimension2d<unsigned int>&,
            const solosnake::window& );

        void handle_fullscreen_changed(
            const solosnake::FullscreenState&,
            const solosnake::window& );

        void handle_minimised_changed(
            const solosnake::MinimisedState&,
            const solosnake::window& );

        void accumulate_local_game_actions();

    private:

        typedef void ( game::*renderFunc_t )( const unsigned long dtMs ) const;

        const Board_state*   get_boardstate() const;

        const Game_hex_grid&  get_gamehexgrid() const;

        std::vector<std::shared_ptr<player>>                local_players_;
        std::vector<gameaction>                             local_gameactions_cache_;
        std::vector<gameaction>                             server_gameactions_cache_;
        std::shared_ptr<user_settings>                      userSettings_;
        std::shared_ptr<datapaths>                          datapaths_;
        std::shared_ptr<solosnake::ilanguagetext>           language_;
        std::shared_ptr<solosnake::luaguicommandexecutor>   lce_;
        renderFunc_t                                        renderFunc_;
        std::shared_ptr<solosnake::sound_renderer>          sound_renderer_;
        std::shared_ptr<solosnake::rendering_system>        renderer_;
        std::shared_ptr<drawgame>                           boardRenderer_;
        std::shared_ptr<gamestate>                          sharedgamestate_;
        std::shared_ptr<pendinginstructions>                pendinginstructions_;
        std::shared_ptr<xmlscreenviews>                     guiViews_;
        std::unique_ptr<boardcamera>                        camera_;
        unsigned int                                        unpaused_frame_duration_;
        bool                                                not_paused_;
        solosnake::filepath                                 xmlScreenFile_;
        bool                                                fullyLoaded_;
    };

    //-------------------------------------------------------------------------

    inline void game::render_game( const unsigned int msDelta )
    {
        ( this->*renderFunc_ )( msDelta );
    }

}

#endif
