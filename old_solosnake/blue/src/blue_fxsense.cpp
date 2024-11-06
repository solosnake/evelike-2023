#include "solosnake/blue/blue_fxsense.hpp"
#include "solosnake/blue/blue_fxsound.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_ifxrenderer.hpp"
#include "solosnake/blue/blue_machine_events.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/fxtriangles.hpp"
#include "solosnake/persistance.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/sound_renderer.hpp"

namespace blue
{
    class fxsense::fxsensedata
    {
    public:

        fxsensedata( float y );

        ~fxsensedata();

        void reset( const float x, const float z, const float radius );

        std::shared_ptr<fxsound> sound_;
        float                    radius_;
    };

    fxsense::fxsensedata::fxsensedata( float y ) : sound_( fxsound::make_fxsound( y ) ), radius_( 0.0f )
    {
    }

    fxsense::fxsensedata::~fxsensedata()
    {
    }

    void fxsense::fxsensedata::reset( const float x, const float z, const float radius )
    {
        radius_ = radius;
        sound_->reset( x, z );
    }

    //-------------------------------------------------------------------------

    class fxsense::fxsenserenderer : public ifxrenderer
    {
    public:

        fxsenserenderer(
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>&,
            const solosnake::persistance& );

        ~fxsenserenderer();

        void draw_fx( const float, const float, const float[2], const bool, const ifx& ) override;

    private:

        solosnake::deferred_renderer*           prenderer_;
        solosnake::sound_renderer*              srenderer_;
        solosnake::texturehandle_t              sfx_t1_;
        solosnake::texturehandle_t              sfx_t2_;
        solosnake::sound_renderer::BufferHandle sense_sound_handle_;
        float                                   sense_board_height_;
        float                                   texture_size_factor_;
    };

    fxsense::fxsenserenderer::fxsenserenderer( const std::shared_ptr<solosnake::rendering_system>& r,
                                               const std::shared_ptr<solosnake::sound_renderer>& sr,
                                               const solosnake::persistance& settings )
        : prenderer_( r->get_renderer_ptr().get() )
        , srenderer_( sr.get() )
        , sfx_t1_( solosnake::texturehandle_t() )
        , sfx_t2_( solosnake::texturehandle_t() )
        , sense_board_height_( settings[BLUE_GAME_SENSEFX_HEIGHT].as_float() )
        , texture_size_factor_( settings[BLUE_GAME_SENSEFX_FACTOR].as_float() )
    {
        sfx_t1_ = r->cache().get_texture( settings[BLUE_GAME_SENSEFX_RGBA_TEX] );
        sfx_t2_ = r->cache().get_texture( settings[BLUE_GAME_SENSEFX_WARP_TEX] );
        sense_sound_handle_ = sr->load_buffer( settings[BLUE_GAME_SENSEFX_WAV] );
    }

    fxsense::fxsenserenderer::~fxsenserenderer()
    {
    }

    void fxsense::fxsenserenderer::draw_fx( const float timeline,
                                            const float,
                                            const float xz[2],
                                            const bool justStarted,
                                            const ifx& parent )
    {
        assert( timeline >= 0.0f && timeline <= 1.0f );

        auto data = reinterpret_cast<const fxsensedata*>( parent.event_data() );

        // +1 because of the starting radius.
        const float sfx_size = texture_size_factor_ * hexgrid::tile_inner_radius()
                               * ( timeline * data->radius_ + 1.0f );
        const float sfx_opacity = 1.0f - timeline;

        solosnake::fxtriangle fx[2];
        initialise_plane( fx, xz, sense_board_height_, sfx_size, sfx_size );

        solosnake::colour c( 1.0f, 1.0f, 1.0f, sfx_opacity );
        prenderer_->draw_sfx( solosnake::deferred_renderer::SfxDiffuse, sfx_t1_, sfx_t2_, c, fx, 2 );

        if( justStarted )
        {
            // data->sound_->play_sound( srenderer_, sense_sound_handle_ );
        }
    }

    //-------------------------------------------------------------------------

    fxsense::fxsense( float sense_board_height, ifxrenderer& fx )
        : ifx( SensedEffect, fx )
        , data_( std::make_unique<fxsensedata>( sense_board_height ) )
    {
    }

    fxsense::~fxsense()
    {
    }

    void fxsense::restart( Sensed s )
    {
        float xz[2];
        hexgrid::calculate_xy( s.gridXY_, xz );

        // A hex is two radii wide. A sense radius is measured in tiles to cross.
        data_->reset( xz[0], xz[1], 2.0f * s.radius_ );

        ifx::restart( xz, s.duration_in_ticks_ * BLUE_FRAME_TICK_DURACTION_MS );
    }

    std::unique_ptr<ifxrenderer> fxsense::make_fxrenderer(
        const std::shared_ptr<solosnake::rendering_system>& r,
        const std::shared_ptr<solosnake::sound_renderer>& sr,
        const solosnake::persistance& s )
    {
        return std::make_unique<fxsense::fxsenserenderer>( r, sr, s );
    }
}
