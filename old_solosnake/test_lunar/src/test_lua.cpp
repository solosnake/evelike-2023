#include "solosnake/external/lua.hpp"
#include <iostream>
#include <fstream>
#include "solosnake/testing/testing.hpp"

namespace
{

    bool report_errors( lua_State* L, int status )
    {
        bool ok = true;

        if( status != 0 )
        {
            ok = false;
            std::cerr << "-- " << lua_tostring( L, -1 ) << std::endl;
            lua_pop( L, 1 ); // remove error message
        }

        return ok;
    }

    void write_lua_file( const char* filename )
    {
        const char luatest[] = "print (\"Hello World!\")\n";
        std::ofstream ofile( filename );
        ofile << luatest;
    }

    bool do_lua_file( const char* filename )
    {
        write_lua_file( filename );

        lua_State* L = luaL_newstate();

        luaL_openlibs( L ); // provides io.*

        std::cerr << "-- Loading file: " << filename << std::endl;

        int s = luaL_loadfile( L, filename );

        if( s == 0 )
        {
            // execute Lua program
            s = lua_pcall( L, 0, LUA_MULTRET, 0 );
        }

        report_errors( L, s );
        lua_close( L );

        std::cerr << std::endl;

        return s == 0;
    }
}

TEST( lua, testfile )
{
    {
        EXPECT_TRUE( do_lua_file( "lua_test.txt" ) );
    }
}

namespace
{
    int global = 0;

    int SetGlobal( lua_State* L )
    {
        global = static_cast<int>( luaL_checknumber( L, 1 ) );
        return 0;
    }
}

TEST( lua, luaL_loadbuffer_does_not_execute_code )
{
    {
        lua_State* L = luaL_newstate();
        luaL_openlibs( L );

        // Add function to lua:
        lua_pushcfunction( L, SetGlobal );
        lua_setglobal( L, "SetGlobal" );

        const char f[] = "print (\"In Snippet...\")\nSetGlobal(12345)\n";
        int s = luaL_loadbuffer( L, f, strlen( f ), "f" );

        EXPECT_FALSE( global == 12345 );

        if( s == 0 )
        {
            // execute Lua snippet
            s = lua_pcall( L, 0, LUA_MULTRET, 0 );

            EXPECT_TRUE( global == 12345 );
        }

        bool ok = report_errors( L, s );
        lua_close( L );

        EXPECT_TRUE( ok );
    }
}

TEST( lua, luaL_loadbuffers_can_work_together )
{
    {
        int ok = 0;

        global = 0;

        lua_State* L = luaL_newstate();
        luaL_openlibs( L );

        // Add function to lua:
        lua_pushcfunction( L, SetGlobal );
        lua_setglobal( L, "SetGlobal" );

        const char f[] = "print (\"In Snippet1 ...\")\n\n";
        int s1 = luaL_loadbuffer( L, f, strlen( f ), "f" );
        ok = lua_pcall( L, 0, LUA_MULTRET, 0 );
        report_errors( L, ok );
        EXPECT_TRUE( ok == 0 );
        EXPECT_TRUE( s1 == 0 );

        const char g[] = "function g()\nprint (\"In g...\")\nSetGlobal(2)\nend\n";
        int s2 = luaL_loadbuffer( L, g, strlen( g ), "g" );
        ok = lua_pcall( L, 0, LUA_MULTRET, 0 );
        report_errors( L, ok );
        EXPECT_TRUE( ok == 0 );
        EXPECT_TRUE( s2 == 0 );

        const char h[] = "function h()\nprint (\"In h...\")\nSetGlobal(3)\nend\n";
        int s3 = luaL_loadbuffer( L, h, strlen( h ), "h" );
        ok = lua_pcall( L, 0, LUA_MULTRET, 0 );
        report_errors( L, ok );
        EXPECT_TRUE( ok == 0 );
        EXPECT_TRUE( s3 == 0 );

        EXPECT_TRUE( global == 0 );

        if( s1 == 0 && s2 == 0 && s3 == 0 )
        {
            lua_getglobal( L, "h" );
            ok = lua_pcall( L, 0, LUA_MULTRET, 0 );
            report_errors( L, ok );
            EXPECT_TRUE( ok == 0 );
            EXPECT_TRUE( global == 3 );

            lua_getglobal( L, "g" );
            ok = lua_pcall( L, 0, LUA_MULTRET, 0 );
            report_errors( L, ok );
            EXPECT_TRUE( ok == 0 );
            EXPECT_TRUE( global == 2 );
        }

        lua_close( L );
    }
}

TEST( lua, execute_snippet )
{
    {
        lua_State* L = luaL_newstate();
        luaL_openlibs( L );

        const char f[] = "print (\"Hello World from Snippet!\")\nprint ( v )";

        int s = luaL_loadbuffer( L, f, strlen( f ), "f" );

        if( s == 0 )
        {
            // execute Lua snippet
            lua_pushstring( L, "MyVariable" );
            lua_setglobal( L, "v" );
            s = lua_pcall( L, 0, LUA_MULTRET, 0 );
        }

        bool ok = report_errors( L, s );
        lua_close( L );

        EXPECT_TRUE( ok );
    }
}

TEST( lua, two_snippets_with_functions )
{
    {
        lua_State* L = luaL_newstate();
        luaL_openlibs( L );

        const char f[] = "function f (x, y)\n"
                         "    return x + y;\n"
                         "end\n";

        const char body[] = "y = f(2,3)\n"
                            "print( \"Answer from y is\" .. y )\n"
                            "print( f )\n";

        int s = luaL_loadbuffer( L, f, strlen( f ), "buffer_f" );

        if( s == 0 )
        {
            // execute Lua snippet
            s = lua_pcall( L, 0, LUA_MULTRET, 0 );
        }

        bool ok = report_errors( L, s );

        EXPECT_TRUE( ok );

        s = luaL_loadbuffer( L, body, strlen( body ), "buffer_body" );

        if( s == 0 )
        {
            // execute Lua snippet
            s = lua_pcall( L, 0, LUA_MULTRET, 0 );
        }

        ok = report_errors( L, s );

        lua_close( L );

        EXPECT_TRUE( ok );
    }
}

TEST( lua, calling_snippetbuilt_function )
{
    {
        lua_State* L = luaL_newstate();
        luaL_openlibs( L );

        const char f[] = "function possible_widgetname_OnAction (x, y)\n"
                         "    print( \"In possible_widgetname_OnAction\" )\n"
                         "    return x + y;\n"
                         "end\n";

        int s = luaL_loadbuffer( L, f, strlen( f ), "buffer_f" );

        if( s == 0 )
        {
            // execute Lua snippet
            s = lua_pcall( L, 0, LUA_MULTRET, 0 );
        }

        bool ok = report_errors( L, s );

        EXPECT_TRUE( ok );

        // Call built snippet functional manually:
        lua_getglobal( L, "possible_widgetname_OnAction" ); // function to be called
        lua_pushnumber( L, 2.0 );                         // push 1st argument
        lua_pushnumber( L, 3.0 );                         // push 2nd argument

        // do the call (2 arguments, 1 result)
        s = lua_pcall( L, 2, 1, 0 );
        if( s != 0 )
        {
            report_errors( L, s );
        }

        // retrieve result
        EXPECT_TRUE( lua_isnumber( L, -1 ) != 0 );

        if( lua_isnumber( L, -1 ) )
        {
            double z = lua_tonumber( L, -1 );
            lua_pop( L, 1 ); // pop returned value

            EXPECT_TRUE( z == 5.0 );
        }

        lua_close( L );

        EXPECT_TRUE( ok );
    }
}

TEST( lua, calling_snippetbuilt_function_ref_other_function )
{
    {
        int s = 0;

        lua_State* L = luaL_newstate();
        luaL_openlibs( L );

        const char e[] = "function add_two_numbers (x, y)\n"
                         "    print( \"In add_two_numbers\" )\n"
                         "    return x + y;\n"
                         "end\n";

        if( luaL_loadbuffer( L, e, strlen( e ), "buffer_e" ) == 0 )
        {
            // execute Lua snippet
            s = lua_pcall( L, 0, LUA_MULTRET, 0 );
        }

        const char f[] = "function possible_widgetname_OnAction (x, y)\n"
                         "    print( \"In possible_widgetname_OnAction\" )\n"
                         "    return add_two_numbers( x, y)\n"
                         "end\n";

        if( luaL_loadbuffer( L, f, strlen( f ), "buffer_f" ) == 0 )
        {
            // execute Lua snippet
            s = lua_pcall( L, 0, LUA_MULTRET, 0 );
        }

        bool ok = report_errors( L, s );
        EXPECT_TRUE( ok );

        // Call built snippet functional manually:
        lua_getglobal( L, "possible_widgetname_OnAction" ); // function to be called
        lua_pushnumber( L, 2.0 );                         // push 1st argument
        lua_pushnumber( L, 3.0 );                         // push 2nd argument

        // do the call (2 arguments, 1 result)
        s = lua_pcall( L, 2, 1, 0 );
        if( s != 0 )
        {
            report_errors( L, s );
        }

        // retrieve result
        EXPECT_TRUE( 0 != lua_isnumber( L, -1 ) );

        if( lua_isnumber( L, -1 ) )
        {
            double z = lua_tonumber( L, -1 );
            lua_pop( L, 1 ); // pop returned value

            EXPECT_TRUE( z == 5.0 );
        }

        lua_close( L );

        EXPECT_TRUE( ok );
    }
}