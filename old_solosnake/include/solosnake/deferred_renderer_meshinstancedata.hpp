#ifndef solosnake_deferred_renderer_meshinstancedata_hpp
#define solosnake_deferred_renderer_meshinstancedata_hpp

#include <cstdint>
#include "solosnake/inline.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/bgra.hpp"

namespace solosnake
{
    //! Data passed in to customise each rendered instance of a mesh.
    //! Fill this structure out and pass in one per rendered instance to the
    //! deferred_renderer. This structure is used to specify the instances
    //! transform (e.g. its world space location and rotation), the colour to
    //! be used on any 'team' areas of the mesh's texture channels,
    //! and the modifier to be applied to the Emissive channel of the mesh's
    //! textures.
    //! This struct must remain a POD type.
    struct meshinstancedata
    {
    private:

        //! This is applied to the instance along with the view and
        //! projection matrix.
        float transform4x4_[16];

        //! The r value is used to hue shift the non grey parts of the
        //! texture to a new color value.
        std::uint32_t rgbHueShift_;

        //! Colours the emissive texture channels 0 and 1.
        std::uint32_t emissiveColours_[2];

    public:

        //! Default mesh instance data is created with identity matrix and white
        // team colour.
        meshinstancedata()
            : rgbHueShift_(0)
        {
            load_identity_4x4( transform4x4_ );
            emissiveColours_[0] = emissiveColours_[1] = 0u;
        }

        //! Copies the data from the other instance, but sets up the new instance
        //! as having a matrix = rY * rX * other * m4x4, where rY and rX are
        //! Radians of rotation about their respective axis.
        meshinstancedata( const meshinstancedata& other,
                          const float rX,
                          const float rY,
                          const float* unaliased m4x4 )
            : rgbHueShift_(other.rgbHueShift_)
        {
            float localTransform[16];

            if( rY != 0.0f || rX != 0.0f )
            {
                float mXY[16];
                load_rotation_xy_4x4( Radians( rX ), Radians( rY ), mXY );
                mul_4x4( mXY, other.transform4x4_, localTransform );
            }
            else
            {
                load_copy_4x4( other.transform4x4_, localTransform );
            }

            mul_4x4( localTransform, m4x4, transform4x4_ );
            emissiveColours_[0] = other.emissiveColours_[0];
            emissiveColours_[1] = other.emissiveColours_[1];
        }

        void translate( const float* unaliased xyz )
        {
            solosnake::set_translation_4x4( xyz, transform4x4_ );
        }

        void set_transform_4x4( const float* unaliased m4x4 )
        {
            solosnake::copy_4x4( m4x4, transform4x4_ );
        }

        SS_INLINE std::uint32_t bgra_to_uint32_rgba( const Bgra& c )
        {
            return
                ( static_cast<std::uint32_t>( c.blue() ) <<  0 ) |
                ( static_cast<std::uint32_t>( c.green() ) <<  8 ) |
                ( static_cast<std::uint32_t>( c.red() ) << 16 ) |
                ( static_cast<std::uint32_t>( c.alpha() ) << 24 );
        }

        //! x is a value in the range 0 .. 1.0.
        SS_INLINE void set_team_hue_shift( float x )
        {
            rgbHueShift_ = bgra_to_uint32_rgba( Bgra( static_cast<std::uint8_t>( x * 255.0f ) ) );
        }

        SS_INLINE void set_emissive_channel_bgra( const size_t channel, const Bgra& c )
        {
            assert( channel < 2 );
            emissiveColours_[channel] = bgra_to_uint32_rgba( c );
        }

        SS_INLINE void set_emissive_channels_bgra( const Bgra& c0, const Bgra& c1 )
        {
            emissiveColours_[0] = bgra_to_uint32_rgba( c0 );
            emissiveColours_[1] = bgra_to_uint32_rgba( c1 );
        }

        SS_INLINE const float* location_4x4() const
        {
            return transform4x4_;
        }

        SS_INLINE float* location_4x4()
        {
            return transform4x4_;
        }
    };
}

#endif
