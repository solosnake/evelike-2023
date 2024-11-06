#include "solosnake/fxtriangles.hpp"
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/matrix2d.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/point.hpp"

namespace solosnake
{
    namespace
    {
        void set_standard_uvs( solosnake::fxtriangle* fx, size_t n )
        {
            for( size_t i = 0; i < n; ++i )
            {
                fx[2 * i].vert[0].u = 0.0f;
                fx[2 * i].vert[0].v = 0.0f;
                fx[2 * i].vert[1].u = 1.0f;
                fx[2 * i].vert[1].v = 0.0f;
                fx[2 * i].vert[2].u = 1.0f;
                fx[2 * i].vert[2].v = 1.0f;
                fx[2 * i + 1].vert[0].u = 1.0f;
                fx[2 * i + 1].vert[0].v = 1.0f;
                fx[2 * i + 1].vert[1].u = 0.0f;
                fx[2 * i + 1].vert[1].v = 1.0f;
                fx[2 * i + 1].vert[2].u = 0.0f;
                fx[2 * i + 1].vert[2].v = 0.0f;
            }
        }
    }

    void initialise_baffle( solosnake::fxtriangle* fx, float const xz[2], float y, float sfx_size )
    {
        // X
        fx[0].vert[0].x = xz[0];
        fx[0].vert[0].y = y - sfx_size;
        fx[0].vert[0].z = xz[1] - sfx_size;
        fx[0].vert[1].x = xz[0];
        fx[0].vert[1].y = y + sfx_size;
        fx[0].vert[1].z = xz[1] - sfx_size;
        fx[0].vert[2].x = xz[0];
        fx[0].vert[2].y = y + sfx_size;
        fx[0].vert[2].z = xz[1] + sfx_size;

        fx[1].vert[0].x = xz[0];
        fx[1].vert[0].y = y + sfx_size;
        fx[1].vert[0].z = xz[1] + sfx_size;
        fx[1].vert[1].x = xz[0];
        fx[1].vert[1].y = y - sfx_size;
        fx[1].vert[1].z = xz[1] + sfx_size;
        fx[1].vert[2].x = xz[0];
        fx[1].vert[2].y = y - sfx_size;
        fx[1].vert[2].z = xz[1] - sfx_size;

        // Y
        fx[2].vert[0].x = xz[0] - sfx_size;
        fx[2].vert[0].y = y;
        fx[2].vert[0].z = xz[1] - sfx_size;
        fx[2].vert[1].x = xz[0] + sfx_size;
        fx[2].vert[1].y = y;
        fx[2].vert[1].z = xz[1] - sfx_size;
        fx[2].vert[2].x = xz[0] + sfx_size;
        fx[2].vert[2].y = y;
        fx[2].vert[2].z = xz[1] + sfx_size;

        fx[3].vert[0].x = xz[0] + sfx_size;
        fx[3].vert[0].y = y;
        fx[3].vert[0].z = xz[1] + sfx_size;
        fx[3].vert[1].x = xz[0] - sfx_size;
        fx[3].vert[1].y = y;
        fx[3].vert[1].z = xz[1] + sfx_size;
        fx[3].vert[2].x = xz[0] - sfx_size;
        fx[3].vert[2].y = y;
        fx[3].vert[2].z = xz[1] - sfx_size;

        // Z
        fx[4].vert[0].x = xz[0] - sfx_size;
        fx[4].vert[0].y = y - sfx_size;
        fx[4].vert[0].z = xz[1];
        fx[4].vert[1].x = xz[0] + sfx_size;
        fx[4].vert[1].y = y - sfx_size;
        fx[4].vert[1].z = xz[1];
        fx[4].vert[2].x = xz[0] + sfx_size;
        fx[4].vert[2].y = y + sfx_size;
        fx[4].vert[2].z = xz[1];

        fx[5].vert[0].x = xz[0] + sfx_size;
        fx[5].vert[0].y = y + sfx_size;
        fx[5].vert[0].z = xz[1];
        fx[5].vert[1].x = xz[0] - sfx_size;
        fx[5].vert[1].y = y + sfx_size;
        fx[5].vert[1].z = xz[1];
        fx[5].vert[2].x = xz[0] - sfx_size;
        fx[5].vert[2].y = y - sfx_size;
        fx[5].vert[2].z = xz[1];

        set_standard_uvs( fx, 3 );
    }

    void initialise_billboard( solosnake::fxtriangle* unaliased fx,
                               float x,
                               float y,
                               float z,
                               float x_scale,
                               float y_scale,
                               const float* unaliased invView4x4 )
    {
        float xaxis[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
        float xaligned[4];
        float yaxis[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
        float yaligned[4];
        solosnake::transform_4x1( xaxis, invView4x4, xaligned );
        solosnake::transform_4x1( yaxis, invView4x4, yaligned );

        solosnake::mul4( xaligned, x_scale );
        solosnake::mul4( yaligned, y_scale );

        fx[0].vert[0].x = x - xaligned[0] - yaligned[0];
        fx[0].vert[0].y = y - xaligned[1] - yaligned[1];
        fx[0].vert[0].z = z - xaligned[2] - yaligned[2];
        fx[0].vert[1].x = x + xaligned[0] - yaligned[0];
        fx[0].vert[1].y = y + xaligned[1] - yaligned[1];
        fx[0].vert[1].z = z + xaligned[2] - yaligned[2];
        fx[0].vert[2].x = x + xaligned[0] + yaligned[0];
        fx[0].vert[2].y = y + xaligned[1] + yaligned[1];
        fx[0].vert[2].z = z + xaligned[2] + yaligned[2];

        fx[1].vert[0].x = x + xaligned[0] + yaligned[0];
        fx[1].vert[0].y = y + xaligned[1] + yaligned[1];
        fx[1].vert[0].z = z + xaligned[2] + yaligned[2];
        fx[1].vert[1].x = x - xaligned[0] + yaligned[0];
        fx[1].vert[1].y = y - xaligned[1] + yaligned[1];
        fx[1].vert[1].z = z - xaligned[2] + yaligned[2];
        fx[1].vert[2].x = x - xaligned[0] - yaligned[0];
        fx[1].vert[2].y = y - xaligned[1] - yaligned[1];
        fx[1].vert[2].z = z - xaligned[2] - yaligned[2];

        set_standard_uvs( fx, 1 );
    }

    void initialise_offset_billboard( fxtriangle* unaliased fx,
                                      float x,
                                      float y,
                                      float z,
                                      float x_scale,
                                      float y_scale,
                                      float z_offset,
                                      const float* unaliased invView4x4 )
    {
        // DOES NOT WORK!!!

        float zaxis[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
        float zaligned[4];
        solosnake::transform_4x1( zaxis, invView4x4, zaligned );

        initialise_billboard( fx, x, y, z, x_scale, y_scale, invView4x4 );

        // Offset billboard towards/away from eye.
        for( size_t t = 0u; t < 2u; ++t )
        {
            for( size_t v = 0u; v < 3u; ++v )
            {
                fx[t].vert[v].x += z_offset * zaligned[0];
                fx[t].vert[v].y += z_offset * zaligned[1];
                fx[t].vert[v].z += z_offset * zaligned[2];
            }
        }
    }

    void initialise_plane(
        solosnake::fxtriangle* fx, float const xz[2], float y, float x_scale, float y_scale )
    {
        fx[0].vert[0].x = xz[0] - x_scale;
        fx[0].vert[0].y = y;
        fx[0].vert[0].z = xz[1] - y_scale;
        fx[0].vert[0].u = 0.0f;
        fx[0].vert[0].v = 0.0f;
        fx[0].vert[1].x = xz[0] + x_scale;
        fx[0].vert[1].y = y;
        fx[0].vert[1].z = xz[1] - y_scale;
        fx[0].vert[1].u = 1.0f;
        fx[0].vert[1].v = 0.0f;
        fx[0].vert[2].x = xz[0] + x_scale;
        fx[0].vert[2].y = y;
        fx[0].vert[2].z = xz[1] + y_scale;
        fx[0].vert[2].u = 1.0f;
        fx[0].vert[2].v = 1.0f;

        fx[1].vert[0].x = xz[0] + x_scale;
        fx[1].vert[0].y = y;
        fx[1].vert[0].z = xz[1] + y_scale;
        fx[1].vert[0].u = 1.0f;
        fx[1].vert[0].v = 1.0f;
        fx[1].vert[1].x = xz[0] - x_scale;
        fx[1].vert[1].y = y;
        fx[1].vert[1].z = xz[1] + y_scale;
        fx[1].vert[1].u = 0.0f;
        fx[1].vert[1].v = 1.0f;
        fx[1].vert[2].x = xz[0] - x_scale;
        fx[1].vert[2].y = y;
        fx[1].vert[2].z = xz[1] - y_scale;
        fx[1].vert[2].u = 0.0f;
        fx[1].vert[2].v = 0.0f;
    }

    void initialise_beam( solosnake::fxtriangle* unaliased fx4,
                          const float* unaliased fromXZ,
                          const float* unaliased toXZ,
                          float timeline,
                          float y,
                          float beamwidth,
                          float beamlength )
    {
        const float rotate90[] = { 0.0f, -1.0f, 1.0f, 0.0f };
        float dxz[2];
        float offset[2];

        timeline = timeline > 1.0f ? 1.0f : timeline;

        solosnake::sub2( toXZ, fromXZ, dxz );
        const float spanlength = solosnake::normalise2( dxz );
        solosnake::transform_2x1( dxz, rotate90, offset );
        solosnake::mul2( offset, beamwidth );

        float total_length = beamlength + spanlength;
        float current_location = timeline * total_length;

        float headxz[2];
        float tailxz[2];
        solosnake::madd2( dxz, current_location, fromXZ, headxz );
        solosnake::madd2( dxz, current_location - beamlength, fromXZ, tailxz );

        fx4[0].vert[0].x = headxz[0] + offset[0];
        fx4[0].vert[0].y = y;
        fx4[0].vert[0].z = headxz[1] + offset[1];
        fx4[0].vert[0].u = 0.0f;
        fx4[0].vert[0].v = 0.0f;

        fx4[0].vert[1].x = headxz[0] - offset[0];
        fx4[0].vert[1].y = y;
        fx4[0].vert[1].z = headxz[1] - offset[1];
        fx4[0].vert[1].u = 0.0f;
        fx4[0].vert[1].v = 1.0f;

        fx4[0].vert[2].x = tailxz[0] - offset[0];
        fx4[0].vert[2].y = y;
        fx4[0].vert[2].z = tailxz[1] - offset[1];
        fx4[0].vert[2].u = 1.0f;
        fx4[0].vert[2].v = 1.0f;

        fx4[1].vert[0].x = tailxz[0] - offset[0];
        fx4[1].vert[0].y = y;
        fx4[1].vert[0].z = tailxz[1] - offset[1];
        fx4[1].vert[0].u = 1.0f;
        fx4[1].vert[0].v = 1.0f;

        fx4[1].vert[1].x = tailxz[0] + offset[0];
        fx4[1].vert[1].y = y;
        fx4[1].vert[1].z = tailxz[1] + offset[1];
        fx4[1].vert[1].u = 1.0f;
        fx4[1].vert[1].v = 0.0f;

        fx4[1].vert[2].x = headxz[0] + offset[0];
        fx4[1].vert[2].y = y;
        fx4[1].vert[2].z = headxz[1] + offset[1];
        fx4[1].vert[2].u = 0.0f;
        fx4[1].vert[2].v = 0.0f;
    }
}
