#ifndef solosnake_set_vsynch_hpp
#define solosnake_set_vsynch_hpp

namespace solosnake
{
#define SS_VSYNCH_WHEN_POSSIBLE (-1)
#define SS_VSYNCH_OFF           (0)
#define SS_VSYNCH_ON            (1)

    //! Tries to set the requested interval.
    //! See http://www.opengl.org/registry/specs/EXT/wgl_swap_control.txt
    bool set_vsynch( const int interval );
}

#endif
