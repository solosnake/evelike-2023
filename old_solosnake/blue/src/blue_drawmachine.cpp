#include <utility>
#include <cstdint>
#include "solosnake/blue/blue_blueprint.hpp"
#include "solosnake/blue/blue_drawmachine.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_machine.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_thruster.hpp"
#include "solosnake/renderer_cache.hpp"

using namespace solosnake;
using namespace std;

#define BLUE_ENGINEGLOW_FADE_FACTOR (0.95f)
#define BLUE_ENGINEGLOW_QUENCH_AT   (4)
#define BLUE_ENGINE_EMISSIVE_INDEX  (0)
#define BLUE_LIGHTS_EMISSIVE_INDEX  (1)

namespace blue
{
    drawmachine::drawmachine( const Machine& m, renderer_cache& cache, float boardY )
        : machine_( &m )
        , previous_position_( m.get_position_info() )
        , current_position_( m.get_position_info() )
        , position_interpolation_( 1.0f )
        , boardY_( boardY )
        , visiblemachine_()
    {
        load_identity_4x4( transform4x4_ );

        const blueprint& bp = * machine_->machine_blueprint();
        assert( bp.softpoint_count() > 0u );

        const point3d localOffset(0.0f, 0.0f, 0.0f);
        modelnode childnode = cache.instance_model( bp.blueprint_chassis().modelname() );
        childnode.translate(localOffset.xyz);

        visiblemachine_.node_     = std::move(childnode);
        visiblemachine_.channels_ = bp.blueprint_chassis().channels();
        visiblemachine_.node_.set_all_team_hue_shift( machine_->team_hue_shift() );
    }

    void drawmachine::orient_hardpoints()
    {
        const vector<Turning_hardpoint>& hps = machine_->turning_hardpoints();

#if 0
        for( size_t i = 0u; i < hardpoint_count_; ++i )
        {
            const size_t ci = thruster_count_ + i;

            if( components_[ci].node_.child_count() > 0 )
            {
                const auto a  = hps[i].current_angle_;
                const auto ar = angle_as_radians( a ).value();
                components_[ci].node_.child( 0 ).set_y_rotation( ar );
            }
        }
#endif
    }

    void drawmachine::update_emissive_colours()
    {
        bgra litengine;

        const float intensity1a = machine_->readout( visiblemachine_.emissive_intensity_src( 1u, 0u ) );
        const float intensity1b = machine_->readout( visiblemachine_.emissive_intensity_src( 1u, 1u ) );

        const bgra lights = visiblemachine_.emissive_colour( BLUE_LIGHTS_EMISSIVE_INDEX ) * (intensity1a * intensity1b);

        if( machine_->is_advancing() )
        {
            const float thrustlvl = machine_->readout( ReadoutAdvanceSpeedLvl );
            litengine = visiblemachine_.emissive_colour( BLUE_ENGINE_EMISSIVE_INDEX ) * thrustlvl;
        }
        else if( machine_->is_turning() )
        {
            const float turnLvl = machine_->readout( ReadoutTurnSpeedLvl );
            litengine = visiblemachine_.emissive_colour( BLUE_ENGINE_EMISSIVE_INDEX ) * turnLvl;
        }

        if( engine_glow_.non_zero() )
        {
            engine_glow_ *= BLUE_ENGINEGLOW_FADE_FACTOR;
            engine_glow_.quench( BLUE_ENGINEGLOW_QUENCH_AT);
        }

        engine_glow_ = engine_glow_.strength() > litengine.strength() ? engine_glow_ :litengine;

        visiblemachine_.node_.set_all_emissive_channels_colours( engine_glow_, lights );
    }

    point2d drawmachine::interpolate_position( const float frameFraction )
    {
        position_interpolation_ = min( position_interpolation_ + frameFraction, 1.0f );

        float current_angle = current_position_.angle_as_radians().value();

        float current_offsetXY[2];
        current_position_.board_offset_after_n_frames( 0.0, current_offsetXY );

        point2d current_worldXY;
        hexgrid::calculate_xy( current_position_.gridXY_, current_worldXY );

        current_worldXY[0] += current_offsetXY[0];
        current_worldXY[1] += current_offsetXY[1];

        if( position_interpolation_ < 1.0f )
        {
            const float previous_angle = previous_position_.angle_as_radians().value();

            float previous_offsetXY[2];
            previous_position_.board_offset_after_n_frames( 0.0f, previous_offsetXY );

            float previous_worldXY[2];
            hexgrid::calculate_xy( previous_position_.gridXY_, previous_worldXY );

            previous_worldXY[0] += previous_offsetXY[0];
            previous_worldXY[1] += previous_offsetXY[1];

            // Interpolate from previous to current across frame.
            current_worldXY[0] = previous_worldXY[0] + ( position_interpolation_
                                                         * ( current_worldXY[0] - previous_worldXY[0] ) );

            current_worldXY[1] = previous_worldXY[1] + ( position_interpolation_
                                                         * ( current_worldXY[1] - previous_worldXY[1] ) );

            if( current_position_.turning_ )
            {
                if( BLUE_POSITIONINFO_CCW == current_position_.turning_ )
                {
                    // CCW we expect the current angle to be a lower value
                    // than the previous angle.
                    current_angle = current_angle > previous_angle ? ( current_angle - SS_TWOPI ) : current_angle;
                }
                else
                {
                    // CW we expect the current angle to be greater than
                    // the previous angle.
                    current_angle = current_angle < previous_angle ? ( current_angle + SS_TWOPI ) : current_angle;
                }

                current_angle = previous_angle + ( position_interpolation_ * ( current_angle - previous_angle ) );
            }
        }

#if 1
        load_rotation_y_4x4( radians( current_angle ), transform4x4_ );

        set_translation_4x4( current_worldXY[0], boardY_, current_worldXY[1], transform4x4_ );
#else
        // ROTATIONS DISABLED
        load_translation_4x4( offsetXY[0] + worldXY[0], boardY_, offsetXY[1] + worldXY[1], transform4x4_ );
#endif

        return current_worldXY;
    }

    point2d drawmachine::update_animations( float frameFraction )
    {
        const point2d pos = interpolate_position( frameFraction );

        orient_hardpoints();

        update_emissive_colours();

        return pos;
    }
}
