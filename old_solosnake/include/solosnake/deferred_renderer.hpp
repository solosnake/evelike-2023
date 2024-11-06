#ifndef solosnake_deferred_renderer_hpp
#define solosnake_deferred_renderer_hpp

#include <memory>
#include <vector>
#include "solosnake/bgra.hpp"
#include "solosnake/colour.hpp"
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/meshhandle.hpp"
#include "solosnake/quad.hpp"
#include "solosnake/texquad.hpp"
#include "solosnake/texturehandle.hpp"
#include "solosnake/unaliased.hpp"

//! When SS_DEBUGGING_RENDERER is DEFINED the renderer can display a range
//! of different views of the data it uses to compose the default
//! scene.
//! For performance reasons this is a compile time switch and not a
//! dynamic variable. The costs of a generic renderer with a variety of
//! view modes is expensive for a class intended to be optimally performant.
// #define SS_DEBUGGING_RENDERER

//! Print annoying warning when we are building a Release build with a
//! debugging renderer.
#ifdef NDEBUG
#   ifdef SS_DEBUGGING_RENDERER
#       ifdef _MSC_VER // Visual Studio supports pragma messages
#           ifndef SOLOSNAKE_STRINGIFY
#               define SOLOSNAKE_STRINGIFY(x) #x
#               define SOLOSNAKE_TOSTRING(x) SOLOSNAKE_STRINGIFY(x)
#           endif
#           pragma message(__FILE__ "(" SOLOSNAKE_TOSTRING( __LINE__) "): Warning: Building SS_DEBUGGING_RENDERER in Release.")
#       endif
#   endif
#endif

#ifdef SS_DEBUGGING_RENDERER
#   ifndef SOLOSNAKE_STRINGIFY
#       define SOLOSNAKE_STRINGIFY(x) #x
#       define SOLOSNAKE_TOSTRING(x) SOLOSNAKE_STRINGIFY(x)
#   endif
#   pragma message(__FILE__ "(" SOLOSNAKE_TOSTRING( __LINE__) "): Building SS_DEBUGGING_RENDERER.")
#endif

namespace solosnake
{
    //! \brief A 3 pass deferred deferred_renderer.
    //!
    //!              +y
    //!               |
    //!               |
    //!               | _ _ _ _  +x
    //!              /
    //!             /
    //!            /
    //!          +z
    //!
    //!
    //!   Counter-clockwise : Visible
    //!   Clockwise         : Culled.
    //!   Handedness        : Right Handed.
    //!
    //!
    //!   Models should be built facing down the x axis (forwards).
    //!
    //!
    //!   Screen coordinates for screen-quad elements:
    //!
    //!        0,0                    1,0
    //!           +------------------+        +-------- x
    //!           |                  |        |
    //!           |         +        |        |
    //!           |                  |        |
    //!           +------------------+        y
    //!        0,1                    1,1
    //!
    //!
    //!  Window coordinates for un_project:
    //!
    //!        0,0                    W,0
    //!           +------------------+   / 1.0 (zFar)
    //!           |                  |  /
    //!           |         +        | /
    //!           |                  |/ 0.0 (zNear)
    //!           +------------------+
    //!        0,H                    W,H
    //!
    //!
    //! Texture coordinates and texel centres:
    //!
    //! For screen quads, coordinates should not be adjusted towards texel
    //! centres. The coordinates (0,0) will produce the texel at (0,0) at the
    //! desired position if the texel:pixel ratio is 1:1.
    class deferred_renderer
    {
    public:

        enum Quality
        {
            LowQuality,
            MediumQuality,
            HighQuality,
            HighestQuality
        };

        enum SfxType
        {
            SfxEmissive,
            SfxDiffuse
        };

        enum DebugRenderMode
        {
            SceneRenderMode,           //!<  0 Default rendermode, displays the same scene as non-debug.
            DiffuseTexRenderMode,      //!<  1 Shows the diffuse texture.
            NormalTexRenderMode,       //!<  2 Normal texture, untransformed.
            EmissiveTex0RenderMode,    //!<  3 Shows the first emissive channel.
            EmissiveTex1RenderMode,    //!<  4 Shows the second emissive channel.
            EmissiveTexRenderMode,     //!<  5 Shows the final emissive result.
            SpecularChannelRenderMode, //!<  6 Displays the specular channel as greyscale.
            LightingNormalsRenderMode, //!<  7 Transformed normals used for lighting.
            NoSpecularRenderMode,      //!<  8 No specular lighting contribution.
            NoDiffuseRenderMode,       //!<  9 No diffuse lighting contribution.
            WireFrameRenderMode,       //!< 10 Render in wireframe.
            LitVolumes,                //!< 11 Show the areas within the light volumes (including directional).
            ShowMipMapLevels           //!< 12 Diffuse textures will be loaded with a different color per mipmap.
        };

        struct point_light
        {
            point_light();

            float           worldX;
            float           worldY;
            float           worldZ;
            float           radius;
            float           red;
            float           green;
            float           blue;
        };

        // The light direction vector will be normalized before use.
        struct directional_light
        {
            directional_light();

            float           directionX;
            float           directionY;
            float           directionZ;
            float           red;
            float           green;
            float           blue;
        };

        struct scene_sun
        {
            texturehandle_t sun_texure_;
            sun_noise       sun_noise_;
        };

        //! Specifies a textured alpha blended screen quad in screen coordinates.
        //! (0,0) is top left, (1,1) is bottom right.
        struct screen_quad
        {
            screen_quad();
            void set_colour( Bgra );

            quad            screenCoord;
            texquad         texCoord;
            Bgra            cornerColours[4];
        };

        struct dynbufferdrawparams
        {
            dynbufferdrawparams();

            dynbuffer*      buffer;
            texturehandle_t texture;
            unsigned int    num_indices_to_draw;
            float           matrix[16];
            float           rgba[4];
            bool            applyView;
            bool            applyProj;
            bool            writeZ;
            bool            readZ;
            bool            alphaBlend;
            bool            strips;
        };

        //! @param buffers_width The width of the internal buffer the image
        //! is rendered onto. The screen width is recommended as a default.
        //! @param buffers_height The height of the internal buffer the image
        //! is rendered onto. The screen height is recommended as a default.
        //! @a buffers_width and @a buffers_width will be used to initialise
        //! internal viewport settings. Supersampling is possible by setting
        //! the values of buffers_width and buffers_height to be multiples of
        //! the user's screen size. Similarly it may be possible to improve
        //! performance by using buffers smaller than the intended viewport.
        deferred_renderer(
            const unsigned int buffers_width,
            const unsigned int buffers_height,
            const Quality qualityOfRenderer );

        ~deferred_renderer();

        meshhandle_t load_mesh( const std::shared_ptr<imesh>& );

        texturehandle_t load_texture( const std::shared_ptr<iimgsrc>& );

        texturehandle_t load_sun_colour_ramp_texture( const iimgBGR_ptr& );

        void load_skybox( const cubemap& );

        void enable_skybox( const bool );

        dynbuffer* alloc_dynbuffer( const unsigned int nverts, const unsigned int nindices );

        void free_dynbuffer( dynbuffer* );

        void lock_dynbuffer( dynbuffer* );

        void unlock_dynbuffer( dynbuffer*, const unsigned int newvcount, const unsigned int newicount );

        void unlock_dynbuffer_after_colour_change( dynbuffer* );

        void set_viewport( int x, int y, const unsigned int width, const unsigned int height );

        void set_view_matrix( const float* unaliased m4x4 );

        void set_proj_matrix( const float* unaliased m4x4, const float zNear );

        void set_skybox_3x3_matrix( const float* unaliased r3x3 );

        void get_view_matrix( float* m4x4 ) const;

        void get_inv_view_matrix( float* m4x4 ) const;

        void set_ambiant_light( float red, float green, float blue );

        void set_debug_rendermode( DebugRenderMode );

        void start_scene();

        void draw_mesh_instances( meshhandle_t, const meshinstancedata* const, const size_t n );

        void draw_dynbuffer( const dynbufferdrawparams& );

        void draw_point_light( const point_light& );

        void draw_sun( texturehandle_t, const sun_noise&, const sun_location_and_radius& );

        void draw_directional_light( const directional_light& );

        void draw_screen_quads( texturehandle_t, const screen_quad*, const unsigned int n );

        void draw_sfx( SfxType, texturehandle_t, texturehandle_t, const colour&, const fxtriangle*, const unsigned int n );

        void end_scene();

        //! At least this many triangles were sent to the scene.
        unsigned int scene_triangle_count() const;

        void un_project( unsigned int x, unsigned int y, float Z, float* unaliased worldpointXYZ ) const;

        Quality quality_level() const;

        static void load_perspective(
            float FOVradians,
            size_t windowWidth,
            size_t windowHeight,
            float zNear,
            float zFar,
            float* unaliased m4x4 );

        static void load_orthographic(
            float left,
            float right,
            float bottom,
            float top,
            float nearVal,
            float farVal,
            float* unaliased m4x4 );

    private:

        deferred_renderer();

        deferred_renderer( const deferred_renderer& );

        deferred_renderer& operator=( const deferred_renderer& );

        class internalrenderer;

    private:

        std::unique_ptr<internalrenderer> r_;
    };
}

#endif
