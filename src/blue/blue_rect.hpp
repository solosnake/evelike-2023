#ifndef blue_rect_hpp
#define blue_rect_hpp

#include <cassert>
#include "solosnake/blue/blue_point2d.hpp"

namespace blue
{
    /// A rect which assumes that left <= right and top <= bottom.
    /// Microsoft Windows: "For screen coordinates, the origin is the
    /// upper-left corner of the screen."
    ///
    /// top/left
    ///     + - +
    ///     |   |
    ///     + - +
    ///          bottom/right = top + height/left + width
    ///
    ///  left < right
    ///  top  < bottom
    class Rect
    {
    public:

        Rect() = default;

        //Rect( int lft, int tp, const dimension2dui& wh );

        Rect( int lft, int tp, unsigned int w, unsigned int h );

        int left() const noexcept;

        int top() const noexcept;

        int right() const noexcept;

        int bottom() const noexcept;

        int width() const noexcept;

        int height() const noexcept;

        /// Inclusive of top and left and exclusive of bottom and right.
        bool contains( int x, int y ) const noexcept;

    private:

        int left_   = 0;
        int top_    = 0;
        int width_  = 0;
        int height_ = 0;
    };

    /// A rect which assumes that left <= right and top <= bottom.
    /// Microsoft Windows: "For screen coordinates, the origin is the
    /// upper-left corner of the screen."
    ///
    /// top/left
    ///     + - +
    ///     |   |
    ///     + - +
    ///          bottom/right = top + height/left + width
    ///
    ///  left < right
    ///  top  < bottom
    class Rectf
    {
    public:

        Rectf() = default;

        Rectf( float lft, float tp, float w, float h );

        float left() const noexcept;

        float top() const noexcept;

        float right() const noexcept;

        float bottom() const noexcept;

        float width() const noexcept;

        float height() const noexcept;

        Point2d centrepoint() const noexcept;

    private:

        float left_     = 0.0f;
        float top_      = 0.0f;
        float width_    = 0.0f;
        float height_   = 0.0f;
    };
}

#include "solosnake/blue/blue_rect.inl"
#endif
