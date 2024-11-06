#include <cassert>
#include <cmath>
#include "solosnake/blue/blue_drawboard.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/throw.hpp"

namespace blue
{
    drawboard::drawboard(
        solosnake::rendering_system& r, const drawboardparams& params )
        : renderer_( r.get_renderer_ptr() )
        , drawbuffer_()
        , params_( params )
    {
        assert( ! renderer_.expired() );
    }

    drawboard::~drawboard()
    {
        try
        {
            release_board_buffer();
        }
        catch( ... )
        {
        }

        assert( drawbuffer_.buffer == nullptr );
    }

    void drawboard::release_board_buffer()
    {
        if( drawbuffer_.buffer )
        {
            auto r = renderer_.lock();

            if( r )
            {
                r->free_dynbuffer( drawbuffer_.buffer );
                drawbuffer_.buffer = nullptr;
            }
        }
    }

    void drawboard::make_board_buffer( const hexgrid& h, const bool onlyValidTiles )
    {
        release_board_buffer();

        std::shared_ptr<solosnake::deferred_renderer> r( renderer_ );

        unsigned int ntiles = onlyValidTiles ? h.valid_tiles_count() : h.max_grid_size();

        if( ntiles > 0u )
        {
            // We need 6 vertices per tile. We need 8 indices (7+restarter) per
            // tile.
            const unsigned int nverts   = 6u * ntiles;
            const unsigned int nindices = 10u * ntiles;

            if( nverts > 65535u )
            {
                ss_throw( "Board vertex requirements exceeds max possible vertex buffer size." );
            }

            // Now alloc the buffer:
            drawbuffer_.buffer  = r->alloc_dynbuffer( nverts, nindices );
            drawbuffer_.strips  = true;
            drawbuffer_.readZ   = true;
            drawbuffer_.writeZ  = false;
            drawbuffer_.num_indices_to_draw = nindices;
            drawbuffer_.alphaBlend = true;

            drawbuffer_.rgba[0] = 1.0f;
            drawbuffer_.rgba[1] = 1.0f;
            drawbuffer_.rgba[2] = 1.0f;
            drawbuffer_.rgba[3] = params_.transparency_; // Alpha board is drawn with.

            if( drawbuffer_.buffer )
            {
                solosnake::dynbuffervertex* v   = drawbuffer_.buffer->vertices();
                solosnake::dynbufferuv* uv      = drawbuffer_.buffer->texcoords();
                solosnake::dynbufferRGBA* clr   = drawbuffer_.buffer->colours();
                unsigned short* is              = drawbuffer_.buffer->indices();

                drawbuffer_.texture = params_.texture_;

                r->lock_dynbuffer( drawbuffer_.buffer );
                {
                    // Calculate UVs using a default hexagon.
                    float commonUVs[12];
                    {
                        Hex_coord uvcentre = { 0, 0 };
                        float uvs[18];

                        // Scale down so that the width is 1. The default
                        // width is 4.0f / std::sqrt(3.0f).
                        const float scale = std::sqrt( 3.0f ) / 4.0f;
                        hexgrid::get_hexagon_vertices( uvcentre, scale, uvs );

                        commonUVs[0]  = params_.tw_ * uvs[ 0] + params_.tx_;
                        commonUVs[1]  = params_.th_ * uvs[ 2] + params_.ty_;
                        commonUVs[2]  = params_.tw_ * uvs[ 3] + params_.tx_;
                        commonUVs[3]  = params_.th_ * uvs[ 5] + params_.ty_;
                        commonUVs[4]  = params_.tw_ * uvs[ 6] + params_.tx_;
                        commonUVs[5]  = params_.th_ * uvs[ 8] + params_.ty_;
                        commonUVs[6]  = params_.tw_ * uvs[ 9] + params_.tx_;
                        commonUVs[7]  = params_.th_ * uvs[11] + params_.ty_;
                        commonUVs[8]  = params_.tw_ * uvs[12] + params_.tx_;
                        commonUVs[9]  = params_.th_ * uvs[14] + params_.ty_;
                        commonUVs[10] = params_.tw_ * uvs[15] + params_.tx_;
                        commonUVs[11] = params_.th_ * uvs[17] + params_.ty_;
                    }

                    size_t tileCounter = 0;

                    unsigned short icounter = 0;

                    const bool drawAllTiles = ! onlyValidTiles;

                    for( int8_t y = 0; y < h.grid_height(); ++y )
                    {
                        for( int8_t x = 0; x < h.grid_width(); ++x )
                        {
                            ++tileCounter;
                            const Hex_coord xy = Hex_coord::make_coord( x, y );
                            const bool draw = drawAllTiles || is_not_offboard_tile( h.contents( xy ) );

                            if( draw )
                            {
                                hexgrid::get_hexagon_vertices( xy, params_.tile_shrink_factor_, reinterpret_cast<float*>( v ) );

                                // Texture coords: they are always the same:
                                std::memcpy( uv, commonUVs, sizeof( commonUVs ) );

                                // Indices:
                                is[0] = icounter + 1u;
                                is[1] = icounter + 0u;
                                is[2] = icounter + 2u;
                                is[3] = icounter + 3u;
                                is[4] = solosnake::dynbuffer::StripRestartMarker;

                                is[5] = icounter + 3u;
                                is[6] = icounter + 0u;
                                is[7] = icounter + 4u;
                                is[8] = icounter + 5u;
                                is[9] = solosnake::dynbuffer::StripRestartMarker;

                                v        += 6u;
                                uv       += 6u;
                                icounter += 6u;
                                is       += 10u;
                            }
                        }
                    }
                }

                set_board_colours( h, ntiles, clr );

                r->unlock_dynbuffer( drawbuffer_.buffer, nverts, nindices );
            }
            else
            {
                ss_throw( "Unable to allocate dyn buffer to render hex board." );
            }
        }
    }

    void drawboard::update_board_colours( const hexgrid& h )
    {
        if( drawbuffer_.buffer )
        {
            assert( ( drawbuffer_.buffer->colours_count() % 6u ) == 0u );
            std::shared_ptr<solosnake::deferred_renderer> r( renderer_ );
            r->lock_dynbuffer( drawbuffer_.buffer );
            set_board_colours( h, drawbuffer_.buffer->colours_count() / 6u,  drawbuffer_.buffer->colours() );
            r->unlock_dynbuffer_after_colour_change( drawbuffer_.buffer );
        }
        else
        {
            ss_throw( "Unable to allocate dyn buffer to render hex board." );
        }
    }

    void drawboard::render() const
    {
        if( drawbuffer_.buffer )
        {
            std::shared_ptr<solosnake::deferred_renderer> r( renderer_ );
            r->draw_dynbuffer( drawbuffer_ );
        }
    }
}
