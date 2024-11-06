#include "solosnake/blue/blue_fxfiring.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_machine_events.hpp"
#include "solosnake/blue/blue_secondaryevent.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_ifxrenderer.hpp"
#include "solosnake/blue/blue_fxsound.hpp"
#include "solosnake/colour.hpp"
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/fxtriangles.hpp"
#include "solosnake/persistance.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/sound_renderer.hpp"

using namespace std;

namespace blue
{
    //-------------------------------------------------------------------------

    class fxweaponfiring::fxfiringdata
    {
    public:

        explicit fxfiringdata( float y );

        ~fxfiringdata();

        void update_sound_location(
            const float x,
            const float z ) const;

        void set_sound_speed_location_and_direction(
            const float speed,
            const float x,
            const float y,
            const float z,
            const float* dirXZ ) const;

        void play_sound(
            solosnake::sound_renderer*,
            solosnake::sound_renderer::BufferHandle ) const;

        float beam_y() const;

        float beam_width() const;

        float beam_length() const;

        float beam_illumination_radius() const;

        const float* beam_end_xz() const;

        unsigned short duration_in_ticks() const;

        solosnake::colour& beam_colour();

        const solosnake::colour& beam_colour() const;

        void reset( float x, float z );

    private:

        shared_ptr<fxsound>       sound_;
        Shot_fired                 shotfired_;
        float                     beam_end_xz_[2];
        float                     beam_width_;
        float                     beam_length_;
        float                     beam_illumination_radius_;
        solosnake::colour         beam_colour_;
        unsigned short            duration_in_ticks_;
    };


    //-------------------------------------------------------------------------


    fxweaponfiring::fxfiringdata::fxfiringdata( float y )
        : sound_( fxsound::make_fxsound( y ) )
        , shotfired_()
        , beam_width_( 0.2f )
        , beam_length_( 0.4f )
        , beam_illumination_radius_( 1.0f )
        , beam_colour_( 1.0f, 0.0, 0.0f )
        , duration_in_ticks_( BLUE_FIRING_DURATION_TICKS )
    {
        beam_end_xz_[0] = beam_end_xz_[1] = 0.0f;
    }

    fxweaponfiring::fxfiringdata::~fxfiringdata()
    {
    }

    void fxweaponfiring::fxfiringdata::update_sound_location( const float x, const float z ) const
    {
        sound_->set_xz( x, z );
    }

    void fxweaponfiring::fxfiringdata::set_sound_speed_location_and_direction( const float speed,
                                                                               const float x,
                                                                               const float y,
                                                                               const float z,
                                                                               const float* dirXZ ) const
    {
        solosnake::point3d d( dirXZ[0], 0.0f, dirXZ[1] );
        solosnake::point3d pos( 0.0f, y, 0.0f );
        sound_->set_direction_and_speed( d, speed );
        sound_->set_location( pos );
    }

    inline void fxweaponfiring::fxfiringdata::play_sound(
        solosnake::sound_renderer* sr,
        solosnake::sound_renderer::BufferHandle h ) const
    {
        sound_->play_sound( sr, h );
    }

    inline float fxweaponfiring::fxfiringdata::beam_y() const
    {
        return sound_->location()[1];
    }

    inline float fxweaponfiring::fxfiringdata::beam_illumination_radius() const
    {
        return beam_illumination_radius_;
    }

    inline float fxweaponfiring::fxfiringdata::beam_width() const
    {
        return beam_width_;
    }

    inline float fxweaponfiring::fxfiringdata::beam_length() const
    {
        return beam_length_;
    }

    inline const float* fxweaponfiring::fxfiringdata::beam_end_xz() const
    {
        return beam_end_xz_;
    }

    inline solosnake::colour& fxweaponfiring::fxfiringdata::beam_colour()
    {
        return beam_colour_;
    }

    inline const solosnake::colour& fxweaponfiring::fxfiringdata::beam_colour() const
    {
        return beam_colour_;
    }

    inline unsigned short fxweaponfiring::fxfiringdata::duration_in_ticks() const
    {
        return duration_in_ticks_;
    }

    void fxweaponfiring::fxfiringdata::reset( float x, float z )
    {
        beam_end_xz_[0] = x;
        beam_end_xz_[1] = z;
        sound_->reset( x, z );
    }


    //-------------------------------------------------------------------------


    class fxweaponfiring::fxfiringrenderer : public ifxrenderer
    {
    public:

        fxfiringrenderer(
            const shared_ptr<solosnake::rendering_system>&,
            const shared_ptr<solosnake::sound_renderer>&,
            const solosnake::persistance& );

        ~fxfiringrenderer();

        void draw_fx( const float, const float, const float[2], const bool, const ifx& ) override;

    private:

        solosnake::deferred_renderer*           prenderer_;
        solosnake::sound_renderer*              srenderer_;
        solosnake::texturehandle_t              sfx_t1_;
        solosnake::texturehandle_t              sfx_t2_;
        solosnake::sound_renderer::BufferHandle firing_sound_handle_;
    };


    //-------------------------------------------------------------------------


    fxweaponfiring::fxfiringrenderer::fxfiringrenderer( const shared_ptr<solosnake::rendering_system>& r,
                                                        const shared_ptr<solosnake::sound_renderer>& sr,
                                                        const solosnake::persistance& settings )
        : prenderer_( r->get_renderer_ptr().get() )
        , srenderer_( sr.get() )
        , sfx_t1_( solosnake::texturehandle_t() )
        , sfx_t2_( solosnake::texturehandle_t() )
    {
        sfx_t1_ = r->cache().get_texture( "laser.bmp" );
        sfx_t2_ = r->cache().get_texture( "black.bmp" );

        firing_sound_handle_ = sr->load_buffer( "laser0.wav" );
    }

    fxweaponfiring::~fxweaponfiring()
    {
    }

    fxweaponfiring::fxfiringrenderer::~fxfiringrenderer()
    {
    }

    void fxweaponfiring::fxfiringrenderer::draw_fx( const float timeline,
                                                    const float,
                                                    const float xz[2],
                                                    const bool justStarted,
                                                    const ifx& parent )
    {
        assert( timeline >= 0.0f && timeline <= 1.0f );

        auto data = reinterpret_cast<const fxweaponfiring::fxfiringdata*>( parent.event_data() );

        solosnake::fxtriangle beam[4];

        initialise_beam( beam,
                         xz,
                         data->beam_end_xz(),
                         timeline,
                         data->beam_y(),
                         data->beam_width(),
                         data->beam_length() );

        prenderer_->draw_sfx( solosnake::deferred_renderer::SfxEmissive,
                              sfx_t1_,
                              sfx_t2_,
                              data->beam_colour(),
                              beam,
                              2u );

        // Add a point light at head of beam: take midpoint of beam
        // as point to add light.
        solosnake::deferred_renderer::point_light light;
        light.worldX = ( beam[0].vert[0].x + beam[0].vert[1].x ) * 0.5f;
        light.worldZ = ( beam[0].vert[0].z + beam[0].vert[1].z ) * 0.5f;
        light.worldY = data->beam_y();
        light.red    = data->beam_colour().red();
        light.green  = data->beam_colour().green();
        light.blue   = data->beam_colour().blue();
        light.radius = data->beam_illumination_radius() * hexgrid::tile_inner_radius();

        prenderer_->draw_point_light( light );

        if( justStarted )
        {
            float dir[2];
            solosnake::sub2( xz, data->beam_end_xz(), dir );
            const float spanlength = solosnake::normalise2( dir );

            data->set_sound_speed_location_and_direction( spanlength * 10.0f,
                                                          light.worldX,
                                                          light.worldY,
                                                          light.worldZ,
                                                          dir );

            data->update_sound_location( light.worldX, light.worldZ );

            data->play_sound( srenderer_, firing_sound_handle_ );
        }
        else
        {
            data->update_sound_location( light.worldX, light.worldZ );
        }
    }


    //-------------------------------------------------------------------------


    fxweaponfiring::fxweaponfiring( float y, ifxrenderer& fx )
        : ifx( WeaponFiringEffect, fx )
        , data_( make_unique<fxfiringdata>( y ) )
    {
    }

    void fxweaponfiring::restart( const Shot_fired& f )
    {
        float firerXZ[2];
        float beamEndXZ[2];
        hexgrid::calculate_xy( f.shot_fired_.firerXY_, firerXZ );

        if( f.shot_missed() )
        {
            hexgrid::calculate_xy( f.shot_fired_.targetXY_, beamEndXZ );
        }
        else
        {
            hexgrid::calculate_xy( f.hit_xy_, beamEndXZ );
        }

        data_->reset( beamEndXZ[0], beamEndXZ[1] );

        ifx::restart( firerXZ, BLUE_FRAME_TICK_DURACTION_MS * data_->duration_in_ticks() );
    }

    unique_ptr<ifxrenderer> fxweaponfiring::make_fxrenderer( const shared_ptr<solosnake::rendering_system>& r,
                                                             const shared_ptr<solosnake::sound_renderer>& sr,
                                                             const solosnake::persistance& s )
    {
        return make_unique<fxweaponfiring::fxfiringrenderer>( r, sr, s );
    }
}
