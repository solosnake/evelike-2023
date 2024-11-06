#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/ixmlelementreader.hpp"
#include "solosnake/image.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/load_skybox.hpp"
#include "solosnake/make_iimg.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/external/xml.hpp"
#include <memory>

namespace solosnake
{
    namespace
    {
        const char* const texture_names[6] =
        {
            "positive_x",
            "negative_x",
            "positive_y",
            "negative_y",
            "positive_z",
            "negative_z"
        };

        class skyboxreader : public ixmlelementreader
        {
        public:

            skyboxreader();

            cubemap make_skybox( const ifilefinder& texfinder ) const;

        private:

            void load_bitmaps( const ifilefinder& texfinder, image bmps[6] ) const;

            bool read_from_element( const TiXmlElement& xml ) override;

            bool read_ended( bool goodRead ) override;

        private:

            std::string texture_names_[6];
        };

        skyboxreader::skyboxreader() : ixmlelementreader( "skybox" )
        {
        }

        cubemap skyboxreader::make_skybox( const ifilefinder& texfinder ) const
        {
            image bmps[6];
            load_bitmaps( texfinder, bmps );

            cubemap sb;
            for( unsigned int i = 0; i < 6; ++i )
            {
                sb[i] = make_iimgBGR( std::move( bmps[i] ) );
            }

            return sb;
        }

        void skyboxreader::load_bitmaps( const ifilefinder& texfinder, image bmps[6] ) const
        {
            for( size_t i = 0; i < 6; ++i )
            {
                auto url = texfinder.get_file( texture_names_[i] );
                if( bmps[i].open( url, false ) )
                {
                    if( bmps[i].format() != image::format_bgr )
                    {
                        ss_err( "Skybox texture ", texture_names[i], " is not RGB format" );
                        ss_throw( "A cubemap texture was not RGB format" );
                    }
                }
            }
        }

        bool skyboxreader::read_from_element( const TiXmlElement& xml )
        {
            bool name_was_read = false;

            for( size_t i = 0; i < 6; ++i )
            {
                if( texture_names_[i].empty() && ( false == name_was_read ) )
                {
                    name_was_read
                        = solosnake::read_attribute( xml, texture_names[i], texture_names_[i], true );
                    name_was_read = name_was_read && ( false == texture_names_[i].empty() );
                }
            }

            return name_was_read;
        }

        bool skyboxreader::read_ended( bool goodRead )
        {
            for( size_t i = 0; i < 6; ++i )
            {
                if( texture_names_[i].empty() )
                {
                    return false;
                }
            }

            return goodRead;
        }
    }

    cubemap load_skybox( const filepath& url, const ifilefinder& texfinder )
    {
        skyboxreader sbr;

        if( ixmlelementreader::read( url.string().c_str(), sbr ) )
        {
            return sbr.make_skybox( texfinder );
        }

        ss_err( "Unable to load cubemap ", url.string() );
        ss_throw( "Unable do load cubemap" );
    }
}
