#include <cassert>

#include "solosnake/blue/blue_bgr.hpp"

namespace blue
{
    inline Bgr::Bgr( std::uint8_t x ) noexcept
        : blue_( x ), green_( x ), red_( x )
    {
    }

    inline Bgr::Bgr( std::uint8_t b, std::uint8_t g, std::uint8_t r ) noexcept
        : blue_( b ), green_( g ), red_( r )
    {
    }

    inline Bgr::Bgr( const std::uint8_t* pixelsBGR ) noexcept
        : blue_( pixelsBGR[0] ), green_( pixelsBGR[1] ), red_( pixelsBGR[2] )
    {
        assert( pixelsBGR );
    }

    inline std::uint8_t Bgr::blue() const noexcept
    {
        return blue_;
    }

    inline std::uint8_t Bgr::green() const noexcept
    {
        return green_;
    }

    inline std::uint8_t Bgr::red() const noexcept
    {
        return red_;
    }

    inline bool Bgr::operator==( const Bgr& rhs ) const noexcept
    {
        return blue_ == rhs.blue_ && green_ == rhs.green_ && red_ == rhs.red_;
    }

    inline bool Bgr::operator!=( const Bgr& other ) const noexcept
    {
        return false == ( *this == other );
    }

    inline bool Bgr::operator<( const Bgr& rhs ) const noexcept
    {
        return ( blue_ < rhs.blue_ ) || ( blue_ == rhs.blue_ && green_ < rhs.green_ )
               || ( blue_ == rhs.blue_ && green_ == rhs.green_ && red_ < rhs.red_ );
    }

    inline Bgr Bgr::from_floats( float b, float g, float r ) noexcept
    {
        return Bgr( static_cast<std::uint8_t>( b * 255.0f ),
                    static_cast<std::uint8_t>( g * 255.0f ),
                    static_cast<std::uint8_t>( r * 255.0f ) );
    }

    inline Bgr operator*( const float f, const Bgr& c ) noexcept
    {
        return Bgr( static_cast<std::uint8_t>( std::min<float>( 255.0f, f * c.blue() ) ),
                    static_cast<std::uint8_t>( std::min<float>( 255.0f, f * c.green() ) ),
                    static_cast<std::uint8_t>( std::min<float>( 255.0f, f * c.red() ) ) );
    }

    inline bool Bgr::non_zero() const noexcept
    {
        return blue_ > 0 || green_ > 0 || red_ > 0;
    }

    inline Bgr& Bgr::operator /= ( const std::uint8_t rhs ) noexcept
    {
        blue_  /= rhs;
        green_ /= rhs;
        red_   /= rhs;
        return *this;
    }

    inline Bgr& Bgr::operator *= ( const float rhs ) noexcept
    {
        blue_  = static_cast<std::uint8_t>( rhs * blue_ );
        green_ = static_cast<std::uint8_t>( rhs * green_ );
        red_   = static_cast<std::uint8_t>( rhs * red_ );
        return *this;
    }

    inline void Bgr::quench( const std::uint8_t cutoff ) noexcept
    {
        blue_  = blue_ < cutoff ? 0 : blue_;
        green_ = green_ < cutoff ? 0 : green_;
        red_   = red_ < cutoff ? 0 : red_;
    }

    inline std::uint32_t Bgr::strength() const noexcept
    {
        return std::max<std::uint32_t>( blue_, 1 )
               * std::max<std::uint32_t>( green_, 1 )
               * std::max<std::uint32_t>( red_, 1 );
    }
}
