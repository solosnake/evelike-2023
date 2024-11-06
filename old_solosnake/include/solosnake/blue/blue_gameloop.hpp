#ifndef blue_gameloop_hpp
#define blue_gameloop_hpp

#include <string>
#include <vector>
#include "solosnake/byte.hpp"
#include "solosnake/gui.hpp"
#include "solosnake/igameloop.hpp"
#include "solosnake/iscreenstate.hpp"
#include "solosnake/lockstepserver.hpp"
#include "solosnake/rect.hpp"
#include "solosnake/window.hpp"

namespace solosnake
{
    class window;
}

namespace blue
{
    class user_settings;
    class datapaths;
    class game;
    class gamestartupargs;
    class luagameoptions;
    class translator;

    //! This is the core game loop for the 'blue' game.
    //! It takes an XML file name which it will pass to through
    //! to the ctor of the inner game.
    class gameloop :
        public solosnake::igameloop,
        //public solosnake::network::lockstepserver::inetworkgameloop,
        public solosnake::iscreenstate,
        public solosnake::windowobserver
    {
        struct HeapOnly { };

    public:

        //! Only callable from make_shared_host and make_shared_client.
        gameloop(
            const solosnake::filepath& xmlScreenFile,
            const std::shared_ptr<gamestartupargs>&,
            const unsigned int n_players,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<solosnake::ioswindow>& wnd,
            const std::shared_ptr<datapaths>&,
            const std::shared_ptr<translator>&,
            const bool is_host,
            const HeapOnly& );

        //! Create shared host.
        static std::shared_ptr<gameloop> make_shared_host(
            const solosnake::filepath& xmlScreenFile,
            const std::shared_ptr<gamestartupargs>&,
            const unsigned int n_players,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<solosnake::ioswindow>& wnd,
            const std::shared_ptr<datapaths>&,
            const std::shared_ptr<translator>& );

        //! Create shared client.
        static std::shared_ptr<gameloop> make_shared_client(
            const solosnake::filepath& xmlScreenFile,
            const unsigned int n_players,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<solosnake::ioswindow>& wnd,
            const std::shared_ptr<datapaths>&,
            const std::shared_ptr<translator>& );

        virtual ~gameloop();

        bool is_screen_ended() const override;

        solosnake::nextscreen get_next_screen() const override;

    private:

        void ctor_common_code(
            const std::string& xmlScreenFile,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<datapaths>&,
            const std::shared_ptr<solosnake::ilanguagetext>& );

        void on_event(
            const solosnake::windowchange&,
            const solosnake::window& ) override;

        void get_frame_message(
            solosnake::network::bytebuffer& ) ;

        void initial_preframe_update() override;

        solosnake::LoopResult preframe_update(
            const unsigned long milliseconds ) override;

        solosnake::LoopResult initial_frame_update() override;

        void end_frame(
            const std::map<std::string, solosnake::network::bytebuffer>&,
            const unsigned long milliseconds ) ;

        solosnake::LoopResult frame_update(
            const unsigned long milliseconds ) override ;

        void advance_game_one_frame() override;

        void final_frame_update(
            const unsigned long milliseconds ) override;

        void setup_participants(
            const std::string&,
            const std::vector<std::string>& ) ;

        solosnake::LoopResult do_one_update_loop_cycle(
            const unsigned long milliseconds );

        solosnake::LoopResult process_inputs(
            const solosnake::iinput_events& );

        void process_message(
            const std::string& msgSender,
            const solosnake::network::byte* msg,
            const size_t msgSize );

        void update_frame(
            const unsigned long milliseconds );

        void render_gameloop(
            const unsigned long milliseconds );

        bool in_error_state() const;

    private:

        gameloop( const gameloop& );

        gameloop& operator=( const gameloop& );

        //! Class for tracking the states of the ALL participants.
        class stages
        {
        public:

            void initialise(
                const std::string& localparticipant,
                const std::vector<std::string>& otherparticipants );

            void participant_is_loaded( const std::string& );

            bool all_are_loaded() const;

        private:

            bool all_have_flag_set( unsigned int ) const;

            std::map<std::string, size_t> participant_states_;
        };

        struct fps_counter
        {
            fps_counter();

            std::string do_one_update_loop_cycle( const unsigned long ms );

            unsigned long   frames_too_long_for_60fps_;
            unsigned long   frames_too_long_for_30fps_;
            unsigned long   total_frames_count_;
            unsigned long   sample_time_;
        };

        enum GameLoopState
        {
            IsPlayingGame,
            IsStartingUp,
            IsHostSendingArgs,
            IsClientWaitingForArgs,
            IsLoadingLocallyWhileListeningForAllLoaded,
            HasLoadedLocallyAndIsListeningForAllLoaded,
            IsListeningForAllLoaded,
            HasFinishedPlayingGame,
            HasEncounteredLoadingErrors,
            HasEncounteredPlaytimeErrors
        };

        void change_state_to( GameLoopState nextState );

        bool try_load_game();

        // msgSize is guaranteed to be never zero.
        void process_gameactions_message( const solosnake::network::byte*, size_t msgSize );

        std::string                         client_name_;
        stages                              stage_states_;
        std::shared_ptr<solosnake::window>  window_;
        std::shared_ptr<luagameoptions>     options_;       // Exposes functions to lua.
        std::shared_ptr<game>               game_;
        std::shared_ptr<gamestartupargs>    startupargs_;
        GameLoopState                       currentState_;
        unsigned int                        playerCount_;   // The number of players in the match, including this player.
        fps_counter                         fps_;
        bool                                is_host_;
    };

}

#endif
