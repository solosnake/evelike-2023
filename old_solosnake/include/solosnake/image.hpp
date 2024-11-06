#ifndef solosnake_image_hpp
#define solosnake_image_hpp

#include <vector>
#include <iosfwd>
#include <memory>
#include <string>
#include <cstdint>
#include "solosnake/filepath.hpp"
#include "solosnake/bgr.hpp"
#include "solosnake/bgra.hpp"
#include "solosnake/nothrow.hpp"

namespace solosnake
{
    //! Class for loading and saving of images. Handles many DIB and BMP image
    //! types, and saves Windows BMP compatible files. Supports smoothed basic
    //! triangle rendering and some pixel and pixel-plane operations.
    //! Images with alpha - 0x0 is fully transparent, 0xFF is full opaque.
    class image
    {
    public:

        enum colorformat
        {
            format_bgr,
            format_bgra
        };

        image();

        image( image&& );

        image( const image& );

        image( unsigned int w, unsigned int h, colorformat );

        image( unsigned int w, unsigned int h, solosnake::Bgra );

        image( unsigned int w, unsigned int h, solosnake::Bgr );

        image( std::istream&, bool widen = false );

        image( const filepath&, bool widen = false );

        ~image() SS_NOEXCEPT
        {
        }

        bool open( const filepath&, bool widen = false ) SS_NOEXCEPT;

        colorformat format() const;

        size_t layers_count() const;

        unsigned int width() const;

        unsigned int height() const;

        unsigned int bytes_size() const;

        const std::uint8_t* data() const;

        std::uint8_t* data();

        bool save( std::ostream& ) const;

        bool save( const char* filename ) const;

        bool save( const std::string& filename ) const;

        image& operator=( image && );

        bool operator==( const image& ) const;

        bool operator!=( const image& ) const;

        void flip_vertically();

        image flipped() const;

        bool empty() const;

        void swap( image& ) SS_NOEXCEPT;

        void clear_to_bgra( const Bgra& c )
        {
            clear_to_bgra( c.blue(), c.green(), c.red(), c.alpha() );
        }

        void clear_to_bgra( std::uint8_t b, std::uint8_t g, std::uint8_t r, std::uint8_t a );

        void draw_triangle( float x1,
                            float y1,
                            const Bgra&,
                            float x2,
                            float y2,
                            const Bgra&,
                            float x3,
                            float y3,
                            const Bgra& );

        void set_pixel( const size_t x, const size_t y, const Bgra& );

        const std::uint8_t* pixel_at( const size_t x, const size_t y ) const;

        static std::uint8_t* pixel( const size_t x, const size_t y, image& i, const colorformat f );

        static const std::uint8_t* pixel( const size_t x, const size_t y, const image& i, const colorformat f );

        static void copy_layer( const image& src, size_t srcLayer, image& dest, size_t destLayer );

        static int compare_layer( const image& src,
                                  const size_t srcLayer,
                                  const image& dest,
                                  const size_t destLayer );

        static void clear_layer_to( image& src, const size_t layer, const std::uint8_t value );

    private:
        std::vector<std::uint8_t> img_;
        unsigned int width_;
        unsigned int height_;
    };


    //-------------------------------------------------------------------------

    inline image::image() : img_(), width_( 0 ), height_( 0 )
    {
    }

    inline const std::uint8_t* image::data() const
    {
        return img_.data();
    }

    inline std::uint8_t* image::data()
    {
        return img_.data();
    }

    inline unsigned int image::bytes_size() const
    {
        return static_cast<unsigned int>( img_.size() );
    }

    inline size_t image::layers_count() const
    {
        return format() == image::format_bgr ? 3 : 4;
    }

    inline unsigned int image::height() const
    {
        return height_;
    }

    inline unsigned int image::width() const
    {
        return width_;
    }

    inline image& image::operator=( image && rhs )
    {
        height_ = rhs.height_;
        width_ = rhs.width_;
        img_ = std::move( rhs.img_ );
        return *this;
    }

    inline image::image( image&& other )
    {
        *this = std::move( other );
    }

    inline bool image::operator==( const image& rhs ) const
    {
        return width_ == rhs.width_ && height_ == rhs.height_ && img_ == rhs.img_;
    }

    inline bool image::operator!=( const image& rhs ) const
    {
        return width_ != rhs.width_ || height_ != rhs.height_ || img_ != rhs.img_;
    }

    inline bool image::empty() const
    {
        return img_.empty();
    }
}

#endif
