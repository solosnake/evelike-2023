#include <string>
#include "solosnake/deferred_renderer_to_lua.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/external/lua.hpp"

namespace solosnake
{
    namespace
    {
        //! If r is non null attempts to set the corresponding named debug
        //! render mode. This may have no effect if the application is not
        //! using a debug enabled renderer.
        //! SceneRenderMode,           -->  0 Default rendermode, displays the same scene as non-debug.
        //! DiffuseTexRenderMode,      -->  1 Shows the diffuse texture.
        //! NormalTexRenderMode,       -->  2 Normal texture, untransformed.
        //! EmissiveTex0RenderMode,    -->  3 Shows the first emissive channel.
        //! EmissiveTex1RenderMode,    -->  4 Shows the second emissive channel.
        //! EmissiveTexRenderMode,     -->  5 Shows the final emissive result.
        //! SpecularChannelRenderMode, -->  6 Displays the specular channel as greyscale.
        //! LightingNormalsRenderMode, -->  7 Transformed normals used for lighting.
        //! NoSpecularRenderMode,      -->  8 No specular lighting contribution.
        //! NoDiffuseRenderMode,       -->  9 No diffuse lighting contribution.
        //! WireFrameRenderMode,       --> 10 Render in wireframe.
        //! LitVolumes,                --> 11 Show areas touched by lights
        //! ShowMipMapLevels           --> 12 Diffuse textures will be loaded with a different color per mipmap.
        bool set_debug_rendermode( const std::string& modename, deferred_renderer* r )
        {
            auto mode = deferred_renderer::SceneRenderMode;

            if( 0 == modename.compare( "SceneRenderMode" ) )
            {
                // Do nothing, its already set to this mode.
                // Default render mode, displays the same scene as non-debug.
            }
            else if( 0 == modename.compare( "DiffuseTexRenderMode" ) )
            {
                mode = deferred_renderer::DiffuseTexRenderMode;
            }
            else if( 0 == modename.compare( "NormalTexRenderMode" ) )
            {
                mode = deferred_renderer::NormalTexRenderMode;
            }
            else if( 0 == modename.compare( "EmissiveTex0RenderMode" ) )
            {
                mode = deferred_renderer::EmissiveTex0RenderMode;
            }
            else if( 0 == modename.compare( "EmissiveTex1RenderMode" ) )
            {
                mode = deferred_renderer::EmissiveTex1RenderMode;
            }
            else if( 0 == modename.compare( "EmissiveTexRenderMode" ) )
            {
                mode = deferred_renderer::EmissiveTexRenderMode;
            }
            else if( 0 == modename.compare( "SpecularChannelRenderMode" ) )
            {
                mode = deferred_renderer::SpecularChannelRenderMode;
            }
            else if( 0 == modename.compare( "LightingNormalsRenderMode" ) )
            {
                mode = deferred_renderer::LightingNormalsRenderMode;
            }
            else if( 0 == modename.compare( "NoSpecularRenderMode" ) )
            {
                mode = deferred_renderer::NoSpecularRenderMode;
            }
            else if( 0 == modename.compare( "NoDiffuseRenderMode" ) )
            {
                mode = deferred_renderer::NoDiffuseRenderMode;
            }
            else if( 0 == modename.compare( "WireFrameRenderMode" ) )
            {
                mode = deferred_renderer::WireFrameRenderMode;
            }
            else if( 0 == modename.compare( "LitVolumes" ) )
            {
                mode = deferred_renderer::LitVolumes;
            }
            else if( 0 == modename.compare( "ShowMipMapLevels" ) )
            {
                mode = deferred_renderer::ShowMipMapLevels;
            }
            else
            {
                return false;
            }

            r->set_debug_rendermode( mode );

            return true;
        }

        int lua_set_debug_rendermode( lua_State* L )
        {
            // Get upvalue (the deferred_renderer*)
            auto r = 
                reinterpret_cast<deferred_renderer*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );

            if( r && lua_isstring( L, 1 ) )
            {
                const std::string mode( luaL_checkstring( L, 1 ) );

                if( false == set_debug_rendermode( mode, r ) )
                {
                    luaL_error( L, "Unknown rendermode '%s'", luaL_checkstring( L, 1 ) );
                }
            }

            return 0;
        }
    }

    void expose_renderer_to_lua( deferred_renderer* r, lua_State* L )
    {
        if( r )
        {
            lua_pushlightuserdata( L, r );
            lua_pushcclosure( L, &lua_set_debug_rendermode, 1 );
            lua_setglobal( L, "set_debug_rendermode" );
        }
        else
        {
        }
    }
}
