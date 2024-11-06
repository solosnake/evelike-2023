#ifndef blue_dimension_hpp
#define blue_dimension_hpp

#include <cassert>

namespace blue
{
    /// Used to indicate types that have width & height, e.g. rectangles etc.
    template <typename T> class Dimension2d
    {
    public:

        Dimension2d() : width_( T() ), height_( T() )
        {
        }

        Dimension2d( T w, T h ) : width_( w ), height_( h )
        {
            assert( w == 0 || w > 0 );
            assert( h == 0 || h > 0 );
        }

        T width() const
        {
            return width_;
        }

        T height() const
        {
            return height_;
        }

    private:
        T width_;
        T height_;
    };


    /// Used to indicate types that have width & height, e.g. rectangles etc.
    template <typename T> class dimension3d
    {
    public:

        dimension3d() : width_( T() ), height_( T() ), depth_( T() )
        {
        }

        dimension3d( T w, T h, T d ) : width_( w ), height_( h ), depth_( d )
        {
            assert( w == 0 || w > 0 );
            assert( h == 0 || h > 0 );
            assert( d == 0 || d > 0 );
        }

        T width() const
        {
            return width_;
        }

        T height() const
        {
            return height_;
        }

        T depth() const
        {
            return depth_;
        }

        T x() const
        {
            return width_;
        }

        T y() const
        {
            return height_;
        }

        T z() const
        {
            return depth_;
        }

    private:
        T width_;
        T height_;
        T depth_;
    };

    typedef Dimension2d<float>         dimension2df;
    typedef Dimension2d<int>           dimension2di;
    typedef Dimension2d<unsigned int>  dimension2dui;

    typedef dimension3d<float>         dimension3df;
    typedef dimension3d<int>           dimension3di;
    typedef dimension3d<unsigned int>  dimension3dui;
}

#endif
