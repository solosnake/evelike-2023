#ifndef solosnake_image_hpp
#define solosnake_image_hpp

#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <cstdint>

namespace solosnake
{
    struct Bgr {
        std::uint8_t blue;
        std::uint8_t green;
        std::uint8_t red;
    };

    struct Bgra {
        std::uint8_t blue;
        std::uint8_t green;
        std::uint8_t red;
        std::uint8_t alpha;
    };

    //! Class for loading and saving of images. Handles many DIB and BMP Image
    //! types, and saves Windows BMP compatible files. Supports some pixel and
    //! pixel-plane operations. Images with alpha - 0x0 is fully transparent,
    //! 0xFF is full opaque.
    class Image
    {
    public:

        enum class Format
        {
            BGR,
            BGRA
        };

        Image() = default;

        Image( unsigned int w, unsigned int h, Format );

        Image( unsigned int w, unsigned int h, Bgra );

        Image( unsigned int w, unsigned int h, Bgr );

        Image( unsigned int width, const std::vector<Bgra>& pixels );

        Image( const std::string_view& );

        Image( const std::string_view&, Format );

        bool open( const std::string_view& ) noexcept;

        bool open( const std::string_view&, Format ) noexcept;

        Format format() const;

        std::size_t layers_count() const;

        std::size_t pixels_count() const;

        std::uint32_t width() const;

        std::uint32_t height() const;

        std::size_t   bytes_size() const;

        const std::uint8_t* data() const;

        std::uint8_t* data();

        const std::uint8_t* pixels() const;

        std::uint8_t* pixels();

        bool save( const std::string_view& filename ) const;

        bool operator==( const Image& ) const;

        bool operator!=( const Image& ) const;

        void flip_vertically();

        Image flipped() const;

        bool empty() const;

        void clear_to_bgra( const Bgra& c );

        void clear_to_bgra( std::uint8_t b, std::uint8_t g, std::uint8_t r, std::uint8_t a );

        void set_pixel( std::size_t x, std::size_t y, const Bgra& );

        const std::uint8_t* pixel_at( std::size_t x, std::size_t y ) const;

        static std::uint8_t* pixel( std::size_t x, std::size_t y, Image& i, const Format f );

        static const std::uint8_t* pixel( std::size_t x, std::size_t y, const Image& i, const Format f );

        static void copy_layer( const Image& src, std::size_t srcLayer, Image& dest, std::size_t destLayer );

        static int compare_layer( const Image& src,
                                  std::size_t srcLayer,
                                  const Image& dest,
                                  std::size_t destLayer );

        static void clear_layer_to( Image& src, std::size_t layer, std::uint8_t value );

    private:

        std::vector<std::uint8_t>   img_;
        std::uint32_t               width_  { 0u };
        std::uint32_t               height_ { 0u };
    };
}

#endif
