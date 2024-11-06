#include "solosnake/external/lua/lunar.hpp"

#if 0

// Original example code:

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "lunar.h"

//
// New Account objects can be created in a Lua script with either the Account:new(...)
// or Account(...) syntax. The latter is implemented using the call event for the method
// table. Any C++ objects created by a Lua script will be deleted by the userdata gc event.
//
class Account {
  lua_Number m_balance;
public:
  static const char className[];
  static Lunar<Account>::RegType methods[];

  Account(lua_State *L)      { m_balance = luaL_checknumber(L, 1); }
  int deposit (lua_State *L) { m_balance += luaL_checknumber(L, 1); return 0; }
  int withdraw(lua_State *L) { m_balance -= luaL_checknumber(L, 1); return 0; }
  int balance (lua_State *L) { lua_pushnumber(L, m_balance); return 1; }
  ~Account() { printf("deleted Account (%p)\n", this); }
};

const char Account::className[] = "Account";

Lunar<Account>::RegType Account::methods[] = {
  LUNAR_DECLARE_METHOD(Account, deposit),
  LUNAR_DECLARE_METHOD(Account, withdraw),
  LUNAR_DECLARE_METHOD(Account, balance),
  {0,0}
};

int main(int argc, char *argv[])
{
  lua_State *L = lua_open();

  luaL_openlibs(L);

  Lunar<Account>::Register(L);

  if(argc>1) luaL_dofile(L, argv[1]);

  lua_gc(L, LUA_GCCOLLECT, 0);  // collected garbage
  lua_close(L);
  return 0;
}

// Lua:

function printf(...) io.write(string.format(unpack(arg))) end

function Account:show()
  printf("Account balance = $%0.02f\n", self:balance())
end

a = Account(100)
b = Account:new(30)

print('a =', a)
print('b =', b)
print('metatable =', getmetatable(a))
print('Account =', Account)
table.foreach(Account, print)

a:show() a:deposit(50.30) a:show() a:withdraw(25.10) a:show()

parent = {}

function parent:rob(amount)
  amount = amount or self:balance()
  self:withdraw(amount)
  return amount
end

getmetatable(Account).__index = parent

a:rob(20)

//

Test Code Output

$ ./test setup.lua
loading 'setup.lua'
running application
Account balance = $100.00
Account balance = $30.00
a =     Account (0x22fcb0)
b =     Account (0xa041d98)
metatable =     table: 0xa044f28
Account =       table: 0xa044f28
show    function: 0xa0464a0
balance function: 0xa0455f8
withdraw        function: 0xa045300
deposit function: 0xa045508
new     function: 0xa044fe8
Account balance = $100.00
Account balance = $150.30
Account balance = $125.20
lua_debug> a:show()
Account balance = $125.20
lua_debug> b:show()
Account balance = $30.00
lua_debug> b:deposit(0.01)  b:show()
Account balance = $30.01
lua_debug> c = Account(19.99)  c:deposit(b:rob())  c:show()
Account balance = $50.00
lua_debug> b:show()
Account balance = $0.00
lua_debug> b = nil
lua_debug> print('c =', c)
c =     Account (0xa048cf8)
lua_debug> cont
deleted Account (0x22fcb0)
okay
deleted Account (0xa041d98)
close
deleted Account (0xa048cf8)
done


// Solosnake example code:

luabuttonwidget::luabuttonwidget(
        const std::string& name,
        const solosnake::rect& area,
        const int zLayer,
        const std::shared_ptr<luaguicommandexecutor> lce,
        const ilua_widget::functionnames& fnames )
        : ilua_widget( name, area, zLayer, lce, fnames )
    {
        lua_State* L = lce->lua();
        int stackIndex = lunar<luabuttonwidget>::push( L, this, false );
        lua_pushlstring ( L, name.c_str(), name.length() );
        lua_pushvalue( L, stackIndex );
        lua_settable( L, LUA_GLOBALSINDEX );
    }


    LUNAR_CLASS_FUNCTIONS( luabuttonwidget ) =
    {
        LUNAR_FUNCTION( luabuttonwidget, get_width ),
        LUNAR_FUNCTION( luabuttonwidget, set_width ),
        LUNAR_FUNCTION( luabuttonwidget, get_height ),
        LUNAR_FUNCTION( luabuttonwidget, set_height ),
        LUNAR_FUNCTION( luabuttonwidget, get_left ),
        LUNAR_FUNCTION( luabuttonwidget, set_left ),
        LUNAR_FUNCTION( luabuttonwidget, get_top ),
        LUNAR_FUNCTION( luabuttonwidget, set_top ),
        LUNAR_FUNCTION( luabuttonwidget, get_colour ),
        LUNAR_FUNCTION( luabuttonwidget, set_colour ),
        LUNAR_FUNCTION_END
    };


    //-------------------------------------------------------------------------
    // Example of dynamically extending a class:
    //-------------------------------------------------------------------------

    // Here we create a table and add into it a "rob" function.
    parent = {}
    parent.rob = function ( self )
        print( "ROBBERY!!" )
    end

    // Here we add the rob method to our lunar type "luabackgroundwidget".
    // "luabackgroundwidget" is the classname.
    getmetatable(luabackgroundwidget).__index = parent

    // Here we call the newly added method on a known instance of the
    // "luabackgroundwidget" object.
    background:rob() // prints "ROBBERY!!"


#endif

namespace solosnake
{
    //-----------------------------------------------------
    //! Calls the named function from the user data
    //! function table.
    //-----------------------------------------------------
    int lunar_base::call_function( lua_State* lua,
                                   const char* funcname,
                                   const char* classname,
                                   int nargs,
                                   int nresults,
                                   int errfunc )
    {
        const int ibase = lua_gettop( lua ) - nargs; // userdata index

        if( nullptr == luaL_checkudata( lua, ibase, classname ) )
        {
            lua_settop( lua, ibase - 1 ); // drop userdata and args
            lua_pushfstring( lua, "not a valid %s userdata", classname );
            return -1;
        }

        lua_pushstring( lua, funcname ); // funcname name
        lua_gettable( lua, ibase );    // get funcname from userdata

        if( lua_isnil( lua, -1 ) )
        {
            // no funcname?
            lua_settop( lua, ibase - 1 ); // drop userdata and args
            lua_pushfstring( lua, "%s missing funcname '%s'", classname, funcname );
            assert( ! "Lunar could not find function." );
            return -1;
        }

        lua_insert( lua, ibase ); // put funcname under userdata, args

        // call funcname
        const int status = lua_pcall( lua, 1 + nargs, nresults, errfunc );

        if( status )
        {
            const char* msg = lua_tostring( lua, -1 );
            if( msg == NULL )
            {
                msg = "(error with no message)";
            }
            lua_pushfstring( lua, "%s:%s status = %d\n%s", classname, funcname, status, msg );
            lua_remove( lua, ibase ); // remove old message
            return -1;
        }

        return lua_gettop( lua ) - ibase + 1; // number of results
    }

    //-----------------------------------------------------
    //!
    //-----------------------------------------------------
    void* lunar_base::push_userdata( lua_State* lua, void* key, size_t sz )
    {
        void* ud = NULL;
        lua_pushlightuserdata( lua, key );
        lua_gettable( lua, -2 ); // lookup[key]

        if( lua_isnil( lua, -1 ) )
        {
            lua_pop( lua, 1 ); // drop nil
            lua_checkstack( lua, 3 );
            ud = lua_newuserdata( lua, sz ); // create new userdata
            lua_pushlightuserdata( lua, key );
            lua_pushvalue( lua, -2 ); // dup userdata
            lua_settable( lua, -4 ); // lookup[key] = userdata
        }

        return ud;
    }

    //-----------------------------------------------------
    //!
    //-----------------------------------------------------
    void lunar_base::make_weak_table( lua_State* lua, const char* mode )
    {
        lua_newtable( lua );
        lua_pushvalue( lua, -1 ); // table is its own imetatable
        lua_setmetatable( lua, -2 );
        lua_pushliteral( lua, "__mode" );
        lua_pushstring( lua, mode );
        lua_settable( lua, -3 ); // imetatable.__mode = mode
    }

    //-----------------------------------------------------
    //!
    //-----------------------------------------------------
    void lunar_base::make_subtable( lua_State* lua, int table_index, const char* name, const char* mode )
    {
        lua_pushstring( lua, name );
        lua_gettable( lua, table_index );
        if( lua_isnil( lua, -1 ) )
        {
            lua_pop( lua, 1 );
            lua_checkstack( lua, 3 );
            make_weak_table( lua, mode );
            lua_pushstring( lua, name );
            lua_pushvalue( lua, -2 );
            lua_settable( lua, table_index );
        }
    }
}
