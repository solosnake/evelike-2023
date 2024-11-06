#ifndef solosnake_img_hpp
#define solosnake_img_hpp

#include <cassert>
#include "solosnake/filepath.hpp"
#include "solosnake/image.hpp"
#include "solosnake/noexcept.hpp"
#include "solosnake/deferred_renderer_types.hpp"

namespace solosnake
{
    //! Implementation of the deferred renderer iimg interface as a wrapper
    //! around the solosnake::image class.
    //! This image is always an RGBA , and its source file images must
    //! be in RGBA format.
    class img : public iimg
    {
    public:

        img()
        {
        }

        img( const filepath& );

        img( img&& i )
        {
            *this = std::move( i );
        }

        img& operator=( img && other )
        {
            if( this != &other )
            {
                img_ = std::move( other.img_ );
            }

            return *this;
        }

        img( image&& i ) : img_( std::move( i ) )
        {
            assert( img_.format() == solosnake::image::format_bgra );
        }

        explicit img( const solosnake::image& i ) : img_( i )
        {
            assert( i.format() == solosnake::image::format_bgra );
        }

        virtual ~img() SS_NOEXCEPT
        {
        }

        unsigned int img_width() const override
        {
            return img_.width();
        }

        unsigned int img_height() const override
        {
            return img_.height();
        }

        const unsigned char* img_bgra_pixels() const override
        {
            return img_.data();
        }

        bool operator==( const img& rhs ) const
        {
            return img_ == rhs.img_;
        }

        bool operator!=( const img& rhs ) const
        {
            return img_ != rhs.img_;
        }

        //! Very simple and basic implementation of a source for img.
        class src : public iimgsrc
        {
        public:

            explicit src( const filepath& p )
                : img_( std::make_shared<img>( p ) ), name_( p.string() )
            {
            }

            virtual ~src() SS_NOEXCEPT
            {
            }

            std::string get_image_name() const override
            {
                return name_;
            }

            iimg_ptr get_image() override
            {
                return img_;
            }

        private:
            iimg_ptr img_;
            std::string name_;
        };

    private:
        image img_;
    };
}

#endif
