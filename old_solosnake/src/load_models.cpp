#include "solosnake/load_models.hpp"
#include "solosnake/modeldescription.hpp"
#include "solosnake/modelnodedescription.hpp"
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/filepath.hpp"
#include "solosnake/img.hpp"
#include "solosnake/image.hpp"
#include "solosnake/external/lua.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/logging.hpp"

using namespace std;

namespace solosnake
{
    namespace
    {
        // Forward declarations.
        static modelnodedescription read_modelnodedescription( lua_State* );
        static void read_modelnodedescription_childnodes( lua_State* L, modelnodedescription& );

        template <typename T> inline T lua_check( lua_State* L, int n )
        {
            return static_cast<T>( luaL_checknumber( L, n ) );
        }

        template <> inline string lua_check<std::string>( lua_State* L, int n )
        {
            return string( luaL_checkstring( L, n ) );
        }

        //! Raises an error if not a table, or named table not found.
        template <typename T>
        void read_table_contents_into_vector( lua_State* L, const char* key, vector<T>& v )
        {
            luaL_checktype( L, -1, LUA_TTABLE );
            lua_pushstring( L, key );
            lua_gettable( L, -2 );
            luaL_checktype( L, -1, LUA_TTABLE );

            const size_t uN = lua_rawlen( L, -1 );

            if( uN < static_cast<size_t>( numeric_limits<int>::max() - 1 ) )
            {
                v.clear();
                v.reserve( uN );

                const int iN = static_cast<int>( uN + 1 );
                for( int i = 1; i < iN; ++i )
                {
                    lua_rawgeti( L, -1, i );
                    v.push_back( static_cast<T>( lua_check<T>( L, -1 ) ) );
                    lua_pop( L, 1 );
                }
            }
            else
            {
                luaL_error( L, "Too many table elements." );
            }

            lua_pop( L, 1 );
        }

        // TODO: Not used at the moment. Remove finally
#if 0
        //! Reads a named table entry as a float.
        static void read_table_entry_as_float( lua_State* L, const char* key, float& f )
        {
            luaL_checktype( L, -1, LUA_TTABLE );
            lua_pushstring( L, key );
            lua_gettable( L, -2 );
            f = lua_check<float>( L, -1 );
            lua_pop( L, 1 );
        }
#endif

        //! Reads a named table entry as a string.
        static void read_table_entry_as_string( lua_State* L, const char* key, string& s )
        {
            luaL_checktype( L, -1, LUA_TTABLE );
            lua_pushstring( L, key );
            lua_gettable( L, -2 );
            s = lua_check<std::string>( L, -1 );
            lua_pop( L, 1 );
        }

        //! Reads the 'childnodes' table within the current table and adds
        //! the modelnodedescriptions it reads into the node @a n.
        static void read_modelnodedescription_childnodes( lua_State* L, modelnodedescription& n )
        {
            luaL_checktype( L, -1, LUA_TTABLE );
            lua_pushstring( L, "childnodes" );
            lua_gettable( L, -2 );
            {
                // Did we get a table (of tables) named 'childnodes'?
                luaL_checktype( L, -1, LUA_TTABLE );

                // How many elements does it contain (these should themselves
                // be tables.
                const size_t uN = lua_rawlen( L, -1 );

                if( uN < static_cast<size_t>( numeric_limits<int>::max() ) )
                {
                    const int iN = static_cast<int>( uN );

                    // Read each table in 'childnodes'
                    for( int i = 1; i < iN + 1; ++i )
                    {
                        lua_rawgeti( L, -1, i );
                        n.add_child( read_modelnodedescription( L ) );
                        lua_pop( L, 1 );
                    }
                }
                else
                {
                    luaL_error( L, "Too many table elements." );
                }
            }
            lua_pop( L, 1 );
        }

        //! Reads a model node description from the current Lua table.
        static modelnodedescription read_modelnodedescription( lua_State* L )
        {
            luaL_checktype( L, -1, LUA_TTABLE );

            string node_name;
            string mesh_name;
            // Instance data info.
            vector<float> transform4x4;

            node_name.reserve( 32 );
            mesh_name.reserve( 32 );
            transform4x4.reserve( 4 * 4 );

            read_table_entry_as_string( L, "nodename", node_name );
            read_table_entry_as_string( L, "mesh", mesh_name );
            read_table_contents_into_vector( L, "transform", transform4x4 );
            if( transform4x4.size() != ( 4 * 4 ) )
            {
                luaL_error( L, "Invalid transform matrix in model file." );
            }

            // Use the instance data defaults for team RGB etc.
            meshinstancedata data;

            data.set_transform_4x4( transform4x4.data() );
            vector<float>().swap( transform4x4 );

            modelnodedescription desc( move( node_name ), move( mesh_name ), data );

            // Load children:
            read_modelnodedescription_childnodes( L, desc );

            return desc;
        }

        //! Reads a model description from Lua table.
        static modeldescription read_modeldescription( lua_State* L )
        {
            string name;
            vector<string> meshfiles;

            name.reserve( 32 );
            meshfiles.reserve( 4 );

            read_table_entry_as_string( L, "name", name );
            read_table_contents_into_vector( L, "meshfiles", meshfiles );

            return modeldescription( move( name ), move( meshfiles ), read_modelnodedescription( L ) );
        }

        //! Mapped to the lua function 'model', reads the descriptions into the
        //! users array.
        static int loadluamodeldescriptions( lua_State* L )
        {
            auto descs = reinterpret_cast
                         <vector<modeldescription>*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
            assert( descs );

            descs->push_back( read_modeldescription( L ) );

            return 0;
        }
    }

    //! Loads zero or more model descriptions from the named Lua file.
    vector<modeldescription> load_model_descriptions( const filepath& filename )
    {
        if( ! std::filesystem::is_regular_file( filename ) )
        {
            ss_throw( "File specified for load_model_descriptions does not exist." );
        }

        vector<modeldescription> descrs;

        auto L = solosnake::lua::create();
        luaL_openlibs( *L );

        // Associate the lua model loading C function with the model
        // files 'mesh' method, which results in this function being called
        // when the file is 'run'.
        lua_pushlightuserdata( *L, &descrs );
        lua_pushcclosure( *L, &loadluamodeldescriptions, 1 );
        lua_setglobal( *L, "model" );

        if( 0 != luaL_dofile( *L, filename.string().c_str() ) )
        {
            // Error.
            ss_err( "Error loading model descriptions from file : ", filename.string().c_str() );
            ss_throw( "Error loading model descriptions file." );
        }

        return descrs;
    }
}
