#include "solosnake/fontcache.hpp"
#include "solosnake/good_rand.hpp"
#include "solosnake/hexspacing.hpp"
#include "solosnake/img_cache.hpp"
#include "solosnake/image.hpp"
#include "solosnake/ioswindow.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/mesh_cache.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/skybox_cache.hpp"
#include "solosnake/sound_renderer.hpp"
#include "solosnake/styledwidgetrenderer.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/widgetrenderingstyles.hpp"
#include "solosnake/blue/blue_asteroid.hpp"
#include "solosnake/blue/blue_drawgame.hpp"
#include "solosnake/blue/blue_board_state.hpp"
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_datapathfinder.hpp"
#include "solosnake/blue/blue_drawboard.hpp"
#include "solosnake/blue/blue_gamestartupargs.hpp"
#include "solosnake/blue/blue_gamestate.hpp"
#include "solosnake/blue/blue_gamesfx.hpp"
#include "solosnake/blue/blue_gamebackground_widgetsfactory.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_log_events.hpp"
#include "solosnake/blue/blue_machine.hpp"
#include "solosnake/blue/blue_menuwidgetsfactory.hpp"
#include "solosnake/blue/blue_pendinginstructions.hpp"
#include "solosnake/blue/blue_player.hpp"
#include "solosnake/blue/blue_settings_objects.hpp"
#include "solosnake/blue/blue_sun.hpp"
#include "solosnake/blue/blue_suncache.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/blue/blue_settings.hpp"

using namespace std;
using namespace solosnake;

namespace blue
{
    namespace
    {
        //! Uses the value of the settings key BLUE_GAME_WIDGET_STYLEFILE to look up
        //! load and create a default widget rendering style. This is the style that
        //! will be used for rendering any widget which does not specify a style to use.
        //! Specified styles override the default, however a default is required by
        //! the engine.
        shared_ptr<iwidgetrenderer>
        make_default_widgetrenderingstyle(
            const shared_ptr<user_settings>& usersettings,
            const shared_ptr<rendering_system>& rndrSystem,
            const shared_ptr<datapaths>& paths )
        {
            const string stylename = usersettings->value( BLUE_GAME_WIDGET_STYLEFILE );

            auto url = paths->get_styles_filepath( stylename );

            auto wrr = styledwidgetrenderer::make_styledwidgetrenderer( url, rndrSystem );

            return wrr;
        }
    }

    std::shared_ptr<pendinginstructions> make_pendinginstructions()
    {
        return std::make_shared<pendinginstructions>();
    }

    std::shared_ptr<player> make_localplayer()
    {
        return std::make_shared<player>();
    }

    unique_ptr<irand> make_irand( unsigned int seed )
    {
        return make_good_rand(seed);
    }

    shared_ptr<rendering_system>
    make_rendering_system( const shared_ptr<window>& w,
                           const shared_ptr<user_settings>& usersettings,
                           const shared_ptr<datapaths>& paths )
    {
        auto meshfinder   = make_shared<datapathfinder>( paths, &datapaths::get_meshes_filepath );
        auto modelfinder  = make_shared<datapathfinder>( paths, &datapaths::get_models_filepath );
        auto fontfinder   = make_shared<datapathfinder>( paths, &datapaths::get_fonts_filepath );
        auto texfinder    = make_shared<datapathfinder>( paths, &datapaths::get_textures_filepath );
        auto skyboxfinder = make_shared<datapathfinder>( paths, &datapaths::get_skyboxes_filepath );
        auto sunfinder    = make_shared<datapathfinder>( paths, &datapaths::get_suns_filepath );

        auto mshcache     = make_shared<mesh_cache>( meshfinder, texfinder );
        auto imgcache     = make_shared<img_cache>( texfinder );
        auto skyboxcache  = make_shared<skybox_cache>( skyboxfinder, texfinder );

        // Extract settings:
        deferred_renderer::Quality quality = deferred_renderer::HighQuality;

        if( 0 == usersettings->value( UO_WINDOW_QUALITY ).str().compare( UO_WINDOW_QUALITY_HIGHEST ) )
        {
            quality = deferred_renderer::HighestQuality;
        }
        else if( 0 == usersettings->value( UO_WINDOW_QUALITY ).str().compare( UO_WINDOW_QUALITY_HIGH ) )
        {
            quality = deferred_renderer::HighQuality;
        }
        else if( 0 == usersettings->value( UO_WINDOW_QUALITY ).str().compare( UO_WINDOW_QUALITY_MEDIUM ) )
        {
            quality = deferred_renderer::MediumQuality;
        }
        else if( 0 == usersettings->value( UO_WINDOW_QUALITY ).str().compare( UO_WINDOW_QUALITY_LOW ) )
        {
            quality = deferred_renderer::LowQuality;
        }

        return rendering_system::create(
                   w, quality, modelfinder, fontfinder, mshcache, imgcache, skyboxcache );
    }

    shared_ptr<sound_renderer>
    make_sound_system( const shared_ptr<user_settings>& usersettings,
                       const shared_ptr<datapaths>& paths )
    {
        const float worldscale = 1.0f;

        auto soundfinder = make_shared<datapathfinder>( paths, &datapaths::get_sounds_filepath );

        // Extract settings:
        sound_renderer::FalloffModel model = sound_renderer::LinearFalloff;

        if( 0 == usersettings->value( UO_SOUND_MODEL ).str().compare( UO_SOUND_MODEL_INVERSE ) )
        {
            model = sound_renderer::InverseFalloff;
        }
        else if( 0 == usersettings->value( UO_SOUND_MODEL ).str().compare( UO_SOUND_MODEL_EXPONENTIAL ) )
        {
            model = sound_renderer::ExponentialFalloff;
        }

        size_t maxsounds = usersettings->value( UO_SOUND_MAX_SOUNDS ).as_uint();

        return make_shared<sound_renderer>( soundfinder, model, worldscale, maxsounds );
    }

    //! Does NOT set a default game file.
    shared_ptr<gamestartupargs>
    make_game_startup_args( const shared_ptr<user_settings>& usersettings )
    {
        return make_shared<gamestartupargs>( usersettings->value( BLUE_GAME_RANDOMSEED ).as_uint(),
                                             usersettings->value( BLUE_GAME_FILE ).str() );
    }

    //! Construct the board game state of play from the startup args, which may
    //! have been received over the network, and the local files and user
    //! settings.
    unique_ptr<boardstate>
    make_empty_boardstate( unique_ptr<solosnake::irand> rands,
                           const shared_ptr<gamestartupargs>&,
                           const shared_ptr<user_settings>&,
                           const filepath& gamefileurl )
    {
#pragma message(__FILE__ "(192): Warning: make_empty_boardstate is temporary.")
        /*
                if( gamefileurl.empty() )
                {
                    ss_err( "No gamefile specified." );
                    ss_throw( "Board file not found." );
                }

                if( !( std::filesystem::exists( gamefileurl )
                        && std::filesystem::is_regular_file( gamefileurl ) ) )
                {
                    ss_err( "Unable to find board file:", gamefileurl.string() );
                    ss_throw( "Board file not found." );
                }
        */

        return make_unique<boardstate>( move( rands ),
                                        Game_hex_grid( gamefileurl.string() ),
                                        vector<asteroid>(),
                                        vector<sun>() );
    }

    //! Construct the board game state of play from the startup args, which may
    //! have been received over the network, and the local files and user
    //! settings.
    unique_ptr<boardstate>
    make_test_boardstate( unique_ptr<solosnake::irand> rands,
                          const shared_ptr<gamestartupargs>&,
                          const shared_ptr<user_settings>&,
                          const filepath& gamefileurl )
    {

#pragma message(__FILE__ "(236): Warning: make_test_boardstate is temporary.")
        /*
                if( gamefileurl.empty() )
                {
                    ss_err( "No gamefile specified." );
                    ss_throw( "Board file not found." );
                }



                if( !( std::filesystem::exists( gamefileurl )
                        && std::filesystem::is_regular_file( gamefileurl ) ) )
                {
                    ss_err( "Unable to find board file:", gamefileurl.string() );
                    ss_throw( "Board file not found." );
                }
        */

        // TODO Fill in asteroids.
        vector<asteroid> asteroids;
        asteroids.push_back( asteroid( PanguiteOre,  500u, Hex_coord::make_coord( 10,  9 ) ) );
        asteroids.push_back( asteroid( KamaciteOre,  400u, Hex_coord::make_coord( 10, 10 ) ) );
        asteroids.push_back( asteroid( AtaxiteOre,   300u, Hex_coord::make_coord( 10, 11 ) ) );
        asteroids.push_back( asteroid( ChondriteOre, 200u, Hex_coord::make_coord( 10, 12 ) ) );


        asteroids.push_back( asteroid( PanguiteOre,  500u, Hex_coord::make_coord( 53, 53 ) ) );
        asteroids.push_back( asteroid( KamaciteOre,  400u, Hex_coord::make_coord( 52, 52 ) ) );
        asteroids.push_back( asteroid( AtaxiteOre,   300u, Hex_coord::make_coord( 52, 53 ) ) );
        asteroids.push_back( asteroid( ChondriteOre, 200u, Hex_coord::make_coord( 53, 52 ) ) );

        // TODO Sun file.
        vector<sun> suns;
        suns.push_back( sun( SunStrength1, SunType0, Hex_coord::make_coord( 5,  5 ) ) );
        suns.push_back( sun( SunStrength3, SunType1, Hex_coord::make_coord( 11, 10 ) ) );
        suns.push_back( sun( SunStrength5, SunType2, Hex_coord::make_coord( 13, 11 ) ) );
        suns.push_back( sun( SunStrength7, SunType3, Hex_coord::make_coord( 20, 20 ) ) );

        suns.push_back( sun( SunStrength7, SunType3, Hex_coord::make_coord( 55, 55 ) ) );

        return make_unique<boardstate>( move( rands ),
                                        Game_hex_grid( gamefileurl.string() ),
                                        move( asteroids ),
                                        move( suns ) );
    }


    unique_ptr<boardstate>
    make_picture_boardstate( const string& picture,
                             const shared_ptr<datapaths>& paths,
                             unique_ptr<irand> rands,
                             const shared_ptr<gamestartupargs>&,
                             const shared_ptr<user_settings>& usersettings )
    {
        auto pic = paths->get_boards_filepath( picture );

        solosnake::image mappic( pic );

        hexgrid hg( mappic.width(), mappic.height(), empty_tile_value(), offboard_tile_value() );

        const bgr red( 0, 0, 255 );       // Sun
        const bgr blue( 255, 0, 0 );      // Asteroid
        const bgr white( 255, 255, 255 ); // Tile
        const bgr black( 0, 0, 0 );       // Off board

        vector<asteroid> asteroids;
        vector<sun> suns;

        for( unsigned int y = 0; y < mappic.height(); ++y )
        {
            for( unsigned int x = 0; x < mappic.width(); ++x )
            {
                auto hx = Hex_coord::make_coord( x, y );

                solosnake::bgr pix( mappic.pixel_at( x, y ) );

                if( pix == red )
                {
                    suns.push_back( sun( SunStrength7, SunType0, hx ) );
                    hg.set_contents( hx, offboard_tile_value() );
                }
                else if( pix == blue )
                {
                    asteroids.push_back( asteroid( KamaciteOre,  400u, hx ) );
                    hg.set_contents( hx, empty_tile_value() );
                }
                else if( pix == white )
                {
                    hg.set_contents( hx, empty_tile_value() );
                }
                else if( pix == black )
                {
                    hg.set_contents( hx, offboard_tile_value() );
                }
                else
                {
                    ss_throw( "Unknown pixel colour on board." );
                }
            }
        }

        return make_unique<boardstate>( move( rands ),
                                        Game_hex_grid( std::move( hg ) ),
                                        move( asteroids ),
                                        move( suns ) );
    }

    //! Construct the board game state of play from the startup args, which may
    //! have been received over the network, and the local files and user
    //! settings.
    unique_ptr<boardstate>
    make_example_boardstate( const shared_ptr<datapaths>& paths,
                             unique_ptr<irand> rands,
                             const shared_ptr<gamestartupargs>& args,
                             const shared_ptr<user_settings>& usersettings,
                             const filepath& )
    {
        return make_picture_boardstate( "example.bmp", paths, move( rands ),
                                        args, usersettings );
    }

    //! Construct the board game state of play from the startup args, which may
    //! have been received over the network, and the local files and user
    //! settings.
    unique_ptr<boardstate>
    make_background_boardstate( const shared_ptr<datapaths>& paths,
                                unique_ptr<irand> rands,
                                const shared_ptr<gamestartupargs>& args,
                                const shared_ptr<user_settings>& usersettings,
                                const filepath& )
    {
        return make_picture_boardstate( "background.bmp", paths, move( rands ),
                                        args, usersettings );
    }

    //! The model-view-controllers have their own view of the board state, which
    //! included view specific info such as selection sets and rendering info.
    shared_ptr<gamestate>
    make_gamestate( unique_ptr<boardstate> board, const shared_ptr<user_settings>& )
    {
        return make_shared<gamestate>( move( board ) );
    }

    std::unique_ptr<gamesfx>
    make_gamesfx( const shared_ptr<solosnake::rendering_system>& r,
                  const shared_ptr<solosnake::sound_renderer>& sr,
                  const solosnake::persistance& settings,
                  const std::shared_ptr<datapaths>& )
    {
        return std::make_unique<gamesfx>( r, sr, settings );
    }

    shared_ptr<drawgame>
    make_drawboardstate( const shared_ptr<gamestate>& gameState,
                         const shared_ptr<solosnake::rendering_system>& rndrSystem,
                         const shared_ptr<solosnake::sound_renderer>& sr,
                         const shared_ptr<user_settings>& usersettings,
                         const shared_ptr<datapaths>& paths,
                         const solosnake::bgra sunColour,
                         const solosnake::bgra tilecolours[3],
                         const solosnake::bgr ambient )
    {
        return drawgame::make_shared_drawgame( *usersettings,
                                               gameState,
                                               rndrSystem,
                                               sr,
                                               paths,
                                               sunColour,
                                               tilecolours,
                                               ambient );
    }

    //! Creates a widget rendering style retrieval system object, and inserts into
    //! it the default blue widget renderer. Other widget rendering styles can be
    //! loaded from the data/styles directory during loading of widgets, which
    //! are allowed specify their the name of the rendering style to use via the
    //! 'style' attribute.
    shared_ptr<solosnake::widgetrenderingstyles>
    make_widgetrendingstyles( const shared_ptr<solosnake::rendering_system>& rndrSystem,
                              const shared_ptr<user_settings>& usersettings,
                              const shared_ptr<datapaths>& paths )
    {
        auto defaultStyle = make_default_widgetrenderingstyle( usersettings, rndrSystem, paths );

        auto stylefinder  = make_shared<datapathfinder>( paths, &datapaths::get_styles_filepath );

        return make_shared<solosnake::widgetrenderingstyles>( stylefinder, rndrSystem, defaultStyle );
    }

    //! This factory is incapable of making any widgets which interact with
    //! the game / board etc.
    shared_ptr<widgetsfactory>
    make_menu_widgetsfactory( const shared_ptr<user_settings>& usersettings,
                              const shared_ptr<luaguicommandexecutor>& gce,
                              const shared_ptr<widgetrenderingstyles>& rndrStyles,
                              const shared_ptr<ilanguagetext>& language )
    {
        return make_shared<menuwidgetsfactory>( usersettings, gce, rndrStyles, language );
    }

    //! This factory is makes all the standard menu widgets AND widgets which
    //! interact with the game / board etc.
    shared_ptr<widgetsfactory>
    make_gamebackground_widgetsfactory( const shared_ptr<luaguicommandexecutor>& lua,
                                        const shared_ptr<widgetrenderingstyles>& rndrStyles,
                                        const shared_ptr<ilanguagetext>& language,
                                        const weak_ptr<igameview>& gv,
                                        const shared_ptr<deck>& playersdeck,
                                        const shared_ptr<pendinginstructions>& pender,
                                        const shared_ptr<compiler>& compilers,
                                        const shared_ptr<datapaths>& paths )
    {
        // Add the hexagon function to lua here.
        solosnake::expose_hexspacing_function( lua->lua() );

        auto stylefinder  = make_shared<datapathfinder>( paths, &datapaths::get_styles_filepath );

        return make_shared<gamebackground_widgetsfactory>(
                   gv, lua, rndrStyles, stylefinder, language, playersdeck, pender, compilers );
    }
}
