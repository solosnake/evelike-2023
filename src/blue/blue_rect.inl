#include "solosnake/blue/blue_rect.hpp"

namespace blue
{
    /// Returns true if (x,y) is in or on the rectangle.
    inline bool is_in_or_on_rect( const Rect& r, int x, int y )
    {
        return x >= r.left() && x <= r.right() && y >= r.top() && y <= r.bottom();
    }

    //--------------------------------------------------------------------------

    inline Rect::Rect( int lft, int tp, unsigned int w, unsigned int h )
        : left_( lft )
        , top_( tp )
        , width_( static_cast<int>(w) )
        , height_( static_cast<int>(h) )
    {
        assert( width_ >= 0 );
        assert( height_ >= 0 );
        assert( left() <= right() );
        assert( top() <= bottom() );
    }

    inline int Rect::left() const noexcept
    {
        return left_;
    }

    inline int Rect::top() const noexcept
    {
        return top_;
    }

    inline int Rect::right() const noexcept
    {
        return left_ + width_;
    }

    inline int Rect::bottom() const noexcept
    {
        return top_ + height_;
    }

    inline int Rect::width() const noexcept
    {
        return width_;
    }

    inline int Rect::height() const noexcept
    {
        return height_;
    }

    inline bool Rect::contains( int x, int y ) const noexcept
    {
        return ( x >= left_ && y >= top_ ) && ( x < ( right() ) ) && ( y < bottom() );
    }

    //--------------------------------------------------------------------------

    inline Rectf::Rectf( float lft, float tp, float w, float h )
        : left_( lft )
        , top_( tp )
        , width_( w )
        , height_( h )
    {
        assert( width_ >= 0 );
        assert( height_ >= 0 );
        assert( left() <= right() );
        assert( top() <= bottom() );
    }

    inline float Rectf::left() const noexcept
    {
        return left_;
    }

    inline float Rectf::top() const noexcept
    {
        return top_;
    }

    inline float Rectf::right() const noexcept
    {
        return left_ + width_;
    }

    inline float Rectf::bottom() const noexcept
    {
        return top_ + height_;
    }

    inline float Rectf::width() const noexcept
    {
        return width_;
    }

    inline float Rectf::height() const noexcept
    {
        return height_;
    }

    inline Point2d Rectf::centrepoint() const noexcept
    {
        return Point2d( width_ * 0.5f + left_, height_ * 0.5f + top_ );
    }
}
