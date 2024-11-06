#include "solosnake/hexspacing.hpp"
#include "solosnake/angles.hpp"

namespace
{
    template<typename T>
    static void calc_hex_spacing( const T xy[2], T size, T spacing, unsigned int direction, T result[2] )
    {
        const double r = size / 2.0;

        const double centre[2] = { xy[0] + r, xy[1] + r };

        const double pi = 3.1415926535897932384626433832795;

        const double angles[6] =
        {
            3.0 / 2.0 * pi,  // 270
            11.0 / 6.0 * pi,  // 330
            1.0 / 6.0 * pi,  //  30
            1.0 / 2.0 * pi,  //  90
            5.0 / 6.0 * pi,  // 150
            7.0 / 6.0 * pi   // 210
        };

        const double distance_to_side =  r * ( std::sqrt( 3.0 ) / 2.0 );

        const double distance_between_centres = spacing + 2.0 * distance_to_side;

        const double dx = cos( angles[direction] );
        const double dy = sin( angles[direction] );

        result[0] = static_cast<T>( -r + centre[0] + dx * distance_between_centres );
        result[1] = static_cast<T>( -r + centre[1] + dy * distance_between_centres );
    }
}

extern "C"
{
    static int lua_hexspacing( lua_State* L )
    {
        lua_Number x = luaL_checknumber( L, 1 );
        lua_Number y = luaL_checknumber( L, 2 );

        lua_Number size  = luaL_checknumber( L, 3 );
        lua_Number space = luaL_checknumber( L, 4 );

        lua_Integer d = luaL_checkinteger( L, 5 ) % 6;

        lua_Number result[2] = { lua_Number(), lua_Number() };

        lua_Number xy[2] = { x, y };

        calc_hex_spacing( xy, size, space, static_cast<unsigned int>( d ), result );

        lua_pushnumber( L, result[0] );
        lua_pushnumber( L, result[1] );

        return 2;
    }
}

namespace solosnake
{
    void hexspacing( const float xy[2], float size, float spacing, unsigned int direction, float result[2] )
    {
        calc_hex_spacing<float>( xy, size, spacing, direction, result );
    }

    void expose_hexspacing_function( lua_State* L )
    {
        lua_pushcfunction( L, lua_hexspacing );
        lua_setglobal( L, "hexspacing" );
    }
}
