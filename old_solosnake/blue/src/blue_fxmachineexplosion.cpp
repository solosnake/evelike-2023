#include "solosnake/blue/blue_fxmachineexplosion.hpp"
#include "solosnake/blue/blue_ifxrenderer.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_machine_events.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_fxsound.hpp"
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/fxtriangles.hpp"
#include "solosnake/iworldobject.hpp"
#include "solosnake/persistance.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/sound_renderer.hpp"

using namespace std;

namespace blue
{
    //-------------------------------------------------------------------------

    class fxmachineexplosion::fxmachineexplosiondata
    {
    public:

        explicit fxmachineexplosiondata(
            float y );

        ~fxmachineexplosiondata();

        void reset(
            const Machine_exploded&,
            float x,
            float z );

        float explosion_y() const;

        shared_ptr<fxsound>     sound_;
        Machine_exploded         explosion_;
        unsigned short          duration_in_ticks_;
    };

    fxmachineexplosion::fxmachineexplosiondata::fxmachineexplosiondata( float y )
        : sound_( fxsound::make_fxsound( y ) )
        , explosion_()
        , duration_in_ticks_( BLUE_EXPLOSION_DURATION_TICKS )
    {
    }

    fxmachineexplosion::fxmachineexplosiondata::~fxmachineexplosiondata()
    {
    }

    void fxmachineexplosion::fxmachineexplosiondata::reset( const Machine_exploded& e, float x, float z )
    {
        explosion_ = e;
        sound_->reset( x, z );
    }

    float fxmachineexplosion::fxmachineexplosiondata::explosion_y() const
    {
        return sound_->location()[1];
    }

    //-------------------------------------------------------------------------

    class fxmachineexplosion::fxmachineexplosionrenderer : public ifxrenderer
    {
    public:

        fxmachineexplosionrenderer(
            const shared_ptr<solosnake::rendering_system>&,
            const shared_ptr<solosnake::sound_renderer>&,
            const solosnake::persistance& );

        ~fxmachineexplosionrenderer();

        void draw_fx( const float, const float, const float[2], const bool, const ifx& ) override;

    private:

        solosnake::deferred_renderer*           prenderer_;
        solosnake::sound_renderer*              srenderer_;
        solosnake::texturehandle_t              sfx_background_[2];
        solosnake::texturehandle_t              sfx_ring_[2];
        solosnake::sound_renderer::BufferHandle sound_handle_;
    };

    //-------------------------------------------------------------------------

    fxmachineexplosion::fxmachineexplosionrenderer::fxmachineexplosionrenderer(
        const shared_ptr<solosnake::rendering_system>& r,
        const shared_ptr<solosnake::sound_renderer>& sr,
        const solosnake::persistance& settings )
        : prenderer_( r->get_renderer_ptr().get() )
        , srenderer_( sr.get() )
    {
        sfx_background_[0] = r->cache().get_texture( settings[BLUE_GAME_EXPLODE_BACK_RGBA_TEX] );
        sfx_background_[1] = r->cache().get_texture( settings[BLUE_GAME_EXPLODE_BACK_WARP_TEX] );
        sfx_ring_[0]       = r->cache().get_texture( settings[BLUE_GAME_EXPLODE_RING_RGBA_TEX] );
        sfx_ring_[1]       = r->cache().get_texture( settings[BLUE_GAME_EXPLODE_RING_WARP_TEX] );

        sound_handle_ = sr->load_buffer( settings[BLUE_GAME_EXPLODE_WAV] );
    }

    fxmachineexplosion::fxmachineexplosionrenderer::~fxmachineexplosionrenderer()
    {
    }

    void fxmachineexplosion::fxmachineexplosionrenderer::draw_fx( const float timeline,
                                                                  const float ms,
                                                                  const float xz[2],
                                                                  const bool justStarted,
                                                                  const ifx& parent )
    {
        assert( timeline >= 0.0f && timeline <= 1.0f );
        assert( parent.event_data() );

        auto data = reinterpret_cast
                    <const fxmachineexplosion::fxmachineexplosiondata*>( parent.event_data() );

        // We over-brighten the explosion relative to its range. Range is
        // how much beyond the exploding machines tile is touched by the blast
        // wave.
        const float min_intensity       = 1.5f;
        const float explosion_intensity = max<float>( min_intensity, data->explosion_.explosionRange );
        const float explosion_range     = 2.0f * hexgrid::tile_inner_radius() * data->explosion_.explosionRange;
        const float explosion_y_        = data->explosion_y();

        // Show expanding ring indicating splash effect. The ring
        // is displayed for 1 second (1000 milliseconds).
        const float ring_lifetime = 1000.0f;
        if( explosion_range > 0.0f && ( justStarted || ms <= ring_lifetime ) )
        {
            // Radius is measured in tiles.
            const float ring_range     = explosion_range;
            const float ring_timeline  = ms / ring_lifetime;
            const float ring_opacity   = 1.0f - ring_timeline;
            const float ring_size      = hexgrid::tile_inner_radius() + ring_timeline * ring_range;
            const float ring_intensity = ring_opacity * explosion_intensity;

            solosnake::colour colour_of_ring( ring_intensity,
                                              ring_intensity,
                                              ring_intensity,
                                              ring_opacity );

            solosnake::fxtriangle ring[2];
            initialise_plane( ring, xz, explosion_y_, ring_size, ring_size );

            prenderer_->draw_sfx( solosnake::deferred_renderer::SfxEmissive,
                                  sfx_ring_[0],
                                  sfx_ring_[1],
                                  colour_of_ring,
                                  ring,
                                  2 );
        }

        float inview4x4[16];
        prenderer_->get_inv_view_matrix( inview4x4 );

        // Show background.
        const float bg_size      = ( 1.0f + timeline ) * ( hexgrid::tile_inner_radius() + explosion_range );
        const float bg_opacity   = 1.0f - timeline;
        const float bg_intensity = bg_opacity * explosion_intensity;

        solosnake::colour colour_of_background( bg_intensity,
                                                bg_intensity,
                                                bg_intensity,
                                                bg_opacity );

        solosnake::fxtriangle background[2];

        initialise_billboard( background,
                              xz[0],
                              explosion_y_,
                              xz[1],
                              bg_size,
                              bg_size * 0.5f,
                              inview4x4 );

        prenderer_->draw_sfx( solosnake::deferred_renderer::SfxEmissive,
                              sfx_background_[0],
                              sfx_background_[1],
                              colour_of_background,
                              background,
                              2 );

        // Add a point light with radius 0.5 + explosion range.
        solosnake::deferred_renderer::point_light light;
        light.worldX = xz[0];
        light.worldY = explosion_y_;
        light.worldZ = xz[1];
        light.red    = colour_of_background.red();
        light.green  = colour_of_background.green();
        light.blue   = colour_of_background.blue();
        light.radius = hexgrid::tile_inner_radius() + explosion_range;
        prenderer_->draw_point_light( light );

        if( justStarted )
        {
            data->sound_->play_sound( srenderer_, sound_handle_ );
        }
    }

    //-------------------------------------------------------------------------

    fxmachineexplosion::fxmachineexplosion( const float y, ifxrenderer& fx )
        : ifx( MachineExplodedEffect, fx )
        , data_( make_unique<fxmachineexplosion::fxmachineexplosiondata>( y ) )
    {
    }

    fxmachineexplosion::~fxmachineexplosion()
    {
    }

    void fxmachineexplosion::restart( Machine_exploded hx )
    {
        float xz[2];
        hexgrid::calculate_xy( hx.gridXY_, xz );

        data_->reset( hx, xz[0], xz[1] );

        ifx::restart( xz, BLUE_FRAME_TICK_DURACTION_MS * data_->duration_in_ticks_ );
    }

    unique_ptr<ifxrenderer> fxmachineexplosion::make_fxrenderer(
        const shared_ptr<solosnake::rendering_system>& r,
        const shared_ptr<solosnake::sound_renderer>& sr,
        const solosnake::persistance& s )
    {
        return make_unique<fxmachineexplosion::fxmachineexplosionrenderer>( r, sr, s );
    }
}
