/**
* Based on code I originally wrote on 10/02/2004   13:24:46
* A version of the Lunar template code found
* http://lua-users.org/wiki/CppBindingWithLunar
*/
#ifndef lunar_hpp
#define lunar_hpp
#include <cstddef>
#include <cassert>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

//-------------------------------------------------------------------------
// Macros for declaring required fields.
#define LUNAR_CLASS(CLASS)                                                                         \
    friend class solosnake::lunar<CLASS>;                                                          \
    static const char classname[];                                                                 \
    static const solosnake::lunar<CLASS>::RegType functions[]

#define LUNAR_CLASS_FUNCTIONS(CLASS)                                                               \
    const char CLASS::classname[] = #CLASS;                                                        \
    const solosnake::lunar<CLASS>::RegType CLASS::functions[]
#define LUNAR_FUNCTION(CLASS, NAME)                                                                \
    {                                                                                              \
        #NAME, &CLASS::NAME                                                                        \
    }
#define LUNAR_FUNCTION_END                                                                         \
    {                                                                                              \
        0, NULL                                                                                    \
    }

namespace solosnake
{
    class lunar_base
    {
    public:
        //-----------------------------------------------------
        //! Macro-like function for setting a string in an
        //! indexed table.
        //-----------------------------------------------------
        static void set( lua_State* L, int table_index, const char* key )
        {
            lua_pushstring( L, key );
            lua_insert( L, -2 ); // swap value and key
            lua_settable( L, table_index );
        }

        //-----------------------------------------------------
        //!
        //-----------------------------------------------------
        static void make_weak_table( lua_State* L, const char* mode );

        //-----------------------------------------------------
        //!
        //-----------------------------------------------------
        static void make_subtable( lua_State* L, int table_index, const char* name, const char* mode );

        //-----------------------------------------------------
        //!
        //-----------------------------------------------------
        static void* push_userdata( lua_State* L, void* key, size_t sz );

        //-----------------------------------------------------
        //! Calls the named function from the user data
        //! function table.
        //-----------------------------------------------------
        static int call_function( lua_State* L,
                                  const char* funcname,
                                  const char* classname,
                                  int nargs,
                                  int nresults,
                                  int errfunc );
    };

    //-------------------------------------------------------------------------
    //!
    //! A template class based upon the Lunar template found at:
    //! http://Lua-users.org/wiki/CppBindingWithLunar
    //! This class automates the linking of classes with the L data objects. It
    //! requires that the data type T has a static const variable named 'classname'
    //! and a static array of RegTypes named 'methods_'.
    //!
    //-------------------------------------------------------------------------
    template <typename T> class lunar
    {
    private:
        struct UserdataType
        {
            T* pT;
        };

    public:
        typedef int ( T::*Mfp )( lua_State* );

        //! This function was removed from Lua 5.1 by 5.2
        static int luaL_typerror (lua_State *L, int narg, const char *tname) {
          const char *msg = lua_pushfstring(L, "%s expected, got %s", tname, luaL_typename(L, narg));
          return luaL_argerror(L, narg, msg);
        }


        // Note this must be pod struct as non-aggregates cannot be
        // initialised in an initializer list.
        struct RegType
        {
            const char* name;
            Mfp func;
        };

        //-----------------------------------------------------
        //! Calls the named function from the user data
        //! function table.
        //-----------------------------------------------------
        static inline int call( lua_State* L,
                                const char* funcname,
                                int nargs = 0,
                                int nresults = LUA_MULTRET,
                                int errfunc = 0 )
        {
            return lunar_base::call_function( L, funcname, T::classname, nargs, nresults, errfunc );
        }

        //-----------------------------------------------------
        //! Returns true if and only if the type T has already
        //! been registered with L.
        //-----------------------------------------------------
        static bool is_type_registered( lua_State* L )
        {
            luaL_getmetatable( L, T::classname ); // lookup imetatable in Lua registry
            const bool isRegistered = 1 != lua_isnil( L, -1 );
            lua_pop( L, 1 );
            return isRegistered;
        }

        //-----------------------------------------------------
        //! Registers the type T, via its classname, with
        //! L. Adds all static methods found as callable
        //! functions
        //-----------------------------------------------------
        static void register_type( lua_State* L, const bool allowCreationInLua = true )
        {
            lua_newtable( L );
            const int imethods = lua_gettop( L );
            luaL_newmetatable( L, T::classname );
            const int imetatable = lua_gettop( L );

            // Store method table in globals so that
            // scripts can add functions written in L.
            lua_pushvalue( L, imethods );
            lua_insert( L, -1 ); // swap value and key
            lua_setglobal( L, T::classname );

            // Hide imetatable from Lua getmetatable()
            lua_pushvalue( L, imethods );
            lunar_base::set( L, imetatable, "__metatable" );
            lua_pushvalue( L, imethods );
            lunar_base::set( L, imetatable, "__index" );
            lua_pushcfunction( L, object_to_string );
            lunar_base::set( L, imetatable, "__tostring" );
            lua_pushcfunction( L, object_garbage_collect );
            lunar_base::set( L, imetatable, "__gc" );

            // Metatable for function table.
            lua_newtable( L );

            if( allowCreationInLua )
            {
                lua_pushcfunction( L, create_object );
                lua_pushvalue( L, -1 );                 // dup create_object function.
                lunar_base::set( L, imethods, "new" );  // add create_object to function table
                lunar_base::set( L, -3, "__call" );     // mt.__call = create_object
            }

            lua_setmetatable( L, imethods );

            // Fill method table with function from class T.
            for( const RegType* f = T::functions; f->name; ++f )
            {
                const char* fname = f->name;
                lua_pushstring( L, fname );
                // Need to cast away const to allow conversion to void.
                lua_pushlightuserdata( L, const_cast<RegType*>( f ) );
                lua_pushcclosure( L, thunk, 1 );
                lua_settable( L, imethods );
            }

            // Drop imetatable and function table.
            lua_pop( L, 2 );
        }

        //-----------------------------------------------------
        //! push onto the Lua stack a user data containing a
        //! pointer to T object.
        //-----------------------------------------------------
        static int push( lua_State* L, T* obj, bool gc = false )
        {
            if( !obj )
            {
                lua_pushnil( L );
                return 0;
            }

            luaL_getmetatable( L, T::classname ); // lookup imetatable in Lua registry
            if( lua_isnil( L, -1 ) )
            {
                if( gc )
                {
                    delete obj;
                }

                return luaL_error( L, "%s missing imetatable", T::classname );
            }

            int imt = lua_gettop( L );
            lunar_base::make_subtable( L, imt, "userdata", "v" );

            UserdataType* ud = static_cast<UserdataType*>( 
              lunar_base::push_userdata( L, obj, sizeof( UserdataType ) ) );

            if( ud )
            {
                ud->pT = obj; // store pointer to object in userdata
                lua_pushvalue( L, imt );
                lua_setmetatable( L, -2 );
                if( !gc )
                {
                    lua_checkstack( L, 3 );
                    lunar_base::make_subtable( L, imt, "do not trash", "k" );
                    lua_pushvalue( L, -2 );
                    lua_pushboolean( L, 1 );
                    lua_settable( L, -3 );
                    lua_pop( L, 1 );
                }
            }

            lua_replace( L, imt );
            lua_settop( L, imt );

            return imt; // index of user data containing pointer to T object
        }

        //-----------------------------------------------------
        //! Get user data from Lua stack and return pointer to
        //! T object
        //-----------------------------------------------------
        static T* checked_get( lua_State* L, int narg )
        {
            UserdataType* ud = static_cast<UserdataType*>( luaL_checkudata( L, narg, T::classname ) );

            if( ud )
            {
                return ud->pT; // pointer to T object;
            }
            else
            {
                luaL_typerror( L, narg, T::classname );
                return nullptr;
            }
        }

        //-----------------------------------------------------
        //! Register a named instance as a global in Lua.
        //! The type must first have been registered with Lua.
        //-----------------------------------------------------
        static void add_as_global( lua_State* L, T* instance, const char* name, bool allowLuaGC )
        {
            assert( L );
            assert( instance );
            assert( lunar<T>::is_type_registered( L ) );

            const int stackIndex = lunar<T>::push( L, instance, allowLuaGC );
            lua_pushvalue( L, stackIndex );
            lua_setglobal( L, name );
        }

    private:
        lunar(); // Not implemented.

        //-----------------------------------------------------
        //! Member function dispatcher.
        //-----------------------------------------------------
        static int thunk( lua_State* L )
        {
            // Stack has user data, followed by method args
            T* obj = checked_get( L, 1 ); // Get 'self', or if you prefer, 'this'
            lua_remove( L, 1 );         // Remove self so member function args start at index 1
            // get member function from upvalue
            const RegType* f = static_cast<const RegType*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
            return ( obj->*( f->func ) )( L ); // call member function
        }

        //-----------------------------------------------------
        //! Creates a new T (using new) and pushes onto the Lua
        //! stack a user data containing a pointer to T object.
        //-----------------------------------------------------
        static inline int create_object( lua_State* L )
        {
            lua_remove( L, 1 );      // Use classname:new(), instead of classname.new()
            push( L, new T( L ), true ); // call constructor for T objects
            // object_garbage_collect will delete this object
            return 1; // user data containing pointer to T object
        }

        //-----------------------------------------------------
        //! Garbage collection meta-method.
        //-----------------------------------------------------
        static int object_garbage_collect( lua_State* L )
        {
            if( luaL_getmetafield( L, 1, "do not trash" ) )
            {
                lua_pushvalue( L, 1 ); // dup userdata
                lua_gettable( L, -2 );
                if( !lua_isnil( L, -1 ) )
                {
                    return 0; // do not delete object
                }
            }

            UserdataType* ud = static_cast<UserdataType*>( lua_touserdata( L, 1 ) );

            if( ud->pT )
            {
                delete ud->pT; // call destructor for T objects
            }

            return 0;
        }

        //-----------------------------------------------------
        //! Prints debugging information to string. The address
        //! and class name are printed.
        //-----------------------------------------------------
        static inline int object_to_string( lua_State* L )
        {
            lua_pushfstring( L,
                             "userdata (lunar): %s (%p)",
                             T::classname,
                             reinterpret_cast<const UserdataType*>( lua_touserdata( L, 1 ) )->pT );
            return 1;
        }
    };
}
#endif
