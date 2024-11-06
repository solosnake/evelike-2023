#ifndef solosnake_quad_hpp
#define solosnake_quad_hpp

namespace solosnake
{
    //! A very basic unconstrained quad struct, used for specifying locations
    //! on texture maps and on screen rectangles.
    class quad
    {
    public:

        quad() : x_( 0.0f ), y_( 0.0f ), width_( 0.0f ), height_( 0.0f )
        {
        }

        quad( float x, float y, float w, float h ) : x_( x ), y_( y ), width_( w ), height_( h )
        {
        }

        bool contains( const float x, const float y ) const
        {
            return (x >= x_) && (y >= y_) && (x <= (x_+width_)) && (y <= (y_+height_));
        }

        float x_;
        float y_;
        float width_;
        float height_;
    };
}

#endif
