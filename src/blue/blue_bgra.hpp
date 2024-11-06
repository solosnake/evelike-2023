#ifndef blue_bgra_hpp
#define blue_bgra_hpp

#include <string>
#include <cstdint>
#include "solosnake/blue/blue_bgr.hpp"

namespace blue
{
    /// Blue / green / red / alpha byte colour type. 0xFF is fully opaque alpha,
    /// 0x00 is fully transparent. Default colour is Transparent Black (0,0,0,0).
    class Bgra
    {
    public:

        Bgra() = default;

        explicit Bgra(
            std::uint8_t b,
            std::uint8_t g,
            std::uint8_t r,
            std::uint8_t a );

        explicit Bgra(
            std::uint8_t b,
            std::uint8_t g,
            std::uint8_t r );

        explicit Bgra(
            const Bgr& c,
            std::uint8_t a );

        /// Sets all r g b and a to x.
        explicit Bgra( std::uint8_t x );

        /// Sets all r g b and a to x.
        explicit Bgra( const std::uint8_t* p );

        /// Constructs from uint32 colour as string, either base 10 or 16.
        /// @code
        /// Bgra myColour( "0xFF00AA42" );
        /// Bgra white( "0xFFFFFFFF" );
        /// Bgra red( "0x0000FFFF" );
        /// Bgra red1( "65535" );
        /// assert( red1 == red );
        /// @endcode
        explicit Bgra( const std::string& );

        /// Returns true of any element rbga is non zero.
        bool non_zero() const;

        /// Any elements with values below cutoff will be set to zero.
        void quench( const std::uint8_t cutoff );

        /// Returns the product of all non zero elements.
        std::uint32_t strength() const;

        std::uint8_t blue() const;

        std::uint8_t green() const;

        std::uint8_t red() const;

        std::uint8_t alpha() const;

        Bgr blue_green_red() const;

        bool operator==( const Bgra& other ) const;

        bool operator!=( const Bgra& other ) const;

        bool operator<( const Bgra& rhs ) const;

        Bgra& operator /= ( const std::uint8_t rhs );

        Bgra& operator *= ( const float rhs );

        Bgra operator*( const float f ) const;

        std::uint32_t to_uint32() const;

        static Bgra from_floats( const float b, const float g, const float r, const float a );

    private:

        Bgr           bgr_   = {};
        std::uint8_t  alpha_ = 0u;
    };
}

#include "solosnake/blue/blue_bgra.inl"
#endif
