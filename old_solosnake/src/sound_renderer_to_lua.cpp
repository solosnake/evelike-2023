#include "solosnake/sound_renderer_to_lua.hpp"
#include "solosnake/external/lua.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/sound_renderer.hpp"
#include <cassert>

namespace solosnake
{
    namespace
    {
        //! Loads the named sound and returns its handle cast to a lua_Integer.
        lua_Integer load_sound( sound_renderer* sounds, const char* soundfilename )
        {
            lua_Integer snd = 0;

            try
            {
                auto h = sounds->load_buffer( soundfilename );
                snd = static_cast<lua_Integer>( h );
            }
            catch( const SS_EXCEPTION_TYPE& e )
            {
                ss_err( "Exception '", e.what(), "' while loading sound: ", soundfilename );
                assert( !"Exception loading sound." );
            }

            return snd;
        }

        void play_sound( sound_renderer* sounds, lua_Integer h )
        {
            try
            {
                sounds->play_buffer( static_cast<sound_renderer::BufferHandle>( h ) );
            }
            catch( const SS_EXCEPTION_TYPE& e )
            {
                ss_err( "Exception '", e.what(), "' while playing sound: ", h );
                assert( !"Exception playing sound." );
            }
        }

        //! Calls load_sound.
        int lua_load_sound( lua_State* L )
        {
            // Get upvalue (the sound_renderer*)
            sound_renderer* p = reinterpret_cast<sound_renderer*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );

            if( lua_isstring( L, 1 ) )
            {
                lua_pushinteger( L, load_sound( p, luaL_checkstring( L, 1 ) ) );
            }

            return 1;
        }

        //! Calls load_sound.
        int lua_play_sound( lua_State* L )
        {
            // Get upvalue (the menuscreen*)
            sound_renderer* p = reinterpret_cast<sound_renderer*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );

            if( lua_isnumber( L, 1 ) )
            {
                play_sound( p, luaL_checkinteger( L, 1 ) );
            }

            return 0;
        }
    }

    void expose_soundrenderer_to_lua( sound_renderer* sounds, lua_State* L )
    {
        assert( L );
        assert( sounds );

        lua_pushlightuserdata( L, sounds );
        lua_pushcclosure( L, &lua_load_sound, 1 );
        lua_setglobal( L, "load_sound" );

        lua_pushlightuserdata( L, sounds );
        lua_pushcclosure( L, &lua_play_sound, 1 );
        lua_setglobal( L, "play_sound" );
    }
}