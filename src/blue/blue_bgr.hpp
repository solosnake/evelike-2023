#ifndef blue_bgr_hpp
#define blue_bgr_hpp

#include <algorithm>
#include <cstdint>
#include <string>

namespace blue
{
    /// Blue / green / red byte colour type.
    class Bgr
    {
    public:

        Bgr() = default;

        explicit Bgr( std::uint8_t x ) noexcept;

        explicit Bgr( std::uint8_t b, std::uint8_t g, std::uint8_t r ) noexcept;

        explicit Bgr( const std::uint8_t* pixelsBGR ) noexcept;

        /// Constructs from uint32 colour as string, either base 10 or 16.
        /// @code
        /// Bgra myColour( "0x00AA4200" );
        /// Bgra white(  "0xFFFFFF00" );
        /// Bgra red(    "0x0000FF00" );
        /// Bgra green(  "0x00FF0000" );
        /// Bgra blue(   "0xFF000000" );
        /// @endcode
        explicit Bgr( const std::string& );

        /// Returns true of any element is non zero.
        bool non_zero() const noexcept;

        /// Any elements with values below cutoff will be set to zero.
        void quench( const std::uint8_t cutoff ) noexcept;

        /// Returns the product of all non zero elements.
        std::uint32_t strength() const noexcept;

        std::uint8_t blue() const noexcept;

        std::uint8_t green() const noexcept;

        std::uint8_t red() const noexcept;

        std::uint32_t to_uint32() const noexcept;

        bool operator == ( const Bgr& rhs ) const noexcept;

        bool operator != ( const Bgr& other ) const noexcept;

        bool operator < ( const Bgr& rhs ) const noexcept;

        Bgr& operator /= ( const std::uint8_t rhs ) noexcept;

        Bgr& operator *= (const float rhs ) noexcept;

        static Bgr from_floats( float b, float g, float r ) noexcept;

    private:

        std::uint8_t blue_{0u};
        std::uint8_t green_{0u};
        std::uint8_t red_{0u};
    };
}

#include "solosnake/blue/blue_bgr.inl"

#endif
