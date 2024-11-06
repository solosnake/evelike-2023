#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/make_iimg.hpp"
#include "solosnake/image.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    namespace
    {
        class imageiimg : public iimg
        {
        public:

            explicit imageiimg( image&& pic ) : picture_( std::move( pic ) )
            {
            }

            virtual ~imageiimg() SS_NOEXCEPT
            {
            }

            unsigned int img_width() const override
            {
                return picture_.width();
            }

            unsigned int img_height() const override
            {
                return picture_.height();
            }

            const unsigned char* img_bgra_pixels() const override
            {
                return picture_.data();
            }

        private:
            image picture_;
        };

        class imageiimgBGR : public iimgBGR
        {
        public:

            explicit imageiimgBGR( image&& pic ) : picture_( std::move( pic ) )
            {
            }

            virtual ~imageiimgBGR() SS_NOEXCEPT
            {
            }

            unsigned int img_width() const override
            {
                return picture_.width();
            }

            unsigned int img_height() const override
            {
                return picture_.height();
            }

            const unsigned char* img_bgr_pixels() const override
            {
                return picture_.data();
            }

        private:
            image picture_;
        };
    }

    //-------------------------------------------------------------------------

    std::shared_ptr<iimg> make_iimg( image&& pic )
    {
        if( pic.format() != image::format_bgra )
        {
            ss_throw( "make_iimg requires bgra images." );
        }

        return std::make_shared<imageiimg>( std::move( pic ) );
    }

    std::shared_ptr<iimgBGR> make_iimgBGR( image&& pic )
    {
        if( pic.format() != image::format_bgr )
        {
            ss_throw( "make_iimgBG requires bgr images." );
        }

        return std::make_shared<imageiimgBGR>( std::move( pic ) );
    }
}
