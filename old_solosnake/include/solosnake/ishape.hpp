#ifndef solosnake_ishape_hpp
#define solosnake_ishape_hpp

namespace solosnake
{

    //! A shape is a closed space with an inside and an outside. It is used
    //! in the GUI code for customising widgets. It is a 2D object.
    //!
    //! N.B. The shape is used to define a sub area within a rectangle that
    //!      is sensitive to mouse clicks - to render this, use an alpha blended
    //!      rectangle - there is no need to change any aspects of rendering.
    class ishape
    {
    public:
        virtual ~ishape();

        //! The shape bounding area should lie inside 0,0 to 1,1. 0,0 is
        //! considered to be inside the bounding area, 1,1 is outside.
        //! 0,0 is the top-left corner location.
        virtual bool is_xy_inside_shape( float x, float y ) const = 0;
    };
}

#endif
