#include <vector>
#include <algorithm>
#include <boost/filesystem/path.hpp>
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/blue/blue_boardstaterenderer.hpp"
#include "solosnake/blue/blue_drawboard.hpp"
#include "solosnake/blue/blue_datapathfinder.hpp"
#include "solosnake/blue/blue_gamestate.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_machine.hpp"
#include "solosnake/blue/blue_blueprint.hpp"

using namespace std;

namespace blue
{
    //! Heap only ctor.
    boardstaterenderer::boardstaterenderer( shared_ptr<drawboard> drawBoard,
                                            shared_ptr<solosnake::rendering_system> rndrSystem,
                                            shared_ptr<datapaths> paths,
                                            const HeapOnly& )
        : drawboard_( drawBoard )
        , rendering_( rndrSystem )
        , prenderer_( rndrSystem->get_renderer_ptr().get() )
    {
        bp_models_loaded_.reserve( 32 );
    }

    boardstaterenderer::~boardstaterenderer()
    {
    }

    boardstaterenderer_ptr boardstaterenderer::make_shared( shared_ptr<gamestate> gameState,
            shared_ptr<drawboard> drawBoard,
            shared_ptr
            <solosnake::rendering_system> rndrSystem,
            shared_ptr<datapaths> paths )
    {
        auto bsp = std::make_shared<boardstaterenderer>( drawBoard, rndrSystem, paths, HeapOnly() );

        // Wait until after construction before attaching.
        event::Flags_t flagsAboutMachines = event::EMachineAddedToBoard
                                            | event::EMachinePositionalInfoUpdate;
        bsp->gamestate_ = gameState;
        gameState->on_event( flagsAboutMachines ) += solosnake::handler
                <event>( &boardstaterenderer::on_event, std::weak_ptr<boardstaterenderer>( bsp ) );

        return bsp;
    }

    void boardstaterenderer::cache_blueprint_models( std::shared_ptr<blueprint> bp )
    {
        // Compact the array of weak pointers, removing any expired ones.
        auto newEnd = remove_if( bp_models_loaded_.begin(),
                                 bp_models_loaded_.end(),
                                 [&]( const weak_ptr<blueprint>& w )
        { return w.expired(); } );

        bp_models_loaded_.resize( distance( bp_models_loaded_.begin(), newEnd ) );

        const bool alreadyHasBP
            = binary_search( bp_models_loaded_.begin(),
                             bp_models_loaded_.end(),
                             bp,
                             [&]( const std::weak_ptr<blueprint>& lhs, const weak_ptr<blueprint>& rhs )
        { return lhs.lock().get() < rhs.lock().get(); } );

        if( false == alreadyHasBP )
        {
            // Record that we have seen and loaded this blueprint's models.
            bp_models_loaded_.push_back( bp );

            sort( bp_models_loaded_.begin(),
                  bp_models_loaded_.end(),
                  [&]( const std::weak_ptr<blueprint>& lhs, const weak_ptr<blueprint>& rhs )
            { return lhs.lock().get() < rhs.lock().get(); } );

            rendering_->get_cache_ptr()->load_models_file( bp->chassis_modelfile() );

            for( size_t i = 0; i < bp->occupied_softpoint_count(); ++i )
            {
                rendering_->get_cache_ptr()->load_models_file(
                    bp->occupied_softpoint( i ).softpoint_modelfile() );
            }

            for( size_t i = 0; i < bp->occupied_hardpoint_count(); ++i )
            {
                rendering_->get_cache_ptr()->load_models_file(
                    bp->occupied_hardpoint( i ).hardpoint_modelfile() );
            }
        }
    }

    void boardstaterenderer::add_machine_instance_to_board( const Machine& m )
    {
        auto machinebp = m.machine_blueprint();

        cache_blueprint_models( machinebp );

        auto instance = rendering_->get_cache_ptr()->instance_model( machinebp->chassis_model() );

        for( size_t i = 0; i < machinebp->occupied_softpoint_count(); ++i )
        {
            float localSoftpointOffsetXYZ[3] = { i * 2.0f, 2.0f, 0.0f };
            solosnake::matrix4x4_t localSoftpointTransform;
            solosnake::load_translation( localSoftpointOffsetXYZ, localSoftpointTransform );
            instance.add_modelinstance( rendering_->get_cache_ptr()->instance_model(
                                            machinebp->occupied_softpoint( i ).softpoint_model() ),
                                        localSoftpointTransform );
        }

        const float w = board_tilewidth();
        const float boardY = board_y();
        float worldXY[2];
        float offsetXY[2];
        m.position().offset_after_n_frames( 0.0f, offsetXY );
        hexgrid::calculate_scaled_world_coord( m.position().gridXY_, w, worldXY );
        solosnake::load_rotation_y( HALFPI_RADIANS + m.position().angle_as_radians(),
                                    instance.location() );
        solosnake::set_translation(
            w * offsetXY[0] + worldXY[0], boardY, w * offsetXY[1] + worldXY[1], instance.location() );

        instance[0].set_team_rgb( 0.0f, 0.0f, 1.0f );
        instance[0].set_emissive( 0.0f );

        // The Machine arrays are kept in synch.
        machineInstances_.push_back( instance );
        machines_.push_back( &m );
    }

    void boardstaterenderer::update_machine_instance_position( const event::positionalinfoupdate& pos )
    {
        const float w = board_tilewidth();
        const float boardY = board_y();

        for( size_t i = 0; i < machines_.size(); ++i )
        {
            if( machines_[i] == pos.machine_ )
            {
                float worldXY[2];
                float offsetXY[2];
                pos.machine_position_info_.offset_after_n_frames( 0.0f, offsetXY );
                hexgrid::calculate_scaled_world_coord( pos.machine_position_info_.gridXY_, w, worldXY );
                solosnake::load_rotation_y( HALFPI_RADIANS + pos.machine_position_info_.angle_as_radians(),
                                            machineInstances_[i].location() );
                solosnake::set_translation( w * offsetXY[0] + worldXY[0],
                                            boardY,
                                            w * offsetXY[1] + worldXY[1],
                                            machineInstances_[i].location() );

                if( pos.machine_->is_accelerating() )
                {
                    machineInstances_[i][0].set_emissive( 1.0f );
                }
                else
                {
                    machineInstances_[i][0].set_emissive( pos.machine_position_info_.dXYdt_ / static_cast
                                                          <float>( pos.machine_->max_speed() ) );
                    assert( machineInstances_[i][0].emissive() >= 0.0f );
                    assert( machineInstances_[i][0].emissive() <= 1.0f );
                }

                assert( machineInstances_[i][0].emissive() >= 0.0f && machineInstances_[i][0].emissive()
                        <= 1.0f );

                break;
            }
        }
    }

    // We will only be passed the subset of the events we requested.
    void boardstaterenderer::on_event( const event& e )
    {
        switch( e.event_type() )
        {
            case event::EMachineAddedToBoard:
                add_machine_instance_to_board( *e.datum_.machineaddedtoboard_.machine_ );
                break;

            case event::EMachinePositionalInfoUpdate:
                update_machine_instance_position( e.datum_.positionalinfoupdate_ );
                break;
        }
    }

    float boardstaterenderer::board_tilewidth() const
    {
        return drawboard_->board_tilewidth();
    }

    float boardstaterenderer::board_y() const
    {
        return drawboard_->board_y();
    }

    solosnake::rect boardstaterenderer::get_window_rect() const
    {
        return rendering_->get_window_rect();
    }

    void boardstaterenderer::set_view_matrix( const float* unaliased v )
    {
        prenderer_->set_view_matrix( v );
    }

    void boardstaterenderer::set_proj_matrix( const float* unaliased p, const float zNear )
    {
        prenderer_->set_proj_matrix( p, zNear );
    }

    void boardstaterenderer::start_scene()
    {
        prenderer_->set_ambiant_light( 0.2f, 0.2f, 0.2f );
        prenderer_->start_scene();
    }

    void boardstaterenderer::render_boardstate()
    {
        using namespace solosnake;

        drawboard_->render( *prenderer_ );

        {
            // TODO Proper board lighting etc.
            // ss_wrn( "TEMP BOARD RENDERING" );
            deferred_renderer::directional_light dirlight;
            dirlight.directionX = 1.0f;
            dirlight.directionY = -1.0f;
            dirlight.directionZ = 0.0f;
            dirlight.red = 0.1f;
            dirlight.blue = 0.1f;
            dirlight.green = 0.1f;

            prenderer_->add_directional_light( dirlight );

            deferred_renderer::point_light light;

            light.worldX = 4.0f;
            light.worldY = 4.0f;
            light.worldZ = 1.0f;
            light.red = 1.0f;
            light.green = 1.0f;
            light.blue = 1.0f;
            light.radius = 8.0f;

            prenderer_->add_point_light( light );

            if( !machineInstances_.empty() )
            {
                scene_.clear_instances();

                for_each( machineInstances_.begin(),
                          machineInstances_.end(),
                          [&]( const solosnake::modelinstance & m )
                { m.draw_into( scene_ ); } );

                scene_.render_scene( *prenderer_ );
                scene_.clear_instances();
            }
        }
    }

    void boardstaterenderer::end_scene()
    {
        prenderer_->end_scene();
    }

    void boardstaterenderer::swap_buffers()
    {
        rendering_->swap_buffers();
    }
}