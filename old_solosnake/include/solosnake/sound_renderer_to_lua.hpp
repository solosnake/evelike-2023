#ifndef solosnake_sound_renderer_to_lua_hpp
#define solosnake_sound_renderer_to_lua_hpp

struct lua_State;

namespace solosnake
{
    class sound_renderer;

    //! Assumes the sound renderer and lua lifetimes are managed so that the
    //! lua object is destroyed before the sound renderer.
    //! Exposes global functions to load and play sounds files in Lua scripts.
    void expose_soundrenderer_to_lua( sound_renderer*, lua_State* );
}

#endif
