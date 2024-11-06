#ifndef blue_hex_steps_hpp
#define blue_hex_steps_hpp

#include <functional>

namespace blue
{
    //! Calls back the provided function for every hexagonal tile under the
    //! line between the tiles [startx,starty] to [endx,endy], including
    //! [endx,endy] and excluding [startx,starty]. This is not the same as the
    //! minimum steps needed to move from start to end.
    //! If the callback function returns false then the algorithm is stopped
    //! and no more tiles are processed.
    void hex_steps_under_line(
        int startx, int starty, int endx, int endy, std::function<bool( int, int )> );
}

#endif
