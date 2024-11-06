#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/throw.hpp"
#include <cassert>

namespace solosnake
{
    namespace
    {
        // Method is exposed to Lua as global "void exit_screen( <optional screen name> )".
        int lua_guicommandexecutor_exit_screen( lua_State* L )
        {
            // Get upvalue (the luaguicommandexecutor*)
            auto p = reinterpret_cast<luaguicommandexecutor*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
            assert( p );

            if( lua_isstring( L, 1 ) )
            {
                p->set_next_screen( luaL_checkstring( L, 1 ) );
            }

            p->end_screen();

            return 0;
        }
    }

    //! Required to protect shared-from-this.
    std::shared_ptr<luaguicommandexecutor> luaguicommandexecutor::make_shared()
    {
        return std::make_shared<luaguicommandexecutor>( HeapOnly() );
    }

    //! Create Lua state and open libraries and add any required
    //! functionality.
    luaguicommandexecutor::luaguicommandexecutor( const HeapOnly& ) : lua_( 0 ), screen_ended_( false )
    {
        lua_State* L = luaL_newstate();

        if( nullptr == L )
        {
            ss_throw( "lua_open() failed." );
        }

        luaL_openlibs( L );
        lua_ = L;

        // Expose a function "exit_screen" to Lua, which takes an
        // optional string param (the next screen to transition to).
        lua_pushlightuserdata( L, this );
        lua_pushcclosure( L, &lua_guicommandexecutor_exit_screen, 1 );
        lua_setglobal( L, "exit_screen" );
    }

    //! Garbage collect and close Lua and zero pointer.
    luaguicommandexecutor::~luaguicommandexecutor()
    {
        try
        {
            lua_State* L = lua_;
            lua_ = 0;

            lua_gc( L, LUA_GCCOLLECT, 0 );
            lua_close( L );
        }
        catch( ... )
        {
        }
    }

    //! Should return true if the screen should exit and transition.
    bool luaguicommandexecutor::is_screen_ended() const
    {
        return screen_ended_;
    }

    //! Returns the nextscreen object to which this screen will
    //! transition to when exit screen is called.
    nextscreen luaguicommandexecutor::get_next_screen() const
    {
        return next_screen_;
    }

    //! Sets the transition screen name.
    void luaguicommandexecutor::set_next_screen( const std::string& screenname )
    {
        next_screen_ = nextscreen( screenname );
    }

    //! is_screen_ended will return true after this call, and the screen loop
    //! will transition to the screen returned by 'next_screen'.
    void luaguicommandexecutor::end_screen()
    {
        screen_ended_ = true;
    }
}
