#ifndef solosnake_colour_hpp
#define solosnake_colour_hpp

#include "solosnake/bgr.hpp"
#include "solosnake/bgra.hpp"

namespace solosnake
{
    //! Four float rgba colour type. Colour is allowed to exceed 1.0
    //! in RGBA to allow for special effects etc.
    class colour
    {
    public:

        static colour from_0xFF( std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a )
        {
            return colour( r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f );
        }

        inline colour() : r_( 0.0f ), g_( 0.0f ), b_( 0.0f ), a_( 0.0f )
        {
        }

        inline colour( float r, float g, float b ) : r_( r ), g_( g ), b_( b ), a_( 1.0f )
        {
        }

        inline colour( float r, float g, float b, float a ) : r_( r ), g_( g ), b_( b ), a_( a )
        {
        }

        inline colour( const colour& rgb, float a ) : r_( rgb.r_ ), g_( rgb.g_ ), b_( rgb.b_ ), a_( a )
        {
        }

        inline const float* rgba() const
        {
            return &r_;
        }

        inline float red() const
        {
            return r_;
        }

        inline float green() const
        {
            return g_;
        }

        inline float blue() const
        {
            return b_;
        }

        inline float alpha() const
        {
            return a_;
        }

        unsigned char ured() const
        {
            return static_cast<unsigned char>( r_ * 255.0f );
        }

        unsigned char ugreen() const
        {
            return static_cast<unsigned char>( g_ * 255.0f );
        }

        unsigned char ublue() const
        {
            return static_cast<unsigned char>( b_ * 255.0f );
        }

        unsigned char ualpha() const
        {
            return static_cast<unsigned char>( a_ * 255.0f );
        }

        colour interpolate( float f, const colour& other ) const;

        bool operator<( const colour& rhs ) const;
        bool operator==( const colour& rhs ) const;
        bool operator!=( const colour& rhs ) const;

        operator Bgra() const;
        operator const float* () const
        {
            return &r_;
        }

    private:
        float r_;
        float g_;
        float b_;
        float a_;
    };

    //-------------------------------------------------------------------------

    template <typename F> inline colour operator*( colour c, F f )
    {
        return colour( static_cast<float>( c.red() * f ),
                       static_cast<float>( c.green() * f ),
                       static_cast<float>( c.blue() * f ),
                       static_cast<float>( c.alpha() * f ) );
    }

    inline colour operator+( colour lhs, colour rhs )
    {
        return colour( lhs.red() + rhs.red(),
                       lhs.green() + rhs.green(),
                       lhs.blue() + rhs.blue(),
                       lhs.alpha() + rhs.alpha() );
    }

    //! Linearly interpolates between this colour and the other colour.
    inline colour colour::interpolate( float f, const colour& other ) const
    {
        assert( f >= 0.0f && f <= 1.0f );
        return colour(
                   f * ( other.r_ - r_ ) + r_,
                   f * ( other.g_ - g_ ) + g_,
                   f * ( other.b_ - b_ ) + b_,
                   f * ( other.a_ - a_ ) + a_ );
    }

    //! Returns a Bgra version of the colour.
    inline colour::operator Bgra() const
    {
        return Bgra( ublue(), ugreen(), ured(), ualpha() );
    }

    inline bool colour::operator<( const colour& rhs ) const
    {
        return ( r_ < rhs.r_ )
               || ( r_ == rhs.r_ && g_ < rhs.g_ )
               || ( r_ == rhs.r_ && g_ == rhs.g_ && b_ < rhs.b_ )
               || ( r_ == rhs.r_ && g_ == rhs.g_ && b_ == rhs.b_ && a_ < rhs.a_ );
    }

    inline bool colour::operator==( const colour& rhs ) const
    {
        return r_ == rhs.r_ && g_ == rhs.g_ && b_ == rhs.b_ && a_ == rhs.a_;
    }

    inline bool colour::operator!=( const colour& rhs ) const
    {
        return r_ != rhs.r_ || g_ != rhs.g_ || b_ != rhs.b_ || a_ != rhs.a_;
    }

    static_assert( sizeof( colour ) == ( 4 * sizeof( float ) ), "colour is padded" );
}

#endif
