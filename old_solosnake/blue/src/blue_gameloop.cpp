#include <algorithm>
#include <memory>
#include <sstream>
#include "solosnake/blue/blue_board_state.hpp"
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_game.hpp"
#include "solosnake/blue/blue_gamestartupargs.hpp"
#include "solosnake/blue/blue_gameloop.hpp"
#include "solosnake/blue/blue_luagameoptions.hpp"
#include "solosnake/blue/blue_messages.hpp"
#include "solosnake/blue/blue_translator.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/loopresult.hpp"
#include "solosnake/persistance.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/sound_renderer.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/timer.hpp"
#include "solosnake/rect.hpp"
#include "solosnake/unreachable.hpp"

using namespace solosnake;
using namespace std;

#define BLUE_PARTICIPANT_IS_LOADED (0x1u)

namespace blue
{
    gameloop::gameloop( const solosnake::filepath& xmlScreenFile,
                        const shared_ptr<gamestartupargs>& startupargs,
                        const unsigned int n_players,
                        const shared_ptr<user_settings>& userSettings,
                        const shared_ptr<ioswindow>& wnd,
                        const shared_ptr<datapaths>& dataPaths,
                        const shared_ptr<translator>& tl8,
                        const bool is_host,
                        const HeapOnly& )
        : client_name_()
        , stage_states_()
        , window_( window::make_shared( wnd ) )
        , options_()
        , game_()
        , startupargs_( startupargs )
        , currentState_( IsStartingUp )
        , playerCount_( n_players )
        , fps_()
        , is_host_( is_host )
    {
        auto lce = luaguicommandexecutor::make_shared();
        options_ = luagameoptions::make_gameoptions( lce, userSettings, tl8, wnd );
        game_    = game::make_shared_game( xmlScreenFile, window_, userSettings, dataPaths, tl8, lce );
        window_->add_observer( static_pointer_cast<windowobserver>( game_ ) );
    }

    shared_ptr<gameloop>
    gameloop::make_shared_client( const solosnake::filepath& xmlScreenFile,
                                  const unsigned int n_players,
                                  const shared_ptr<user_settings>& userSettings,
                                  const shared_ptr<ioswindow>& wnd,
                                  const shared_ptr<datapaths>& dataPaths,
                                  const shared_ptr<translator>& tl8 )
    {
        const shared_ptr<gamestartupargs> empty_args;

        auto loop =  make_shared<gameloop>( xmlScreenFile,
                                            empty_args,
                                            n_players,
                                            userSettings,
                                            wnd,
                                            dataPaths,
                                            tl8,
                                            false, // Client, not host.
                                            HeapOnly() );
        return loop;
    }

    shared_ptr<gameloop>
    gameloop::make_shared_host( const solosnake::filepath& xmlScreenFile,
                                const shared_ptr<gamestartupargs>& startupargs,
                                const unsigned int n_players,
                                const shared_ptr<user_settings>& userSettings,
                                const shared_ptr<ioswindow>& wnd,
                                const shared_ptr<datapaths>& dataPaths,
                                const shared_ptr<translator>& tl8 )
    {
        return make_shared<gameloop>( xmlScreenFile,
                                      startupargs,
                                      n_players,
                                      userSettings,
                                      wnd,
                                      dataPaths,
                                      tl8,
                                      true, // Host, not client.
                                      HeapOnly() );
    }

    gameloop::~gameloop()
    {
        game_.reset();
    }

    //! All state changes are routed through this call.
    void gameloop::change_state_to( gameloop::GameLoopState nextState )
    {
        currentState_ = nextState;
    }

    bool gameloop::is_screen_ended() const
    {
        assert( 0 );
        return false;
    }

    //! Returns the screen this screen should exit to.
    nextscreen gameloop::get_next_screen() const
    {
        return game_->get_next_screen();
    }

    void gameloop::initial_preframe_update()
    {
    }

    LoopResult gameloop::preframe_update( const unsigned long )
    {
        const bool shut = window_->get_new_events().is_shutdown();
        return shut ? StopLooping : LoopAgain;
    }

    //! Called once only, assuming 'preframe_update' has not stopped the loop.
    LoopResult gameloop::initial_frame_update()
    {
        assert( currentState_ == IsStartingUp );

        window_->show();

        if (try_load_game())
        {
            currentState_ = IsPlayingGame;
        }

        return do_one_update_loop_cycle( 0 );
    }

    //! Continuously called during loop with the delta since the last
    //! 'frame_update' call, in ms. Returning false will stop the loop.
    LoopResult gameloop::frame_update( const unsigned long ms )
    {
        return do_one_update_loop_cycle( ms );
    }

    //! The application is killing the update loop.
    //! Called with the number of the frame, and the delta
    //! since the last 'frame_update' call, in ms.
    void gameloop::final_frame_update( const unsigned long )
    {
    }

    bool gameloop::in_error_state() const
    {
        return HasEncounteredLoadingErrors  == currentState_ ||
               HasEncounteredPlaytimeErrors == currentState_;
    }

    LoopResult gameloop::do_one_update_loop_cycle( const unsigned long ms )
    {
        window_->set_title( fps_.do_one_update_loop_cycle( ms ) );

        const iinput_events& events = window_->get_new_events();

        update_frame( ms );
        render_gameloop( ms );

        if( LoopAgain == process_inputs( events ) &&
                false == events.is_shutdown() &&
                false == in_error_state() )
        {
            return LoopAgain;
        }
        else
        {
            return StopLooping;
        }
    }

    LoopResult gameloop::process_inputs( const iinput_events& e )
    {
        return game_->handle_inputs( e );
    }

    void gameloop::update_frame( const unsigned long ms )
    {
        if( currentState_ == IsPlayingGame )
        {
            game_->update_frame( ms );
        }
    }

    void gameloop::advance_game_one_frame()
    {
        if (currentState_ == IsPlayingGame)
        {
            game_->advance_one_frame();
        }
    }

    void gameloop::render_gameloop( const unsigned long ms )
    {
        if( currentState_ == IsPlayingGame )
        {
            game_->render_game( ms );
        }
    }

    void gameloop::on_event( const windowchange&, const window& )
    {
        assert( ! "Not implemented" );
    }

    bool gameloop::try_load_game()
    {
        auto t = make_timer();
        bool loaded = game_->try_load_game( startupargs_ );
        ss_log( "Loading game took\t\t", t->elapsed() );
        return loaded;
    }

    void gameloop::process_gameactions_message( const network::byte* msg, size_t msgSize )
    {
        assert( msg );
        assert( msgSize > 0 );
        game_->process_gameactions_message( msg, msgSize );
    }

    //! The request from the server for this frame's message.
    void gameloop::get_frame_message( network::bytebuffer& msg )
    {
        if( currentState_ == IsPlayingGame )
        {
            game_->get_frame_message( msg );
        }
        else
        {
            char charMsg;

            // IsPlayingGame will never appear in this switch!
            switch( currentState_ )
            {
                case IsStartingUp:
                {
                    ss_dbg( "IsStartingUp" );
                    change_state_to( is_host_ ? IsHostSendingArgs : IsClientWaitingForArgs );
                }
                break;

                case IsHostSendingArgs:
                {
                    ss_dbg( "IsHostSendingArgs" );
                    assert( is_host_ && startupargs_ );
                    basic_ostringstream<char> os;
                    os << char( StartingConditionMessage );
                    os << startupargs_->to_str();
                    msg = network::bytebuffer( os.str() );
                    change_state_to( IsLoadingLocallyWhileListeningForAllLoaded );
                }
                break;

                case IsClientWaitingForArgs:
                {
                    // Sends no messages:
                    ss_dbg( "IsClientWaitingForArgs" );
                }
                break;

                case IsLoadingLocallyWhileListeningForAllLoaded:
                {
                    ss_dbg( "IsLoadingLocallyWhileListeningForAllLoaded" );
                    if( try_load_game() )
                    {
                        change_state_to( HasLoadedLocallyAndIsListeningForAllLoaded );
                    }
                    else
                    {
                        ss_wrn( "gameloop : HasEncounteredLoadingErrors" );
                        change_state_to( HasEncounteredLoadingErrors );
                    }
                }
                break;

                case HasLoadedLocallyAndIsListeningForAllLoaded:
                {
                    ss_dbg( "HasLoadedLocallyAndIsListeningForAllLoaded" );
                    charMsg = char( LoadedMessage );
                    msg = network::bytebuffer( &charMsg, 1 );
                    change_state_to( IsListeningForAllLoaded );
                }
                break;

                case IsListeningForAllLoaded:
                {
                    ss_dbg( "IsListeningForAllLoaded" );
                }
                break;

                case HasFinishedPlayingGame:
                {
                    ss_dbg( "HasFinishedPlayingGame" );
                }
                break;

                case HasEncounteredLoadingErrors:
                {
                    ss_dbg( "HasEncounteredLoadingErrors" );
                    charMsg = char( LoadingErrorMessage );
                    msg = network::bytebuffer( &charMsg, 1 );
                }
                break;

                case HasEncounteredPlaytimeErrors:
                {
                    ss_dbg( "HasEncounteredPlaytimeErrors" );
                    charMsg = char( PlayingErrorMessage );
                    msg = network::bytebuffer( &charMsg, 1 );
                }
                break;

                default:
                    ss_unreachable;
                    break;
            };
        }
    }

    //! We will receive our own messages here too.
    void gameloop::process_message( const string& msgSender,
                                    const network::byte* msg,
                                    const size_t msgSize )
    {
        // We will filter out messages which are invalid for out state.
        switch( msg[0] )
        {
            case LoadingErrorMessage:
            {
                if( msgSender == client_name_ )
                {
                    ss_err( "This client encountered loading errors." );
                }
                else
                {
                    ss_err( "A networked client encountered loading errors." );
                }
            }
            break;

            case StartingConditionMessage:
            {
                if( msgSender != client_name_ )
                {
                    if( currentState_ == IsClientWaitingForArgs )
                    {
                        auto startupargs = make_shared<gamestartupargs>( msg + 1 );
                        startupargs_.swap( startupargs );
                        change_state_to( IsLoadingLocallyWhileListeningForAllLoaded );
                    }
                    else
                    {
                        ss_wrn( "StartingConditionMessage received while in unexpected "
                                "state." );
                    }
                }
            }
            break;

            case LoadedMessage:
            {
                if( false == stage_states_.all_are_loaded() )
                {
                    // We also encounter our own name here.
                    stage_states_.participant_is_loaded( msgSender );

                    if( stage_states_.all_are_loaded() )
                    {
                        // Everyone is loaded, including ourselves.
                        change_state_to( IsPlayingGame );
                    }
                }
            }
            break;

            case GameActionsMessage:
            {
                if( currentState_ == IsPlayingGame )
                {
                    if( msgSize > 1 )
                    {
                        // Do not pass zero sized message onwards :
                        // as zero = no message.
                        process_gameactions_message( msg + 1, msgSize - 1 );
                    }
                }
            }
            break;

            default:
                break;
        }
    }

    //! Notification from the server about this frame's messages.
    //! This is main loop.
    void gameloop::end_frame( const map<string, network::bytebuffer>& msgs, unsigned long )
    {
        const auto end = msgs.cend();
        auto i = msgs.cbegin();

        while( i != end )
        {
            if( false == i->second.is_empty() )
            {
                process_message( i->first, i->second.data(), i->second.size() );
            }

            ++i;
        }

        advance_game_one_frame();
    }

    //! Callback from the lockstep server, use this opportunity to initialise player
    //! specific settings and states.
    void gameloop::setup_participants( const string& me, const vector<string>& others )
    {
        stage_states_.initialise( me, others );
        client_name_ = me;
    }

    //--------------------------------------------------------------------------
    // stages
    //--------------------------------------------------------------------------


    void gameloop::stages::initialise( const string& localparticipant,
                                       const vector<string>& participants )
    {
        participant_states_[localparticipant] = 0;

        // Initialise all the participant state flags to zero.
        for_each( participants.begin(),
                  participants.end(),
                  [&]( const string & name )
        { participant_states_[name] = 0; } );
    }

    void gameloop::stages::participant_is_loaded( const string& participant )
    {
        auto s = participant_states_.find( participant );

        if( s != participant_states_.end() )
        {
            s->second |= BLUE_PARTICIPANT_IS_LOADED;
        }
        else
        {
            ss_throw( "Unknown participant." );
        }
    }

    //! Returns true if this flag is set on all participant states.
    bool gameloop::stages::all_have_flag_set( unsigned int flag ) const
    {
        auto i   = participant_states_.cbegin();
        auto end = participant_states_.cend();

        while( i != end )
        {
            if( 0u == ( i->second & flag ) )
            {
                return false;
            }

            ++i;
        }

        return true;
    }

    bool gameloop::stages::all_are_loaded() const
    {
        return all_have_flag_set( BLUE_PARTICIPANT_IS_LOADED );
    }

    //--------------------------------------------------------------------------
    // fps counter
    //--------------------------------------------------------------------------

    gameloop::fps_counter::fps_counter()
        : frames_too_long_for_60fps_( 0u )
        , frames_too_long_for_30fps_( 0u )
        , total_frames_count_( 0u )
        , sample_time_( 0u )
    {
    }

    string gameloop::fps_counter::do_one_update_loop_cycle( const unsigned long ms )
    {
        sample_time_ += ms;
        ++total_frames_count_;

        auto estimated_fps = sample_time_ > 0u
                             ? ( total_frames_count_ * 1000u ) / sample_time_
                             : 0u;

        if( sample_time_ > ( 1000 * 10 ) )
        {
            // Clear counter every ~10s
            sample_time_ = 0u;
            total_frames_count_ = 1;
            frames_too_long_for_30fps_ = 0;
            frames_too_long_for_60fps_ = 0;
        }

        if( ms > ( 1000u / 30u ) )
        {
            ++frames_too_long_for_30fps_;
            ++frames_too_long_for_60fps_;
        }
        else if( ms > ( 1000u / 60u ) )
        {
            ++frames_too_long_for_60fps_;
        }

        auto percent_60fps = ( 100u * ( total_frames_count_ - frames_too_long_for_60fps_ ) )
                             / total_frames_count_;
        auto percent_30fps = ( 100u * ( frames_too_long_for_30fps_ ) ) / total_frames_count_;

        ostringstream os;
        os << percent_60fps << "% 60fps, " << percent_30fps
           << "% slower than 30fps, ~" << estimated_fps << " fps";
#ifndef NDEBUG
        os << " (DEBUG)";
#endif

        return os.str();
    }
}
