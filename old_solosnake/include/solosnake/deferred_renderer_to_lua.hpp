#ifndef deferred_renderer_to_lua_hpp
#define deferred_renderer_to_lua_hpp

#include "solosnake/external/lua.hpp"

namespace solosnake
{
    class deferred_renderer;

    //! Exposes a series of deferred renderer specific functions to lua.
    //! Only one renderer can be exposed to a lua state at any given time.
    //! The renderer must outlive the lua state.
    //! The following functions are exposed:
    //! set_debug_rendermode( const char* rendermodename );
    void expose_renderer_to_lua( 
        deferred_renderer*,
        lua_State* );
}

#endif
