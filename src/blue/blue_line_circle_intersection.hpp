#ifndef blue_line_circle_intersection_hpp
#define blue_line_circle_intersection_hpp

namespace blue
{
    class Line2d;
    class Circle;
    class Intersection2d;

    /// Returns the intersection of the line @a L and the Circle @a C.
    Intersection2d line_circle_intersection( const Line2d& L, const Circle& C );
}

#endif
