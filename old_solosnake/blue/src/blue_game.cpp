#include <cassert>
#include <algorithm>
#include <vector>
#include <random>
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/deferred_renderer_to_lua.hpp"
#include "solosnake/image.hpp"
#include "solosnake/languagetexts.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/make_iimg.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/persistance.hpp"
#include "solosnake/perlin_noise_skybox.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/sound_renderer.hpp"
#include "solosnake/sound_renderer_to_lua.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/timer.hpp"
#include "solosnake/vkeys.hpp"
#include "solosnake/external/xml.hpp"
#include "solosnake/blue/blue_board_state.hpp"
#include "solosnake/blue/blue_blueprint.hpp"
#include "solosnake/blue/blue_compiler.hpp"
#include "solosnake/blue/blue_create_gamecamera.hpp"
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_datapathfinder.hpp"
#include "solosnake/blue/blue_deck.hpp"
#include "solosnake/blue/blue_drawboard.hpp"
#include "solosnake/blue/blue_drawgame.hpp"
#include "solosnake/blue/blue_game.hpp"
#include "solosnake/blue/blue_gamecommands.hpp"
#include "solosnake/blue/blue_gamestartupargs.hpp"
#include "solosnake/blue/blue_gamestate.hpp"
#include "solosnake/blue/blue_iboardcamera.hpp"
#include "solosnake/blue/blue_iscreenview.hpp"
#include "solosnake/blue/blue_keymapping.hpp"
#include "solosnake/blue/blue_log_events.hpp"
#include "solosnake/blue/blue_load_skybox.hpp"
#include "solosnake/blue/blue_machineparts.hpp"
#include "solosnake/blue/blue_messages.hpp"
#include "solosnake/blue/blue_pickresult.hpp"
#include "solosnake/blue/blue_pendinginstructions.hpp"
#include "solosnake/blue/blue_player.hpp"
#include "solosnake/blue/blue_screenxmlnames.hpp"
#include "solosnake/blue/blue_selecteddeck.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_settings_objects.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/blue/blue_widgetsfactory.hpp"
#include "solosnake/blue/blue_xmlscreenviews.hpp"

// Z near is initialized to this, but the one loaded from user settings is used.
#define Z_NEAR_DEFAULT      (0.1f)
//#define SS_STRESSBOT_SCRIPT "nop.txt"
#define SS_STRESSBOT_SCRIPT "random-rotate-and-fire.txt"
#define SS_SKYBOXNAME       "ppa.xml"
#define SS_STRESSBOT_COUNT  (5)
#define SS_STRESS_BIGBOTS   (1)
#define SS_BOARDLIT         (0.275f)
#define SS_BOARDALPHA       (0.5f)

using namespace solosnake;
using namespace std;

namespace blue
{
    namespace
    {
        // Translates 1 or more messages in the buffer into gameaction(s) and
        // pushes them into the existing array.
        void translate_message_to_gameactions( const network::byte* unaliased msg,
                                               size_t msgSize,
                                               vector<gameaction>& actions )
        {
            assert( msg );
            assert( msgSize > 0 );

            while( msgSize > 0 )
            {
                gameaction action( msg );
                assert( action.bytes_size() <= msgSize );
                msg     += action.bytes_size();
                msgSize -= action.bytes_size();
                actions.push_back( move( action ) );
            }
        }

        void translate_gameactions_to_message( const vector<gameaction>& actions,
                                               network::bytebuffer& msg )
        {
            if( !actions.empty() )
            {
                msg += static_cast<solosnake::network::byte>( GameActionsMessage );

                for( size_t i = 0; i < actions.size(); ++i )
                {
                    msg += actions[i].to_bytebuffer();
                }
            }
        }

        //! Initial view set on game, replaced by proper controller-view once
        //! the game is loaded.
        class firstview : public iscreenview
        {
        public:

            firstview() : iscreenview( "firstview" )
            {
            }

        private:

            void render_view( const unsigned long ) const override
            {
            }

            void handle_fullscreen_changed( const solosnake::FullscreenState& ) override
            {
            }

            void handle_minimised_changed( const solosnake::MinimisedState& ) override
            {
            }

            void handle_screensize_changed( const solosnake::dimension2d<unsigned int>& ) override
            {
            }

            solosnake::LoopResult handle_inputs( const iinput_events& ) override
            {
                return LoopAgain;
            }

            void activate_view() override
            {
            }

            void deactivate_view() override
            {
            }

            void advance_one_frame() override
            {
            }
        };

        //! Initial first state, never actually used.
        class firststate : public solosnake::iscreenstate
        {
            bool is_screen_ended() const override
            {
                return false;
            }

            nextscreen get_next_screen() const override
            {
                ss_wrn( "Returning empty nextscreen, this should cause game to exit." );
                return nextscreen();
            }
        };

        //! Make the dummy screen views to use before the game is loaded.
        shared_ptr<xmlscreenviews> make_default_game_screenviews()
        {
            return xmlscreenviews::make_shared( make_shared<firstview>(), make_shared<firststate>() );
        }

        //! Exposes board states to lua for inspection etc.
        void expose_boardstate_to_lua( const boardstate&, shared_ptr<solosnake::luaguicommandexecutor> )
        {
        }


        // ******************** TEMP CODE ONLY ***************************

        //! Returns an array of gridDim x gridDim board positions, with
        //! position[0] always being (0,0).
        vector<pair<unsigned char, unsigned char>> make_random_board_positions( size_t gridDim )
        {
            vector<pair<unsigned char, unsigned char>> positions( gridDim );
            positions.reserve( gridDim * gridDim );

            if( gridDim > 0 )
            {
                // Always place first bot on 0,0
                positions.push_back( make_pair<unsigned char, unsigned char>( 0, 0 ) );

                for( unsigned int x = 1; x < gridDim; ++x )
                {
                    for( unsigned int y = 1; y < gridDim; ++y )
                    {
                        positions.push_back( pair<unsigned char, unsigned char>( x, y ) );
                    }
                }

                // Randomly shuffle all positions except position #0.
                if( positions.size() > 1 )
                {
                    // Use a standardized and repeatable rand so as not to get
                    // differences on Linux.
                    minstd_rand0 generator( 0xB33F );
                    std::shuffle(positions.begin() + 1, positions.end(), generator);
                }
            }

            return positions;
        }

        void fill_cargo_with_building_materials( Machine& m )
        {
            amount oneUnitOfEach[BLUE_TRADABLE_TYPES_COUNT];
            for( size_t i = 0; i < BLUE_TRADABLE_TYPES_COUNT; ++i )
            {
                oneUnitOfEach[i][i] = 1;
            }

            bool hasSpace = m.cargo().free_volume() > 0;

            while( hasSpace )
            {
                // For now only add the building blocks.
                for( size_t i = Metals; i <= Alkalis; ++i )
                {
                    hasSpace = m.try_add_cargo( oneUnitOfEach[i] );
                }
            }
        }

        class testmachinemaker
        {
        public:

            static const unsigned short APs[19];
            static const unsigned short TAPs[4];

            testmachinemaker( shared_ptr<datapaths> paths,
                              shared_ptr<compiler> cc,
                              shared_ptr<machineparts> parts,
                              unsigned int defaultPeriod )
                : datapaths_( paths )
                , cc_( cc )
                , machineparts_( parts )
                , ap_( APs, APs + sizeof( APs ) / sizeof( unsigned short ) )
                , tap_( TAPs, TAPs + sizeof( TAPs ) / sizeof( unsigned short ) )
                , next_ap_index_( 0 )
                , next_tap_index_( 0 )
                , defaultPeriod_( defaultPeriod )
            {
            }

            Thruster_attachpoint next_thrusterattachpoint()
            {
                return tap_.at( next_tap_index_++ );
            }

            AttachPoint next_attachpoint()
            {
                return ap_.at( next_ap_index_++ );
            }

            unique_ptr<Machine> make_machine( const char* name, const char* scriptname, ... )
            {
                ss_log( "Making Machine ", name );

                typedef map<AttachPoint, blueprint::Oriented_hardpoint> hpmap_t;
                typedef map<AttachPoint, Softpoint> spmap_t;
                typedef map<Thruster_attachpoint, Thruster> thrustermap_t;

                next_ap_index_ = next_tap_index_ = 0;
                hpmap_t hardpoints;
                map<Thruster_attachpoint, Thruster> thrusters;
                map<AttachPoint, Softpoint> softpoints;
                va_list args;
                va_start( args, scriptname );

                for( const char* a = va_arg( args, const char* ); a != nullptr; a = va_arg( args, const char* ) )
                {
                    if( auto hp = machineparts_->get_hardpoint( a ) )
                    {
                        blueprint::Oriented_hardpoint ohp( blueprint::Rotated270, *hp );
                        hardpoints.insert( hpmap_t::value_type( next_attachpoint(), ohp ) );
                    }
                    else if( auto sp = machineparts_->get_softpoint( a ) )
                    {
                        softpoints.insert( spmap_t::value_type( next_attachpoint(), *sp ) );
                    }
                    else if( auto thstr = machineparts_->get_thruster( a ) )
                    {
                        thrusters.insert( thrustermap_t::value_type( next_thrusterattachpoint(), *thstr ) );
                    }
                    else
                    {
                        ss_err( "Unknown component name ", a );
                        ss_throw( "Unknown component name" );
                    }
                }
                va_end( args );

                static uint8_t hue = 2;
                hue += 2;
                hue %= 255;
                uint8_t hue_shift = hue;

                shared_ptr<blueprint> bpo = make_shared<blueprint>( machineparts_->get_chassis( 0 ), thrusters, softpoints, hardpoints );
                vector<shared_ptr<blueprint>> knownBPOs;
                auto testScriptURL = datapaths_->get_scripts_filepath( scriptname );
                instructions compiled = cc_->compile_file( testScriptURL );

                return Machine::create_machine( name, hue_shift, bpo, knownBPOs, defaultPeriod_, move( compiled ) );
            }

        private:

            shared_ptr<datapaths>       datapaths_;
            shared_ptr<compiler>        cc_;
            shared_ptr<machineparts>    machineparts_;
            vector<unsigned short>      ap_;
            vector<unsigned short>      tap_;
            size_t                      next_ap_index_;
            size_t                      next_tap_index_;
            unsigned int                defaultPeriod_;
        };

        const unsigned short testmachinemaker::APs[19] =
        {
            64, 62, 66, 44, 42, 46, 68, 60, 48, 40,
            24, 22, 26, 20, 28,  3,  5,  1,  7
        };

        const unsigned short testmachinemaker::TAPs[4] =
        {
            83, 85, 81, 87
        };

        unique_ptr<Machine> make_small_test_machine( const char* name,
                                                     const char* scriptname,
                                                     shared_ptr<datapaths> datapaths,
                                                     compiler& cc,
                                                     const machineparts& parts,
                                                     unsigned int defaultPeriod )
        {
            ss_dbg( "TEMP: make_test_machine..." );

            // TODO REMOVE THIS make_test_machine
            typedef map<AttachPoint, blueprint::Oriented_hardpoint> hpmap_t;

            hpmap_t hardpoints;
            map<Thruster_attachpoint, Thruster> thrusters;
            map<AttachPoint, Softpoint> softpoints;

            const size_t numSoftpoints = parts.softpoint_count();
            size_t softpointIndex = 0;

            const size_t numHardpoints = parts.hardpoint_count();

            if( numHardpoints > 0 )
            {
                size_t hardpointIndex = 0;
                blueprint::Oriented_hardpoint hp1(
                    blueprint::Rotated0, parts.get_hardpoint( hardpointIndex++ % numHardpoints ) );
                blueprint::Oriented_hardpoint hp3(
                    blueprint::Rotated90, parts.get_hardpoint( hardpointIndex++ % numHardpoints ) );
                hardpoints.insert( hpmap_t::value_type( AttachPoint( 1 ), hp1 ) );
                hardpoints.insert( hpmap_t::value_type( AttachPoint( 3 ), hp3 ) );
            }
            else
            {
                softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                       AttachPoint( 1 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
                softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                       AttachPoint( 3 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            }

            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 20 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 22 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );

            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 40 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 42 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );

            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 60 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 62 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );

            if( parts.thruster_count() > 0 )
            {
                const size_t numThrusters = parts.thruster_count();
                size_t thrusterIndex = 0;
                thrusters.insert( map<Thruster_attachpoint, Thruster>::value_type(
                                      Thruster_attachpoint( 81 ),
                                      parts.get_thruster( thrusterIndex++ % numThrusters ) ) );
                thrusters.insert( map<Thruster_attachpoint, Thruster>::value_type(
                                      Thruster_attachpoint( 83 ),
                                      parts.get_thruster( thrusterIndex++ % numThrusters ) ) );
            }
            else
            {
                softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                       AttachPoint( 81 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
                softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                       AttachPoint( 83 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            }

            shared_ptr<blueprint> bpo = make_shared<blueprint>( parts.get_chassis( 0 ), thrusters, softpoints, hardpoints );
            vector<shared_ptr<blueprint>> knownBPOs;

            static uint8_t hue = 0;
            hue += 2;
            hue %= 255;
            uint8_t hue_shift = hue;

            auto testScriptURL = datapaths->get_scripts_filepath( scriptname );
            auto compiled = cc.compile_file( testScriptURL );

            return Machine::create_machine(
                       name, hue_shift, bpo, knownBPOs, defaultPeriod, move( compiled ) );
        }

        unique_ptr<Machine> make_test_machine( const char* name,
                                               const char* scriptname,
                                               shared_ptr<datapaths> datapaths,
                                               const compiler& cc,
                                               const machineparts& parts,
                                               unsigned int defaultPeriod )
        {
            ss_dbg( "TEMP: make_test_machine..." );

            // TODO REMOVE THIS make_test_machine

            typedef map<AttachPoint, blueprint::Oriented_hardpoint> hpmap_t;

            map<Thruster_attachpoint, Thruster> thrusters;
            map<AttachPoint, Softpoint> softpoints;
            hpmap_t hardpoints;

            const size_t numSoftpoints = parts.softpoint_count();
            size_t softpointIndex = 0;

            const size_t numHardpoints = parts.hardpoint_count();
            size_t hardpointIndex = 0;

            // Add up to four hardpoints only.
            for( size_t i = 0; i < min<size_t>( numHardpoints, 4u ); ++i )
            {
                // Inserts at 1, 3, 5, 7.
                const AttachPoint ap( static_cast<uint16_t>( 1 + 2 * i ) );
                blueprint::Oriented_hardpoint hp(
                    blueprint::Rotated270,
                    parts.get_hardpoint( hardpointIndex++ % numHardpoints ) );
                hardpoints.insert( hpmap_t::value_type( ap, hp ) );
            }

            // Fill remaining of 1, 3, 5, 7 with softpoints:
            for( size_t i = hardpointIndex; i < 4; ++i )
            {
                // Inserts at 1, 3, 5, 7.
                const AttachPoint ap( static_cast<uint16_t>( 1 + 2 * i ) );
                softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                       ap, parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            }

            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 20 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 22 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 24 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 26 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 28 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );

            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 40 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 42 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 44 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 46 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 48 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );

            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 60 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 62 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 64 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 66 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );
            softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                   AttachPoint( 68 ), parts.get_softpoint( softpointIndex++ % numSoftpoints ) ) );

            if( parts.thruster_count() > 0 )
            {
                const size_t numThrusters = parts.thruster_count();
                size_t thrusterIndex = 0;
                thrusters.insert( map<Thruster_attachpoint, Thruster>::value_type(
                                      Thruster_attachpoint( 81 ),
                                      parts.get_thruster( thrusterIndex++ % numThrusters ) ) );
                thrusters.insert( map<Thruster_attachpoint, Thruster>::value_type(
                                      Thruster_attachpoint( 83 ),
                                      parts.get_thruster( thrusterIndex++ % numThrusters ) ) );
                thrusters.insert( map<Thruster_attachpoint, Thruster>::value_type(
                                      Thruster_attachpoint( 85 ),
                                      parts.get_thruster( thrusterIndex++ % numThrusters ) ) );
                thrusters.insert( map<Thruster_attachpoint, Thruster>::value_type(
                                      Thruster_attachpoint( 87 ),
                                      parts.get_thruster( thrusterIndex++ % numThrusters ) ) );
            }
            else
            {
                // FAKED THRUSTER MODEL
                softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                       AttachPoint( 81 ), parts.get_softpoint( numSoftpoints ) ) );
                softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                       AttachPoint( 83 ), parts.get_softpoint( numSoftpoints ) ) );
                softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                       AttachPoint( 85 ), parts.get_softpoint( numSoftpoints ) ) );
                softpoints.insert( map<AttachPoint, Softpoint>::value_type(
                                       AttachPoint( 87 ), parts.get_softpoint( numSoftpoints ) ) );
            }

            shared_ptr<blueprint> bpo = make_shared<blueprint>( parts.get_chassis( 0 ), thrusters, softpoints, hardpoints );
            vector<shared_ptr<blueprint>> knownBPOs;

            static uint8_t hue = 0;
            hue += 2;
            hue %= 255;
            uint8_t hue_shift = hue;

            auto testScriptURL = datapaths->get_scripts_filepath( scriptname );
            auto compiled = cc.compile_file( testScriptURL );

            return Machine::create_machine(
                       name, hue_shift, bpo, knownBPOs, defaultPeriod, move( compiled ) );
        }
    }

    shared_ptr<game> game::make_shared_game(
        const solosnake::filepath& xmlScreenFile,
        const shared_ptr<solosnake::window>& wnd,
        const shared_ptr<user_settings>& userSettings,
        const shared_ptr<datapaths>& datapaths,
        const shared_ptr<ilanguagetext>& language,
        const shared_ptr<luaguicommandexecutor>& lce )
    {
        auto t = make_timer();

        auto g = make_shared<game>( xmlScreenFile, wnd, userSettings, datapaths, language, lce, HeapOnly() );

        wnd->add_observer( static_pointer_cast<windowobserver>( g ) );

        ss_log( "'game' ctor took \t", t->delta() );

        return g;
    }

    game::game(
        const solosnake::filepath& xmlScreenFile,
        const std::shared_ptr<solosnake::window>& wnd,
        const shared_ptr<user_settings>& userSettings,
        const shared_ptr<datapaths>& datapaths,
        const shared_ptr<ilanguagetext>& language,
        const shared_ptr<luaguicommandexecutor>& lce,
        const HeapOnly& )
        : userSettings_( userSettings )
        , datapaths_( datapaths )
        , language_( language )
        , lce_( lce )
        , renderFunc_( nullptr )
        , sound_renderer_( make_sound_system( userSettings, datapaths ) )
        , renderer_( make_rendering_system( wnd, userSettings, datapaths ) )
        , guiViews_( make_default_game_screenviews() )
        , unpaused_frame_duration_( 0u )
        , not_paused_( true )
        , xmlScreenFile_( xmlScreenFile )
        , fullyLoaded_( false )
    {
        const rect wndRect = wnd->get_window_rect();
        set_screensize( dimension2d<unsigned int>( wndRect.width(), wndRect.height() ) );

        renderFunc_ = &game::render_without_loaded_game;

        // r->renderer().set_debug_rendermode( deferred_renderer::ShowMipMapLevels
        // );
        // r->renderer().set_debug_rendermode(
        // deferred_renderer::DiffuseTexRenderMode );
        // r->renderer().set_debug_rendermode(
        // deferred_renderer::NormalTexRenderMode );
        // r->renderer().set_debug_rendermode(
        // deferred_renderer::LightingNormalsRenderMode );
        // r->renderer().set_debug_rendermode( deferred_renderer::SceneRenderMode );
        // r->renderer().set_debug_rendermode(
        // deferred_renderer::NoDiffuseRenderMode );
        // r->renderer().set_debug_rendermode(
        // deferred_renderer::NoSpecularRenderMode );
        // r->renderer().set_debug_rendermode(
        // deferred_renderer::LightingNormalsRenderMode );
    }

    game::~game()
    {
    }

    //! Shorthand to returning the shared board game state.
    const boardstate* game::get_boardstate() const
    {
        return sharedgamestate_->boardgamestate();
    }

    //! Shorthand to returning the shared board game state hex grid.
    const Game_hex_grid& game::get_gamehexgrid() const
    {
        return sharedgamestate_->boardgamestate()->hexboard();
    }

    //! Creates the internal model from the args and the paths and user settings,
    //! creates the views, removes the default first view,
    //! and shares the model with the new views.
    bool game::try_load_game( const shared_ptr<gamestartupargs>& args )
    {
        // NOTE: It is important this function is exception safe. It
        //       must either work, or fail without making any changes.

        if( fullyLoaded_ )
        {
            ss_err( "load_game called more than once on same 'game' object." );
            ss_throw( "load_game called more than once on same 'game' object." );
        }

        try
        {
            // TODO: DATA DRIVE THIS:
#pragma message(__FILE__ "(718): Warning: try_load_game is not data driven.")
            //auto boardfilePath = datapaths_->get_boards_filepath( userSettings_->value( BLUE_GAME_FILE ) );
            solosnake::filepath boardfilePath;

            // Create game components
            auto localPlayer = make_localplayer();
            auto deckname    = get_selected_decks_name( *userSettings_ );
            auto playersdeck = load_selected_deck( deckname, localPlayer, datapaths_ );
            auto cc          = make_shared<compiler>();
            auto parts       = make_shared<machineparts>( *datapaths_ );
            auto pending     = make_pendinginstructions();
            auto boardstate  = make_test_boardstate( make_irand( args->random_seed() ),
                                                     args,
                                                     userSettings_,
                                                     boardfilePath );

            // Components that depend on or use the renderer.
            auto widgetstyles = make_widgetrendingstyles( renderer_, userSettings_, datapaths_ );
            auto widgets      = make_gamebackground_widgetsfactory( lce_,
                                                                    widgetstyles,
                                                                    language_,
                                                                    this->shared_from_this(),
                                                                    playersdeck,
                                                                    pending,
                                                                    cc,
                                                                    datapaths_ );

            // Load skybox: eventually this string will come from whatever
            // data struct describes the board etc.
            auto sb = renderer_->cache().get_skybox( SS_SKYBOXNAME );
            renderer_->renderer().load_skybox( *sb );
            renderer_->renderer().enable_skybox( true );

            // Create array of local players (typically local AI and user).
            vector<shared_ptr<player>> localPlayers;
            localPlayers.push_back( localPlayer );

            auto wndSize = renderer_->get_rendering_window_ptr()->get_window_dimensions();

            unsigned int defaultPeriod = userSettings_->value( BLUE_GAME_PULSE_PERIOD ).as_uint();

            auto machinemaker = make_shared<testmachinemaker>( datapaths_, cc, parts, defaultPeriod );

            auto addkillbot
                = [&]( const char* scriptname, Hex_coord xy, HexFacingName f, const unsigned int botId )
            {
                char name[4] = { 'a', 'a', 'a', 0 };
                const char zaRange = 'z' - 'a';
                name[2] += ( botId % ( 1 + 'z' - 'a' ) );
                name[1] += ( ( botId / ( zaRange ) ) % ( 1 + zaRange ) );
                name[0] += ( ( botId / ( zaRange * zaRange ) ) % ( 1 + zaRange ) );

                auto bot = machinemaker->make_machine( name,
                                                       scriptname,
                                                       "laser",
                                                       "laser",
                                                       "laser",
                                                       "thruster1",
                                                       "thruster1",
                                                       "thruster1",
                                                       "thruster1",
                                                       "cpu",
                                                       "capacitor",
                                                       "capacitor",
                                                       "comms",
                                                       "electronics",
                                                       "generic01",
                                                       "generic02",
                                                       "generic03",
                                                       "radar",
                                                       "radardome",
                                                       "reactor",
                                                       nullptr );

                fill_cargo_with_building_materials( *bot );
                boardstate->add_machine_to_board( xy, f, move( bot ) );
            };

            auto addsmallbot
                = [&]( const char* scriptname, Hex_coord xy, HexFacingName f, const unsigned int botId )
            {
                char name[3] = { 'a', 'a', 0 };
                name[1] += ( botId % ( 1 + 'z' - 'a' ) );
                name[0] += ( ( botId / ( 'z' - 'a' ) ) % ( 1 + 'z' - 'a' ) );
                unique_ptr<Machine> machine1( make_small_test_machine( name, scriptname, datapaths_, *cc, *parts, defaultPeriod ) );
                fill_cargo_with_building_materials( *machine1 );
                boardstate->add_machine_to_board( xy, f, move( machine1 ) );
            };

            // Machines etc are added now, AFTER creation. This ensures
            // all interested parties get all information.

            // TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE
            {
                unsigned int botnumber = 0;

                // Add central test bot:
                const Hex_coord centreXY =
                {
                    static_cast<int8_t>( boardstate->grid_width() / 2u ),
                    static_cast<int8_t>( boardstate->grid_height() / 2u )
                };

                // This bot will buy materials from nearby bot and build
                // and supply code for the next bot.
                auto daire = machinemaker->make_machine( "DAIRE",
                                                         "daire.txt",
                                                         "laser",
                                                         "laser",
                                                         "laser",
                                                         "thruster1",
                                                         "thruster1",
                                                         "thruster1",
                                                         "thruster1",
                                                         "cpu",
                                                         "capacitor",
                                                         "capacitor",
                                                         "comms",
                                                         "electronics",
                                                         "generic01",
                                                         "generic02",
                                                         "generic03",
                                                         "radar",
                                                         "radardome",
                                                         "reactor",
                                                         nullptr );

                daire->give_credits( 0xFFFF );

                boardstate->add_machine_to_board( centreXY, FacingTile3, move( daire ) );


                // This bot will sell materials but initially has no stock.
                auto shop = machinemaker->make_machine( "SHOP",
                                                        "shop.txt",
                                                        "laser",
                                                        "laser",
                                                        "laser",
                                                        "thruster1",
                                                        "thruster1",
                                                        "thruster1",
                                                        "thruster1",
                                                        "cpu",
                                                        "capacitor",
                                                        "capacitor",
                                                        "comms",
                                                        "electronics",
                                                        "generic01",
                                                        "generic02",
                                                        "generic03",
                                                        "radar",
                                                        "radardome",
                                                        "reactor",
                                                        nullptr );

                const Hex_coord nearbyXY2 = hexgrid::calculate_neighbour_coord( centreXY, FacingTile4, 1 );

                boardstate->add_machine_to_board( nearbyXY2, FacingTile3, move( shop ) );

                // This bot will mine materials from nearby both and build
                // and supply code for the next bot.
                auto miner = machinemaker->make_machine( "MINER",
                                                         "shopminer.txt",
                                                         "miner",
                                                         "miner",
                                                         "miner",
                                                         "thruster1",
                                                         "thruster1",
                                                         "thruster1",
                                                         "thruster1",
                                                         "cpu",
                                                         "capacitor",
                                                         "capacitor",
                                                         "comms",
                                                         "electronics",
                                                         "generic01",
                                                         "generic02",
                                                         "generic03",
                                                         "radar",
                                                         "radardome",
                                                         "reactor",
                                                         nullptr );


                const Hex_coord nearbyXY3 = hexgrid::calculate_neighbour_coord( nearbyXY2, FacingTile4, 1 );

                boardstate->add_machine_to_board( nearbyXY3, FacingTile3, move( miner ) );

                // Add rows of bots.
                const unsigned int botRowDepth = SS_STRESSBOT_COUNT;

                // Add left rows of bots:
                for( unsigned int y = 0; y < botRowDepth; ++y )
                {
                    for( unsigned int x = 0; x < boardstate->grid_width(); ++x )
                    {
                        Hex_coord botXY = Hex_coord::make_coord( static_cast<int8_t>( x ),
                                                               static_cast<int8_t>( y ) );
#if SS_STRESS_BIGBOTS
                        addkillbot( SS_STRESSBOT_SCRIPT, botXY, FacingTile3, botnumber++ );
#else
                        addsmallbot( SS_STRESSBOT_SCRIPT, botXY, FacingTile3, botnumber++ );
#endif
                    }
                }

                // Add right rows of bots:
                for( unsigned int y = boardstate->grid_height() - botRowDepth;
                        y < boardstate->grid_height();
                        ++y )
                {
                    for( unsigned int x = 0; x < boardstate->grid_width(); ++x )
                    {
                        Hex_coord botXY = { static_cast<int8_t>( x ), static_cast<int8_t>( y ) };
#if SS_STRESS_BIGBOTS
                        addkillbot( SS_STRESSBOT_SCRIPT, botXY, FacingTile3, botnumber++ );
#else
                        addsmallbot( SS_STRESSBOT_SCRIPT, botXY, FacingTile3, botnumber++ );
#endif
                    }
                }
            }
            // TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE

            // Now we have all the game state ready. Expose it to Lua
            expose_boardstate_to_lua( *boardstate, lce_ );
            expose_soundrenderer_to_lua( sound_renderer_.get(), lce_->lua() );
            expose_renderer_to_lua( renderer_->get_renderer_ptr().get(), lce_->lua() );

            // Now create the views containing the GUIs. The game state is exposed
            // to the Lua which the widgets will have access too.
            auto guiViews = xmlscreenviews::make_shared( xmlScreenFile_, wndSize, widgets );

            // Create good viewing matrix.
            auto cam = create_tiltcamera( boardstate->grid(), *userSettings_ );
            camera_  = make_unique<boardcamera>( boardstate->grid(), *userSettings_, std::move( cam ), renderer_ );

            const solosnake::bgra boardSunTintColour( 187, 153, 221, 164 );

            const solosnake::bgra boardTileColours[3] =
            {
                solosnake::bgra( 187, 153, 221, static_cast<uint8_t>( SS_BOARDALPHA * 255.0 ) ),
                solosnake::bgra( 221, 153, 187, static_cast<uint8_t>( SS_BOARDALPHA * 255.0 ) ),
                solosnake::bgra( 221, 187, 153, static_cast<uint8_t>( SS_BOARDALPHA * 255.0 ) )
            };

            const solosnake::bgr ambientbgr( static_cast<uint8_t>( SS_BOARDLIT * 255.0 ) );

            // Create emptygame and emptygame rendering: note move of boardstate here!
            auto sharedmodel   = make_gamestate( move( boardstate ), userSettings_ );

            auto boardrenderer = make_drawboardstate( sharedmodel,
                                                      renderer_,
                                                      sound_renderer_,
                                                      userSettings_,
                                                      datapaths_,
                                                      boardSunTintColour,
                                                      boardTileColours,
                                                      ambientbgr );

            // All done, safe to assign to members.
            local_players_.swap( localPlayers );
            boardRenderer_.swap( boardrenderer );
            sharedgamestate_.swap( sharedmodel );
            guiViews_.swap( guiViews );
            pendinginstructions_.swap( pending );
            fullyLoaded_ = true;

            // Change rendering function pointer:
            renderFunc_ = &game::render_with_loaded_game;

            // WIREFRAME
            // renderer_->renderer().set_debug_rendermode(
            //  deferred_renderer::WireFrameRenderMode );

            ss_log( sharedgamestate_->boardgamestate()->machines_count(), " bots on board." );

            return true;
        }
        catch( const SS_EXCEPTION_TYPE& e )
        {
            ss_err( e.what() );
            // assert( !"Exception while trying to load game." );
        }

        return false;
    }

    //! Main game frame update. Passes on the translated game commands (actions)
    //! Incoming messages are translates to gameaction(s) and stored in
    //! an array which is passed to the shared game state at the end of the
    //! current frame.
    void game::process_gameactions_message( const network::byte* msgs, const size_t msgSize )
    {
        assert( msgs );
        assert( msgSize > 0 );

        translate_message_to_gameactions( msgs, msgSize, server_gameactions_cache_ );
    }

    void game::advance_one_frame()
    {
        sharedgamestate_->advance_one_frame( server_gameactions_cache_ );
        server_gameactions_cache_.clear();
        guiViews_->advance_one_frame();
        unpaused_frame_duration_ = 0u;
    }

    void game::get_frame_message( network::bytebuffer& msg )
    {
        // Accumulate all the local actions (from players local to this client
        // and AI local to this client).
        accumulate_local_game_actions();
        translate_gameactions_to_message( local_gameactions_cache_, msg );
        local_gameactions_cache_.clear();
    }

    //! Stores the user's game actions for this frame.
    void game::accumulate_local_game_actions()
    {
        for( size_t i = 0; i < local_players_.size(); ++i )
        {
            const auto playerActions = local_players_[i]->take_actions();
            local_gameactions_cache_.reserve( local_gameactions_cache_.size() + playerActions.size() );

            // Add all of this players actions:
            for( size_t a = 0; a < playerActions.size(); ++a )
            {
                local_gameactions_cache_.push_back( playerActions[a] );
            }
        }
    }

    //! Stops or continues the game loop.
    solosnake::LoopResult game::handle_inputs( const solosnake::iinput_events& e )
    {
        return guiViews_->handle_inputs( e );
    }

    //! Handles events generated by the game window.
    void game::on_event( const windowchange& e, const window& w )
    {
        switch( e.event_type() )
        {
            case windowchange::FullscreenChange:
                handle_fullscreen_changed( *e.fullscreenstate(), w );
                break;

            case windowchange::MinimisedChange:
            case windowchange::UnminimisedChange:
                handle_minimised_changed( e.minimisedstate(), w );
                break;

            case windowchange::ResizeChange:
                handle_screensize_changed( *e.resized_dimensions(), w );
                break;

            default:
                break;
        }
    }

    void game::handle_screensize_changed( const solosnake::dimension2d<unsigned int>& r, const window& )
    {
        set_screensize( r );
    }

    void game::handle_fullscreen_changed( const FullscreenState&, const window& )
    {
    }

    void game::handle_minimised_changed( const MinimisedState&, const window& )
    {
    }

    // Informs ALL VIEWS of the screen size changes. Changes renderer viewport.
    void game::set_screensize( const solosnake::dimension2d<unsigned int>& r )
    {
        renderer_->renderer().set_viewport( 0, 0, r.width(), r.height() );
        guiViews_->handle_screensize_changed( r );
    }

    pickresult game::get_boardpick( solosnake::screenxy xy ) const
    {
        return camera_->get_boardpick( xy, get_gamehexgrid().grid() );
    }

    solosnake::point3d game::get_boardplanepick( solosnake::screenxy xy ) const
    {
        return camera_->get_boardplanepick( xy );
    }

    void game::move_camera_forwards( float units )
    {
        camera_->move_towards_lookat( units );
    }

    solosnake::point3d game::camera_location() const
    {
        return camera_->location();
    }

    solosnake::point3d game::camera_lookedat() const
    {
        return camera_->lookedat();
    }

    void game::translate_camera( float x, float z )
    {
        camera_->translate( x, z );
    }

    void game::rotate_camera( solosnake::radians angle )
    {
        camera_->rotate( angle );
    }

    // Returns the screen that the game moves to when exiting from this screen.
    // This is the name of the XML file which dictates the screen to load.
    solosnake::nextscreen game::get_next_screen() const
    {
        return guiViews_->get_next_screen();
    }

    // Informs the game to begin the internal game playing.
    void game::begin_playing()
    {
    }

    // Informs the game that the internal client considers the game to be ended
    // or possibly knows / acknowledges the game has ended.
    void game::end_playing()
    {
    }

    void game::move_listener_to_camera_location()
    {
        // Set listener position.
        float up[3];
        float side[3];
        float forwards[3];
        camera_->get_orientation( up, side, forwards );
        auto location = camera_->location();
        sound_renderer_->set_listener_orientation( location, up, forwards );
    }

    //! Called as frequently as the server can manage, this is used to tick
    //! forward things like input etc.
    void game::update_frame( unsigned int ms )
    {
        move_listener_to_camera_location();

        sound_renderer_->update();

        if( not_paused_ )
        {
            unpaused_frame_duration_ += ms;
        }
    }

    void game::instructions_sent_highlight( Hex_coord )
    {
    }

    const Machine* game::get_machine_at( Hex_coord xy ) const
    {
        return get_boardstate()->machine_at_coord( xy );
    }

    void game::render_without_loaded_game( const unsigned long dtMs ) const
    {
        deferred_renderer* const prenderer = renderer_->get_renderer_ptr().get();

        prenderer->start_scene();
        guiViews_->render_active_view( dtMs );
        prenderer->end_scene();
        renderer_->swap_buffers();
    }

    void game::render_with_loaded_game( const unsigned long dtMs ) const
    {
        assert( fullyLoaded_ );

        drawgame* const pboardrenderer = boardRenderer_.get();

        // TODO - THE VIEW SHOULD DRAW THE BOARD.
        camera_->setup_scene_view_and_proj_matrices();
        pboardrenderer->start_scene();
        pboardrenderer->draw_boardstate( not_paused_, dtMs, unpaused_frame_duration_ / BLUE_FRAME_TICK_DURACTION_MS_F );
        guiViews_->render_active_view( dtMs );
        pboardrenderer->end_scene();
        renderer_->swap_buffers();
    }
}
