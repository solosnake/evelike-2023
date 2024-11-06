#include "solosnake/blue/blue_bgra.hpp"

namespace blue
{
    inline Bgra::Bgra(
      std::uint8_t b,
      std::uint8_t g,
      std::uint8_t r,
      std::uint8_t a )
      : bgr_( b, g, r )
      , alpha_( a )
    {
    }

    inline Bgra::Bgra(
      std::uint8_t b,
      std::uint8_t g,
      std::uint8_t r )
      : bgr_( b, g, r )
      , alpha_( 0xFF )
    {
    }

    inline Bgra::Bgra(
      const Bgr& c,
      std::uint8_t a )
      : bgr_( c )
      , alpha_( a )
    {
    }

    inline Bgra::Bgra( std::uint8_t x )
      : bgr_( x, x, x )
      , alpha_( x )
    {
    }

    inline Bgra::Bgra( const std::uint8_t* p )
      : bgr_( p[0], p[1], p[2] )
      , alpha_( p[3] )
    {
    }

    inline std::uint8_t Bgra::blue() const
    {
      return bgr_.blue();
    }

    inline std::uint8_t Bgra::green() const
    {
      return bgr_.green();
    }

    inline std::uint8_t Bgra::red() const
    {
      return bgr_.red();
    }

    inline std::uint8_t Bgra::alpha() const
    {
      return alpha_;
    }

    inline Bgr Bgra::blue_green_red() const
    {
      return bgr_;
    }

    inline bool Bgra::operator==( const Bgra& other ) const
    {
      return bgr_ == other.bgr_ && alpha_ == other.alpha_;
    }

    inline bool Bgra::operator!=( const Bgra& other ) const
    {
      return !( *this == other );
    }

    inline bool Bgra::operator<( const Bgra& rhs ) const
    {
      return bgr_ < rhs.bgr_ || ( bgr_ == rhs.bgr_ && alpha_ < rhs.alpha_ );
    }

    inline Bgra Bgra::from_floats( const float b, const float g, const float r, const float a )
    {
      return Bgra(
        static_cast<std::uint8_t>( b * 255.0f ),
        static_cast<std::uint8_t>( g * 255.0f ),
        static_cast<std::uint8_t>( r * 255.0f ),
        static_cast<std::uint8_t>( a * 255.0f ) );
    }

    inline Bgra Bgra::operator*( const float f ) const
    {
      return Bgra(
        static_cast<std::uint8_t>( f * blue() ),
        static_cast<std::uint8_t>( f * green() ),
        static_cast<std::uint8_t>( f * red() ),
        static_cast<std::uint8_t>( f * alpha() ) );
    }

    inline Bgra operator*( const float f, const Bgra& c )
    {
      return Bgra(
        static_cast<std::uint8_t>( std::min<float>( 255.0f, f * c.blue()  ) ),
        static_cast<std::uint8_t>( std::min<float>( 255.0f, f * c.green() ) ),
        static_cast<std::uint8_t>( std::min<float>( 255.0f, f * c.red()   ) ),
        static_cast<std::uint8_t>( std::min<float>( 255.0f, f * c.alpha() ) ) );
    }

    inline bool Bgra::non_zero() const
    {
        return bgr_.non_zero() || alpha_ > 0;
    }

    inline Bgra& Bgra::operator /= (const std::uint8_t rhs )
    {
        bgr_   /= rhs;
        alpha_ /= rhs;
        return *this;
    }

    inline Bgra& Bgra::operator *= ( const float rhs )
    {
        bgr_   *= rhs;
        alpha_ = static_cast<std::uint8_t>( rhs * alpha_ );
        return *this;
    }

    inline void Bgra::quench( const std::uint8_t cutoff )
    {
        bgr_.quench( cutoff );
        alpha_ = alpha_ < cutoff ? 0 : alpha_;
    }

    inline std::uint32_t Bgra::strength() const
    {
        return bgr_.strength() * std::max<std::uint32_t>( alpha_, 1 );
    }
}
