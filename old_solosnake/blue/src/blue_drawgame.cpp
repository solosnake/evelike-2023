#include <algorithm>
#include <iomanip>
#include <sstream>
#include <vector>
#include "solosnake/filepath.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/point.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/unreachable.hpp"
#include "solosnake/blue/blue_asteroid.hpp"
#include "solosnake/blue/blue_blueprint.hpp"
#include "solosnake/blue/blue_datapathfinder.hpp"
#include "solosnake/blue/blue_drawasteroid.hpp"
#include "solosnake/blue/blue_drawgame.hpp"
#include "solosnake/blue/blue_drawgameboard.hpp"
#include "solosnake/blue/blue_drawmachine.hpp"
#include "solosnake/blue/blue_drawsun.hpp"
#include "solosnake/blue/blue_datapathfinder.hpp"
#include "solosnake/blue/blue_gamestate.hpp"
#include "solosnake/blue/blue_gamesfx.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_machine.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_settings_objects.hpp"
#include "solosnake/blue/blue_secondaryevent.hpp"
#include "solosnake/blue/blue_sun.hpp"
#include "solosnake/blue/blue_thruster.hpp"
#include "solosnake/blue/blue_user_settings.hpp"

using namespace std;
using namespace solosnake;

#define BLUE_EXPLOSION_BOARD_Y  (BLUE_BOARD_Y + BLUE_ATTACHPOINT_SCALE)
#define SCENE_LIGHT_STRENGTH    (0.725f)

namespace blue
{
    namespace
    {
        //-------------------------------------------------------------------------
        // ERROR CATEGORISATION
        //-------------------------------------------------------------------------

        const char* as_error_message( uint8_t ev )
        {
            switch( ev )
            {
                case Machine::ProgramRaisedError:
                    return "An error instruction was encountered during execution.";

                case Machine::ExpectedValueNotFoundAtLine:
                    return "A line was expected to contain a Value type but it did not.";

                case Machine::ExpectedLocationNotFoundAtLine:
                    return "A line was expected to contain a Location type but it did not.";

                case Machine::AssertionFailed:
                    return "Assertion Failed.";

                case Machine::UnknownInstruction:
                    return "An unknown instruction was encountered.";

                default:
                    ss_unreachable;
            }

            return "";
        }

        float distance_between_two_tiles()
        {
            Hex_coord a = Hex_coord::make_coord( 0, 0 );
            Hex_coord b = Hex_coord::make_coord( 0, 1 );
            float axy[2];
            float bxy[2];
            hexgrid::calculate_xy( a, axy );
            hexgrid::calculate_xy( b, bxy );
            return distance2( axy, bxy );
        }
    }

    drawgame::drawgame( const user_settings& settings,
                        const shared_ptr<rendering_system>& rndrSystem,
                        const shared_ptr<ifilefinder>& sunfinder,
                        const HeapOnly& )
        : rendering_( rndrSystem )
        , scene_sun_types_( settings, *sunfinder, *rndrSystem->get_cache_ptr() )
        , prenderer_( rndrSystem->get_renderer_ptr().get() )
        , distance_between_two_tiles_( distance_between_two_tiles() )
    {
        skyboxRotationRates_[0] = settings[BLUE_GAME_SKYBOXROTATEX].as_float();
        skyboxRotationRates_[1] = settings[BLUE_GAME_SKYBOXROTATEY].as_float();

        asteroid_filenames_[PanguiteOre]  = settings[BLUE_GAME_PANGUITE_MODEL];
        asteroid_filenames_[KamaciteOre]  = settings[BLUE_GAME_KAMACITE_MODEL];
        asteroid_filenames_[AtaxiteOre]   = settings[BLUE_GAME_ATAXITE_MODEL];
        asteroid_filenames_[ChondriteOre] = settings[BLUE_GAME_CHONDRITE_MODEL];

        bp_models_loaded_.reserve( 32 );
    }

    drawgame::~drawgame()
    {
        if( drawboard_ )
        {
            drawboard_->release_board_buffer();
        }
    }

    shared_ptr<drawgame> drawgame::make_shared_drawgame( const user_settings& settings,
                                                         const shared_ptr<gamestate>& gameState,
                                                         const shared_ptr<rendering_system>& rndrSystem,
                                                         const shared_ptr<sound_renderer>& sr,
                                                         const shared_ptr<datapaths>& paths,
                                                         const bgra sunColour,
                                                         const bgra tileColours[3],
                                                         const bgr ambient )
    {
        auto sunfinder = make_shared<datapathfinder>( paths, &datapaths::get_suns_filepath );
        auto dbs       = make_shared<drawgame>( settings, rndrSystem, sunfinder, HeapOnly() );

        dbs->ambient_rgb_[0] = ambient.red()   / 255.0f;
        dbs->ambient_rgb_[1] = ambient.green() / 255.0f;
        dbs->ambient_rgb_[2] = ambient.blue()  / 255.0f;

        dbs->sfx_ = make_gamesfx( rndrSystem, sr, settings, paths );

        const boardstate* board = gameState->boardgamestate();

        // Add existing machines to board.
        for( size_t i = 0; i < board->machines_count(); ++i )
        {
            dbs->add_machine_instance_to_board( *board->machine_at( i ) );
        }

        for( size_t i = 0; i < board->asteroids_count(); ++i )
        {
            dbs->add_asteroid_instance_to_board( *board->asteroid_at( i ) );
        }

        for( size_t i = 0; i < board->suns_count(); ++i )
        {
            dbs->add_sun_instance_to_board( *board->sun_at( i ) );
        }

        // Create drawboard now that board state is correct.
        auto convertToRGBA = []( bgra c ) -> dynbufferRGBA
        {
            dynbufferRGBA d = { c.red(), c.green(), c.blue(), c.alpha() };
            return d;
        };

        dbs->drawboard_ = make_unique<drawgameboard>( *rndrSystem,
                                                      settings,
                                                      convertToRGBA( sunColour ),
                                                      convertToRGBA( tileColours[0] ),
                                                      convertToRGBA( tileColours[1] ),
                                                      convertToRGBA( tileColours[2] ) );

        dbs->drawboard_->make_board_buffer( board->grid(), true );

        // Wait until after construction before attaching.
        dbs->gamestate_ = gameState;
        dbs->gamestate_.lock()->add_observer( static_pointer_cast<boardobserver>( dbs ) );

        return dbs;
    }

    rect drawgame::get_window_rect() const
    {
        return rendering_->get_window_rect();
    }

    void drawgame::set_view_matrix( const float* unaliased v )
    {
        prenderer_->set_view_matrix( v );
    }

    void drawgame::set_proj_matrix( const float* unaliased p, const float zNear )
    {
        prenderer_->set_proj_matrix( p, zNear );
    }

    void drawgame::start_scene()
    {
        prenderer_->set_ambiant_light( ambient_rgb_[0], ambient_rgb_[1], ambient_rgb_[2] );
        prenderer_->start_scene();
    }

    void drawgame::end_scene()
    {
        prenderer_->end_scene();
    }

    void drawgame::swap_buffers()
    {
        rendering_->swap_buffers();
    }

    size_t drawgame::get_drawmachine_index( const Machine* m ) const
    {
#ifndef NDEBUG
        auto i = lower_bound( drawable_machines_.cbegin(),
                              drawable_machines_.cend(),
                              m,
                              [&]( const unique_ptr<drawmachine>& d, const Machine * p )
        { return d->get_machine() < m; } );

        assert( i != drawable_machines_.end() );

        return distance( drawable_machines_.cbegin(), i );
#else
        return distance( drawable_machines_.cbegin(),
                         lower_bound( drawable_machines_.cbegin(),
                                      drawable_machines_.cend(),
                                      m,
                                      [&]( const unique_ptr<drawmachine>& d, const Machine * p )
        { return d->get_machine() < m; } ) );
#endif
    }

    drawmachine* drawgame::get_drawmachine( const Machine* m )
    {
#ifndef NDEBUG
        auto i = lower_bound( drawable_machines_.cbegin(),
                              drawable_machines_.cend(),
                              m,
                              [&]( const unique_ptr<drawmachine>& d, const Machine* )
        { return d->get_machine() < m; } );

        assert( i != drawable_machines_.end() );

        return i->get();
#else
        return lower_bound( drawable_machines_.cbegin(),
                            drawable_machines_.cend(),
                            m,
                            [&]( const unique_ptr<drawmachine>& d, const Machine* )
        { return d->get_machine() < m; } )->get();
#endif
    }

    void drawgame::cache_asteroid_model( OreTypes ore )
    {
        // After this the cache should be able to return model nodes
        // via their ore name. The asteroid file names are set in the
        // user settings and the asteroid files need to contain the correct
        // models by name.
        rendering_->get_cache_ptr()->load_models_file( asteroid_filenames_[ore] );
    }

    void drawgame::cache_blueprint_models( const std::shared_ptr<blueprint>& bp )
    {
        // Compact the array of weak pointers, removing any expired ones.
        auto newEnd = remove_if( bp_models_loaded_.begin(),
                                 bp_models_loaded_.end(),
                                 [&]( const weak_ptr<blueprint>& w )
        { return w.expired(); } );

        bp_models_loaded_.resize( distance( bp_models_loaded_.begin(), newEnd ) );

        const bool alreadyHasBP
            = binary_search( bp_models_loaded_.cbegin(),
                             bp_models_loaded_.cend(),
                             bp,
                             [&]( const std::weak_ptr<blueprint>& lhs, const weak_ptr<blueprint>& rhs )
        { return lhs.lock().get() < rhs.lock().get(); } );

        if( false == alreadyHasBP )
        {
            // Record that we have seen and loaded this blueprint's models.
            bp_models_loaded_.emplace_back( bp );

            sort( bp_models_loaded_.begin(),
                  bp_models_loaded_.end(),
                  [&]( const weak_ptr<blueprint>& lhs, const weak_ptr<blueprint>& rhs )
            { return lhs.lock().get() < rhs.lock().get(); } );

            rendering_->get_cache_ptr()->load_models_file( bp->blueprint_chassis().modelfile() );
        }
    }

    void drawgame::sort_machines()
    {
        // Sort by Machine address.
        sort( drawable_machines_.begin(),
              drawable_machines_.end(),
              []( const unique_ptr<drawmachine>& lhs, const unique_ptr<drawmachine>& rhs )
        { return lhs->get_machine() < rhs->get_machine(); } );
    }

    void drawgame::add_machine_instance_to_board( const Machine& m )
    {
        if( m.is_alive() )
        {
            // Ensure bp is in cache.
            cache_blueprint_models( m.machine_blueprint() );

            auto d = make_unique<drawmachine>( m, *rendering_->get_cache_ptr(), BLUE_BOARD_Y );
            drawable_machines_.push_back( move( d ) );

            sort_machines();
        }
        else
        {
            ss_wrn( "Dead Machine added to board." );
        }
    }

    void drawgame::add_asteroid_instance_to_board( const asteroid& a )
    {
        if( a.is_on_board() )
        {
            cache_asteroid_model( a.ore_type() );
            auto node = rendering_->get_cache_ptr()->instance_model( tradabletype_name( a.ore_type() ) );
            auto d = make_unique<drawasteroid>( a, move( node ) );
            drawable_asteroids_.push_back( move( d ) );
        }
    }

    void drawgame::add_sun_instance_to_board( const sun& s )
    {
        auto sunstuff = scene_sun_types_.get_sun_type_scene_sun( s.sun_type() );
        auto d = make_unique<drawsun>( s, sunstuff );
        drawable_suns_.push_back( move( d ) );
    }

    void drawgame::update_machine_instance_position( const Machine_event& e )
    {
        get_drawmachine( e.machine_ )->update_location( e.datum_.positionalupdate_ );
    }

    void drawgame::remove_machine( const Machine* m )
    {
        drawable_machines_.erase( drawable_machines_.begin() + get_drawmachine_index( m ) );
    }

    void drawgame::log_error( const Machine_event& e )
    {
        ss_log( as_error_message( e.datum_.error_.errorType ),
                " Line: ",
                e.datum_.error_.errorLine );

        std::cout << as_error_message( e.datum_.error_.errorType )
                  << " Line: "
                  << e.datum_.error_.errorLine
                  << std::endl;
    }

    void drawgame::log_assertion( const Machine_event& e )
    {
        char opTxt[3] = { 0 };
        ostringstream os;

        const uint16_t op1 = e.datum_.asserted_.op1;
        const uint16_t op2 = e.datum_.asserted_.op2;

        if( instruction::assert_op_to_str( e.datum_.asserted_.operand, opTxt ) )
        {
            os << "assert( " << op1 << ' ' << opTxt << ' ' << op2 << " )";
        }
        else
        {
            if( opTxt[0] )
            {
                os << "assert( " << opTxt << ' ' << op1 << " )";
            }
            else
            {
                os << "assert( 0 )";
            }
        }

        ss_log( os.str() );
        std::cout << os.str() << std::endl;
    }

    void drawgame::print_text( const Machine_event& e )
    {
        ss_log( e.datum_.text_.io_ );
        std::cout << e.datum_.text_.io_ << std::endl;
    }

    void drawgame::print_instruction( const Machine_event& e )
    {
        const instruction i = instruction::from_asm( e.datum_.instruction_.cmd_,
                                                     e.datum_.instruction_.args_[0],
                                                     e.datum_.instruction_.args_[1],
                                                     e.datum_.instruction_.args_[2] );

        ss_log( i );
        cout << i << std::endl;
    }

    void drawgame::handle_board_machineevent( const Machine_event& e )
    {
        switch( e.event_id() )
        {
            case EMachineAddedToBoard:
                add_machine_instance_to_board( *e.machine_ );
                break;

            case EMachinePositionalInfoUpdate:
                update_machine_instance_position( e );
                break;

            case EMachineChangeTile:
                break;

            case EMachineSensed:
                add_sensing_effect( e );
                break;

            case EMachineFired:
                break;

            case EMachineBroadcasted:
                break;

            case EMachineTransmitted:
                break;

            case EMachineInitiatedSelfDestruct:
                break;

            case EMachineExploded:
                remove_machine( e.machine_ );
                add_machine_explosion( e.datum_.machineexploded_ );
                break;

            case EMachineError:
                log_error( e );
                break;

            case EMachineAsserted:
                log_assertion( e );
                break;

            case EMachinePrintText:
                print_text( e );
                break;

            case EMachinePrintInstruction:
                print_instruction( e );
                break;

            case EMachineStartedBuild:
                break;

            case EMachineCompletedBuild:
                break;

            case EMachineTransferredCargo:
                break;

            case EMachineReceivedDamage:
                break;

            case EMachineReceivedRepairs:
                break;

#ifndef NDEBUG
            default:
                assert( !"Unhandled Machine_event type" );
                break;
#else
            default:
                ss_unreachable;
#endif
        }
    }

    void drawgame::handle_board_secondaryevent( const Secondary_event& e )
    {
        switch( e.event_id() )
        {
            // Shots
            case SEWeaponFiredAndHitMachine:
                handle_machine_weapon_fired_and_hit_machine_event( e );
                break;
            case SEWeaponFiredAndHitAsteroid:
                handle_machine_weapon_fired_and_hit_asteroid_event( e );
                break;
            case SEWeaponFiredAndMissed:
                handle_machine_weapon_fired_and_missed_event( e );
                break;

            // Cap drain
            case SECapDrainFiredAndHitMachine:
                handle_machine_capdrain_fired_and_hit_machine_event( e );
                break;
            case SECapDrainFiredAndHitAsteroid:
                handle_machine_capdrain_fired_and_hit_asteroid_event( e );
                break;
            case SECapDrainFiredAndMissed:
                handle_machine_capdrain_fired_and_missed_event( e );
                break;

            // Cap xfer
            case SECapXferFiredAndHitMachine:
                handle_machine_capxfer_fired_and_hit_machine_event( e );
                break;
            case SECapXferFiredAndHitAsteroid:
                handle_machine_capxfer_fired_and_hit_asteroid_event( e );
                break;
            case SECapXferFiredAndMissed:
                handle_machine_capxfer_fired_and_missed_event( e );
                break;

            // Armour repair
            case SEArmourRepFiredAndHitMachine:
                handle_machine_repper_fired_and_hit_machine_event( e );
                break;
            case SEArmourRepFiredAndHitAsteroid:
                handle_machine_repper_fired_and_hit_asteroid_event( e );
                break;
            case SEArmourRepFiredAndMissed:
                handle_machine_repper_fired_and_missed_event( e );
                break;

            // Mining
            case SEMiningFiredAndHitMachine:
                handle_machine_miner_fired_and_hit_machine_event( e );
                break;
            case SEMiningFiredAndHitAsteroid:
                handle_machine_miner_fired_and_hit_asteroid_event( e );
                break;
            case SEMiningFiredAndMissed:
                handle_machine_miner_fired_and_missed_event( e );
                break;

            // Asteroid
            case SEAsteroidDepleted:
                handle_asteroid_depleted_event( e );
                break;

            // Transfers
            case SECashTransferred:
                handle_cash_transferred_event( e );
                break;

#ifndef NDEBUG
            default:
                assert( !"Unhandled secondary type" );
                break;
#else
            default:
                ss_unreachable;
#endif
        }
    }

    // We will only be passed the subset of the events we requested.
    void drawgame::on_event( const boardevent& e, const boardeventcontext& c )
    {
        if( e == BoardMachineEvent )
        {
            handle_board_machineevent( *reinterpret_cast<const Machine_event*>( c ) );
        }
        else
        {
            handle_board_secondaryevent( *reinterpret_cast<const Secondary_event*>( c ) );
        }
    }

    void drawgame::draw_asteroid( drawasteroid& a, const bool advanceAnimations, const unsigned int dtMs )
    {
        if( advanceAnimations )
        {
            a.update_animations( dtMs );
        }

        float xz[2];
        hexgrid::calculate_xy( a.get_asteroid()->board_xy(), xz );

        a.draw_into( scene_ );
    }

    void drawgame::draw_sun( drawsun& s, const bool advanceAnimations, const unsigned int dtMs )
    {
        if( advanceAnimations )
        {
            s.update_animations( dtMs );
        }

        s.draw_into( prenderer_, distance_between_two_tiles_ );
    }

    void drawgame::draw_machine( drawmachine& m, const bool advanceAnimations, const float frameFraction )
    {
        const auto xz = m.update_animations( frameFraction );

        deferred_renderer::point_light light;
        light.worldX = xz[0];
        light.worldY = 0.5f;
        light.worldZ = xz[1];
        light.red    = 0.05f;
        light.green  = 0.05f;
        light.blue   = 0.05f;
        light.radius = 1.0f;

        prenderer_->draw_point_light( light );

        // TODO: Machines should add their own lights to the rendered scene.
        m.draw_into( scene_ );
    }

    void drawgame::rotate_skybox( const unsigned int dtMs )
    {
        skyboxRotations_[0] = radians( 0.001f * dtMs * skyboxRotationRates_[0] + skyboxRotations_[0].value() ).clamped();
        skyboxRotations_[1] = radians( 0.001f * dtMs * skyboxRotationRates_[1] + skyboxRotations_[0].value() ).clamped();
        float rot3x3[9];
        load_rotation_xy_3x3( skyboxRotations_[0], skyboxRotations_[1], rot3x3 );
        prenderer_->set_skybox_3x3_matrix( rot3x3 );
    }

    void drawgame::draw_boardstate( const bool advanceAnimations,
                                    const unsigned int dtMs,
                                    const float frameFraction )
    {
        if( advanceAnimations )
        {
            rotate_skybox( dtMs );
        }

        auto state = gamestate_.lock();
        const Game_hex_grid& g = state->boardgamestate()->hexboard();
        const rectf r = g.surface_rect();

        drawboard_->update_board_colours( g.grid() );
        drawboard_->render();

        // Ambient light has been already set in drawgame::start_scene.

        deferred_renderer::directional_light overheadlight;
        overheadlight.directionX =  1.0f;
        overheadlight.directionY = -2.0f;
        overheadlight.directionZ =  1.0f;
        overheadlight.red        =  SCENE_LIGHT_STRENGTH;
        overheadlight.blue       =  SCENE_LIGHT_STRENGTH;
        overheadlight.green      =  SCENE_LIGHT_STRENGTH;
        prenderer_->draw_directional_light( overheadlight );

        scene_.clear_instances();

        if( ! drawable_machines_.empty() )
        {
            size_t index = 0u;
            size_t size  = drawable_machines_.size();

            while( index != size )
            {
                drawmachine* const m = drawable_machines_[index].get();
                if( m->get_machine()->is_alive() )
                {
                    draw_machine( *m, advanceAnimations, frameFraction );
                    ++index;
                }
                else
                {
                    --size;
                    swap( drawable_machines_[index], drawable_machines_[size] );
                }
            }

            // Clear away dead machines.
            drawable_machines_.resize( size );
            sort_machines();
        }

        if( ! drawable_asteroids_.empty() )
        {
            size_t index = 0;
            size_t size  = drawable_asteroids_.size();

            while( index != size )
            {
                drawasteroid* a = drawable_asteroids_[index].get();
                if( a->get_asteroid()->is_on_board() )
                {
                    draw_asteroid( *a, advanceAnimations, dtMs );
                    ++index;
                }
                else
                {
                    --size;
                    swap( drawable_asteroids_[index], drawable_asteroids_[size] );
                }
            }

            // Clear away dead asteroids
            drawable_asteroids_.resize( size );
        }

        for( size_t i = 0u; i < drawable_suns_.size(); ++i )
        {
            draw_sun( *drawable_suns_[i], advanceAnimations, dtMs );
        }

        sfx_->update( advanceAnimations, dtMs );
        sfx_->draw_fx();

        scene_.render_scene( *prenderer_ );
        scene_.clear_instances();
    }

    //------------------------------- SFX -------------------------------------

    void drawgame::add_machine_explosion( const Machine_exploded& hx )
    {
        sfx_->add_machine_exploded_fx( BLUE_EXPLOSION_BOARD_Y, hx );
    }

    void drawgame::add_sensing_effect( const Machine_event& sense )
    {
        sfx_->add_sense_fx( BLUE_EXPLOSION_BOARD_Y, sense.datum_.sensed_ );
    }

    void drawgame::handle_machine_weapon_fired_and_hit_machine_event( const Secondary_event& event )
    {
        sfx_->add_machine_weapon_firing_fx( BLUE_EXPLOSION_BOARD_Y, event.datum_.shotfired_ );
    }

    void drawgame::handle_machine_weapon_fired_and_hit_asteroid_event( const Secondary_event& event )
    {
        ss_dbg( "MACHINE WEAPON FIRED AND HIT ASTEROID" );
    }

    void drawgame::handle_machine_weapon_fired_and_missed_event( const Secondary_event& event )
    {
        ss_dbg( "MACHINE WEAPON FIRED AND MISSED" );
    }

    void drawgame::handle_machine_capdrain_fired_and_hit_machine_event( const Secondary_event& event )
    {
        sfx_->add_machine_capdrain_firing_fx( BLUE_EXPLOSION_BOARD_Y, event.datum_.shotfired_ );
    }

    void drawgame::handle_machine_capdrain_fired_and_hit_asteroid_event( const Secondary_event& event )
    {
        ss_dbg( "MACHINE CAPDRAIN FIRED AND HIT ASTEROID" );
    }

    void drawgame::handle_machine_capdrain_fired_and_missed_event( const Secondary_event& event )
    {
        ss_dbg( "MACHINE CAPDRAIN FIRED AND MISSED" );
    }

    void drawgame::handle_machine_capxfer_fired_and_hit_machine_event( const Secondary_event& event )
    {
        sfx_->add_machine_capxfer_firing_fx( BLUE_EXPLOSION_BOARD_Y, event.datum_.shotfired_ );
    }

    void drawgame::handle_machine_capxfer_fired_and_hit_asteroid_event( const Secondary_event& event )
    {
        ss_dbg( "MACHINE CAPXFER FIRED AND HIT ASTEROID" );
    }

    void drawgame::handle_machine_capxfer_fired_and_missed_event( const Secondary_event& event )
    {
        ss_dbg( "MACHINE CAPXFER FIRED AND MISSED" );
    }

    void drawgame::handle_machine_repper_fired_and_hit_machine_event( const Secondary_event& event )
    {
        sfx_->add_machine_repper_firing_fx( BLUE_EXPLOSION_BOARD_Y, event.datum_.shotfired_ );
    }

    void drawgame::handle_machine_repper_fired_and_hit_asteroid_event( const Secondary_event& event )
    {
        ss_dbg( "MACHINE REPPER FIRED AND HIT ASTEROID" );
    }

    void drawgame::handle_machine_repper_fired_and_missed_event( const Secondary_event& event )
    {
        ss_dbg( "MACHINE REPPER FIRED AND MISSED" );
    }

    void drawgame::handle_machine_miner_fired_and_hit_machine_event( const Secondary_event& event )
    {
        ss_dbg( "MACHINE MINER FIRED AND HIT MACHINE" );
    }

    void drawgame::handle_machine_miner_fired_and_hit_asteroid_event( const Secondary_event& event )
    {
        sfx_->add_machine_miner_firing_fx( BLUE_EXPLOSION_BOARD_Y, event.datum_.shotfired_ );
    }

    void drawgame::handle_machine_miner_fired_and_missed_event( const Secondary_event& )
    {
        ss_dbg( "MACHINE MINER FIRED AND MISSED" );
    }

    void drawgame::handle_asteroid_depleted_event( const Secondary_event& e )
    {
        //sfx_->add_asteroid_depleted_fx( BLUE_EXPLOSION_BOARD_Y, e.datum_. );
    }

    void drawgame::handle_cash_transferred_event( const Secondary_event& e )
    {
        //sfx_->add_cash_transferred_fx( BLUE_EXPLOSION_BOARD_Y, event.datum_.shotfired_ );
    }
}