#ifndef lua_hpp
#define lua_hpp
#include <memory>
#include <cstdint> // Finds INT_MAX, allows Lua to use > 16 bits internally.

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "solosnake/external/lua/lunar.hpp"

namespace solosnake
{
    /**
        * OO wrapper for Lua. Does not have ay libraries loaded.
        * Use luaL_openlibs(L) to load default libs.
        */
    class lua : public std::enable_shared_from_this<lua>
    {
    public:
        static std::shared_ptr<lua> create()
        {
            return std::shared_ptr<lua>( new lua() );
        }

        operator lua_State* ()
        {
            return L_;
        }

        operator const lua_State* () const
        {
            return L_;
        }

        ~lua()
        {
            if( L_ )
            {
                lua_gc( L_, LUA_GCCOLLECT, 0 );
                lua_close( L_ );
                L_ = 0;
            }
        }

    private:

        lua() : L_( luaL_newstate() ) // Was lua_open in Lua 5.1.
        {
        }

        lua_State* L_;
    };
}

#endif
