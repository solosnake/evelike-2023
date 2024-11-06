#include "solosnake/matrix3d.hpp"
#include "solosnake/renderer_cache.hpp"
#include "solosnake/blue/blue_asteroid.hpp"
#include "solosnake/blue/blue_drawasteroid.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_settings.hpp"

namespace blue
{
    // DAIRE HACK FIX
    static float asteroid_rotator = 0.1234f;

    drawasteroid::drawasteroid( const asteroid& a,
                                solosnake::modelnode&& n )
        : asteroid_( &a )
        , rotation_angle_( asteroid_rotator += 0.111f )
        , rotation_rate_( 0.00005f + sin( asteroid_rotator ) / 10000.0f )
        , node_( std::move( n ) )
    {
        float worldXZ[2];
        hexgrid::calculate_xy( asteroid_->board_xy(), worldXZ );

        worldXYZ_[0] = worldXZ[0];
        worldXYZ_[1] = BLUE_BOARD_Y;
        worldXYZ_[2] = worldXZ[1];

        solosnake::load_translation_4x4( worldXYZ_, transform4x4_ );

        node_.set_all_emissive_channels_colours(
            solosnake::bgra( 40, 117, 183, 0 ),
            solosnake::bgra( 32, 133, 183, 0 ) );
    }

    void drawasteroid::update_animations( const unsigned int dtMs )
    {
        rotation_angle_ += dtMs * rotation_rate_;

        solosnake::load_rotation_y_4x4( solosnake::radians( rotation_angle_ ), transform4x4_ );
        solosnake::set_translation_4x4( worldXYZ_, transform4x4_ );
    }
}
