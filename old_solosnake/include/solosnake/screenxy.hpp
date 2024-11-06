#ifndef solosnake_screenxy_hpp
#define solosnake_screenxy_hpp

#include <cassert>
#include <cstdint>

namespace solosnake
{
    //! Screen coordinates are laid out as follows:
    //!
    //!         0,0                    1,0
    //!            +------------------+
    //!            |                  |
    //!            |         +        |
    //!            |                  |
    //!            +------------------+
    //!         0,1                    1,1
    //!
    //!  Max supported screen dimension is 32768.
    class screenxy
    {
    public:

        screenxy() : x_( 0 ), y_( 0 )
        {
        }

        explicit screenxy( std::uint32_t xy )
            : x_( static_cast<std::int16_t>( xy & 0x0000FFFFU ) )
            , y_( static_cast<std::int16_t>( ( xy & 0xFFFF0000U ) >> 16 ) )
        {
            assert( screen_xy() == xy );
        }

        screenxy( std::int16_t x, std::int16_t y ) : x_( x ), y_( y )
        {
            assert( screenxy( screen_xy() ).screen_x() == x );
            assert( screenxy( screen_xy() ).screen_y() == y );
        }

        //! The coordinate is relative to the upper-left corner of the client area.
        int screen_x() const
        {
            return x_;
        }

        //! The coordinate is relative to the upper-left corner of the client area.
        int screen_y() const
        {
            return y_;
        }

        std::uint32_t screen_xy() const
        {
            return ( 0x0000FFFFU & static_cast<std::uint32_t>( x_ ) )
                 | ( 0xFFFF0000U & static_cast<std::uint32_t>( y_ ) << 16 );
        }

        bool operator==( const screenxy& rhs ) const
        {
            return x_ == rhs.x_ && y_ == rhs.y_;
        }

        bool operator!=( const screenxy& rhs ) const
        {
            return x_ != rhs.x_ || y_ != rhs.y_;
        }

        //! Returns a clamped screenxy: it will be within 0 to width and 0 to
        //! height, inclusive.
        screenxy clamped( std::int16_t width, std::int16_t height ) const
        {
            assert( width > 0 && height > 0 );

            screenxy result = *this;

            if( result.x_ < 0 )
            {
                result.x_ = 0;
            }
            else if( result.x_ > width )
            {
                result.x_ = width;
            }

            if( result.y_ < 0 )
            {
                result.y_ = 0;
            }
            else if( result.y_ > height )
            {
                result.y_ = height;
            }

            return result;
        }

    private:
        std::int16_t x_;
        std::int16_t y_;
    };
}

#endif
