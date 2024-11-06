#ifndef blue_iboardview_hpp
#define blue_iboardview_hpp

#include <memory>
#include "solosnake/point.hpp"
#include "solosnake/radians.hpp"

namespace solosnake
{
    class screenxy;
}

namespace blue
{
    struct pickresult;
    struct Hex_coord;
    class Instructions;
    class Machine;

    //! Interface of board presented to widgets etc. If the widgets are
    //! the 'Controller' in MVC, this is the 'Model'.
    class iboardview
    {
    public:

        virtual ~iboardview();

        virtual pickresult get_boardpick( solosnake::screenxy xy ) const = 0;

        //! Assumes the eye is looking towards the board, returns the
        //! pick point of the screen position xy in the board plane.
        virtual solosnake::point3d get_boardplanepick( solosnake::screenxy xy ) const = 0;

        virtual solosnake::point3d camera_location() const = 0;

        virtual solosnake::point3d camera_lookedat() const = 0;

        virtual void translate_camera( float x, float z )  = 0;

        virtual void rotate_camera( solosnake::radians )  = 0;

        virtual void move_camera_forwards( float units )  = 0;
    };
}

#endif
