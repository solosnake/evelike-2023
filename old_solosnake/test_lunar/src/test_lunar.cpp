#include "solosnake/external/lua.hpp"
#include <iostream>
#include <fstream>
#include "solosnake/testing/testing.hpp"

namespace
{
    class Account
    {
        lua_Number m_balance;

    public:
        LUNAR_CLASS( Account );

        Account( double balance = 0 ) : m_balance( balance )
        {
        }

        void deposit( double amount )
        {
            m_balance += amount;
        }

        void withdraw( double amount )
        {
            m_balance -= amount;
        }

        double balance( void )
        {
            return m_balance;
        }

        ~Account()
        {
            printf( "deleted Account (%p)\n", this );
        }

        // Lua interface

        Account( lua_State* L ) : m_balance( luaL_checknumber( L, 1 ) )
        {
        }

        int deposit( lua_State* L )
        {
            deposit( luaL_checknumber( L, 1 ) );
            return 0;
        }

        int withdraw( lua_State* L )
        {
            withdraw( luaL_checknumber( L, 1 ) );
            return 0;
        }

        int balance( lua_State* L )
        {
            lua_pushnumber( L, balance() );
            return 1;
        }

        int show( lua_State* L )
        {
            printf( "Account balance = $%0.02f\n", balance() );
            return 0;
        }
    };

    LUNAR_CLASS_FUNCTIONS( Account )
    = { LUNAR_FUNCTION( Account, deposit ), LUNAR_FUNCTION( Account, withdraw ),
        LUNAR_FUNCTION( Account, balance ), LUNAR_FUNCTION( Account, show ),
        LUNAR_FUNCTION_END
      };

    int report( lua_State* L, int status )
    {
        if( status )
        {
            const char* msg = lua_tostring( L, -1 );
            if( msg == NULL )
            {
                msg = "(error with no message)";
            }
            fprintf( stderr, "ERROR: %s\n", msg );
            lua_pop( L, 1 );
        }
        return status;
    }

    void report_errors( lua_State* L, int status )
    {
        if( status != 0 )
        {
            std::cerr << "-- " << lua_tostring( L, -1 ) << std::endl;
            lua_pop( L, 1 ); // remove error message
        }
    }

    void write_file( const char* filename, const char* filecontents )
    {
      std::ofstream ofile( filename );
      ofile << filecontents;
    }

    void write_lunar_account_file1( const char* filename )
    {
        const char luatest[] = "function printf(...)\n"
                               "   io.write(string.format(table.unpack({...})))\n"
                               "end\n"
                               "\n"
                               "function Account:show()\n"
                               "   printf(\"Account balance = $%0.02f\\n\", self:balance())\n"
                               "end\n"
                               "\n"
                               "a = Account(100)\n"
                               "b = Account:new(30)\n"
                               "\n"
                               "print('a =', a)\n"
                               "print('b =', b)\n"
                               "print('metatable =', getmetatable(a))\n"
                               "print('Account =', Account)\n"
                               "\n"
                               "for index, value in pairs(Account) do\n"
                               "    print(\"index = \" .. index)\n"
                               //"    print(index .. \":\" .. value)\n"
                               "end\n"
                               "a:show()\n"
                               "a:deposit(50.30)\n"
                               "a:show()\n"
                               "a:withdraw(25.10)\n"
                               "a:show()\n"
                               "parent = {}\n"
                               "\n"
                               "function parent:rob(amount)\n"
                               "   amount = amount or self:balance()\n"
                               "   self:withdraw(amount)\n"
                               "return amount\n"
                               "end\n"
                               "getmetatable(Account).__index = parent\n"
                               "\n"
                               "print('a:show()')\n"
                               "a:show()\n"
                               "print('b:show()')\n"
                               "b:show()\n"
                               "print('print(a:rob(20))')\n"
                               "print(a:rob(20))\n"
                               "print('a:show()')\n"
                               "a:show()\n"
                               "print('b:deposit(a:rob())')\n"
                               "b:deposit(a:rob())\n"
                               "print('a:show()')\n"
                               "a:show()\n"
                               "print('b:show()')\n"
                               "b:show()\n";

        write_file( filename, luatest );
    }

    bool run_lua_file( const char* filename, const char* scripttext )
    {
      write_file( filename, scripttext );

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

      return s == 0;
    }

    bool do_lunar_file1( const char* filename )
    {
        write_lunar_account_file1( filename );

        lua_State* L = luaL_newstate();

        luaL_openlibs( L ); // provides io.*

        std::cerr << "-- Loading file: " << filename << std::endl;

        // Open debug lib and setup debug.traceback to be pcall's error handler.
        luaopen_debug(L);
        lua_getglobal(L, "debug");
        lua_getfield(L, -1, "traceback");
        lua_remove(L, -2);

        int s = luaL_loadfile( L, filename );

        assert( ! solosnake::lunar<Account>::is_type_registered( L ) );

        solosnake::lunar<Account>::register_type( L );

        assert( solosnake::lunar<Account>::is_type_registered( L ) );

        // Error handler is at -1 because the loaded buffer with zero args
        // is at -1
        const int errorhandler = -2;

        if( s == 0 )
        {
            // execute Lua program
            s = lua_pcall( L, 0, LUA_MULTRET, errorhandler );
        }

        report_errors( L, s );
        lua_close( L );

        return s == 0;
    }
}

TEST( lunar, test_lua_table_foreach )
{
    // Lua 5.2 changed (deprecated) table.foreach. Here I am testing how
    // the replacement works as I need confidence in this to use these tests.
    const char* table_foreach_script = 
      "local array = {5, 2, 6, 3, 6}\n"
      "for index, value in pairs(array) do\n"
      "    print(index .. \": \" .. value)\n"
      "end\n"
      ;

    EXPECT_TRUE( run_lua_file( "lunar_test_table_foreach.txt", table_foreach_script ) );
}


TEST( lunar, test_creating_cpp_type_in_lua )
{
    {
        EXPECT_TRUE( do_lunar_file1( "lunar_test_1.txt" ) );
    }
}

TEST( lunar, test_is_and_isnt_registered_type )
{
    {
        lua_State* L = luaL_newstate();
        luaL_openlibs( L );

        EXPECT_FALSE( solosnake::lunar<Account>::is_type_registered( L ) );

        solosnake::lunar<Account>::register_type( L );

        EXPECT_TRUE( solosnake::lunar<Account>::is_type_registered( L ) );

        lua_gc( L, LUA_GCCOLLECT, 0 ); // collected garbage
        lua_close( L );
    }
}

TEST( lunar, test_accessing_cpp_scoped_variable_in_lua )
{
    {
        lua_State* L = luaL_newstate();

        luaL_openlibs( L );

        EXPECT_FALSE( solosnake::lunar<Account>::is_type_registered( L ) );
        solosnake::lunar<Account>::register_type( L );
        EXPECT_TRUE( solosnake::lunar<Account>::is_type_registered( L ) );

        Account a;
        Account* b = new Account( 30 );

        int A = solosnake::lunar<Account>::push( L, &a );
        int B = solosnake::lunar<Account>::push( L, b, true );

        lua_pushvalue( L, A );
        EXPECT_TRUE( 1 == lua_isuserdata( L, -1 ) );
        EXPECT_FALSE( 1 == lua_isnone( L, A ) ); // Returns 1 if stack index is invalid.
        lua_setglobal( L, "a" );

        lua_pushvalue( L, B );
        EXPECT_TRUE( 1 == lua_isuserdata( L, -1 ) );
        EXPECT_FALSE( 1 == lua_isnone( L, B ) ); // Returns 1 if stack index is invalid.
        lua_setglobal( L, "b" );

        lua_pushvalue( L, A );
        lua_pushnumber( L, 100.00 );
        EXPECT_TRUE( 0 == report( L, solosnake::lunar<Account>::call( L, "deposit", 1, 0, 0 ) < 0 ) );

        lua_pushvalue( L, A );
        EXPECT_TRUE( 0 == report( L, solosnake::lunar<Account>::call( L, "show", 0, 0, 0 ) < 0 ) );

        lua_pushvalue( L, B );
        EXPECT_TRUE( 0 == report( L, solosnake::lunar<Account>::call( L, "show", 0, 0, 0 ) < 0 ) );

        lua_gc( L, LUA_GCCOLLECT, 0 ); // collected garbage
        lua_close( L );
    }
}

TEST( lunar, test_stop_use_of_new_Account_in_lua )
{
    {
        lua_State* L = luaL_newstate();

        luaL_openlibs( L );

        EXPECT_FALSE( solosnake::lunar<Account>::is_type_registered( L ) );
        solosnake::lunar<Account>::register_type( L, false );
        EXPECT_TRUE( solosnake::lunar<Account>::is_type_registered( L ) );

        Account a;
        Account* b = new Account( 30 );

        int A = solosnake::lunar<Account>::push( L, &a );
        int B = solosnake::lunar<Account>::push( L, b, true );

        lua_pushvalue( L, A );
        EXPECT_TRUE( 1 == lua_isuserdata( L, -1 ) );
        EXPECT_FALSE( 1 == lua_isnone( L, A ) ); // Returns 1 if stack index is invalid.
        lua_setglobal( L, "a" );

        lua_pushvalue( L, B );
        EXPECT_TRUE( 1 == lua_isuserdata( L, -1 ) );
        EXPECT_FALSE( 1 == lua_isnone( L, B ) ); // Returns 1 if stack index is invalid.
        lua_setglobal( L, "b" );

        // Check previous calls are still registered (we removed the call to "new").
        lua_pushvalue( L, A );
        lua_pushnumber( L, 100.00 );
        EXPECT_TRUE( 0 == report( L, solosnake::lunar<Account>::call( L, "deposit", 1, 0, 0 ) < 0 ) );
        lua_pushvalue( L, A );
        EXPECT_TRUE( 0 == report( L, solosnake::lunar<Account>::call( L, "show", 0, 0, 0 ) < 0 ) );
        lua_pushvalue( L, B );
        EXPECT_TRUE( 0 == report( L, solosnake::lunar<Account>::call( L, "show", 0, 0, 0 ) < 0 ) );

        const char f[] = "print( a )\n"
                         "print( b )\n"
                         "c = Account:new(10)\n"
                         "print( c )\n";

        // Open debug lib and setup debug.traceback to be pcall's error handler.
        luaopen_debug(L);
        lua_getglobal(L, "debug");
        lua_getfield(L, -1, "traceback");
        lua_remove(L, -2);

        EXPECT_TRUE( luaL_loadbuffer( L, f, strlen( f ), "buffer_f" ) == 0 );

        // Error handler is at -2 because the loaded buffer with zero args
        // is at -1
        const int errorhandler = -2;

        int s = lua_pcall( L, 0, LUA_MULTRET, errorhandler );

        if( s != 0 )
        {
            report_errors( L, s );
        }

        EXPECT_TRUE( s != 0 );

        lua_gc( L, LUA_GCCOLLECT, 0 ); // collected garbage
        lua_close( L );
    }
}

TEST( lunar, test_accessing_lunar_in_snippet )
{
    {
        int s;
        lua_State* L = luaL_newstate();

        luaL_openlibs( L );

        EXPECT_FALSE( solosnake::lunar<Account>::is_type_registered( L ) );
        solosnake::lunar<Account>::register_type( L );
        EXPECT_TRUE( solosnake::lunar<Account>::is_type_registered( L ) );

        Account a;

        int A = solosnake::lunar<Account>::push( L, &a );

        const char f[] = "function accountwidget_OnAction (widget, x, y)\n"
                         "    print( \"In accountwidget_OnAction\" )\n"
                         "    print( widget )\n"
                         "    widget:deposit(x)\n"
                         "    widget:withdraw(y)\n"
                         "    return widget:balance()\n"
                         "end\n";

        if( luaL_loadbuffer( L, f, strlen( f ), "buffer_f" ) == 0 )
        {
            // execute Lua snippet
            s = lua_pcall( L, 0, LUA_MULTRET, 0 );
        }

        // Call built snippet functional manually:
        lua_getglobal( L, "accountwidget_OnAction" ); // function to be called
        EXPECT_TRUE( lua_isfunction( L, -1 ) );

        lua_pushvalue( L, A ); // push 1st argument (location of Account).
        EXPECT_TRUE( 1 == lua_isuserdata( L, -1 ) );

        lua_pushnumber( L, 2.0 ); // push 2nd argument
        lua_pushnumber( L, 3.0 ); // push 3rd argument

        // do the call (3 arguments, 1 result)
        s = lua_pcall( L, 3, 1, 0 );
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

            // Deposited 2 withdrew 3 = -1.
            EXPECT_TRUE( z == -1.0 );
        }

        lua_gc( L, LUA_GCCOLLECT, 0 ); // collected garbage
        lua_close( L );
    }
}

class LunarObject
{
    std::shared_ptr<bool> is_alive_;

public:
    LunarObject( std::shared_ptr<bool> alive ) : is_alive_( alive )
    {
    }

    LunarObject( lua_State* L ) : is_alive_( new bool( true ) )
    {
    }

    ~LunarObject()
    {
        *is_alive_ = false;
    }

    LUNAR_CLASS( LunarObject );
};

LUNAR_CLASS_FUNCTIONS( LunarObject ) = { LUNAR_FUNCTION_END };

TEST( lunar, test_lunar_gc_behaviour )
{
    {
        lua_State* L = luaL_newstate();

        luaL_openlibs( L );

        solosnake::lunar<LunarObject>::register_type( L );

        lua_settop( L, 0 );

        std::shared_ptr<bool> object_is_alive( new bool( true ) );
        LunarObject* a = new LunarObject( object_is_alive );
        int A = solosnake::lunar<LunarObject>::push( L, a, true );

        EXPECT_TRUE( *object_is_alive );

        lua_gc( L, LUA_GCCOLLECT, 0 ); // collected garbage

        EXPECT_TRUE( *object_is_alive );

        lua_settop( L, 0 );

        EXPECT_TRUE( *object_is_alive );

        lua_gc( L, LUA_GCCOLLECT, 0 ); // collected garbage
        lua_close( L );
    }
}