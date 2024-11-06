#include <cmath>
#include "solosnake/matrix3d.hpp"
#include "solosnake/renderer_cache.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/persistance.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/blue/blue_sun.hpp"
#include "solosnake/blue/blue_drawsun.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_settings.hpp"

namespace blue
{
    namespace
    {
        float calculate_sun_board_radius( SunStrength s )
        {
            // TODO: These should be in persistance data.
            const float max_radius = hexgrid::tile_inner_radius() * 0.9f;
            const float min_radius = hexgrid::tile_inner_radius() * 0.4f;
            const float radius_step = ( max_radius - min_radius ) / SunStrength7;
            return min_radius + static_cast<float>( s ) * radius_step;
        }

        float calculate_sun_board_y( const float radius )
        {
            return 0.65f * radius + BLUE_BOARD_Y;
        }
    }

    drawsun::drawsun( const sun& a, const scene_sun& ss )
        : sun_( &a )
        , scene_sun_( ss )
        , noise_rotation_rate_( ss.sun_noise_.noise_rotation_ )
        , noise_move_rate_x_( ss.sun_noise_.noise_move_x_ )
        , noise_move_rate_y_( ss.sun_noise_.noise_move_y_ )
    {
        // Reset noise to defaults.
        scene_sun_.sun_noise_ = solosnake::sun_noise();

        if( ss.sun_texture_ == solosnake::texturehandle_t() )
        {
            ss_throw( "Invalid sun texture handle passed to drawsun ctor." );
        }

        float worldXZ[2];
        hexgrid::calculate_xy( sun_->board_xy(), worldXZ );

        const float r = calculate_sun_board_radius( a.sun_strength() );

        sun_xyzr_.world_location_[0] = worldXZ[0];
        sun_xyzr_.world_location_[1] = calculate_sun_board_y( r );
        sun_xyzr_.world_location_[2] = worldXZ[1];
        sun_xyzr_.radius_            = r;

        // Larger suns rotate slower.
        const float radius_rotation_factor = 4.0f;
        noise_rotation_rate_ *= radius_rotation_factor * ( 1.0f - r ) / hexgrid::tile_inner_radius();
    }

    void drawsun::update_animations( const unsigned int dtMs )
    {
        scene_sun_.sun_noise_.noise_move_x_   = std::fmod( scene_sun_.sun_noise_.noise_move_x_   + dtMs * noise_move_rate_x_, 1.0f );
        scene_sun_.sun_noise_.noise_move_y_   = std::fmod( scene_sun_.sun_noise_.noise_move_y_   + dtMs * noise_move_rate_y_, 1.0f );
        scene_sun_.sun_noise_.noise_rotation_ = std::fmod( scene_sun_.sun_noise_.noise_rotation_ + dtMs * noise_rotation_rate_, SS_TWOPI );
    }

    void drawsun::draw_into( solosnake::deferred_renderer* r, const float tile_spacing )
    {
        if( sun_strength() > 0u )
        {
            solosnake::deferred_renderer::point_light sunlight;
            sunlight.worldX = world_xyz()[0];
            sunlight.worldY = world_xyz()[1];
            sunlight.worldZ = world_xyz()[2];
            sunlight.red    = 1.0f;
            sunlight.green  = 0.5f;
            sunlight.blue   = 0.15f;
            sunlight.radius = tile_spacing * static_cast<float>( sun_strength() );

            r->draw_point_light( sunlight );
        }

        r->draw_sun( scene_sun_.sun_texture_, scene_sun_.sun_noise_, sun_xyzr_ );
    }
}