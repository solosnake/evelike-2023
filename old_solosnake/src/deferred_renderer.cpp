#include <cassert>
#include <future>
#include <list>
#include <map>
#include <sstream>
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/filepath.hpp"
#include "solosnake/fxtriangles.hpp"
#include "solosnake/image.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/make_noise_texture_3d.hpp"
#include "solosnake/set_vsynch.hpp"
#include "solosnake/throw.hpp"

// This file declares the OpenGL 3.2 and 3.3 extensions required.
#include "deferred_renderer_extensions.inl"

//  Requirements:
#define MINIMUM_OPENGL_MAJOR                        (3)
#define MINIMUM_OPENGL_MINOR                        (2)
#define MINIMUM_REQUIRED_DRAW_BUFFERS               (4)
#define MINIMUM_REQUIRED_UNIFORM_BUFFER_BINDINGS    (1)
#define MINIMUM_REQUIRED_COLOR_ATTACHMENTS          (4)
#define MINIMUM_REQUIRED_TEXTURE_UNITS              (7)
#define MINIMUM_REQUIRED_VERTEX_ATTRIBS             (11)
#define MINIMUM_REQUIRED_UNIFORM_BLOCK_SIZE         (((4 * 4) * 4) * sizeof(GLfloat))
#define MINIMUM_REQUIRED_STENCIL_BITS               (8)

// Defines used in code:
#define GLOBALS_BLOCK_BINDING_POINT                 (0)
#define FLOATS_PER_SUN_VERTEX                       (8)
#define NUM_UNIFORM_ELEMENTS                        (5)
#define MAX_INSTANCES_PER_BATCH                     (32)
#define MAX_QUADS_PER_BATCH                         (256u)
#define QUAD_RESTARTSTRIP_FLAG                      (imeshgeometry::StripRestartMarker)
#define SS_BUFFER_OFFSET(i)                         ((GLvoid*)(i))
#define INITIAL_LIGHT_STENCIL_NUMBER                (1)
#define MAX_LIGHT_STENCIL_NUMBER                    (200)
#define DEFAULT_AMBIENT_RED                         (1.0f)
#define DEFAULT_AMBIENT_GREEN                       (1.0f)
#define DEFAULT_AMBIENT_BLUE                        (1.0f)
#define DEFAULT_AMBIENT_ALPHA                       (0.0f)
#define DEFAULT_CLEAR_RED                           (0.0f)
#define DEFAULT_CLEAR_GREEN                         (0.0f)
#define DEFAULT_CLEAR_BLUE                          (0.0f)
#define DEFAULT_CLEAR_ALPHA                         (0.0f)
#define LIGHT_SPHERE_VCOUNT                         (60)
#define SKYBOX_VCOUNT                               (36)
#define NO_SCREEN_QUADS_THIS_FRAME                  (numeric_limits<size_t>::max())

// Composition stage texture indices:
#define TEX_INPUT_INDEX_COMPOSITION_SCENE_LITDIFF   (0)
#define TEX_INPUT_INDEX_COMPOSITION_SCENE_LITSPEC   (1)
#define TEX_INPUT_INDEX_COMPOSITION_SCENE_DGBWXY    (2)
#define TEX_INPUT_INDEX_COMPOSITION_SCENE_ERGBDR    (3)

// Post effect stage texture indices:
#define TEX_INPUT_POSTEFFECT_SCENE_SRC              (0)
#define TEX_INPUT_POSTEFFECT_WARP_SRC               (2)

// Model stage texture indices:
#define TEX_INPUT_INDEX_MODEL_DIFFSPEC              (0)
#define TEX_INPUT_INDEX_MODEL_NRMLEMISS             (1)

// We use 4 & 5 for lighting in the hope that
// setting these does not interfere with previous
// state (lighting).
#define TEX_INPUT_INDEX_LIGHTING_NORMALSSPEC        (4)
#define TEX_INPUT_INDEX_LIGHTING_DEPTH              (5)
#define TEX_INPUT_INDEX_GUI                         (2)
#define TEX_INPUT_INDEX_SKYBOX                      (6)
#define TEX_INPUT_INDEX_DYNBUFFERS                  (0)
#define TEX_INPUT_INDEX_SUN_SPHERE_NOISE            (0)
#define TEX_INPUT_INDEX_SUN_SPHERE_RAMP             (1)
#define TEX_INPUT_INDEX_SUN_CORONA_EMISSIVE         (0)
#define TEX_SFX_T1                                  (3)
#define TEX_SFX_T2                                  (4)
#define TEX_SFX_DEPTH                               (5)

// Used in releasing resources:
#define TEX_INPUT_INDEX_COUNT 7

// The vertex attribute channels
#define MSH_VRTX_CHNL                               (0)
#define MSH_UV_CHNL                                 (1)
#define MSH_TANG_CHNL                               (2)
#define MSH_NORMAL_CHNL                             (3)
#define MSH_MAT0_CHNL                               (4)
#define MSH_MAT1_CHNL                               (5)
#define MSH_MAT2_CHNL                               (6)
#define MSH_MAT3_CHNL                               (7)
#define MSH_TEAM_CHNL                               (8)
#define MSH_EMISSIVE_0_CHNL                         (9)
#define MSH_EMISSIVE_1_CHNL                         (10)
#define DYN_VRTX_CHNL                               (0)
#define DYN_UV_CHNL                                 (1)
#define DYN_CLR_CHNL                                (2)
#define SFX_VRTX_CHNL                               (0)
#define SFX_UV_CHNL                                 (1)

// These define which color attachments the composition shader
// writes to. These must match the frame buffer draw buffers.
#define OUTPUT_INDEX_DGBWXY                         (0)
#define OUTPUT_INDEX_ERGBDR                         (1)
#define OUTPUT_INDEX_NRGBSA                         (2)

using namespace std;

namespace solosnake
{
    namespace
    {
        struct screen_quad_corner_t
        {
            GLfloat x;
            GLfloat y;
            GLfloat u;
            GLfloat v;
            GLubyte r;
            GLubyte g;
            GLubyte b;
            GLubyte a;
        };

        struct screen_quad_in_vb_t
        {
            screen_quad_corner_t corners[4];
        };

        static_assert(
            sizeof( screen_quad_in_vb_t ) == ( 4 * 4 * ( sizeof( GLfloat ) + sizeof( GLubyte ) ) ),
            "screen_quad_in_vb_t is not expected size." );

        static_assert(
            sizeof( GLuint ) == sizeof( uint32_t ),
            "GLuint is not 32 bits" );

        // We require the meshinstancedata to be the same size as 16+3 floats:
        static_assert(
            sizeof( meshinstancedata ) == ( sizeof( float ) * ( 16 + 3 ) ),
            "meshinstancedata is not expected size" );

        //-------------------------------------------------------------------------

        static bool opengl_no_errors( const long lineNumber );

#ifndef NDEBUG
        // These functions are only used by 'assert'.
        static inline bool is_bound_array_buffer( const GLint name )
        {
            GLint arrayBufferBinding = 0;
            glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &arrayBufferBinding );
            return ( arrayBufferBinding == name );
        }

        static inline bool opengl_zwrite_is_enabled( const bool enabled )
        {
            // "If flag is GL_FALSE, depth buffer writing is disabled."
            GLint zMask = -1;
            glGetIntegerv( GL_DEPTH_WRITEMASK, &zMask );
            return enabled ? zMask == GL_TRUE : zMask == GL_FALSE;
        }

        static inline bool opengl_ztest_is_enabled( const bool enabled )
        {
            return enabled == ( GL_TRUE == glIsEnabled( GL_DEPTH_TEST ) );
        }
#endif

#ifndef NDEBUG
        static inline void assert_opengl_ok( const long lineNumber )
        {
            assert( opengl_no_errors( lineNumber ) );
        }
#else
        static inline void assert_opengl_ok( const long )
        {
        }
#endif

        static inline void validate_opengl_ok( const long lineNumber )
        {
            assert_opengl_ok( __LINE__ );

            if( false == opengl_no_errors( lineNumber ) )
            {
                ss_throw( "OpenGL error." );
            }
        }

        static inline void delete_fbo( GLuint& fbo ) SS_NOEXCEPT
        {
            if( fbo )
            {
                glDeleteFramebuffers( 1, &fbo );
                fbo = 0;
            }
        }

        static inline void delete_texture( GLuint& tex ) SS_NOEXCEPT
        {
            assert_opengl_ok( __LINE__ );

            if( tex )
            {
                glDeleteTextures( 1, &tex );
                tex = 0;
            }

            assert_opengl_ok( __LINE__ );
        }

        static inline void delete_buffer( GLuint& buffer ) SS_NOEXCEPT
        {
            if( buffer )
            {
                glDeleteBuffers( 1, &buffer );
                buffer = 0;
            }

            assert_opengl_ok( __LINE__ );
        }

        static inline void delete_vertexarray( GLuint& vao ) SS_NOEXCEPT
        {
            if( vao )
            {
                glDeleteVertexArrays( 1, &vao );
                vao = 0;
            }

            assert_opengl_ok( __LINE__ );
        }

        //! Checks OpenGL error state, clearing it, logs any error and the
        //! line number, and return true if the OpenGL error state was not
        //!  GL_NO_ERROR.
        static bool opengl_no_errors( const long lineNumber )
        {
            auto e = glGetError();

            switch( e )
            {
                case GL_NO_ERROR:
                    return true;
                case GL_INVALID_ENUM:
                    ss_err( "GL_INVALID_ENUM detected at line ", lineNumber );
                    break;
                case GL_INVALID_VALUE:
                    ss_err( "GL_INVALID_VALUE detected at line ", lineNumber );
                    break;
                case GL_INVALID_OPERATION:
                    ss_err( "GL_INVALID_OPERATION detected at line ", lineNumber );
                    break;
                case GL_STACK_OVERFLOW:
                    ss_err( "GL_STACK_OVERFLOW detected at line ", lineNumber );
                    break;
                case GL_STACK_UNDERFLOW:
                    ss_err( "GL_STACK_UNDERFLOW detected at line ", lineNumber );
                    break;
                case GL_OUT_OF_MEMORY:
                    ss_err( "GL_OUT_OF_MEMORY detected at line ", lineNumber );
                    break;
                default:
                    ss_err( "Unknown OpenGL error code '", e, "' detected at line ", lineNumber );
                    break;
            }

            assert( !"OpenGL error encountered" );
            return false;
        }


#if 0

        string text_file_to_string( const char* filename )
        {
            ifstream f( filename );
            return string( ( istreambuf_iterator<char>( f ) ), istreambuf_iterator<char>() );
        }

        //! Returns the contents of the named file as a string.
        static string text_file_to_string( const filepath& f )
        {
            std::filesystem::ifstream ifile( f );

            if( false == ifile.is_open() )
            {
                ss_throw( "Unable to open shader file." );
            }

            stringstream is;
            is << ifile.rdbuf();
            return is.str();
        }

#endif

        //! Creates a set of indices for rendering a set of quads as triangle
        //! strips for MAX_QUADS_PER_BATCH quads.
        static vector<GLushort> get_quad_indices()
        {
            // We ALWAYS end a quad with the restart marker, to simplify algorithm.
            vector<GLushort> indices( ( 5u * MAX_QUADS_PER_BATCH ), QUAD_RESTARTSTRIP_FLAG );
            assert( indices.size() < numeric_limits<unsigned short>::max() );

            unsigned short vertexLocation = 0;

            for( size_t i = 0; i < indices.size(); i += 5u, vertexLocation += 4u )
            {
                indices[i    ] = vertexLocation;
                indices[i + 1] = vertexLocation + 1u;
                indices[i + 2] = vertexLocation + 2u;
                indices[i + 3] = vertexLocation + 3u;
            }

            assert( 0 == indices.size() % 5u );

            return indices;
        }

        static void
        transform_quads_to_quadbuffer( const deferred_renderer::screen_quad* unaliased quads,
                                       const size_t n,
                                       screen_quad_in_vb_t* unaliased buffer )
        {
            // The quads will be rendered as a triangle-strip:
            //
            //  0 __ 2
            //   |__|
            //  1    3
            //
            for( size_t i = 0; i < n; ++i, ++buffer, ++quads )
            {
                buffer->corners[0].x =  2.0f * quads->screenCoord.x_ - 1.0f;
                buffer->corners[0].y = -2.0f * quads->screenCoord.y_ + 1.0f;
                buffer->corners[0].u = quads->texCoord.x0;
                buffer->corners[0].v = quads->texCoord.y0;
                buffer->corners[0].r = quads->cornerColours[0].red();
                buffer->corners[0].g = quads->cornerColours[0].green();
                buffer->corners[0].b = quads->cornerColours[0].blue();
                buffer->corners[0].a = quads->cornerColours[0].alpha();

                buffer->corners[1].x =  2.0f * ( quads->screenCoord.x_ ) - 1.0f;
                buffer->corners[1].y = -2.0f * ( quads->screenCoord.y_ + quads->screenCoord.height_ ) + 1.0f;
                buffer->corners[1].u = quads->texCoord.x0;
                buffer->corners[1].v = quads->texCoord.y1;
                buffer->corners[1].r = quads->cornerColours[1].red();
                buffer->corners[1].g = quads->cornerColours[1].green();
                buffer->corners[1].b = quads->cornerColours[1].blue();
                buffer->corners[1].a = quads->cornerColours[1].alpha();

                buffer->corners[2].x = 2.0f * ( quads->screenCoord.x_ + quads->screenCoord.width_ ) - 1.0f;

                buffer->corners[2].y = -2.0f * ( quads->screenCoord.y_ ) + 1.0f;
                buffer->corners[2].u = quads->texCoord.x1;
                buffer->corners[2].v = quads->texCoord.y0;
                buffer->corners[2].r = quads->cornerColours[2].red();
                buffer->corners[2].g = quads->cornerColours[2].green();
                buffer->corners[2].b = quads->cornerColours[2].blue();
                buffer->corners[2].a = quads->cornerColours[2].alpha();

                buffer->corners[3].x =  2.0f * ( quads->screenCoord.x_ + quads->screenCoord.width_ )  - 1.0f;
                buffer->corners[3].y = -2.0f * ( quads->screenCoord.y_ + quads->screenCoord.height_ ) + 1.0f;
                buffer->corners[3].u = quads->texCoord.x1;
                buffer->corners[3].v = quads->texCoord.y1;
                buffer->corners[3].r = quads->cornerColours[3].red();
                buffer->corners[3].g = quads->cornerColours[3].green();
                buffer->corners[3].b = quads->cornerColours[3].blue();
                buffer->corners[3].a = quads->cornerColours[3].alpha();
            }
        }

        static bool framebuffer_is_complete( GLuint fbo )
        {
            const GLenum fbStatus = glCheckFramebufferStatus( fbo );

            switch( fbStatus )
            {
                case GL_FRAMEBUFFER_COMPLETE:
                    break;

                case GL_FRAMEBUFFER_UNDEFINED:
                    ss_err( "GL_FRAMEBUFFER_UNDEFINED" );
                    // is returned if target is the default framebuffer, but the default
                    // framebuffer does not exist.
                    break;

                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                    ss_err( "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" );
                    // is returned if any of the framebuffer attachment points are
                    // framebuffer incomplete.
                    break;

                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                    ss_err( "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" );
                    // is returned if the framebuffer does not have at least one image
                    // attached to it.
                    break;

                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                    ss_err( "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" );
                    // is returned if the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE
                    // is GL_NONE for any color attachment point(s) named by
                    // GL_DRAWBUFFERi.
                    break;

                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                    ss_err( "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" );
                    // is returned if GL_READ_BUFFER is not GL_NONE
                    // and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE
                    // for the color attachment point named
                    // by GL_READ_BUFFER.
                    break;

                case GL_FRAMEBUFFER_UNSUPPORTED:
                    ss_err( "GL_FRAMEBUFFER_UNSUPPORTED" );
                    // is returned if the combination of internal formats of the
                    // attached images violates
                    // an implementation-dependent set of restrictions.
                    break;

                case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                    ss_err( "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" );
                    // is returned if the value of GL_RENDERBUFFER_SAMPLES is not the
                    // same
                    // for all attached renderbuffers; if the value of
                    // GL_TEXTURE_SAMPLES is the not same for all attached textures;
                    // or, if the attached images are a mix of renderbuffers and
                    // textures, the value of GL_RENDERBUFFER_SAMPLES
                    // does not match the value of GL_TEXTURE_SAMPLES.
                    // GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE is also returned if the
                    // value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is
                    // not the same for all attached textures; or, if the attached
                    // images are a mix of renderbuffers and textures,
                    // the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for
                    // all attached textures.
                    break;

                case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                    ss_err( "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" );
                    // is returned if any framebuffer attachment is layered, and any
                    // populated attachment is not layered,
                    // or if all populated color attachments are not from textures of
                    // the same target.
                    break;

                case 0:
                default:
                    ss_err( "Unknown framebuffer completeness error (0)." );
                    // Unknown error:
                    break;
            }

            return fbStatus == GL_FRAMEBUFFER_COMPLETE;
        }

        //! Returns the associated proxy target type, e.g.
        //! GL_PROXY_TEXTURE_2D for GL_TEXTURE_2D.
        static GLenum getAssociatedProxy( GLenum target )
        {
            GLenum proxy = static_cast<GLenum>(0);

            switch( target )
            {
                case GL_TEXTURE_2D:
                    proxy = GL_PROXY_TEXTURE_2D;
                    break;

                case GL_TEXTURE_1D_ARRAY:
                    proxy = GL_PROXY_TEXTURE_1D_ARRAY;
                    break;

                case GL_TEXTURE_RECTANGLE:
                    proxy = GL_PROXY_TEXTURE_RECTANGLE;
                    break;

                case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
                    proxy = GL_PROXY_TEXTURE_CUBE_MAP;
                    break;

                default:
                    assert( !"Invalid handle type." );
                    break;
            }

            return proxy;
        }

        // If format is a proxy no data is actually uploaded.
        static void upload_cubemap( const cubemap& sb, GLint internalFormat, bool proxy )
        {
            assert_opengl_ok( __LINE__ );

            for( size_t i = 0; i < 6; ++i )
            {
                glTexImage2D( static_cast<GLenum>( proxy ? GL_PROXY_TEXTURE_CUBE_MAP
                                                   : GL_TEXTURE_CUBE_MAP_POSITIVE_X + i ),
                              0,
                              internalFormat,
                              static_cast<GLsizei>( sb[i]->img_width() ),
                              static_cast<GLsizei>( sb[i]->img_height() ),
                              0,
                              GL_BGR,
                              GL_UNSIGNED_BYTE,
                              sb[i]->img_bgr_pixels() );
            }

            assert_opengl_ok( __LINE__ );
        }

        //! Creates texture or throws.
        static GLuint create_texture( GLenum target,
                                      GLsizei w,
                                      GLsizei h,
                                      GLint internalFmt,
                                      GLenum srcFmt,
                                      GLenum srcType,
                                      bool isDepthTex = false )
        {
            assert_opengl_ok( __LINE__ );
            assert( w > 0 && h > 0 );
            validate_opengl_ok( __LINE__ );

            // Clear error flag:
            glGetError();

            GLuint texid = 0;

            GLenum proxyTarget = getAssociatedProxy( target );

            glGenTextures( 1, &texid );
            glBindTexture( target, texid );

            glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

            glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

            if( isDepthTex )
            {
                assert_opengl_ok( __LINE__ );

                // See
                // http://lists.apple.com/archives/mac-opengl/2006/Jul/msg00011.html

                glTexParameteri( target, GL_TEXTURE_COMPARE_MODE, GL_NONE );
                assert_opengl_ok( __LINE__ );

                glTexParameteri( target, GL_TEXTURE_COMPARE_FUNC, GL_NEVER );
                assert_opengl_ok( __LINE__ );
            }

            // Check to see we have space...
            glTexImage2D( proxyTarget, 0, internalFmt, w, h, 0, srcFmt, srcType, NULL );

            GLint proxyWidth;
            glGetTexLevelParameteriv( proxyTarget, 0, GL_TEXTURE_WIDTH, &proxyWidth );

            if( proxyWidth > 0 )
            {
                assert_opengl_ok( __LINE__ );

                if( GL_RGBA8 == internalFmt )
                {
#ifndef NDEBUG
                    // Note: static analysis warns that 4 x w x h could in fact be a very large
                    // number, and that overflow is possible.
                    // We could consider a checker like mul_overflows( a, b ) that returned true
                    // if a x b exceeded their size limits?
                    vector<GLubyte> pixels( 4 * w * h, GLbyte( 0 ) );

                    // Create pink check pattern in DEBUG builds.
                    size_t i = 0u;
                    // Here we check that all the sub components of the pixel we will access
                    // are within range.
                    while( (i+4u) <= pixels.size() )
                    {
                      pixels[i + 0u] = 0xEE;
                      pixels[i + 1u] = 0xEE;
                      pixels[i + 2u] = 0x0F;
                      pixels[i + 3u] = 0xEE;
                      i += 8u;
                    }

                    glTexImage2D( target, 0, internalFmt, w, h, 0, srcFmt, srcType, &pixels[0] );
#else
                    glTexImage2D( target, 0, internalFmt, w, h, 0, srcFmt, srcType, NULL );
#endif
                }
                else
                {
                    glTexImage2D( target, 0, internalFmt, w, h, 0, srcFmt, srcType, NULL );
                }

                if( GL_NO_ERROR != glGetError() )
                {
                    delete_texture( texid );
                    ss_throw( "glTexImage2D failed when creating texture." );
                }
            }
            else
            {
                delete_texture( texid );
                ss_throw( "Unable to create texture - proxy query failed." );
            }

            if( GL_OUT_OF_MEMORY == glGetError() )
            {
                delete_texture( texid );
                ss_throw( "OpenGL out of memory while creating texture." );
            }

            // Unbind texture.
            glBindTexture( target, 0 );

            assert_opengl_ok( __LINE__ );

            return texid;
        }

        static GLuint create_depth_texture( GLsizei w, GLsizei h )
        {
            assert_opengl_ok( __LINE__ );
            return create_texture(
                       GL_TEXTURE_2D, w, h, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, true );
        }

        static GLuint create_bgra_texture( GLsizei w, GLsizei h )
        {
            assert_opengl_ok( __LINE__ );
            return create_texture( GL_TEXTURE_2D, w, h, GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE );
        }

        static GLuint create_lighting_texture( GLsizei w, GLsizei h )
        {
            assert_opengl_ok( __LINE__ );
            return create_texture( GL_TEXTURE_2D, w, h, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE );
        }

        //! Returns vertex, fragment or geometry.
        static const char* shaderTypeName( const GLenum shaderType )
        {
            if( shaderType == GL_VERTEX_SHADER )
            {
                return "vertex";
            }
            else if( shaderType == GL_FRAGMENT_SHADER )
            {
                return "fragment";
            }
            else if( shaderType == GL_GEOMETRY_SHADER )
            {
                return "geometry";
            }

            return "unknown";
        }

        //! Compiles a shader or throws.
        static GLuint compile_shader( const char* shaderTxt, GLenum shaderType, const char* debugHelp )
        {
            assert_opengl_ok( __LINE__ );

            GLuint s = 0;

            if( glGetError() == GL_NO_ERROR )
            {
                s = glCreateShader( shaderType );

                if( 0 == s )
                {
                    ss_throw( "glCreateShader failed." );
                }

                glShaderSource( s, 1, reinterpret_cast<const GLchar**>( &shaderTxt ), nullptr );

                glCompileShader( s );

                GLint compiled = 0;
                glGetShaderiv( s, GL_COMPILE_STATUS, &compiled );

                GLsizei logLength = 0;
                glGetShaderiv( s, GL_INFO_LOG_LENGTH, &logLength ); // Includes null terminator.

                if( logLength > 1 )
                {
                    GLsizei charsWritten = 0;
                    vector<GLchar> infoLog( logLength + 1, 0 );
                    glGetShaderInfoLog( s, logLength, &charsWritten, &infoLog[0] );

                    if( compiled == GL_FALSE )
                    {
                        ss_err( "Shader compilation failed : ",
                                debugHelp,
                                shaderTypeName( shaderType ),
                                &infoLog[0] );
                    }
                    else
                    {
                        ss_log( &infoLog[0] );
                    }
                }

                if( compiled == GL_FALSE || glGetError() != GL_NO_ERROR )
                {
                    glDeleteShader( s );

                    ss_throw( "GLSL shader compilation failed." );
                }
            }

            assert( s != 0 );

            return s;
        }

        //-------------------------------------------------------------------------

        //! Shader program : this object compiles and links the vertex and fragment
        //! shaders into a single OpenGL program.
        class program
        {
        public:
            program() : program_( 0 ), vshader_( 0 ), fshader_( 0 ), gshader_( 0 )
            {
            }

            ~program()
            {
                clear();
            }

            void compile( const string& v, const string& f, const char* debugHelp );

            bool link();

            void clear();

            void swap( program& );

            inline operator GLuint() const
            {
                return program_;
            }

        private:
            GLuint program_;
            GLuint vshader_;
            GLuint fshader_;
            GLuint gshader_;
        };

        void program::swap( program& other )
        {
            GLuint p = other.program_;
            GLuint v = other.vshader_;
            GLuint f = other.fshader_;
            GLuint g = other.gshader_;

            other.program_ = program_;
            other.vshader_ = vshader_;
            other.fshader_ = fshader_;
            other.gshader_ = gshader_;

            program_ = p;
            vshader_ = v;
            fshader_ = f;
            gshader_ = g;
        }

        void program::clear()
        {
            if( program_ )
            {
                if( vshader_ )
                {
                    glDetachShader( program_, vshader_ );
                }

                if( fshader_ )
                {
                    glDetachShader( program_, fshader_ );
                }

                if( gshader_ )
                {
                    glDetachShader( program_, gshader_ );
                }

                glDeleteProgram( program_ );
                program_ = 0;

                assert_opengl_ok( __LINE__ );
            }

            if( vshader_ )
            {
                glDeleteShader( vshader_ );
                vshader_ = 0;
                assert_opengl_ok( __LINE__ );
            }

            if( fshader_ )
            {
                glDeleteShader( fshader_ );
                fshader_ = 0;
                assert_opengl_ok( __LINE__ );
            }

            if( gshader_ )
            {
                glDeleteShader( gshader_ );
                gshader_ = 0;
                assert_opengl_ok( __LINE__ );
            }
        }

        bool program::link()
        {
            assert_opengl_ok( __LINE__ );

            if( 0 == program_ )
            {
                return false;
            }

            validate_opengl_ok( __LINE__ );

            assert( glIsProgram( program_ ) );

            assert_opengl_ok( __LINE__ );

            glLinkProgram( program_ );

            assert_opengl_ok( __LINE__ );

            GLint linked = 0;
            glGetProgramiv( program_, GL_LINK_STATUS, &linked );

            assert_opengl_ok( __LINE__ );

            GLsizei logLength = 0;
            glGetProgramiv( program_, GL_INFO_LOG_LENGTH, &logLength ); // Includes null terminator.

            assert_opengl_ok( __LINE__ );

            if( logLength > 1 )
            {
                vector<GLchar> logInfo( static_cast<size_t>( logLength + 1 ), GLchar( 0 ) );
                glGetProgramInfoLog( program_, logLength, &logLength, &logInfo[0] );

                if( linked != GL_TRUE )
                {
                    ss_err( "Linking shader program failed : ", &logInfo[0] );
                }
                else
                {
                    ss_log( &logInfo[0] );
                }
            }

            validate_opengl_ok( __LINE__ );

            return linked == GL_TRUE;
        }

        void program::compile( const string& v, const string& f, const char* debugHelp )
        {
            assert_opengl_ok( __LINE__ );

            clear();

            validate_opengl_ok( __LINE__ );

            vshader_ = compile_shader( v.c_str(), GL_VERTEX_SHADER, debugHelp );
            fshader_ = compile_shader( f.c_str(), GL_FRAGMENT_SHADER, debugHelp );

            program_ = glCreateProgram();

            ss_log( "Created OpenGL program ", program_ );

            glAttachShader( program_, vshader_ );
            validate_opengl_ok( __LINE__ );

            glAttachShader( program_, fshader_ );
            assert_opengl_ok( __LINE__ );
            validate_opengl_ok( __LINE__ );
        }

        //! Helper class to ensure resources are freed.
        class GLObjectHandle
        {
        public:
            GLObjectHandle() : handle_( 0 )
            {
            }

            GLObjectHandle( GLObjectHandle&& other ) : handle_( 0 )
            {
                *this = move( other );
            }

            GLObjectHandle& operator=( GLObjectHandle && other )
            {
                handle_ = move( other.handle_ );
                return *this;
            }

            GLObjectHandle& operator=( GLuint n )
            {
                assert( handle_ == 0 );
                handle_ = n;
                return *this;
            }

            GLObjectHandle( GLuint n ) : handle_( n )
            {
            }

            ~GLObjectHandle()
            {
                assert( handle_ == 0 );
            }

            GLuint* operator&()
            {
                return &handle_;
            }

            operator GLuint() const
            {
                return handle_;
            }

            operator GLuint& ()
            {
                return handle_;
            }

        private:
            GLuint handle_;
        };

        //-------------------------------------------------------------------------

        //! Class containing the OpenGL details of a specific mesh,
        //! as well as the source where the mesh originated. This
        //! allows it to recreate or reload the mesh as needs be,
        //! without having to know anything about storage etc.
        class internalmesh
        {
        public:

            struct details
            {
                details();

                GLuint  vertexArrayObject;
                GLsizei indicesCount;
                GLenum  layout; // Triangles or strips.
            };

            internalmesh( shared_ptr<imesh>,
                          GLuint matricesVBO,
                          deferred_renderer::Quality quality,
                          const bool showMipMaps );

            ~internalmesh() SS_NOEXCEPT;

            details setup_texture_states() const;

            unsigned int tricount() const;

        private:

            void free_resources() SS_NOEXCEPT;

            void validate( shared_ptr<imeshgeometry> ) const;

            void validate( shared_ptr<imeshtextureinfo> ) const;

            iimg_ptr validate( iimg_ptr p ) const;

            GLuint  msh_buffers_[2];  // 0 = IB, 1 = VB
            GLuint  msh_textures_[2]; // Diff+Spec/Normals+Emissive
            details details_;
        };

        //-------------------------------------------------------------------------

        static void get_mipmap_filters( const deferred_renderer::Quality q,
                                        bool hasMipMaps,
                                        GLint& minFilter,
                                        GLint& magFilter )
        {
            // Possible minFilter values: GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST,
            // GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST,
            // GL_LINEAR

            // Possible magFilter values: GL_LINEAR, GL_NEAREST

            // GL_LINEAR_MIPMAP_LINEAR Blends between the four texels
            // nearest to the sample point and blends between the two
            // mip levels that most closely match the size of pixel
            // being sampled.
            //
            // GL_NEAREST - no filtering, no mipmaps
            // GL_LINEAR - filtering, no mipmaps
            // GL_NEAREST_MIPMAP_NEAREST - no filtering, sharp switching
            // between mipmaps
            // GL_NEAREST_MIPMAP_LINEAR - no filtering, smooth transition
            // between mipmaps
            // GL_LINEAR_MIPMAP_NEAREST - filtering, sharp switching between
            // mipmaps
            // GL_LINEAR_MIPMAP_LINEAR - filtering, smooth transition
            // between mipmaps
            //
            // So:
            // GL_LINEAR is bilinear
            // GL_LINEAR_MIPMAP_NEAREST is bilinear with mipmaps
            // GL_LINEAR_MIPMAP_LINEAR is trilinear

            if( hasMipMaps )
            {
                if( q == deferred_renderer::HighQuality || q == deferred_renderer::HighestQuality )
                {
                    minFilter = GL_NEAREST_MIPMAP_LINEAR;
                    magFilter = GL_LINEAR;
                }
                else if( q == deferred_renderer::MediumQuality )
                {
                    // Not fastest but lowest quality of mipmapping.
                    minFilter = GL_NEAREST_MIPMAP_NEAREST;
                    magFilter = GL_LINEAR;
                }
                else
                {
                    // Not fastest but lowest quality of mipmapping.
                    minFilter = GL_NEAREST_MIPMAP_NEAREST;
                    magFilter = GL_NEAREST;
                }
            }
            else
            {
                // When no mipmapping then we can only use GL_NEAREST or GL_LINEAR
                if( q == deferred_renderer::HighQuality || q == deferred_renderer::HighestQuality )
                {
                    minFilter = GL_LINEAR;
                    magFilter = GL_LINEAR;
                }
                else if( q == deferred_renderer::MediumQuality )
                {
                    minFilter = GL_NEAREST;
                    magFilter = GL_NEAREST;
                }
                else
                {
                    minFilter = GL_NEAREST;
                    magFilter = GL_NEAREST;
                }
            }
        }

        // Sets the mipmap settings for this quality renderer.
        static void set_texture_mipmap_settings( const GLuint textureid,
                                                 const deferred_renderer::Quality q,
                                                 const bool hasMipMaps,
                                                 const bool clampToEdge )
        {
            assert_opengl_ok( __LINE__ );
            glBindTexture( GL_TEXTURE_2D, textureid );

            GLint mipmapMin;
            GLint mipmapMag;
            get_mipmap_filters( q, hasMipMaps, mipmapMin, mipmapMag );

            GLint wrapping = GL_CLAMP_TO_EDGE;

            if( false == clampToEdge )
            {
                wrapping = GL_REPEAT;
            }

            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mipmapMag );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapMin );

            glBindTexture( GL_TEXTURE_2D, 0 );
            assert_opengl_ok( __LINE__ );
        }

        static GLuint make_rgb_tex( deferred_renderer::Quality,
                                    const GLvoid* texels,
                                    GLsizei width,
                                    GLsizei height,
                                    bool makemipmap )
        {
            assert_opengl_ok( __LINE__ );
            validate_opengl_ok( __LINE__ );

            GLuint textureid;
            glGenTextures( 1, &textureid );

            assert_opengl_ok( __LINE__ );

            glBindTexture( GL_TEXTURE_2D, textureid );

            // Check to see we have space...
            glTexImage2D(
                GL_PROXY_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL );

            GLint proxyWidth = 0;
            glGetTexLevelParameteriv( GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &proxyWidth );

            if( proxyWidth > 0 )
            {
                assert_opengl_ok( __LINE__ );

                glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, texels );

                if( makemipmap )
                {
                    glGenerateMipmap( GL_TEXTURE_2D );
                }
            }

            glBindTexture( GL_TEXTURE_2D, 0 );

            if( proxyWidth <= 0 )
            {
                delete_texture( textureid );
            }

            validate_opengl_ok( __LINE__ );

            if( 0 == textureid )
            {
                ss_throw( "Failed to create RGBA texture." );
            }

            return textureid;
        }

        static GLuint make_rgba_tex( const deferred_renderer::Quality,
                                     const GLvoid* texels,
                                     const GLsizei width,
                                     const GLsizei height,
                                     bool makemipmap )
        {
            validate_opengl_ok( __LINE__ );

            GLuint textureid;
            glGenTextures( 1, &textureid );

            assert_opengl_ok( __LINE__ );

            glBindTexture( GL_TEXTURE_2D, textureid );

            // Check to see we have space...
            glTexImage2D(
                GL_PROXY_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL );

            GLint proxyWidth = 0;
            glGetTexLevelParameteriv( GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &proxyWidth );

            if( proxyWidth > 0 )
            {
                assert_opengl_ok( __LINE__ );

                glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, texels );

                if( makemipmap )
                {
                    glGenerateMipmap( GL_TEXTURE_2D );
                }
            }

            glBindTexture( GL_TEXTURE_2D, 0 );

            if( proxyWidth <= 0 )
            {
                delete_texture( textureid );
            }

            validate_opengl_ok( __LINE__ );

            if( 0 == textureid )
            {
                ss_throw( "Failed to create RGBA texture." );
            }

            return textureid;
        }

        //-------------------------------------------------------------------------

        static GLuint make_diffuse_spec_tex( deferred_renderer::Quality q,
                                             const GLvoid* texels,
                                             const GLsizei srcWidth,
                                             const GLsizei srcHeight )
        {
            validate_opengl_ok( __LINE__ );
            const GLuint textureid = make_rgba_tex( q, texels, srcWidth, srcHeight, true );
            set_texture_mipmap_settings( textureid, q, true, true );
            validate_opengl_ok( __LINE__ );
            return textureid;
        }

        static GLuint make_compressed_normals_and_emissives( deferred_renderer::Quality q,
                                                             const GLvoid* texels,
                                                             const GLsizei srcWidth,
                                                             const GLsizei srcHeight )
        {
            validate_opengl_ok( __LINE__ );
            const GLuint textureid = make_rgba_tex( q, texels, srcWidth, srcHeight, true );
            set_texture_mipmap_settings( textureid, q, true, true );
            validate_opengl_ok( __LINE__ );
            return textureid;
        }

        static GLuint make_visible_mipmaps_tex( const deferred_renderer::Quality q,
                                                const GLsizei srcWidth,
                                                const GLsizei srcHeight )
        {
            // Uses GL_NEAREST_MIPMAP_NEAREST
            validate_opengl_ok( __LINE__ );

            const uint8_t a = 255u;
            // e.g. 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1
            // e.g. 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1
            const bgra mipmapColours[8] =
            {
                // Red, Orange, Yellow, Green, Black
                bgra( 0, 0, 255, a ), bgra( 0, 128, 255, a ),   bgra( 0, 255, 255, a ), bgra( 0, 255, 0, a ),
                // Blue, Indigo, Violet, White,
                bgra( 255, 0, 0, a ), bgra( 255, 128, 128, a ), bgra( 128, 0, 255, a ), bgra( 255, 255, 255, a )
            };

            image toplevelmipmap( static_cast<unsigned int>( srcWidth ),
                                  static_cast<unsigned int>( srcHeight ),
                                  image::format_bgra );
            toplevelmipmap.clear_to_bgra( mipmapColours[0] );

            const GLuint textureid
                = make_rgba_tex( q, toplevelmipmap.data(), srcWidth, srcHeight, false );

            glBindTexture( GL_TEXTURE_2D, textureid );

            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
            assert_opengl_ok( __LINE__ );

            GLsizei width = srcWidth / 2;
            GLsizei height = srcHeight / 2;

            GLint level = 1;
            GLint proxyWidth = 1;
            while( width > 0 && height > 0 && proxyWidth > 0 )
            {
                // Check to see we have space...
                glTexImage2D( GL_PROXY_TEXTURE_2D,
                              level,
                              GL_RGBA8,
                              width,
                              height,
                              0,
                              GL_BGRA,
                              GL_UNSIGNED_BYTE,
                              NULL );

                glGetTexLevelParameteriv( GL_PROXY_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &proxyWidth );

                if( proxyWidth > 0 )
                {
                    image mipmap( static_cast<unsigned int>( width ),
                                  static_cast<unsigned int>( height ),
                                  image::format_bgra );
                    mipmap.clear_to_bgra( mipmapColours[static_cast<unsigned int>( level )
                                                        % ( sizeof( mipmapColours ) / sizeof( bgra ) )] );

                    glTexImage2D( GL_TEXTURE_2D,
                                  level,
                                  GL_RGBA8,
                                  width,
                                  height,
                                  0,
                                  GL_BGRA,
                                  GL_UNSIGNED_BYTE,
                                  mipmap.data() );

                    validate_opengl_ok( __LINE__ );
                }

                ++level;
                width /= 2;
                height /= 2;
            }

            glBindTexture( GL_TEXTURE_2D, 0 );

            validate_opengl_ok( __LINE__ );

            return textureid;
        }

        //-------------------------------------------------------------------------

        internalmesh::details::details() : vertexArrayObject( 0 ), indicesCount( 0 ), layout( GL_TRIANGLES )
        {
        }

        internalmesh::internalmesh( shared_ptr<imesh> src,
                                    GLuint matricesVBO,
                                    deferred_renderer::Quality quality,
                                    const bool showMipMaps )
        {
            memset( msh_buffers_, 0, sizeof( msh_buffers_ ) );
            memset( msh_textures_, 0, sizeof( msh_textures_ ) );

            validate_opengl_ok( __LINE__ );

            auto geom = src->get_meshgeometry();

            validate( geom );

            details_.layout = geom->index_mode() == imeshgeometry::TrianglesMode ? GL_TRIANGLES
                              : GL_TRIANGLE_STRIP;

            auto texs = src->get_meshgeometrytexinfo();

            validate( texs );

            // Clear error code.
            glGetError();

            glGenVertexArrays( 1, &details_.vertexArrayObject );
            assert_opengl_ok( __LINE__ );

            // The resource freeing assumes we correctly pass this point, so
            // test that this assumption is upheld here:
            if( details_.vertexArrayObject != 0 )
            {
                glGenBuffers( 2, msh_buffers_ );

                // Create the IBO, 16 bit indices
                {
                    details_.indicesCount = static_cast<GLsizei>( geom->index_count() );
                    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, msh_buffers_[0] );
                    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                                  details_.indicesCount * static_cast<GLsizei>( sizeof( GLushort ) ),
                                  geom->indices(),
                                  GL_STATIC_DRAW );
                    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
                    assert_opengl_ok( __LINE__ );
                }

                // Create VBO
                const GLsizei vertexCount = static_cast<GLsizei>( geom->vertex_count() );
                const GLsizei floatsPerDatum = 3 + 2 + 3 + 3;

                // Construct and fill the buffer.
                auto bufferToUpload            = make_shared<vector<GLfloat>>( vertexCount * floatsPerDatum );
                float* unaliased buffer        = bufferToUpload->data();
                const float* unaliased XYZs    = geom->vertices();
                const float* unaliased UVs     = texs->texture_coords();
                const float* unaliased TANGs   = texs->tangents();
                const float* unaliased Normals = texs->smoothed_normals();

                for( GLsizeiptr i = 0; i < vertexCount; ++i )
                {
                    buffer[0] = XYZs[0 + 3 * i];
                    buffer[1] = XYZs[1 + 3 * i];
                    buffer[2] = XYZs[2 + 3 * i];

                    buffer[3] = UVs[0 + 2 * i];
                    buffer[4] = UVs[1 + 2 * i];

                    buffer[5] = TANGs[0 + 3 * i];
                    buffer[6] = TANGs[1 + 3 * i];
                    buffer[7] = TANGs[2 + 3 * i];

                    buffer[8] = Normals[0 + 3 * i];
                    buffer[9] = Normals[1 + 3 * i];
                    buffer[10] = Normals[2 + 3 * i];

                    buffer += floatsPerDatum;
                }

                const GLsizeiptr bufferSize           = static_cast<GLsizeiptr>( sizeof( GLfloat ) ) * floatsPerDatum * vertexCount;
                const GLsizei vertexDataBlockStride   = static_cast<GLsizei>( sizeof( GLfloat ) ) * floatsPerDatum;
                const GLsizei instanceDataBlockStride = static_cast<GLsizei>( sizeof( meshinstancedata ) );
                const size_t rowSize                  = sizeof( GLfloat ) * 4u;
                const GLsizeiptr offsetUVs            = static_cast<GLsizeiptr>( sizeof( GLfloat ) * ( 3u ) );
                const GLsizeiptr offsetTangents       = static_cast<GLsizeiptr>( sizeof( GLfloat ) * ( 3u + 2u ) );
                const GLsizeiptr offsetNormals        = static_cast<GLsizeiptr>( sizeof( GLfloat ) * ( 3u + 2u + 3u ) );

                assert( texs->texture_coords() );

                // Add data to mesh buffer:
                glBindBuffer( GL_ARRAY_BUFFER, msh_buffers_[1] );
                glBufferData( GL_ARRAY_BUFFER, bufferSize, bufferToUpload->data(), GL_STATIC_DRAW );
                glBindBuffer( GL_ARRAY_BUFFER, 0 );
                bufferToUpload.reset();
                assert_opengl_ok( __LINE__ );

                //
                // VALVE say V.A.O.s are slower than the raw calls, always. Replace
                // usage with explicit calls?
                //

                // VAO for the mesh
                glBindVertexArray( details_.vertexArrayObject );

                // Bind the IBO for the VAO
                glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, msh_buffers_[0] );

                // Bind the mesh VBO and setup pointers into it in the VAO
                glBindBuffer( GL_ARRAY_BUFFER, msh_buffers_[1] );

                glVertexAttribPointer( MSH_VRTX_CHNL,
                                       3,
                                       GL_FLOAT,
                                       GL_FALSE,
                                       vertexDataBlockStride,
                                       SS_BUFFER_OFFSET( 0 ) );

                glVertexAttribPointer( MSH_UV_CHNL,
                                       2,
                                       GL_FLOAT,
                                       GL_FALSE,
                                       vertexDataBlockStride,
                                       SS_BUFFER_OFFSET( offsetUVs ) );

                glVertexAttribPointer( MSH_TANG_CHNL,
                                       3,
                                       GL_FLOAT,
                                       GL_FALSE,
                                       vertexDataBlockStride,
                                       SS_BUFFER_OFFSET( offsetTangents ) );

                glVertexAttribPointer( MSH_NORMAL_CHNL,
                                       3,
                                       GL_FLOAT,
                                       GL_FALSE,
                                       vertexDataBlockStride,
                                       SS_BUFFER_OFFSET( offsetNormals ) );

                assert_opengl_ok( __LINE__ );

                // Bind the matrices VBO and setup pointers into it in the VAO
                glBindBuffer( GL_ARRAY_BUFFER, matricesVBO );

                // First matrix:
                glVertexAttribPointer( MSH_MAT0_CHNL,
                                       4,
                                       GL_FLOAT,
                                       GL_FALSE,
                                       instanceDataBlockStride,
                                       SS_BUFFER_OFFSET( 0 ) );

                glVertexAttribPointer( MSH_MAT1_CHNL,
                                       4,
                                       GL_FLOAT,
                                       GL_FALSE,
                                       instanceDataBlockStride,
                                       SS_BUFFER_OFFSET( rowSize * 1 ) );

                glVertexAttribPointer( MSH_MAT2_CHNL,
                                       4,
                                       GL_FLOAT,
                                       GL_FALSE,
                                       instanceDataBlockStride,
                                       SS_BUFFER_OFFSET( rowSize * 2 ) );

                glVertexAttribPointer( MSH_MAT3_CHNL,
                                       4,
                                       GL_FLOAT,
                                       GL_FALSE,
                                       instanceDataBlockStride,
                                       SS_BUFFER_OFFSET( rowSize * 3 ) );

                // Team colour: 2_10_10_10, GL_BGRA
                glVertexAttribPointer( MSH_TEAM_CHNL,
                                       GL_BGRA,
                                       GL_UNSIGNED_BYTE,
                                       GL_TRUE,
                                       instanceDataBlockStride,
                                       SS_BUFFER_OFFSET( rowSize * 4 ) );

                assert_opengl_ok( __LINE__ );

                // Emissive animation channels 0 and 1.
                glVertexAttribPointer( MSH_EMISSIVE_0_CHNL,
                                       GL_BGRA,
                                       GL_UNSIGNED_BYTE,
                                       GL_TRUE,
                                       instanceDataBlockStride,
                                       SS_BUFFER_OFFSET( ( 1 * sizeof( GLuint ) ) + ( rowSize * 4 ) ) );

                glVertexAttribPointer( MSH_EMISSIVE_1_CHNL,
                                       GL_BGRA,
                                       GL_UNSIGNED_BYTE,
                                       GL_TRUE,
                                       instanceDataBlockStride,
                                       SS_BUFFER_OFFSET( ( 2 * sizeof( GLuint ) ) + ( rowSize * 4 ) ) );

                assert_opengl_ok( __LINE__ );

                // Enable the channels in the VAO:
                glEnableVertexAttribArray( MSH_VRTX_CHNL );
                glEnableVertexAttribArray( MSH_UV_CHNL );

                glEnableVertexAttribArray( MSH_TANG_CHNL );
                glEnableVertexAttribArray( MSH_NORMAL_CHNL );

                glEnableVertexAttribArray( MSH_MAT0_CHNL );
                glEnableVertexAttribArray( MSH_MAT1_CHNL );
                glEnableVertexAttribArray( MSH_MAT2_CHNL );
                glEnableVertexAttribArray( MSH_MAT3_CHNL );
                glEnableVertexAttribArray( MSH_TEAM_CHNL );
                glEnableVertexAttribArray( MSH_EMISSIVE_0_CHNL );
                glEnableVertexAttribArray( MSH_EMISSIVE_1_CHNL );

                assert_opengl_ok( __LINE__ );

                // Setup divisors in the VAO:
                glVertexAttribDivisor( MSH_VRTX_CHNL, 0 ); // Per object attributes:
                glVertexAttribDivisor( MSH_UV_CHNL, 0 );
                glVertexAttribDivisor( MSH_TANG_CHNL, 0 );
                glVertexAttribDivisor( MSH_NORMAL_CHNL, 0 );

                glVertexAttribDivisor( MSH_MAT0_CHNL, 1 ); // Per instance attributes:
                glVertexAttribDivisor( MSH_MAT1_CHNL, 1 );
                glVertexAttribDivisor( MSH_MAT2_CHNL, 1 );
                glVertexAttribDivisor( MSH_MAT3_CHNL, 1 );
                glVertexAttribDivisor( MSH_TEAM_CHNL, 1 );
                glVertexAttribDivisor( MSH_EMISSIVE_0_CHNL, 1 );
                glVertexAttribDivisor( MSH_EMISSIVE_1_CHNL, 1 );

                // Finished with this mesh's VAO:
                glBindVertexArray( 0 );
                glBindBuffer( GL_ARRAY_BUFFER, 0 );
                glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

                assert_opengl_ok( __LINE__ );

                // Textures

                auto diff_spec = validate( texs->tex_diffuse_and_spec() );
                auto normals_emiss = validate( texs->tex_normals_and_emiss() );

                // Diffuse BGR / Team A:
                if( showMipMaps )
                {
                    // Load a different color per mipmap level.
                    msh_textures_[0]
                        = make_visible_mipmaps_tex( quality,
                                                    static_cast<GLsizei>( diff_spec->img_width() ),
                                                    static_cast<GLsizei>( diff_spec->img_height() ) );
                }
                else
                {
                    msh_textures_[0]
                        = make_diffuse_spec_tex( quality,
                                                 diff_spec->img_bgra_pixels(),
                                                 static_cast<GLsizei>( diff_spec->img_width() ),
                                                 static_cast<GLsizei>( diff_spec->img_height() ) );
                }

                // Normal map with specular
                msh_textures_[1] = make_compressed_normals_and_emissives(
                                       quality,
                                       normals_emiss->img_bgra_pixels(),
                                       static_cast<GLsizei>( normals_emiss->img_width() ),
                                       static_cast<GLsizei>( normals_emiss->img_height() ) );

                // Finished.
                glBindTexture( GL_TEXTURE_2D, 0 );
                assert_opengl_ok( __LINE__ );

                if( false == opengl_no_errors( __LINE__ ) )
                {
                    free_resources();
                    ss_throw( "Failed to create internalmesh." );
                }
            }
            else
            {
                ss_throw( "Failed to create vertex array object for internalmesh." );
            }
        }

        //! Called by dtor - must NOT throw!
        void internalmesh::free_resources() SS_NOEXCEPT
        {
            try
            {
                assert_opengl_ok( __LINE__ );

                if( details_.vertexArrayObject )
                {
                    glBindVertexArray( 0 );

                    for( size_t i = 0; i < TEX_INPUT_INDEX_COUNT; ++i )
                    {
                        glActiveTexture( static_cast<GLenum>( GL_TEXTURE0 + i ) );
                        glBindTexture( GL_TEXTURE_2D, 0 );
                        assert_opengl_ok( __LINE__ );
                    }

                    delete_vertexarray( details_.vertexArrayObject );

                    // Delete each IBO and VBO.
                    for( size_t i = 0; i < sizeof( msh_buffers_ ) / sizeof( msh_buffers_[0] ); ++i )
                    {
                        delete_buffer( msh_buffers_[i] );
                        assert_opengl_ok( __LINE__ );
                    }

                    // Delete each texture:
                    for( size_t i = 0; i < sizeof( msh_textures_ ) / sizeof( msh_textures_[0] ); ++i )
                    {
                        delete_texture( msh_textures_[i] );
                        assert_opengl_ok( __LINE__ );
                    }
                }

                details_.indicesCount = 0;

                assert_opengl_ok( __LINE__ );
            }
            catch( ... )
            {
                ss_err( "Exception ignored in 'free_resources'" );
            }
        }

        internalmesh::~internalmesh() SS_NOEXCEPT
        {
            free_resources();
        }

        void internalmesh::validate( shared_ptr<imeshgeometry> ) const
        {
            // TODO incomplete function. internalmesh::validate imeshgeometry
        }

        void internalmesh::validate( shared_ptr<imeshtextureinfo> ) const
        {
            // TODO incomplete function. internalmesh::validate imeshtextureinfo
        }

        //! Checks img is valid and has contents.
        iimg_ptr internalmesh::validate( iimg_ptr p ) const
        {
            if( p.get() == nullptr ||
                    p->img_bgra_pixels() == nullptr ||
                    p->img_height() == 0 ||
                    p->img_width() == 0 )
            {
                ss_throw( "Invalid image passed to deferred_renderer." );
            }

            return p;
        }

        internalmesh::details internalmesh::setup_texture_states() const
        {
            assert( msh_textures_[0] );
            assert( msh_textures_[1] );
            assert_opengl_ok( __LINE__ );

            glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_MODEL_DIFFSPEC );
            glBindTexture( GL_TEXTURE_2D, msh_textures_[0] );

            glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_MODEL_NRMLEMISS );
            glBindTexture( GL_TEXTURE_2D, msh_textures_[1] );

            assert_opengl_ok( __LINE__ );

            return details_;
        }

        unsigned int internalmesh::tricount() const
        {
            assert( details_.indicesCount > 1 );

            return ( details_.layout == GL_TRIANGLE_STRIP )
                   ? ( details_.indicesCount / 3 )
                   : ( details_.indicesCount - 2 );
        }
    }

    //-------------------------------------------------------------------------

    deferred_renderer::point_light::point_light()
        : worldX( 0.0f ), worldY( 0.0f ), worldZ( 0.0f ), radius( 1.0f ), red( 1.0f ), green( 1.0f ), blue( 1.0f )
    {
    }

    //-------------------------------------------------------------------------

    deferred_renderer::directional_light::directional_light()
        : directionX( 1.0f ), directionY( 0.0f ), directionZ( 0.0f ), red( 1.0f ), green( 1.0f ), blue( 1.0f )
    {
    }

    //-------------------------------------------------------------------------

    deferred_renderer::screen_quad::screen_quad()
    {
        texCoord.x0 = texCoord.y0 = texCoord.x1 = texCoord.y1 = 0.0f;
        cornerColours[0] = cornerColours[1] = cornerColours[2] = cornerColours[3] = bgra( 0xFF );
    }

    void deferred_renderer::screen_quad::set_colour( bgra c )
    {
        cornerColours[0] = cornerColours[1] = cornerColours[2] = cornerColours[3] = c;
    }

    //-------------------------------------------------------------------------

    namespace
    {

        // Emulate the GLSL types to ensure this is precisely the same as
        // the shader code.

        class vec3
        {
        public:

            inline vec3( float f ) : x( f ), y( f ), z( f )
            {
            }

            inline vec3( const float* xyz ) : x( xyz[0] ), y( xyz[1] ), z( xyz[2] )
            {
            }

            inline vec3( float x, float y, float z ) : x( x ), y( y ), z( z )
            {
            }

            float operator[]( size_t n ) const
            {
                return n == 0 ? x : ( n == 1 ? y : z );
            }

            float x;
            float y;
            float z;
        };

        class vec2
        {
        public:

            inline vec2( float f ) : x( f ), y( f )
            {
            }

            inline vec2( const float* xy ) : x( xy[0] ), y( xy[1] )
            {
            }

            inline vec2( float x, float y ) : x( x ), y( y )
            {
            }

            float x;
            float y;
        };

        class mat3
        {
        public:

            inline mat3( const vec3& a, const vec3& b, const vec3& c )
                : col0( a )
                , col1( b )
                , col2( c )
            {
            }

            vec3 col0;
            vec3 col1;
            vec3 col2;
        };

#if 0
        inline vec3 operator-( const vec3& lhs, const vec3& rhs )
        {
            return vec3( lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z );
        }

        inline vec3 operator*( const vec3& lhs, const vec3& rhs )
        {
            return vec3( lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z );
        }

        inline vec2 operator-( const vec2& lhs, const vec2& rhs )
        {
            return vec2( lhs.x - rhs.x, lhs.y - rhs.y );
        }

        inline vec3 normalize( const vec3& v )
        {
            const float f = sqrt( ( v.x * v.x ) + ( v.y * v.y ) + ( v.z * v.z ) );
            return vec3( v.x / f, v.y / f, v.z / f );
        }

        inline vec3 cross( const vec3& lhs, const vec3& rhs )
        {
            const float c1 = lhs.x;
            const float c2 = lhs.y;
            const float c3 = lhs.z;
            const float d1 = rhs.x;
            const float d2 = rhs.y;
            const float d3 = rhs.z;
            return vec3( c2 * d3 - c3 * d2,
                         c3 * d1 - c1 * d3,
                         c1 * d2 - c2 * d1 );
        }
#endif

    }

    imesh::~imesh() SS_NOEXCEPT
    {
    }

    //-------------------------------------------------------------------------

    deferred_renderer::dynbufferdrawparams::dynbufferdrawparams()
        : buffer( nullptr )
        , texture( texturehandle_t() )
        , num_indices_to_draw( 0 )
        , applyView( true )
        , applyProj( true )
        , writeZ( false )
        , readZ( true )
        , alphaBlend( true )
        , strips( false )
    {
        memset( matrix, 0, sizeof( matrix ) );
        matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
        rgba[0] = rgba[1] = rgba[2] = rgba[3] = 0.5f;
    }

    //-------------------------------------------------------------------------

    struct suninstance
    {
        suninstance( point4d xyzr, texturehandle_t h, const sun_noise& n )
        {
            load_suninstance( xyzr, h, n );
        }

        void load_suninstance( point4d xyzr, texturehandle_t h, const sun_noise& n )
        {
            assert( h != texturehandle_t() );
            position_and_radius_ = xyzr;
            sun_texture_         = h;
            noise_               = n;
        }

        point4d position_and_radius() const
        {
            return position_and_radius_;
        }

        texturehandle_t sun_texture() const
        {
            return sun_texture_;
        }

        float angle() const
        {
            return noise_.noise_rotation_;
        }

        float noise_move_x() const
        {
            return noise_.noise_move_x_;
        }

        float noise_move_y() const
        {
            return noise_.noise_move_y_;
        }

        texturehandle_t sun_texture_;
        point4d         position_and_radius_;
        sun_noise       noise_;
    };

    //-------------------------------------------------------------------------

    struct internalsfx
    {
        internalsfx( GLuint t1, GLuint t2, const colour& c, const fxtriangle* fx, size_t n )
        {
            load( t1, t2, c, fx, n );
        }

        void load( GLuint t1, GLuint t2, const colour& c, const fxtriangle* fx, size_t n )
        {
            assert( fx );
            assert( n > 0u );
            t1_ = t1;
            t2_ = t2;
            sfx_colour_ = c;
            triangles_.resize( n );
            memcpy( triangles_.data(), fx, n * sizeof( fxtriangle ) );
        }

        GLuint t1_;
        GLuint t2_;
        colour sfx_colour_;
        vector<fxtriangle> triangles_;
    };

    bool operator<( const internalsfx& lhs, const internalsfx& rhs );

    //! Sorts by textures and colour only.
    bool operator<( const internalsfx& lhs, const internalsfx& rhs )
    {
        return ( lhs.t1_ < rhs.t1_ ) || ( lhs.t1_ == rhs.t1_ && lhs.t2_ < rhs.t2_ )
               || ( lhs.t1_ == rhs.t1_ && lhs.t2_ == rhs.t2_ && lhs.sfx_colour_ < rhs.sfx_colour_ );
    }

    class sfxrenderer
    {
    public:
        static const size_t BatchTriangleCount = 1024;

        sfxrenderer( const program& sfxProgram, GLuint depthTex )
            : uniformLocationSfxT1Sampler_( 0 )
            , uniformLocationSfxT2Sampler_( 0 )
            , uniformLocationSfxDepthSampler_( 0 )
            , depthTex_( depthTex )
            , current_t1_()
            , current_t2_()
            , current_triangle_count_( 0u )
            , batch_of_triangles_( new fxtriangle[BatchTriangleCount] )
        {
            assert_opengl_ok( __LINE__ );

            glBindVertexArray( 0 );

            // Generate all the buffers at once:
            GLuint buffers[1];
            glGenBuffers( 1, buffers );

            GLuint vaos[1];
            glGenVertexArrays( 1, vaos );

            vao_ = vaos[0];
            tri_vbo_ = buffers[0];

            // 5 floats per triangle: x,y,z,u,v, 3 triangles per vert = 15
            static_assert( sizeof( fxtriangle ) == ( 15 * sizeof( GLfloat ) ),
                           "fxtriangle is not 15 GLfloats" );

            glBindBuffer( GL_ARRAY_BUFFER, tri_vbo_ );
            glBufferData( GL_ARRAY_BUFFER,
                          static_cast<GLsizeiptr>( BatchTriangleCount * sizeof( fxtriangle ) ),
                          nullptr,
                          GL_STREAM_DRAW );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            assert_opengl_ok( __LINE__ );

            // Setup Vertex Array Object:

            glBindVertexArray( vao_ );
            {
                glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
                glBindBuffer( GL_ARRAY_BUFFER, tri_vbo_ );
                glVertexAttribPointer(
                    SFX_VRTX_CHNL, 3, GL_FLOAT, GL_FALSE, sizeof( GLfloat ) * 5, SS_BUFFER_OFFSET( 0 ) );
                glVertexAttribPointer( SFX_UV_CHNL,
                                       2,
                                       GL_FLOAT,
                                       GL_FALSE,
                                       sizeof( GLfloat ) * 5,
                                       SS_BUFFER_OFFSET( sizeof( GLfloat ) * 3 ) );

                glEnableVertexAttribArray( SFX_VRTX_CHNL );
                glEnableVertexAttribArray( SFX_UV_CHNL );

                for( GLuint i = 2; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
                {
                    glDisableVertexAttribArray( i );
                    assert_opengl_ok( __LINE__ );
                }

                for( GLuint i = 0; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
                {
                    glVertexAttribDivisor( i, 0 );
                    assert_opengl_ok( __LINE__ );
                }
            }
            glBindVertexArray( 0 );
            assert_opengl_ok( __LINE__ );

            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            assert_opengl_ok( __LINE__ );

            if( false == opengl_no_errors( __LINE__ ) )
            {
                free_sfx_buffers();
                ss_throw( "Unable to create sfxrenderer object." );
            }

            uniformLocationSfxT1Sampler_ = glGetUniformLocation( sfxProgram, "texT1" );
            uniformLocationSfxT2Sampler_ = glGetUniformLocation( sfxProgram, "texT2" );
            uniformLocationSfxDepthSampler_ = glGetUniformLocation( sfxProgram, "texDepth" );
            uniformLocationSfxRGBA_ = glGetUniformLocation( sfxProgram, "sfxRGBA" );

#ifdef SS_DEBUGGING_RENDERER
            uniformLocationSfxDebugRenderMode_
                = glGetUniformLocation( sfxProgram, "DebugMode" );
#endif

            if( -1 == uniformLocationSfxT1Sampler_    ||
                -1 == uniformLocationSfxT2Sampler_    ||
                -1 == uniformLocationSfxDepthSampler_ ||
                -1 == uniformLocationSfxRGBA_ )
            {
                assert_opengl_ok( __LINE__ );
                assert( !"Missing uniforms." );
                uniformLocationSfxT1Sampler_ = 0;
                uniformLocationSfxT2Sampler_ = 0;
                uniformLocationSfxDepthSampler_ = 0;
                uniformLocationSfxRGBA_ = 0;
                free_sfx_buffers();
                assert_opengl_ok( __LINE__ );
                ss_throw( "Missing uniforms in fxtriangle shader." );
            }
        }

        ~sfxrenderer()
        {
            free_sfx_buffers();
        }

        void free_sfx_buffers() SS_NOEXCEPT
        {
            delete_buffer( tri_vbo_ );
            delete_buffer( vao_ );
        }

        void start_sfx()
        {
            assert_opengl_ok( __LINE__ );
            assert( current_triangle_count_ == 0u );

            const float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glUniform1i( uniformLocationSfxT1Sampler_, TEX_SFX_T1 );
            glUniform1i( uniformLocationSfxT2Sampler_, TEX_SFX_T2 );
            glUniform1i( uniformLocationSfxDepthSampler_, TEX_SFX_DEPTH );
            glUniform4fv( uniformLocationSfxRGBA_, 1, white );
            glBindBuffer( GL_ARRAY_BUFFER, tri_vbo_ );
            glBindVertexArray( vao_ );
            glDisable( GL_CULL_FACE );
            glEnable( GL_DEPTH_TEST );
            glEnable( GL_BLEND );
            assert_opengl_ok( __LINE__ );

            // Use pure additive blend on the diffuse colour.
            glBlendFunci( OUTPUT_INDEX_ERGBDR, GL_ONE, GL_ONE );

            // Use classic alpha blending on the warp outputs.
            glBlendFunci( OUTPUT_INDEX_DGBWXY, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

            glActiveTexture( GL_TEXTURE0 + TEX_SFX_DEPTH );
            glBindTexture( GL_TEXTURE_2D, depthTex_ );

            current_t1_ = texturehandle_t();
            current_t2_ = texturehandle_t();
            current_colour_ = solosnake::colour( 1.0f, 1.0f, 1.0f, 1.0f );

            assert( opengl_ztest_is_enabled( true ) );
            assert( opengl_zwrite_is_enabled( false ) );
            assert_opengl_ok( __LINE__ );
        }

        void render_sfx( const internalsfx& fx )
        {
            if( current_t1_ != fx.t1_ || current_t2_ != fx.t2_ || current_colour_ != fx.sfx_colour_ )
            {
                if( current_triangle_count_ > 0u )
                {
                    draw_current_sfx_buffer();
                }

                if( current_t1_ != fx.t1_ )
                {
                    current_t1_ = fx.t1_;
                    glActiveTexture( GL_TEXTURE0 + TEX_SFX_T1 );
                    glBindTexture( GL_TEXTURE_2D, current_t1_ );
                    assert_opengl_ok( __LINE__ );
                }

                if( current_t2_ != fx.t2_ )
                {
                    current_t2_ = fx.t2_;
                    glActiveTexture( GL_TEXTURE0 + TEX_SFX_T2 );
                    glBindTexture( GL_TEXTURE_2D, current_t2_ );
                    assert_opengl_ok( __LINE__ );
                }

                if( current_colour_ != fx.sfx_colour_ )
                {
                    current_colour_ = fx.sfx_colour_;
                    glUniform4fv( uniformLocationSfxRGBA_, 1, current_colour_ );
                    assert_opengl_ok( __LINE__ );
                }
            }

            size_t remaining_to_draw = fx.triangles_.size();

            while( remaining_to_draw > 0 )
            {
                size_t freespace = BatchTriangleCount - current_triangle_count_;
                size_t added_triangles = remaining_to_draw > freespace ? freespace : remaining_to_draw;

                memcpy( batch_of_triangles_.get() + BatchTriangleCount - freespace,
                        fx.triangles_.data() + fx.triangles_.size() - remaining_to_draw,
                        sizeof( fxtriangle ) * added_triangles );

                remaining_to_draw -= added_triangles;
                current_triangle_count_ += added_triangles;

                if( current_triangle_count_ == BatchTriangleCount )
                {
                    draw_current_sfx_buffer();
                    assert_opengl_ok( __LINE__ );
                }
            }
        }

        void end_sfx()
        {
            if( current_triangle_count_ > 0u )
            {
                draw_current_sfx_buffer();
            }

            glDisable( GL_BLEND );
            glEnable( GL_CULL_FACE );
            glBindVertexArray( 0 );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            assert_opengl_ok( __LINE__ );
        }

    private:

        void draw_current_sfx_buffer()
        {
            assert( current_t1_ != texturehandle_t() );
            assert( current_t2_ != texturehandle_t() );
            assert( current_triangle_count_ > 0u );

            auto data_size = current_triangle_count_ * sizeof( fxtriangle );
            assert_opengl_ok( __LINE__ );
            glBufferData( GL_ARRAY_BUFFER, data_size, NULL, GL_STREAM_DRAW );
            assert_opengl_ok( __LINE__ );

            glBufferSubData( GL_ARRAY_BUFFER, 0, data_size, batch_of_triangles_.get() );
            assert_opengl_ok( __LINE__ );

            glDrawArrays( GL_TRIANGLES, 0, static_cast<GLsizei>( 3 * current_triangle_count_ ) );
            assert_opengl_ok( __LINE__ );

            current_triangle_count_ = 0u;
        }

    private:

        GLObjectHandle          vao_;
        GLObjectHandle          tri_vbo_;
        GLint                   uniformLocationSfxT1Sampler_;
        GLint                   uniformLocationSfxT2Sampler_;
        GLint                   uniformLocationSfxDepthSampler_;
        GLint                   uniformLocationSfxRGBA_;
        GLuint                  depthTex_;
#ifdef SS_DEBUGGING_RENDERER
        GLint                   uniformLocationSfxDebugRenderMode_;
#endif
        texturehandle_t         current_t1_;
        texturehandle_t         current_t2_;
        colour                  current_colour_;
        size_t                  current_triangle_count_;
        unique_ptr<fxtriangle>  batch_of_triangles_;
    };

    //-------------------------------------------------------------------------

    class internaldynbuffer : public dynbuffer
    {
    public:

        GLObjectHandle  db_vuv_vbo_;
        GLObjectHandle  db_clr_vbo_;
        GLObjectHandle  db_ibo_;
        GLObjectHandle  db_vao_;
        unsigned int    db_queuecount_;
        bool            db_unlocked_;
        bool            db_wanted_;
        bool            db_inVRAM_;

        void free_buffers() SS_NOEXCEPT
        {
            assert( db_queuecount_ == 0 && db_unlocked_ );
            delete_buffer( db_vuv_vbo_ );
            delete_buffer( db_clr_vbo_ );
            delete_buffer( db_ibo_ );
            delete_vertexarray( db_vao_ );
        }

        internaldynbuffer( const unsigned int nv, const unsigned int ni )
            : dynbuffer( nv, ni )
            , db_queuecount_( 0 )
            , db_unlocked_( true )
            , db_wanted_( true )
            , db_inVRAM_( false )
        {
            assert_opengl_ok( __LINE__ );

            glBindVertexArray( 0 );

            // Generate all the buffers at once:
            GLuint buffers[3];
            glGenBuffers( 3, buffers );

            GLuint vaos[1];
            glGenVertexArrays( 1, vaos );

            // The 'handles' are not an array types so we need to do the manual
            // copy:
            db_vuv_vbo_ = buffers[0];
            db_clr_vbo_ = buffers[1];
            db_ibo_ = buffers[2];
            db_vao_ = vaos[0];

            glBindBuffer( GL_ARRAY_BUFFER, db_vuv_vbo_ );
            glBufferData( GL_ARRAY_BUFFER,
                          static_cast<GLsizeiptr>( nv * ( sizeof( dynbuffervertex ) + sizeof( dynbufferuv ) ) ),
                          nullptr,
                          GL_STREAM_DRAW );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );

            glBindBuffer( GL_ARRAY_BUFFER, db_clr_vbo_ );
            glBufferData( GL_ARRAY_BUFFER,
                          static_cast<GLsizeiptr>( nv * sizeof( dynbufferRGBA ) ),
                          nullptr,
                          GL_STREAM_DRAW );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );

            assert_opengl_ok( __LINE__ );

            // Index buffer:
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, db_ibo_ );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                          static_cast<GLsizeiptr>( ni * sizeof( GLushort ) ),
                          nullptr,
                          GL_STREAM_DRAW );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

            assert_opengl_ok( __LINE__ );

            //
            // Vertex Array Objects:
            //

            glBindVertexArray( db_vao_ );

            // Bind the IBO for the VAO
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, db_ibo_ );

            // Bind the 1st VBO (verts and uvs) and setup pointers for the VAO
            glBindBuffer( GL_ARRAY_BUFFER, db_vuv_vbo_ );
            {
                static_assert( sizeof( dynbuffervertex ) / sizeof( GLfloat ) == 3,
                               "Size of dynbuffervertex is not 3 GLfloats!" );
                static_assert( sizeof( dynbufferuv ) / sizeof( GLfloat ) == 2,
                               "Size of dynbufferuv is not 2 GLfloats!" );

                glVertexAttribPointer( DYN_VRTX_CHNL, 3, GL_FLOAT, GL_FALSE, 0, SS_BUFFER_OFFSET( 0 ) );
                glVertexAttribPointer( DYN_UV_CHNL,
                                       2,
                                       GL_FLOAT,
                                       GL_FALSE,
                                       0,
                                       SS_BUFFER_OFFSET( ( sizeof( dynbuffervertex ) * nv ) ) );
            }

            // Bind 2nd VBO (colours) and setup pointers for VAO.
            glBindBuffer( GL_ARRAY_BUFFER, db_clr_vbo_ );
            {
                static_assert( sizeof( dynbufferRGBA ) / sizeof( GLchar ) == 4,
                               "Size of dynbufferRGBA is not 4 chars!" );
                glVertexAttribPointer( DYN_CLR_CHNL, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, SS_BUFFER_OFFSET( 0 ) );
            }

            glEnableVertexAttribArray( DYN_VRTX_CHNL );
            glEnableVertexAttribArray( DYN_UV_CHNL );
            glEnableVertexAttribArray( DYN_CLR_CHNL );

            for( GLuint i = 3; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
            {
                glDisableVertexAttribArray( i );
            }

            for( GLuint i = 0; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
            {
                glVertexAttribDivisor( i, 0 );
            }

            // Finished with fullscreen VAO:
            glBindVertexArray( 0 );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

            if( false == opengl_no_errors( __LINE__ ) )
            {
                free_buffers();
                ss_throw( "Unable to create internaldynbuffer object." );
            }

            assert_opengl_ok( __LINE__ );
        }

        virtual ~internaldynbuffer() SS_NOEXCEPT
        {
            free_buffers();
        }
    };

    //-------------------------------------------------------------------------

    //! Hidden pimpl implementation of deferred_renderer.
    class deferred_renderer::internalrenderer
    {
    public:

        internalrenderer( GLsizei screen_width,
                          GLsizei screen_height,
                          deferred_renderer::Quality quality );

        ~internalrenderer() SS_NOEXCEPT;

        void resize_screen( GLsizei, GLsizei );

        meshhandle_t load_mesh( const shared_ptr<imesh>& );

        texturehandle_t load_texture( const shared_ptr<iimgsrc>& );

        texturehandle_t load_sun_colour_ramp_texture( const iimgBGR_ptr& colour_ramp );

        dynbuffer* alloc_dynbuffer( unsigned int, unsigned int );

        void free_dynbuffer( internaldynbuffer* );

        void lock_dynbuffer( internaldynbuffer* );

        void unlock_dynbuffer( internaldynbuffer*, const unsigned int, const unsigned int, const unsigned int );

        void draw_dynbuffer( const deferred_renderer::dynbufferdrawparams& );

        void set_viewport( int x, int y, unsigned int, unsigned int );

        void set_view_matrix( const float* unaliased );

        void set_proj_matrix( const float* unaliased, float );

        void load_skybox( const cubemap& );

        void enable_skybox( const bool );

        void set_skybox_3x3_matrix( const float* unaliased );

        void set_debug_rendermode( deferred_renderer::DebugRenderMode );

        void get_view_matrix( float* unaliased m4x4 ) const;

        void get_inv_view_matrix( float* unaliased m4x4 ) const;

        void start_scene();

        void add_mesh_instances( meshhandle_t, const meshinstancedata* const, GLsizei );

        void set_ambiant_light( float, float, float );

        void draw_directional_light( const deferred_renderer::directional_light& );

        void draw_screen_quads( texturehandle_t, const deferred_renderer::screen_quad*, const unsigned int );

        void draw_sfx( bool, texturehandle_t, texturehandle_t, const colour&, const fxtriangle* x, const unsigned int );

        void draw_point_light( const deferred_renderer::point_light& );

        void draw_sun( texturehandle_t, const sun_noise&, const sun_location_and_radius& );

        void end_scene();

        unsigned int scene_triangle_count() const;

        void un_project( unsigned int, unsigned int, float, float* unaliased ) const;

        meshhandle_t get_new_meshhandle();

        deferred_renderer::Quality quality_level() const
        {
            return qualityLevel_;
        }

    private:
        typedef map<meshhandle_t, shared_ptr<internalmesh>> meshmap_t;

    private:

        internalrenderer( const internalrenderer& );

        internalrenderer& operator=( const internalrenderer& );

        internalmesh* get_mesh( meshhandle_t h ) const;

        void cleanup() SS_NOEXCEPT;

        void update_uniform_block();

        void clear_meshs();

        void clear_states();

        void clear_lights_batch();

        void clear_quads_batch();

        void set_renderviewport_size();

        void setup_states();

        void setup_vsynch_preferences();

        bool are_opengl_requirements_met() const;

        void construct_render_targets( GLsizei w, GLsizei h );

        void assemble_lighting_shader();

        void assemble_mesh_shader();

        void assemble_composition_shader();

        void assemble_screen_quads_shader();

        void assemble_suns_spheres_shader();

        void assemble_suns_coronas_shader();

        void assemble_skybox_shader();

        void assemble_sfx_shader();

        void assemble_posteffect_shader();

        void assemble_dynbuffer_shader();

        void unload_skybox();

        void init_matrices();

        void really_free_dynbuffer( const internaldynbuffer* const );

        void create_globals_uniform_block();

        void create_instance_matrix_buffers();

        void create_screen_quads_quad_buffers();

        void create_sfx();

        void create_noise_textures( std::future<std::vector<std::uint8_t>>&&, const GLsizei size );

        void create_suns_spheres_vao_and_vbo();

        void create_suns_coronas_vao_and_vbo();

        void create_fullscreen_quad_and_light_sphere_and_skybox();

        void cleanup_shaders();

        void cleanup_rendertargets();

        void cleanup_buffers();

        void cleanup_loaded_textures();

        void cleanup_dynbuffers();

        void cleanup_pending_dynbuffers();

        void cleanup_skybox();

        void cleanup_noise_textures();

        void move_to_lighting_stage();

        void render_sun_spheres();

        void render_sun_godrays();

        void render_lights();

        void render_point_lights();

        void render_directional_lights();

        void render_point_light( const float[4], const float[4], const float[4] );

        void render_directional_light( const float[4], const float[4], const float[4] );

        void render_diffuse_sfx();

        void render_emissive_sfx();

        void render_dynbuffers();

        void render_dynbuffer( const deferred_renderer::dynbufferdrawparams& );

        void render_screen_quads();

        void render_screen_quads( const pair<unsigned int, vector<screen_quad_in_vb_t>>& );

        bool is_valid_screen_quads_pagehandle( texturehandle_t ) const;

        pair<unsigned int, vector<screen_quad_in_vb_t>>* get_current_quadbatch();

        pair<unsigned int, vector<screen_quad_in_vb_t>>* get_next_quadbatch();

        string mesh_shader_frag() const;

        string mesh_shader_vert() const;

        string lighting_shader_frag() const;

        string lighting_shader_vert() const;

        string composition_shader_frag() const;

        string composition_shader_vert() const;

        string posteffect_shader_frag() const;

        string posteffect_shader_vert() const;

        string screen_quads_shader_frag() const;

        string screen_quads_shader_vert() const;

        string dynbuffer_shader_frag() const;

        string dynbuffer_shader_vert() const;

        string suns_spheres_shader_frag() const;

        string suns_spheres_shader_vert() const;

        string suns_coronas_shader_frag() const;

        string suns_coronas_shader_vert() const;

        string sfx_shader_frag() const;

        string sfx_shader_vert() const;

        string skybox_shader_frag() const;

        string skybox_shader_vert() const;

        inline void enable_primitive_restart( bool );

        GLuint load_cubemap( const cubemap& );

        texturehandle_t load_texture( const shared_ptr<iimgBGR>&, bool, bool );

    private:

        enum SceneState
        {
            SceneClosed,
            SceneOpen
        };

        struct UniformBlockInfo
        {
            UniformBlockInfo() : offset( 0 ), arrayStrides( 0 ), matrixStrides( 0 )
            {
            }
            GLint offset;
            GLint arrayStrides;
            GLint matrixStrides;
        };

        meshmap_t                                               meshes_;
        vector<GLbyte>                                          uniformBlockClientCopy_;
        vector<GLuint>                                          loaded_textures_;
#ifndef NDEBUG
        vector<string>                                          loaded_texture_sources_;
#endif
        list<pair<unsigned int, vector<screen_quad_in_vb_t>>>   screen_quads_quads_;
        size_t                                                  screen_quads_current_batchindex_;
        vector<deferred_renderer::point_light>                  point_lights_;
        vector<deferred_renderer::directional_light>            directional_lights_;
        vector<unique_ptr<internaldynbuffer>>              activedynbuffers_;
        vector<deferred_renderer::dynbufferdrawparams>          pendingdynbuffers_;
        unique_ptr<sfxrenderer>                                 sfxrenderer_;
        vector<float>                                           sun_vbo_data_;
        vector<unique_ptr<suninstance>>                         sun_instances_;
        size_t                                                  sun_instance_count_;
        vector<unique_ptr<internalsfx>>                         diffuse_sfx_;
        size_t                                                  diffuse_sfxcount_;
        vector<unique_ptr<internalsfx>>                         emissive_sfx_;
        size_t                                                  emissive_sfxcount_;
        viewprojmatrices                                        matrices_;
        GLfloat                                                 skyboxRotation3x3_[9];
        GLfloat                                                 ambientRGBA_[4];
        bool                                                    ambient_changed_;
        UniformBlockInfo                                        uniformBlockViewInfo_;
        UniformBlockInfo                                        uniformBlockProjInfo_;
        UniformBlockInfo                                        uniformBlockViewProjInfo_;
        UniformBlockInfo                                        uniformBlockProjInvInfo_;
        UniformBlockInfo                                        uniformBlockViewport_;
        GLint                                                   lightStencilNumber_;
        GLObjectHandle                                          uniformDataBuffer_;
        GLObjectHandle                                          instanceMatrixVBO_;
        GLObjectHandle                                          screen_quadsQuadsIBO_;
        GLObjectHandle                                          screen_quadsQuadsVAOs_[2];
        GLObjectHandle                                          screen_quadsQuadsVBOs_[2];
        size_t                                                  currentQuadVAOIndex_;
        GLObjectHandle                                          staticGeometryVBO_;
        GLObjectHandle                                          fullscreenquadVAO_;
        GLObjectHandle                                          lightSphereVAO_;
        GLObjectHandle                                          sunSpheresVBO_;
        GLObjectHandle                                          sunSpheresVAO_;
        GLObjectHandle                                          sunCoronasVBO_;
        GLObjectHandle                                          sunCoronasVAO_;
        GLObjectHandle                                          skyboxVAO_;
        GLObjectHandle                                          drawModelsFBO_;
        GLObjectHandle                                          drawLightingFBO_;
        GLObjectHandle                                          diffuseSfxFBO_;
        GLObjectHandle                                          emissiveSfxFBO_;
        GLObjectHandle                                          sunsSpheresFBO_;
        GLObjectHandle                                          sunsCoronaFBO_;
        GLObjectHandle                                          compositionFBO_;
        GLObjectHandle                                          posteffectFBO_;
        GLObjectHandle                                          depthTexture_;
        GLObjectHandle                                          sceneDiffGBWarpXY_;
        GLObjectHandle                                          meshNormalsSpecANDCompositionPassRenderTargetTexture_;
        GLObjectHandle                                          sceneEmissRGBDiffR_;
        GLObjectHandle                                          lightingDiffComponentTexture_;
        GLObjectHandle                                          lightingSpecComponentTexture_;
        GLObjectHandle                                          skyboxTexture_;
        GLObjectHandle                                          noiseTexture_;
        GLint                                                   uniformLocationPosteffectSceneSampler_;
        GLint                                                   uniformLocationPosteffectWarpSampler_;
        GLint                                                   uniformLocationCompositionLightingSpecSampler_;
        GLint                                                   uniformLocationCompositionLightingDiffSampler_;
        GLint                                                   uniformLocationCompositionModelDgbWxySampler_;
        GLint                                                   uniformLocationCompositionModelErgbDrSampler_;
        GLint                                                   uniformLocationBlurSourceSampler_;
        GLint                                                   uniformLocationCompositionAmbientLight_;
        GLint                                                   uniformLocationLightingNormalsSpecSampler_;
        GLint                                                   uniformLocationLightingDepthSampler_;
        GLint                                                   uniformLocationLightingLightColourAndIsPoint_;
        GLint                                                   uniformLocationLightingLightViewCentreAndRadius_;
        GLint                                                   uniformLocationLightingLightCentreAndRadius_;
        GLint                                                   uniformLocationModelDiffuseBGRSpecASampler_;
        GLint                                                   uniformLocationModelNormalsBGEmissRASampler_;
        GLint                                                   uniformLocationQuadsSampler_;
        GLint                                                   uniformLocationSkyboxSampler_;
        GLint                                                   uniformLocationSkyboxMat3x3_;
        GLint                                                   uniformLocationDynbufferSampler_;
        GLint                                                   uniformLocationDynbufferBlendColour_;
        GLint                                                   uniformLocationDynbufferTransform_;
        GLint                                                   uniformLocationSunsNoiseSampler_;
        GLint                                                   uniformLocationSunsRampSampler_;
        GLint                                                   uniformLocationSunsCoronaErgbDrSampler_;
        GLsizei                                                 buffersWidth_;
        GLsizei                                                 buffersHeight_;
        GLsizei                                                 renderViewportWidth_;
        GLsizei                                                 renderViewportHeight_;
        GLint                                                   viewportX_;
        GLint                                                   viewportY_;
        GLint                                                   viewportWidth_;
        GLint                                                   viewportHeight_;
        GLint                                                   pendingViewportX_;
        GLint                                                   pendingViewportY_;
        GLint                                                   pendingViewportWidth_;
        GLint                                                   pendingViewportHeight_;
        program                                                 meshProgram_;
        program                                                 lightingProgram_;
        program                                                 compositionProgram_;
        program                                                 posteffectProgram_;
        program                                                 screen_quadsProgram_;
        program                                                 sfxProgram_;
        program                                                 dynbufferProgram_;
        program                                                 skyboxProgram_;
        program                                                 sunsSpheresProgram_;
        program                                                 sunsCoronasProgram_;
        SceneState                                              state_;
        meshhandle_t                                            next_meshhandle_;
        deferred_renderer::Quality                              qualityLevel_;
        bool                                                    need_to_update_uniform_block_;
        bool                                                    show_skybox_;
        bool                                                    primitiveRestartEnabled_;
        unsigned long                                           scene_triangle_count_;
#ifdef SS_DEBUGGING_RENDERER
        deferred_renderer::DebugRenderMode                      debugRenderMode_;
        GLint                                                   uniformLocationModelDebugRenderMode_;
        GLint                                                   uniformLocationLightingDebugRenderMode_;
        GLint                                                   uniformLocationDynbufferDebugRenderMode_;
        GLint                                                   uniformLocationQuadsDebugRenderMode_;
        GLint                                                   uniformLocationCompositionDebugRenderMode_;
        GLint                                                   uniformLocationPosteffectDebugRenderMode_;
#endif
    };

    //-------------------------------------------------------------------------

#include "deferred_renderer_shaders.inl"

    //-------------------------------------------------------------------------

    //! Constructs the back buffers needed.
    //! Loads the shader program.
    //! Creates the uniform buffers.
    deferred_renderer::internalrenderer::internalrenderer( GLsizei buffers_width,
                                                           GLsizei buffers_height,
                                                           Quality quality )
        : meshes_()
        , uniformBlockClientCopy_()
        , loaded_textures_()
        , screen_quads_quads_()
        , screen_quads_current_batchindex_( NO_SCREEN_QUADS_THIS_FRAME )
        , point_lights_()
        , directional_lights_()
        , activedynbuffers_()
        , pendingdynbuffers_()
        , sfxrenderer_()
        , sun_vbo_data_()
        , sun_instances_()
        , sun_instance_count_( 0u )
        , diffuse_sfx_()
        , diffuse_sfxcount_( 0u )
        , emissive_sfx_()
        , emissive_sfxcount_( 0u )
        , matrices_()
        , ambient_changed_( true )
        , lightStencilNumber_( 0 )
        , uniformDataBuffer_( 0 )
        , instanceMatrixVBO_( 0 )
        , currentQuadVAOIndex_( 0 )
        , staticGeometryVBO_( 0 )
        , fullscreenquadVAO_( 0 )
        , lightSphereVAO_( 0 )
        , sunSpheresVBO_( 0 )
        , sunSpheresVAO_( 0 )
        , sunCoronasVBO_( 0 )
        , sunCoronasVAO_( 0 )
        , skyboxVAO_( 0 )
        , drawModelsFBO_( 0 )
        , drawLightingFBO_( 0 )
        , diffuseSfxFBO_( 0 )
        , emissiveSfxFBO_( 0 )
        , sunsSpheresFBO_( 0 )
        , sunsCoronaFBO_( 0 )
        , compositionFBO_( 0 )
        , posteffectFBO_( 0 )
        , depthTexture_( 0 )
        , uniformLocationPosteffectSceneSampler_( -1 )
        , uniformLocationPosteffectWarpSampler_( -1 )
        , uniformLocationCompositionLightingSpecSampler_( -1 )
        , uniformLocationCompositionLightingDiffSampler_( -1 )
        , uniformLocationCompositionModelDgbWxySampler_( -1 )
        , uniformLocationCompositionModelErgbDrSampler_( -1 )
        , uniformLocationBlurSourceSampler_( -1 )
        , uniformLocationCompositionAmbientLight_( -1 )
        , uniformLocationLightingDepthSampler_( -1 )
        , uniformLocationLightingLightColourAndIsPoint_( -1 )
        , uniformLocationLightingLightViewCentreAndRadius_( -1 )
        , uniformLocationLightingLightCentreAndRadius_( -1 )
        , uniformLocationModelDiffuseBGRSpecASampler_( -1 )
        , uniformLocationModelNormalsBGEmissRASampler_( -1 )
        , uniformLocationQuadsSampler_( -1 )
        , uniformLocationSkyboxSampler_( -1 )
        , uniformLocationSkyboxMat3x3_( -1 )
        , uniformLocationDynbufferSampler_( -1 )
        , uniformLocationDynbufferBlendColour_( -1 )
        , uniformLocationDynbufferTransform_( -1 )
        , uniformLocationSunsNoiseSampler_( -1 )
        , uniformLocationSunsRampSampler_( -1 )
        , uniformLocationSunsCoronaErgbDrSampler_( -1 )
        , buffersWidth_( 0 )
        , buffersHeight_( 0 )
        , renderViewportWidth_( 0 )
        , renderViewportHeight_( 0 )
        , viewportX_( 0 )
        , viewportY_( 0 )
        , viewportWidth_( 0 )
        , viewportHeight_( 0 )
        , pendingViewportX_( 0 )
        , pendingViewportY_( 0 )
        , pendingViewportWidth_( 0 )
        , pendingViewportHeight_( 0 )
        , state_( SceneClosed )
        , next_meshhandle_( 0 )
        , qualityLevel_( quality )
        , need_to_update_uniform_block_( true )
        , show_skybox_( false )
        , primitiveRestartEnabled_( false )
        , scene_triangle_count_( 0u )
#ifdef SS_DEBUGGING_RENDERER
        , debugRenderMode_( deferred_renderer::SceneRenderMode )
        , uniformLocationModelDebugRenderMode_( -1 )
        , uniformLocationLightingDebugRenderMode_( -1 )
        , uniformLocationDynbufferDebugRenderMode_( -1 )
        , uniformLocationQuadsDebugRenderMode_( -1 )
        , uniformLocationCompositionDebugRenderMode_( -1 )
        , uniformLocationPosteffectDebugRenderMode_( -1 )
#endif
    {
        SS_INIT_OPENGL_EXTENSIONS();

        loaded_textures_.reserve( 128 );
        activedynbuffers_.reserve( 8 );
        pendingdynbuffers_.reserve( 8 );
        sun_vbo_data_.reserve( FLOATS_PER_SUN_VERTEX * 4 * 16 );
        sun_instances_.reserve( 16 );
        diffuse_sfx_.reserve( 8 );
        emissive_sfx_.reserve( 8 );

        // Reserve quad entries:
        for( size_t i = 0; i < 4; ++i )
        {
            screen_quads_quads_.push_back( pair<unsigned int, vector<screen_quad_in_vb_t>>() );
        }

        directional_lights_.reserve( 16 );
        point_lights_.reserve( 64 );

        skyboxRotation3x3_[0] = skyboxRotation3x3_[4] = skyboxRotation3x3_[8] = 1.0f;
        skyboxRotation3x3_[1] = skyboxRotation3x3_[2] = skyboxRotation3x3_[3] = 0.0f;
        skyboxRotation3x3_[5] = skyboxRotation3x3_[6] = skyboxRotation3x3_[7] = 0.0f;

        ambientRGBA_[0] = DEFAULT_AMBIENT_RED;
        ambientRGBA_[1] = DEFAULT_AMBIENT_GREEN;
        ambientRGBA_[2] = DEFAULT_AMBIENT_BLUE;
        ambientRGBA_[3] = DEFAULT_AMBIENT_ALPHA;

        if( buffers_width <= 0 || buffers_height <= 0 )
        {
            ss_throw( "Buffer width and buffer height must both be greater than zero." );
        }

        // Check to see if we are using a platform that requires some initialisation or
        // loading
        // phase or call for use of OpenGL 3.2 and 3.3: this call must be made after
        // the OpenGL context has been created but prior to any usage of functionality
        // not included in the OpenGL 'core' system:
#ifdef INIT_OPENGL_EXTENSIONS
        INIT_OPENGL_EXTENSIONS();
#endif

        validate_opengl_ok( __LINE__ );

        // Clear OpenGL error state.
        glGetError();

        if( are_opengl_requirements_met() )
        {
            try
            {
                // Run this expensive task in separate thread until needed.
                const power_of_2 noise_size( 7u );
                auto noisetask = std::async( std::launch::async,
                                             [&](){ return make_noise_texture_3d( noise_size, 4u ); } );

                init_matrices();
                validate_opengl_ok( __LINE__ );

                clear_states();
                validate_opengl_ok( __LINE__ );

                construct_render_targets( buffers_width, buffers_height );
                validate_opengl_ok( __LINE__ );

                set_viewport( 0,
                              0,
                              static_cast<unsigned int>( buffers_width ),
                              static_cast<unsigned int>( buffers_height ) );
                validate_opengl_ok( __LINE__ );

                create_noise_textures( std::move(noisetask), noise_size.value() );
                validate_opengl_ok( __LINE__ );

                assemble_lighting_shader();
                validate_opengl_ok( __LINE__ );

                assemble_mesh_shader();
                validate_opengl_ok( __LINE__ );

                assemble_composition_shader();
                validate_opengl_ok( __LINE__ );

                assemble_posteffect_shader();
                validate_opengl_ok( __LINE__ );

                assemble_sfx_shader();
                validate_opengl_ok( __LINE__ );

                assemble_screen_quads_shader();
                validate_opengl_ok( __LINE__ );

                assemble_dynbuffer_shader();
                validate_opengl_ok( __LINE__ );

                assemble_suns_spheres_shader();
                validate_opengl_ok( __LINE__ );

                assemble_suns_coronas_shader();
                validate_opengl_ok( __LINE__ );

                assemble_skybox_shader();
                validate_opengl_ok( __LINE__ );

                create_globals_uniform_block();
                validate_opengl_ok( __LINE__ );

                create_instance_matrix_buffers();
                validate_opengl_ok( __LINE__ );

                create_suns_spheres_vao_and_vbo();
                validate_opengl_ok( __LINE__ );

                create_suns_coronas_vao_and_vbo();
                validate_opengl_ok( __LINE__ );

                create_fullscreen_quad_and_light_sphere_and_skybox();
                validate_opengl_ok( __LINE__ );

                create_screen_quads_quad_buffers();
                validate_opengl_ok( __LINE__ );

                create_sfx();
                validate_opengl_ok( __LINE__ );

                setup_states();
                validate_opengl_ok( __LINE__ );

                setup_vsynch_preferences();
                validate_opengl_ok( __LINE__ );
            }
            catch( ... )
            {
                cleanup();
                throw;
            }
        }
        else
        {
            ss_throw( "The current OpenGL context does not meet the requirements of "
                      "the deferred_renderer." );
        }

        validate_opengl_ok( __LINE__ );
    }

    //! Clean up all OpenGL assets here.
    deferred_renderer::internalrenderer::~internalrenderer() SS_NOEXCEPT
    {
        cleanup();
    }

    //! Dtor cleanup. Never throws.
    void deferred_renderer::internalrenderer::cleanup() SS_NOEXCEPT
    {
        try
        {
            assert_opengl_ok( __LINE__ );

            cleanup_skybox();
            assert_opengl_ok( __LINE__ );

            cleanup_dynbuffers();
            assert_opengl_ok( __LINE__ );

            cleanup_loaded_textures();
            assert_opengl_ok( __LINE__ );

            clear_meshs();
            assert_opengl_ok( __LINE__ );

            clear_states();
            assert_opengl_ok( __LINE__ );

            cleanup_buffers();
            assert_opengl_ok( __LINE__ );

            cleanup_shaders();
            assert_opengl_ok( __LINE__ );

            cleanup_noise_textures();
            assert_opengl_ok( __LINE__ );

            cleanup_rendertargets();
            assert_opengl_ok( __LINE__ );
        }
        catch( ... )
        {
            // No exceptions exit this call.
        }
    }

    //! Returns a new mesh handle.
    inline meshhandle_t deferred_renderer::internalrenderer::get_new_meshhandle()
    {
        if( next_meshhandle_ == numeric_limits<meshhandle_t>::max() )
        {
            ss_throw( "Model handle limit reached." );
        }

        return ++next_meshhandle_;
    }

    //! So as we are not enabling this for no reason, track this state
    //! ourselves.
    inline void deferred_renderer::internalrenderer::enable_primitive_restart( bool desiredState )
    {
        if( primitiveRestartEnabled_ != desiredState )
        {
            if( desiredState )
            {
                glEnable( GL_PRIMITIVE_RESTART );
            }
            else
            {
                glDisable( GL_PRIMITIVE_RESTART );
            }

            primitiveRestartEnabled_ = desiredState;
        }
    }

    //! Clears the object's matrices to their default states.
    void deferred_renderer::internalrenderer::init_matrices()
    {
        matrices_ = viewprojmatrices();
    }

    void deferred_renderer::internalrenderer::construct_render_targets( GLsizei w, GLsizei h )
    {
        validate_opengl_ok( __LINE__ );

        assert( 0 == drawModelsFBO_ );
        assert( 0 == drawLightingFBO_ );
        assert( 0 == compositionFBO_ );
        assert( 0 == posteffectFBO_ );
        assert( 0 == diffuseSfxFBO_ );
        assert( 0 == emissiveSfxFBO_ );
        assert( 0 == sunsSpheresFBO_ );
        assert( 0 == sunsCoronaFBO_ );

        assert( w > 0 && h > 0 );

        // Highest quality uses super sampling.
        if( deferred_renderer::HighestQuality == qualityLevel_ )
        {
            w *= 2;
            h *= 2;
            ss_log( "Using 2 x FSAA super-sampling back buffers." );
        }

        buffersWidth_ = w;
        buffersHeight_ = h;

        glGetError();

        // Create the frame buffers:
        glGenFramebuffers( 1, &drawModelsFBO_ );
        glGenFramebuffers( 1, &drawLightingFBO_ );
        glGenFramebuffers( 1, &diffuseSfxFBO_ );
        glGenFramebuffers( 1, &emissiveSfxFBO_ );
        glGenFramebuffers( 1, &sunsSpheresFBO_ );
        glGenFramebuffers( 1, &sunsCoronaFBO_ );
        glGenFramebuffers( 1, &compositionFBO_ );
        glGenFramebuffers( 1, &posteffectFBO_ );

        assert_opengl_ok( __LINE__ );

        // Create the mesh pass fbo textures:
        sceneDiffGBWarpXY_ = create_bgra_texture( w, h );
        sceneEmissRGBDiffR_ = create_bgra_texture( w, h );

        // These are shared between mesh and lighting pass:
        depthTexture_ = create_depth_texture( w, h );
        meshNormalsSpecANDCompositionPassRenderTargetTexture_ = create_bgra_texture( w, h );

        // Create the lighting pass fbo textures:
        lightingDiffComponentTexture_ = create_lighting_texture( w, h );
        lightingSpecComponentTexture_ = create_lighting_texture( w, h );

        assert_opengl_ok( __LINE__ );

        //
        // Setup mesh pass FBO:
        //
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, drawModelsFBO_ );
        glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0 + OUTPUT_INDEX_ERGBDR,
                                GL_TEXTURE_2D,
                                sceneEmissRGBDiffR_,
                                0 );
        glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0 + OUTPUT_INDEX_DGBWXY,
                                GL_TEXTURE_2D,
                                sceneDiffGBWarpXY_,
                                0 );
        glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0 + OUTPUT_INDEX_NRGBSA,
                                GL_TEXTURE_2D,
                                meshNormalsSpecANDCompositionPassRenderTargetTexture_,
                                0 );
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture_, 0 );

        // Outputs
        const GLenum meshOutputs[3]
            = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers( 3, meshOutputs );

        if( false == opengl_no_errors( __LINE__ ) || false
                == framebuffer_is_complete( GL_DRAW_FRAMEBUFFER ) )
        {
            cleanup_rendertargets();
            ss_throw( "Model pass FBO status is incomplete." );
        }

        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

        //
        // Setup lighting pass FBO:
        //
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, drawLightingFBO_ );
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightingDiffComponentTexture_, 0 );
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightingSpecComponentTexture_, 0 );
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture_, 0 );

        const GLenum lightingOutputs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers( 2, lightingOutputs );

        if( false == opengl_no_errors( __LINE__ ) || false
                == framebuffer_is_complete( GL_DRAW_FRAMEBUFFER ) )
        {
            cleanup_rendertargets();
            ss_throw( "Lighting pass FBO status is incomplete." );
        }

        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
        assert_opengl_ok( __LINE__ );

        //
        // Setup Sun sphere rendering SFX pass FBO: writes to emissive which,
        // will be blurred if blurring is enabled, and also writes to
        // specular as this is simply added to the final image and so
        // can behave as a proxy emissive.
        //
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, sunsSpheresFBO_ );
        glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0 + TEX_INPUT_INDEX_COMPOSITION_SCENE_ERGBDR,
                                GL_TEXTURE_2D,
                                sceneEmissRGBDiffR_,
                                0 );
        glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0 + TEX_INPUT_INDEX_COMPOSITION_SCENE_DGBWXY,
                                GL_TEXTURE_2D,
                                sceneDiffGBWarpXY_,
                                0 );
        glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0 + TEX_INPUT_INDEX_COMPOSITION_SCENE_LITSPEC,
                                GL_TEXTURE_2D,
                                lightingSpecComponentTexture_,
                                0 );
        glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0 + TEX_INPUT_INDEX_COMPOSITION_SCENE_LITDIFF,
                                GL_TEXTURE_2D,
                                lightingDiffComponentTexture_,
                                0 );
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture_, 0 );

        // Outputs
        const GLenum sunsOutputs[4] =
        { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

        glDrawBuffers( 4, sunsOutputs );

        if( false == opengl_no_errors( __LINE__ ) || false
                == framebuffer_is_complete( GL_DRAW_FRAMEBUFFER ) )
        {
            cleanup_rendertargets();
            ss_throw( "Sun spheres rendering pass FBO status is incomplete." );
        }

        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
        assert_opengl_ok( __LINE__ );

        //
        // Setup Sun corona rendering SFX pass FBO: writes to specular as proxy
        // emissive.
        //
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, sunsCoronaFBO_ );
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightingSpecComponentTexture_, 0 );
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture_, 0 );

        // Outputs
        const GLenum sunsCoronaOutputs[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers( 1, sunsCoronaOutputs );

        if( false == opengl_no_errors( __LINE__ ) || false
                == framebuffer_is_complete( GL_DRAW_FRAMEBUFFER ) )
        {
            cleanup_rendertargets();
            ss_throw( "Sun corona rendering pass FBO status is incomplete." );
        }

        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
        assert_opengl_ok( __LINE__ );

        //
        // Setup emissive SFX pass FBO: writes to emissive and to warp
        // before composition has taken place: the emissives written here
        // will be blurred if blurring is enabled.
        //
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, emissiveSfxFBO_ );
        glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0 + OUTPUT_INDEX_DGBWXY,
                                GL_TEXTURE_2D,
                                sceneDiffGBWarpXY_,
                                0 );
        glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0 + OUTPUT_INDEX_ERGBDR,
                                GL_TEXTURE_2D,
                                sceneEmissRGBDiffR_,
                                0 );
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture_, 0 );

        // Outputs
        const GLenum sfxEmissiveOutputs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers( 2, sfxEmissiveOutputs );

        if( false == opengl_no_errors( __LINE__ ) || false
                == framebuffer_is_complete( GL_DRAW_FRAMEBUFFER ) )
        {
            cleanup_rendertargets();
            ss_throw( "SFX emissive pass FBO status is incomplete." );
        }

        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
        assert_opengl_ok( __LINE__ );

        //
        // Setup diffuse SFX pass FBO: writes to diffuse and to warp
        // after composition but before post processing takes places.
        //
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, diffuseSfxFBO_ );
        glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0 + OUTPUT_INDEX_DGBWXY,
                                GL_TEXTURE_2D,
                                sceneDiffGBWarpXY_,
                                0 );
        glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0 + OUTPUT_INDEX_ERGBDR,
                                GL_TEXTURE_2D,
                                meshNormalsSpecANDCompositionPassRenderTargetTexture_,
                                0 );
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture_, 0 );

        // Outputs
        const GLenum sfxDifffuseOutputs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers( 2, sfxDifffuseOutputs );

        if( false == opengl_no_errors( __LINE__ ) || false
                == framebuffer_is_complete( GL_DRAW_FRAMEBUFFER ) )
        {
            cleanup_rendertargets();
            ss_throw( "SFX diffuse pass FBO status is incomplete." );
        }

        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
        assert_opengl_ok( __LINE__ );

        //
        // Setup composition pass FBO:
        //
        // We re-use the meshNormalsSpecANDCompositionPassRenderTargetTexture_
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, compositionFBO_ );
        glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_2D,
                                meshNormalsSpecANDCompositionPassRenderTargetTexture_,
                                0 );
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture_, 0 );

        // Outputs
        const GLenum compositionOutputs[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers( 1, compositionOutputs );

        if( false == opengl_no_errors( __LINE__ ) || false
                == framebuffer_is_complete( GL_DRAW_FRAMEBUFFER ) )
        {
            cleanup_rendertargets();
            ss_throw( "Composition pass FBO status is incomplete." );
        }

        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
        assert_opengl_ok( __LINE__ );

        //
        // Setup post effect pass FBO:
        //
        // We re-use the sceneEmissRGBDiffR_
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, posteffectFBO_ );
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneEmissRGBDiffR_, 0 );

        // Outputs
        const GLenum poseffectOutputs[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers( 1, poseffectOutputs );

        if( false == opengl_no_errors( __LINE__ ) || false
                == framebuffer_is_complete( GL_DRAW_FRAMEBUFFER ) )
        {
            cleanup_rendertargets();
            ss_throw( "Post effect pass FBO status is incomplete." );
        }

        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::cleanup_rendertargets()
    {
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );

        delete_fbo( drawModelsFBO_ );
        delete_fbo( drawLightingFBO_ );
        delete_fbo( sunsCoronaFBO_ );
        delete_fbo( sunsSpheresFBO_ );
        delete_fbo( emissiveSfxFBO_ );
        delete_fbo( diffuseSfxFBO_ );
        delete_fbo( compositionFBO_ );
        delete_fbo( posteffectFBO_ );

        // Lighting pass textures:
        delete_texture( lightingSpecComponentTexture_ );
        delete_texture( lightingDiffComponentTexture_ );

        // Shared textures:
        delete_texture( depthTexture_ );
        delete_texture( meshNormalsSpecANDCompositionPassRenderTargetTexture_ );

        delete_texture( sceneDiffGBWarpXY_ );
        delete_texture( sceneEmissRGBDiffR_ );
    }

    void deferred_renderer::internalrenderer::cleanup_buffers()
    {
        if( uniformDataBuffer_ )
        {
            glBindBuffer( GL_UNIFORM_BUFFER, 0 );
            delete_buffer( uniformDataBuffer_ );
        }

        if( instanceMatrixVBO_ )
        {
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            delete_buffer( instanceMatrixVBO_ );
        }

        if( screen_quadsQuadsVAOs_[0] )
        {
            glBindVertexArray( 0 );
            delete_vertexarray( screen_quadsQuadsVAOs_[0] );
        }

        if( screen_quadsQuadsVAOs_[1] )
        {
            glBindVertexArray( 0 );
            delete_vertexarray( screen_quadsQuadsVAOs_[1] );
        }

        if( screen_quadsQuadsVBOs_[0] )
        {
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            delete_buffer( screen_quadsQuadsVBOs_[0] );
        }

        if( screen_quadsQuadsVBOs_[1] )
        {
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            delete_buffer( screen_quadsQuadsVBOs_[1] );
        }

        if( screen_quadsQuadsIBO_ )
        {
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            delete_buffer( screen_quadsQuadsIBO_ );
        }

        if( staticGeometryVBO_ )
        {
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            delete_buffer( staticGeometryVBO_ );
        }

        if( fullscreenquadVAO_ )
        {
            glBindVertexArray( 0 );
            delete_vertexarray( fullscreenquadVAO_ );
        }

        if( lightSphereVAO_ )
        {
            glBindVertexArray( 0 );
            delete_vertexarray( lightSphereVAO_ );
        }

        if( skyboxVAO_ )
        {
            glBindVertexArray( 0 );
            delete_vertexarray( skyboxVAO_ );
        }

        if( sunSpheresVBO_ )
        {
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            delete_buffer( sunSpheresVBO_ );
        }

        if( sunSpheresVAO_ )
        {
            glBindVertexArray( 0 );
            delete_vertexarray( sunSpheresVAO_ );
        }

        if( sunCoronasVBO_ )
        {
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            delete_buffer( sunCoronasVBO_ );
        }

        if( sunCoronasVAO_ )
        {
            glBindVertexArray( 0 );
            delete_vertexarray( sunCoronasVAO_ );
        }
    }

    void deferred_renderer::internalrenderer::cleanup_shaders()
    {
        glUseProgram( 0 );

        meshProgram_.clear();
        lightingProgram_.clear();
        compositionProgram_.clear();
        posteffectProgram_.clear();
        screen_quadsProgram_.clear();
        sfxProgram_.clear();
    }

    void deferred_renderer::internalrenderer::cleanup_loaded_textures()
    {
        assert_opengl_ok( __LINE__ );

        for( size_t i = 0; i < loaded_textures_.size(); ++i )
        {
            delete_texture( loaded_textures_[i] );
        }

#ifndef NDEBUG
        loaded_texture_sources_.clear();
#endif

        loaded_textures_.clear();
        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::cleanup_dynbuffers()
    {
        activedynbuffers_.clear();
        pendingdynbuffers_.clear();
    }

    void deferred_renderer::internalrenderer::cleanup_skybox()
    {
        unload_skybox();
    }

    //! Updates the inverse view proj matrix too.
    void deferred_renderer::internalrenderer::update_uniform_block()
    {
        assert_opengl_ok( __LINE__ );

        const GLfloat* unaliased viewSrc = matrices_.view_matrix();
        const GLfloat* unaliased projSrc = matrices_.proj_matrix();
        const GLfloat* unaliased viewprojSrc = matrices_.viewproj_matrix();
        const GLfloat* unaliased projInvSrc = matrices_.inv_proj_matrix();

        // Check that all block are used. Unused blocks will have offset 0
        // which will also be the offset of a used block. An unused block may
        // mean its not referred to in any shaders.
        assert( uniformBlockViewProjInfo_.offset != uniformBlockViewInfo_.offset );
        assert( uniformBlockViewProjInfo_.offset != uniformBlockProjInfo_.offset );
        assert( uniformBlockViewProjInfo_.offset != uniformBlockProjInvInfo_.offset );
        assert( uniformBlockViewProjInfo_.offset != uniformBlockViewport_.offset );

        GLbyte* unaliased viewprojDst = uniformBlockClientCopy_.data()
                                        + uniformBlockViewProjInfo_.offset;
        GLbyte* unaliased viewDst = uniformBlockClientCopy_.data() + uniformBlockViewInfo_.offset;
        GLbyte* unaliased projDst = uniformBlockClientCopy_.data() + uniformBlockProjInfo_.offset;
        GLbyte* unaliased projInvDst = uniformBlockClientCopy_.data() + uniformBlockProjInvInfo_.offset;

        for( GLint y = 0; y < 4; ++y )
        {
            for( GLint x = 0; x < 4; ++x )
            {
                ( reinterpret_cast<GLfloat*>(
                      viewprojDst + ( uniformBlockViewProjInfo_.matrixStrides * y ) ) )[x] = viewprojSrc[x];
                ( reinterpret_cast<GLfloat*>( viewDst + ( uniformBlockViewInfo_.matrixStrides * y ) ) )[x]
                    = viewSrc[x];
                ( reinterpret_cast<GLfloat*>( projDst + ( uniformBlockProjInfo_.matrixStrides * y ) ) )[x]
                    = projSrc[x];
                ( reinterpret_cast<GLfloat*>(
                      projInvDst + ( uniformBlockProjInvInfo_.matrixStrides * y ) ) )[x] = projInvSrc[x];
            }

            viewSrc += 4;
            projSrc += 4;
            viewprojSrc += 4;
            projInvSrc += 4;
        }

        GLfloat* unaliased windowSize = reinterpret_cast
                                        <GLfloat*>( uniformBlockClientCopy_.data() + uniformBlockViewport_.offset );
        windowSize[0] = static_cast<GLfloat>( viewportX_ );
        windowSize[1] = static_cast<GLfloat>( viewportY_ );
        windowSize[2] = static_cast<GLfloat>( renderViewportWidth_ );
        windowSize[3] = static_cast<GLfloat>( renderViewportHeight_ );

        assert( viewportX_ == 0.0f );

        glBufferSubData( GL_UNIFORM_BUFFER,
                         0,
                         static_cast<GLsizeiptr>( uniformBlockClientCopy_.size() ),
                         uniformBlockClientCopy_.data() );

        need_to_update_uniform_block_ = false;

        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::assemble_composition_shader()
    {
        validate_opengl_ok( __LINE__ );

        program compositionProgram;
        compositionProgram.compile( composition_shader_vert(), composition_shader_frag(), "Composition" );

        assert_opengl_ok( __LINE__ );

        // Bindings
        {
            glBindAttribLocation( compositionProgram, 0, "InPosition" );
            glBindFragDataLocation( compositionProgram, 0, "FinalColour" );
            assert_opengl_ok( __LINE__ );
        }

        // Link
        {
            if( false == compositionProgram.link() )
            {
                compositionProgram.clear();
                ss_throw( "Unable to link composition shader program." );
            }

            assert_opengl_ok( __LINE__ );
        }

        // Uniforms belonging to program:
        {
            uniformLocationCompositionLightingSpecSampler_
                = glGetUniformLocation( compositionProgram, "texLightingSpec" );
            uniformLocationCompositionLightingDiffSampler_
                = glGetUniformLocation( compositionProgram, "texLightingDiff" );
            uniformLocationCompositionModelDgbWxySampler_
                = glGetUniformLocation( compositionProgram, "texDgbWxy" );
            uniformLocationCompositionModelErgbDrSampler_
                = glGetUniformLocation( compositionProgram, "texErgbDr" );
            uniformLocationCompositionAmbientLight_
                = glGetUniformLocation( compositionProgram, "ambientLight" );

#ifdef SS_DEBUGGING_RENDERER
            uniformLocationCompositionDebugRenderMode_
                = glGetUniformLocation( compositionProgram, "DebugMode" );
#endif

            if( -1 == uniformLocationCompositionLightingSpecSampler_
                    || -1 == uniformLocationCompositionLightingDiffSampler_
                    || -1 == uniformLocationCompositionModelDgbWxySampler_
                    || -1 == uniformLocationCompositionModelErgbDrSampler_
                    || -1 == uniformLocationCompositionAmbientLight_ )
            {
                // assert( ! "Missing uniforms." );

                // uniformLocationCompositionLightingSpecSampler_ = 0;
                // uniformLocationCompositionLightingDiffSampler_ = 0;
                // uniformLocationCompositionModelDgbWxySampler_  = 0;
                // uniformLocationCompositionModelErgbDrSampler_  = 0;
                // uniformLocationCompositionAmbientLight_        = 0;

                // ss_throw( "Composition fragment shader missing required samplers." );
            }
        }

        validate_opengl_ok( __LINE__ );
        compositionProgram.swap( compositionProgram_ );
    }

    void deferred_renderer::internalrenderer::assemble_sfx_shader()
    {
        program sfxProgram;
        sfxProgram.compile( sfx_shader_vert(), sfx_shader_frag(), "SFX Shader" );

        assert_opengl_ok( __LINE__ );

        glBindAttribLocation( sfxProgram, SFX_VRTX_CHNL, "InVertex" );
        glBindAttribLocation( sfxProgram, SFX_UV_CHNL, "InTexCoord" );
        glBindFragDataLocation( sfxProgram, OUTPUT_INDEX_DGBWXY, "oDgbWxy" );
        glBindFragDataLocation( sfxProgram, OUTPUT_INDEX_ERGBDR, "oSceneRGBA" );

        assert_opengl_ok( __LINE__ );

        if( false == sfxProgram.link() )
        {
            sfxProgram.clear();
            ss_throw( "Unable to link composition shader program." );
        }

        assert_opengl_ok( __LINE__ );

        sfxProgram.swap( sfxProgram_ );
    }

    void deferred_renderer::internalrenderer::assemble_posteffect_shader()
    {
        validate_opengl_ok( __LINE__ );

        program posteffectProgram;
        posteffectProgram.compile( posteffect_shader_vert(), posteffect_shader_frag(), "Posteffect" );

        assert_opengl_ok( __LINE__ );

        // Bindings
        {
            glBindAttribLocation( posteffectProgram, 0, "InPosition" );
            glBindFragDataLocation( posteffectProgram, 0, "FinalColour" );
            assert_opengl_ok( __LINE__ );
        }

        // Link
        {
            if( false == posteffectProgram.link() )
            {
                posteffectProgram.clear();
                ss_throw( "Unable to link posteffect shader program." );
            }

            assert_opengl_ok( __LINE__ );
        }

        // Uniforms belonging to program:
        {
            uniformLocationPosteffectSceneSampler_
                = glGetUniformLocation( posteffectProgram, "texScene" );
            uniformLocationPosteffectWarpSampler_
                = glGetUniformLocation( posteffectProgram, "texWarpBA" );

#ifdef SS_DEBUGGING_RENDERER
            uniformLocationPosteffectDebugRenderMode_
                = glGetUniformLocation( posteffectProgram, "DebugMode" );
#endif

            if( -1 == uniformLocationPosteffectSceneSampler_
                    || -1 == uniformLocationPosteffectWarpSampler_ )
            {
                assert( !"Missing uniforms." );

                uniformLocationPosteffectSceneSampler_ = 0;
                uniformLocationPosteffectWarpSampler_ = 0;

                ss_throw( "Posteffect fragment shader missing required samplers." );
            }
        }

        validate_opengl_ok( __LINE__ );
        posteffectProgram.swap( posteffectProgram_ );
    }

    void deferred_renderer::internalrenderer::assemble_dynbuffer_shader()
    {
        validate_opengl_ok( __LINE__ );

        program dynbufferProgram;

        dynbufferProgram.compile( dynbuffer_shader_vert(), dynbuffer_shader_frag(), "DynBuffers" );

        // Bindings
        {
            glBindAttribLocation( dynbufferProgram, DYN_VRTX_CHNL, "InVertex" );
            glBindAttribLocation( dynbufferProgram, DYN_UV_CHNL,   "InTexCoord" );
            glBindAttribLocation( dynbufferProgram, DYN_CLR_CHNL,  "InColor" );

            glBindFragDataLocation( dynbufferProgram, 0, "FinalColour" );

            assert_opengl_ok( __LINE__ );
        }

        // Link
        {
            if( false == dynbufferProgram.link() )
            {
                dynbufferProgram.clear();
                ss_throw( "Unable to link dynbuffer program." );
            }

            assert_opengl_ok( __LINE__ );
        }

        // Uniforms belonging to program:
        {
            uniformLocationDynbufferSampler_ = glGetUniformLocation( dynbufferProgram, "texDiffuse" );
            uniformLocationDynbufferBlendColour_
                = glGetUniformLocation( dynbufferProgram, "blendColour" );
            uniformLocationDynbufferTransform_ = glGetUniformLocation( dynbufferProgram, "transform" );
            assert_opengl_ok( __LINE__ );

#ifdef SS_DEBUGGING_RENDERER
            uniformLocationDynbufferDebugRenderMode_
                = glGetUniformLocation( dynbufferProgram, "DebugMode" );
#endif

            // Note: its not actually an error for OpenGL to return -1, it
            // can in theory be optimized away.
            if( -1 == uniformLocationDynbufferSampler_ || -1 == uniformLocationDynbufferBlendColour_
                    || -1 == uniformLocationDynbufferTransform_ )
            {
                assert( !"Missing uniforms." );

                uniformLocationDynbufferSampler_ = 0;
                uniformLocationDynbufferBlendColour_ = 0;
                uniformLocationDynbufferTransform_ = 0;

                ss_throw( "dynbuffer fragment shader missing required samplers." );
            }
        }

        validate_opengl_ok( __LINE__ );
        dynbufferProgram.swap( dynbufferProgram_ );
    }

    void deferred_renderer::internalrenderer::assemble_screen_quads_shader()
    {
        validate_opengl_ok( __LINE__ );

        program screen_quadsProgram;
        screen_quadsProgram.compile( screen_quads_shader_vert(), screen_quads_shader_frag(), "GUI" );

        // Bindings
        {
            glBindAttribLocation( screen_quadsProgram, 0, "InData" );
            glBindAttribLocation( screen_quadsProgram, 1, "InCornerColour" );

            glBindFragDataLocation( screen_quadsProgram, 0, "FinalColour" );

            assert_opengl_ok( __LINE__ );
        }

        // Link
        {
            if( false == screen_quadsProgram.link() )
            {
                screen_quadsProgram.clear();
                ss_throw( "Unable to link lighting shader program." );
            }

            assert_opengl_ok( __LINE__ );
        }

        // Uniforms belonging to program:
        {
            uniformLocationQuadsSampler_ = glGetUniformLocation( screen_quadsProgram, "screenQuadsPage" );

#ifdef SS_DEBUGGING_RENDERER
            uniformLocationQuadsDebugRenderMode_
                = glGetUniformLocation( screen_quadsProgram, "DebugMode" );
#endif

            // Note: its not actually an error for OpenGL to return -1, it
            // can in theory be optimized away.
            if( -1 == uniformLocationQuadsSampler_ )
            {
                assert( !"Missing uniforms." );
                uniformLocationQuadsSampler_ = 0;
                ss_throw( "GUI fragment shader missing required samplers." );
            }
        }

        validate_opengl_ok( __LINE__ );
        screen_quadsProgram.swap( screen_quadsProgram_ );
    }

    void deferred_renderer::internalrenderer::assemble_suns_coronas_shader()
    {
        validate_opengl_ok( __LINE__ );

        program sunsProgram;
        sunsProgram.compile( suns_coronas_shader_vert(), suns_coronas_shader_frag(), "Sun Coronas" );

        // Bindings
        {
            glBindAttribLocation( sunsProgram, 0, "InXYZ" );
            glBindAttribLocation( sunsProgram, 1, "InCentreXYZ" );
            glBindAttribLocation( sunsProgram, 2, "InUV" );
            glBindFragDataLocation( sunsProgram, 0, "oLightSpecRGBX" );
            assert_opengl_ok( __LINE__ );
        }

        // Link
        {
            if( false == sunsProgram.link() )
            {
                sunsProgram.clear();
                ss_throw( "Unable to link sun coronas shader program." );
            }

            assert_opengl_ok( __LINE__ );
        }

        // Uniforms belonging to program:
        {
            uniformLocationSunsCoronaErgbDrSampler_
                = glGetUniformLocation( sunsProgram, "EmissRGBDiffRTexture" );

            // Note: its not actually an error for OpenGL to return -1, it
            // can in theory be optimized away.
            if( -1 == uniformLocationSunsCoronaErgbDrSampler_ )
            {
                // assert(!"Missing uniforms.");
                uniformLocationSunsCoronaErgbDrSampler_ = 0;
                // ss_throw("Suns fragment shader missing required sampler.");
            }
        }

        // Uniform block location
        {
            GLuint uniformBlockIndex = glGetUniformBlockIndex( sunsProgram, "Globals" );
            assert_opengl_ok( __LINE__ );

            if( uniformBlockIndex == GL_INVALID_INDEX )
            {
                ss_throw( "Unable to get 'Globals' index in suns program." );
            }

            // Tell program to get it's data for the block via binding point 0.
            glUniformBlockBinding( sunsProgram, uniformBlockIndex, GLOBALS_BLOCK_BINDING_POINT );
            assert_opengl_ok( __LINE__ );
        }

        validate_opengl_ok( __LINE__ );
        sunsProgram.swap( sunsCoronasProgram_ );
    }

    void deferred_renderer::internalrenderer::assemble_suns_spheres_shader()
    {
        validate_opengl_ok( __LINE__ );

        program sunsProgram;
        sunsProgram.compile( suns_spheres_shader_vert(), suns_spheres_shader_frag(), "Sun Spheres" );

        // Bindings
        {
            glBindAttribLocation( sunsProgram, 0, "XYZAngle" );
            glBindAttribLocation( sunsProgram, 1, "UVNoiseMoveAB" );
            glBindFragDataLocation(
                sunsProgram, TEX_INPUT_INDEX_COMPOSITION_SCENE_LITSPEC, "oLightSpecRGBX" );
            glBindFragDataLocation(
                sunsProgram, TEX_INPUT_INDEX_COMPOSITION_SCENE_LITDIFF, "oLightDiffRGBX" );
            glBindFragDataLocation(
                sunsProgram, TEX_INPUT_INDEX_COMPOSITION_SCENE_ERGBDR, "oEmissRGBDiffR" );
            glBindFragDataLocation(
                sunsProgram, TEX_INPUT_INDEX_COMPOSITION_SCENE_DGBWXY, "oDiffGBWarpXY" );

            assert_opengl_ok( __LINE__ );
        }

        // Link
        {
            if( false == sunsProgram.link() )
            {
                sunsProgram.clear();
                ss_throw( "Unable to link sun spheres shader program." );
            }

            assert_opengl_ok( __LINE__ );
        }

        // Uniforms belonging to program:
        {
            uniformLocationSunsNoiseSampler_ = glGetUniformLocation( sunsProgram, "NoiseTexture" );
            uniformLocationSunsRampSampler_ = glGetUniformLocation( sunsProgram, "SunRampTexture" );

            // Note: its not actually an error for OpenGL to return -1, it
            // can in theory be optimized away.
            if( -1 == uniformLocationSunsNoiseSampler_ || -1 == uniformLocationSunsRampSampler_ )
            {
                assert( !"Missing uniforms." );
                uniformLocationSunsNoiseSampler_ = 0;
                uniformLocationSunsRampSampler_ = 0;
                ss_throw( "Suns fragment shader missing required sampler." );
            }
        }

        // Uniform block location
        {
            GLuint uniformBlockIndex = glGetUniformBlockIndex( sunsProgram, "Globals" );
            assert_opengl_ok( __LINE__ );

            if( uniformBlockIndex == GL_INVALID_INDEX )
            {
                ss_throw( "Unable to get 'Globals' index in suns program." );
            }

            // Tell program to get it's data for the block via binding point 0.
            glUniformBlockBinding( sunsProgram, uniformBlockIndex, GLOBALS_BLOCK_BINDING_POINT );
            assert_opengl_ok( __LINE__ );
        }

        validate_opengl_ok( __LINE__ );
        sunsProgram.swap( sunsSpheresProgram_ );
    }

    void deferred_renderer::internalrenderer::assemble_skybox_shader()
    {
        validate_opengl_ok( __LINE__ );

        program skyboxProgram;
        skyboxProgram.compile( skybox_shader_vert(), skybox_shader_frag(), "Skybox" );

        // Bindings
        {
            glBindAttribLocation( skyboxProgram, 0, "InPosition" );
            glBindFragDataLocation( skyboxProgram, 0, "FinalColour" );
            assert_opengl_ok( __LINE__ );
        }

        // Link
        {
            if( false == skyboxProgram.link() )
            {
                skyboxProgram.clear();
                ss_throw( "Unable to link cubemap shader program." );
            }

            assert_opengl_ok( __LINE__ );
        }

        // Uniforms belonging to program:
        {
            uniformLocationSkyboxSampler_ = glGetUniformLocation( skyboxProgram, "skyboxTexture" );
            uniformLocationSkyboxMat3x3_ = glGetUniformLocation( skyboxProgram, "SkyboxRotation3x3" );

            // Note: its not actually an error for OpenGL to return -1, it
            // can in theory be optimized away.
            if( -1 == uniformLocationSkyboxSampler_ || -1 == uniformLocationSkyboxMat3x3_ )
            {
                assert( !"Missing uniforms." );
                uniformLocationSkyboxSampler_ = 0;
                uniformLocationSkyboxMat3x3_ = 0;
                ss_throw( "Skybox fragment shader missing required sampler." );
            }
        }

        // Uniform block location
        {
            GLuint uniformBlockIndex = glGetUniformBlockIndex( skyboxProgram, "Globals" );
            assert_opengl_ok( __LINE__ );

            if( uniformBlockIndex == GL_INVALID_INDEX )
            {
                ss_throw( "Unable to get 'Globals' index in shader program." );
            }

            // Tell program to get it's data for the block via binding point 0.
            glUniformBlockBinding( skyboxProgram, uniformBlockIndex, GLOBALS_BLOCK_BINDING_POINT );
            assert_opengl_ok( __LINE__ );
        }

        validate_opengl_ok( __LINE__ );
        skyboxProgram.swap( skyboxProgram_ );
    }

    void deferred_renderer::internalrenderer::assemble_lighting_shader()
    {
        validate_opengl_ok( __LINE__ );

        program lightingProgram;
        lightingProgram.compile( lighting_shader_vert(), lighting_shader_frag(), "Lighting" );

        // Bindings
        {
            glBindAttribLocation( lightingProgram, 0, "InVertex" );

            glBindFragDataLocation( lightingProgram, 0, "fragDiff" );
            glBindFragDataLocation( lightingProgram, 1, "fragSpec" );

            assert_opengl_ok( __LINE__ );
        }

        // Link
        {
            if( false == lightingProgram.link() )
            {
                lightingProgram.clear();
                ss_throw( "Unable to link lighting shader program." );
            }

            assert_opengl_ok( __LINE__ );
        }

        // Uniforms belonging to program:
        {
            uniformLocationLightingNormalsSpecSampler_       = glGetUniformLocation( lightingProgram, "texNormalsSpec" );
            uniformLocationLightingDepthSampler_             = glGetUniformLocation( lightingProgram, "texDepth" );
            uniformLocationLightingLightColourAndIsPoint_    = glGetUniformLocation( lightingProgram, "lightColourAndIsPoint" );
            uniformLocationLightingLightCentreAndRadius_     = glGetUniformLocation( lightingProgram, "lightCentreAndRadius" );
            uniformLocationLightingLightViewCentreAndRadius_ = glGetUniformLocation( lightingProgram, "lightViewCentreAndRadius" );

#ifdef SS_DEBUGGING_RENDERER
            uniformLocationLightingDebugRenderMode_
                = glGetUniformLocation( lightingProgram, "DebugMode" );
#endif

            // Note: its not actually an error for OpenGL to return -1, it
            // can in theory be optimized away.
            if( -1 == uniformLocationLightingNormalsSpecSampler_
                    || -1 == uniformLocationLightingDepthSampler_
                    || -1 == uniformLocationLightingLightColourAndIsPoint_
                    || -1 == uniformLocationLightingLightViewCentreAndRadius_
                    || -1 == uniformLocationLightingLightCentreAndRadius_ )
            {
                assert( !"Missing uniforms." );

                uniformLocationLightingNormalsSpecSampler_ = 0;
                uniformLocationLightingDepthSampler_ = 0;
                uniformLocationLightingLightColourAndIsPoint_ = 0;
                uniformLocationLightingLightViewCentreAndRadius_ = 0;
                uniformLocationLightingLightCentreAndRadius_ = 0;

                ss_throw( "Lighting fragment shader missing required samplers." );
            }

            assert_opengl_ok( __LINE__ );
        }

        // Uniform block location
        {
            GLuint uniformBlockIndex = glGetUniformBlockIndex( lightingProgram, "Globals" );
            assert_opengl_ok( __LINE__ );

            if( uniformBlockIndex == GL_INVALID_INDEX )
            {
                ss_throw( "Unable to get 'Globals' index in shader program." );
            }

            // Tell program to get it's data for the block via binding point 0.
            glUniformBlockBinding( lightingProgram, uniformBlockIndex, GLOBALS_BLOCK_BINDING_POINT );
            assert_opengl_ok( __LINE__ );
        }

        validate_opengl_ok( __LINE__ );
        lightingProgram.swap( lightingProgram_ );
    }

    void deferred_renderer::internalrenderer::assemble_mesh_shader()
    {
        validate_opengl_ok( __LINE__ );

        program meshprogram;

        meshprogram.compile( mesh_shader_vert(), mesh_shader_frag(), "Model" );

        // Bindings
        {
            // Per object: Vertex, UV Texture coords, and tangent/bitangent
            glBindAttribLocation( meshprogram, MSH_VRTX_CHNL, "InVertex" );
            glBindAttribLocation( meshprogram, MSH_UV_CHNL, "InTexCoord" );

            // Per instance: Model View Matrix
            glBindAttribLocation( meshprogram, MSH_MAT0_CHNL, "InTransform0" );
            glBindAttribLocation( meshprogram, MSH_MAT1_CHNL, "InTransform1" );
            glBindAttribLocation( meshprogram, MSH_MAT2_CHNL, "InTransform2" );
            glBindAttribLocation( meshprogram, MSH_MAT3_CHNL, "InTransform3" );

            // Per instance: "team" colour
            glBindAttribLocation( meshprogram, MSH_TEAM_CHNL, "InTeamColour" );

            // Per instance emissive animation channels.
            glBindAttribLocation( meshprogram, MSH_EMISSIVE_0_CHNL, "InEmissive0" );
            glBindAttribLocation( meshprogram, MSH_EMISSIVE_1_CHNL, "InEmissive1" );

            // These correspond to the write buffers of drawModelsFBO_
            glBindFragDataLocation( meshprogram, OUTPUT_INDEX_DGBWXY, "oDgbWxy" );
            glBindFragDataLocation( meshprogram, OUTPUT_INDEX_ERGBDR, "oErgbDr" );
            glBindFragDataLocation( meshprogram, OUTPUT_INDEX_NRGBSA, "oNrNgNbSa" );

            assert_opengl_ok( __LINE__ );
        }

        // Link
        {
            if( false == meshprogram.link() )
            {
                meshprogram.clear();
                ss_throw( "Unable to link mesh shader program." );
            }

            assert_opengl_ok( __LINE__ );
        }

        // Uniforms belonging to program:
        {
            uniformLocationModelDiffuseBGRSpecASampler_  = glGetUniformLocation( meshprogram, "texDiffuseBGRSpecA" );
            uniformLocationModelNormalsBGEmissRASampler_ = glGetUniformLocation( meshprogram, "texNormalsBGEmissRA" );

#ifdef SS_DEBUGGING_RENDERER
            uniformLocationModelDebugRenderMode_ = glGetUniformLocation( meshprogram, "DebugMode" );
#endif

            if( -1 == uniformLocationModelDiffuseBGRSpecASampler_
                    || -1 == uniformLocationModelNormalsBGEmissRASampler_ )
            {
                assert( !"Missing uniforms." );

                uniformLocationModelDiffuseBGRSpecASampler_ = 0;
                uniformLocationModelNormalsBGEmissRASampler_ = 0;

                ss_throw( "Model fragment shader missing required samplers." );
            }

            assert_opengl_ok( __LINE__ );
        }

        // Uniform block location
        {
            GLuint uniformBlockIndex = glGetUniformBlockIndex( meshprogram, "Globals" );
            assert_opengl_ok( __LINE__ );

            if( uniformBlockIndex == GL_INVALID_INDEX )
            {
                ss_throw( "Unable to get 'Globals' index in shader program." );
            }

            // Tell program to get it's data for the block via binding point 0.
            glUniformBlockBinding( meshprogram, uniformBlockIndex, GLOBALS_BLOCK_BINDING_POINT );
            assert_opengl_ok( __LINE__ );
        }

        validate_opengl_ok( __LINE__ );
        meshprogram.swap( meshProgram_ );
    }

    void deferred_renderer::internalrenderer::create_globals_uniform_block()
    {
        validate_opengl_ok( __LINE__ );

        assert( uniformBlockClientCopy_.empty() );
        assert( uniformDataBuffer_ == 0 );
        assert( lightingProgram_ != 0 );
        assert( meshProgram_ != 0 );

        // We share the "Globals" uniform buffer object block between two programs.
        const GLuint uniformBlockIndexLighting = glGetUniformBlockIndex( lightingProgram_, "Globals" );
        if( uniformBlockIndexLighting == GL_INVALID_INDEX )
        {
            ss_throw( "Unable to get 'Globals' index in lighting shader program." );
        }

        assert_opengl_ok( __LINE__ );

        const GLuint uniformBlockIndexMesh = glGetUniformBlockIndex( meshProgram_, "Globals" );
        if( uniformBlockIndexMesh == GL_INVALID_INDEX )
        {
            ss_throw( "Unable to get 'Globals' index in mesh shader program." );
        }

        assert_opengl_ok( __LINE__ );

        // Find the memory size of buffer on OpenGL's side.
        GLint blockSizeLighting = 0;
        GLint blockSizeMesh = 0;
        glGetActiveUniformBlockiv( lightingProgram_,
                                   uniformBlockIndexLighting,
                                   GL_UNIFORM_BLOCK_DATA_SIZE,
                                   &blockSizeLighting );
        glGetActiveUniformBlockiv(
            meshProgram_, uniformBlockIndexMesh, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSizeMesh );

        assert_opengl_ok( __LINE__ );

        if( blockSizeLighting != blockSizeMesh )
        {
            ss_throw( "Lighting and Mesh programs have different sized \"Globals\" "
                      "uniform block." );
        }

        const GLsizeiptr blockSize = blockSizeLighting;
        uniformBlockClientCopy_.resize( static_cast<size_t>( blockSize ), GLbyte( 0 ) );

        // Create UBO and specify its size and usage.
        glGenBuffers( 1, &uniformDataBuffer_ );
        glBindBuffer( GL_UNIFORM_BUFFER, uniformDataBuffer_ );
        glBufferData( GL_UNIFORM_BUFFER, blockSize, NULL, GL_DYNAMIC_DRAW );

        assert_opengl_ok( __LINE__ );

        // Now we attach the buffer to UBO binding point 0...
        glBindBufferBase( GL_UNIFORM_BUFFER, GLOBALS_BLOCK_BINDING_POINT, uniformDataBuffer_ );

        // Find out the data layout details of the block elements:
        const GLchar* uniformNames[NUM_UNIFORM_ELEMENTS]
            = { "View4x4", "Proj4x4", "ViewProj4x4", "ProjInv4x4", "Viewport" };

        UniformBlockInfo* const infos[NUM_UNIFORM_ELEMENTS] =
        {
            &uniformBlockViewInfo_,    &uniformBlockProjInfo_, &uniformBlockViewProjInfo_,
            &uniformBlockProjInvInfo_, &uniformBlockViewport_
        };

        // We update these as being true when we see that a block is used
        // Unused blocks, which in theory should not happen, will not be
        // written to in the update uniform block call.
        bool recordedAsActive[NUM_UNIFORM_ELEMENTS] = { false, false, false, false };

        const GLuint programs[]  =
        {
            lightingProgram_,    meshProgram_,       skyboxProgram_, sunsSpheresProgram_,
            sunsCoronasProgram_, posteffectProgram_, sfxProgram_
        };

        for( size_t programIndex = 0; programIndex < ( sizeof( programs ) / sizeof( GLuint ) );
                ++programIndex )
        {
            const GLuint program = programs[programIndex];

            // Get the indices of the named uniforms.
            GLuint uniformIndices[NUM_UNIFORM_ELEMENTS] =
            {
              GL_INVALID_INDEX, GL_INVALID_INDEX, GL_INVALID_INDEX, GL_INVALID_INDEX
            };

            glGetUniformIndices( program, NUM_UNIFORM_ELEMENTS, uniformNames, uniformIndices );

            assert_opengl_ok( __LINE__ );

            // Query each named uniform for its offset and stride information.
            for( size_t i = 0; i < NUM_UNIFORM_ELEMENTS; ++i )
            {
                if( uniformIndices[i] != GL_INVALID_INDEX )
                {
                    ss_log(
                        uniformNames[i], " uniform in \"Globals\" block is used by program ", program );

                    if( false == recordedAsActive[i] )
                    {
                        glGetActiveUniformsiv( program,
                                              1,
                                              uniformIndices + i,
                                              GL_UNIFORM_OFFSET,
                                              &( infos[i]->offset ) );

                        glGetActiveUniformsiv( program,
                                               1,
                                               uniformIndices + i,
                                               GL_UNIFORM_ARRAY_STRIDE,
                                               &( infos[i]->arrayStrides ) );

                        glGetActiveUniformsiv( program,
                                               1,
                                               uniformIndices + i,
                                               GL_UNIFORM_MATRIX_STRIDE,
                                               &( infos[i]->matrixStrides ) );

                        assert_opengl_ok( __LINE__ );
                        recordedAsActive[i] = true;
                    }
                }
            }
        }

        for( size_t i = 0; i < NUM_UNIFORM_ELEMENTS; ++i )
        {
            if( false == recordedAsActive[i] )
            {
                ss_wrn( uniformNames[i],
                        " uniform in \"Globals\" block does not seem to be active in any shader" );
            }
        }

        validate_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::create_sfx()
    {
        assert_opengl_ok( __LINE__ );

        assert( depthTexture_ );
        sfxrenderer_.reset( new sfxrenderer( sfxProgram_, depthTexture_ ) );

        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::create_noise_textures( std::future<std::vector<std::uint8_t>>&& task,
                                                                     const GLsizei size )
    {
        assert_opengl_ok( __LINE__ );

        assert( noiseTexture_ == 0u );

        GLuint t;
        glGenTextures( 1, &t );

        glBindTexture( GL_TEXTURE_3D, t );

        glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        assert_opengl_ok( __LINE__ );

        glTexImage3D( GL_PROXY_TEXTURE_3D, 0, GL_RGBA, size, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr );

        GLint proxyWidth = 0;
        glGetTexLevelParameteriv( GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &proxyWidth );
        assert_opengl_ok( __LINE__ );

        if( proxyWidth > 0 )
        {
            glTexImage3D( GL_TEXTURE_3D,
                          0,
                          GL_RGBA,
                          size,
                          size,
                          size,
                          0,
                          GL_RGBA,
                          GL_UNSIGNED_BYTE,
                          task.get().data() );

            glBindTexture( GL_TEXTURE_3D, 0 );

            noiseTexture_ = t;
        }
        else
        {
            glBindTexture( GL_TEXTURE_3D, 0 );
            glDeleteTextures( 1, &t );
            ss_throw( "Unable to create 3D noise texture." );
        }

        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::cleanup_noise_textures()
    {
        assert_opengl_ok( __LINE__ );

        glBindTexture( GL_TEXTURE_3D, 0 );
        glDeleteTextures( 1, &noiseTexture_ );
        delete_texture( noiseTexture_ );

        assert_opengl_ok( __LINE__ );
    }

    //! Screen quads are rendered as indexed triangle strips. We pre-create
    //! the index buffer and re-use this.
    void deferred_renderer::internalrenderer::create_screen_quads_quad_buffers()
    {
        validate_opengl_ok( __LINE__ );

        auto quadIndices = get_quad_indices();

        assert( 0 == screen_quadsQuadsVBOs_[0] && 0 == screen_quadsQuadsVBOs_[1] );
        assert( 0 == screen_quadsQuadsVAOs_[0] && 0 == screen_quadsQuadsVAOs_[1] );
        assert( 0 == screen_quadsQuadsIBO_ );

        glBindVertexArray( 0 );

        // Generate all the buffers at once:
        GLuint buffers[3];
        glGenBuffers( 3, buffers );

        GLuint vaos[2];
        glGenVertexArrays( 2, vaos );

        // The 'handles' are not an array types so we need to do the manual copy:
        screen_quadsQuadsVBOs_[0] = buffers[0];
        screen_quadsQuadsVBOs_[1] = buffers[1];
        screen_quadsQuadsIBO_ = buffers[2];
        screen_quadsQuadsVAOs_[0] = vaos[0];
        screen_quadsQuadsVAOs_[1] = vaos[1];

        glBindBuffer( GL_ARRAY_BUFFER, screen_quadsQuadsVBOs_[0] );
        glBufferData( GL_ARRAY_BUFFER,
                      MAX_QUADS_PER_BATCH * sizeof( screen_quad_in_vb_t ),
                      nullptr,
                      GL_STREAM_DRAW );

        glBindBuffer( GL_ARRAY_BUFFER, screen_quadsQuadsVBOs_[1] );
        glBufferData( GL_ARRAY_BUFFER,
                      MAX_QUADS_PER_BATCH * sizeof( screen_quad_in_vb_t ),
                      nullptr,
                      GL_STREAM_DRAW );

        glBindBuffer( GL_ARRAY_BUFFER, 0 );

        assert_opengl_ok( __LINE__ );

        //
        // Index buffer: triangle strip for quads.
        //

        // We need a quad tri-strip per screen_quads quad, plus one restart index
        // between them:
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, screen_quadsQuadsIBO_ );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                      static_cast<GLsizeiptr>( quadIndices.size() * sizeof( GLushort ) ),
                      &quadIndices.at( 0 ),
                      GL_STATIC_DRAW );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

        assert_opengl_ok( __LINE__ );

        //
        // Vertex Array Objects:
        //

        glBindVertexArray( screen_quadsQuadsVAOs_[0] );

        // Bind the IBO for the VAO
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, screen_quadsQuadsIBO_ );

        // Bind the 1st VBO and setup pointers for the VAO
        glBindBuffer( GL_ARRAY_BUFFER, screen_quadsQuadsVBOs_[0] );

        glVertexAttribPointer( 0,
                               4,
                               GL_FLOAT,
                               GL_FALSE,
                               static_cast<GLsizei>( sizeof( screen_quad_corner_t ) ),
                               SS_BUFFER_OFFSET( 0 ) );
        glVertexAttribPointer( 1,
                               4,
                               GL_UNSIGNED_BYTE,
                               GL_TRUE,
                               static_cast<GLsizei>( sizeof( screen_quad_corner_t ) ),
                               SS_BUFFER_OFFSET( 4 * sizeof( GLfloat ) ) );

        glEnableVertexAttribArray( 0 );
        glEnableVertexAttribArray( 1 );

        for( GLuint i = 2; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
        {
            glDisableVertexAttribArray( i );
        }

        assert_opengl_ok( __LINE__ );

        for( GLuint i = 0; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
        {
            glVertexAttribDivisor( i, 0 );
        }

        assert_opengl_ok( __LINE__ );

        // Bind the 2nd VBO and setup pointers for the VAO

        glBindVertexArray( screen_quadsQuadsVAOs_[1] );

        // Bind the IBO for the VAO
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, screen_quadsQuadsIBO_ );

        // Bind the VBO and setup pointers for the VAO
        glBindBuffer( GL_ARRAY_BUFFER, screen_quadsQuadsVBOs_[1] );

        glVertexAttribPointer( 0,
                               4,
                               GL_FLOAT,
                               GL_FALSE,
                               static_cast<GLsizei>( sizeof( screen_quad_corner_t ) ),
                               SS_BUFFER_OFFSET( 0 ) );

        glVertexAttribPointer( 1,
                               4,
                               GL_UNSIGNED_BYTE,
                               GL_TRUE,
                               static_cast<GLsizei>( sizeof( screen_quad_corner_t ) ),
                               SS_BUFFER_OFFSET( 4 * sizeof( GLfloat ) ) );

        glEnableVertexAttribArray( 0 );
        glEnableVertexAttribArray( 1 );

        for( GLuint i = 2; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
        {
            glDisableVertexAttribArray( i );
        }

        assert_opengl_ok( __LINE__ );

        for( GLuint i = 0; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
        {
            glVertexAttribDivisor( i, 0 );
        }

        if( false == opengl_no_errors( __LINE__ ) )
        {
            glBindVertexArray( 0 );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            glDeleteBuffers( 3, buffers );
            screen_quadsQuadsVBOs_[0] = 0;
            screen_quadsQuadsVBOs_[1] = 0;
            screen_quadsQuadsIBO_ = 0;
            glDeleteVertexArrays( 2, vaos );
            screen_quadsQuadsVAOs_[0] = 0;
            screen_quadsQuadsVAOs_[1] = 0;
            ss_throw( "Unable to create screen_quads quads buffers vertex buffer "
                      "object." );
        }

        // Finished with fullscreen VAO:
        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }

    //! Initialises the instance-matrix vertex buffer object - each instance
    // requires 2
    //! matrices plus the team colour element ( ).
    void deferred_renderer::internalrenderer::create_instance_matrix_buffers()
    {
        validate_opengl_ok( __LINE__ );

        assert( 0 == instanceMatrixVBO_ );

        if( false == opengl_no_errors( __LINE__ ) )
        {
            ss_throw( "Error entering create_instance_matrix_buffers." );
        }

        // Each instance needs 2 4x4 matrices.
        GLuint* pVBO = &instanceMatrixVBO_;
        glGenBuffers( 1, pVBO );

        if( false == opengl_no_errors( __LINE__ ) )
        {
            ss_throw( "Unable to generate instance matrix vertex buffer object." );
        }

        glBindBuffer( GL_ARRAY_BUFFER, instanceMatrixVBO_ );

        if( false == opengl_no_errors( __LINE__ ) )
        {
            glDeleteBuffers( 1, &instanceMatrixVBO_ );
            instanceMatrixVBO_ = 0;
            ss_throw( "Unable to bind instance matrix vertex buffer object." );
        }

        // Clients must align data elements consistent with the requirements
        // of the client platform, with an additional base-level requirement
        // that an offset within a buffer to a datum comprising N bytes be a
        // multiple of N.
        static_assert( GLsizeiptr( MAX_INSTANCES_PER_BATCH * sizeof( meshinstancedata ) ) > 0,
                       "Buffer size invalid" );

        glBufferData( GL_ARRAY_BUFFER,
                      MAX_INSTANCES_PER_BATCH * sizeof( meshinstancedata ),
                      nullptr,
                      GL_STREAM_DRAW );

        glBindBuffer( GL_ARRAY_BUFFER, 0 );

        if( false == opengl_no_errors( __LINE__ ) )
        {
            glDeleteBuffers( 1, &instanceMatrixVBO_ );
            instanceMatrixVBO_ = 0;
            ss_throw( "Unable to reserve/create instance matrix vertex buffer object." );
        }
    }

    void deferred_renderer::internalrenderer::create_suns_coronas_vao_and_vbo()
    {
        // VAO for sun points.
        glGenVertexArrays( 1, &sunCoronasVAO_ );
        glBindVertexArray( sunCoronasVAO_ );

        // Bind the VBO and setup pointers for the VAO
        glGenBuffers( 1, &sunCoronasVBO_ );
        glBindBuffer( GL_ARRAY_BUFFER, sunCoronasVBO_ );

        // XYZ
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), SS_BUFFER_OFFSET( 0 ) );

        // Centre XYZ.
        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), SS_BUFFER_OFFSET( 3 * sizeof( GLfloat ) ) );

        // UV
        glVertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), SS_BUFFER_OFFSET( 6 * sizeof( GLfloat ) ) );

        glEnableVertexAttribArray( 0 );
        glEnableVertexAttribArray( 1 );
        glEnableVertexAttribArray( 2 );

        for( GLuint i = 3; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
        {
            glDisableVertexAttribArray( i );
        }

        assert_opengl_ok( __LINE__ );

        for( GLuint i = 0; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
        {
            glVertexAttribDivisor( i, 0 );
        }

        // Finished with VAO:
        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

        validate_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::create_suns_spheres_vao_and_vbo()
    {
        // VAO for sun spheres.
        glGenVertexArrays( 1, &sunSpheresVAO_ );
        glBindVertexArray( sunSpheresVAO_ );

        // Bind the VBO and setup pointers for the VAO
        glGenBuffers( 1, &sunSpheresVBO_ );
        glBindBuffer( GL_ARRAY_BUFFER, sunSpheresVBO_ );

        // XYZ and noise rotation angle
        glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), SS_BUFFER_OFFSET( 0 ) );

        // // U & V, Noise move X & Z
        glVertexAttribPointer(
            1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), SS_BUFFER_OFFSET( 4 * sizeof( GLfloat ) ) );

        glEnableVertexAttribArray( 0 );
        glEnableVertexAttribArray( 1 );

        for( GLuint i = 2; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
        {
            glDisableVertexAttribArray( i );
        }

        assert_opengl_ok( __LINE__ );

        for( GLuint i = 0; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
        {
            glVertexAttribDivisor( i, 0 );
        }

        // Finished with VAO:
        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

        validate_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::create_fullscreen_quad_and_light_sphere_and_skybox()
    {
        validate_opengl_ok( __LINE__ );

        assert( 0 == staticGeometryVBO_ );
        assert( 0 == fullscreenquadVAO_ );
        assert( 0 == lightSphereVAO_ );
        assert( 0 == skyboxVAO_ );

        glGenBuffers( 1, &staticGeometryVBO_ );

        const GLfloat quad_coords[]
            = { -1.0f, -1.0f, 0.5f, +1.0f, -1.0f, 0.5f, -1.0f, +1.0f, 0.5f, +1.0f, +1.0f, 0.5f };

        GLfloat cube_coords[] =
        {
            -1.0, -1.0, +1.0, // 1
            -1.0, +1.0, +1.0, // 0
            +1.0, -1.0, +1.0, // 2
            +1.0, +1.0, +1.0, // 3
            +1.0, -1.0, +1.0, // 2
            -1.0, +1.0, +1.0, // 0
            +1.0, -1.0, +1.0, // 2
            +1.0, +1.0, +1.0, // 3
            +1.0, -1.0, -1.0, // 6
            +1.0, +1.0, -1.0, // 7
            +1.0, -1.0, -1.0, // 6
            +1.0, +1.0, +1.0, // 3
            +1.0, -1.0, -1.0, // 6
            +1.0, +1.0, -1.0, // 7
            -1.0, -1.0, -1.0, // 5
            -1.0, +1.0, -1.0, // 4
            -1.0, -1.0, -1.0, // 5
            +1.0, +1.0, -1.0, // 7
            -1.0, -1.0, -1.0, // 5
            -1.0, +1.0, -1.0, // 4
            -1.0, -1.0, +1.0, // 1
            -1.0, +1.0, +1.0, // 0
            -1.0, -1.0, +1.0, // 1
            -1.0, +1.0, -1.0, // 4
            +1.0, +1.0, +1.0, // 3
            -1.0, +1.0, +1.0, // 0
            +1.0, +1.0, -1.0, // 7
            -1.0, +1.0, -1.0, // 4
            +1.0, +1.0, -1.0, // 7
            -1.0, +1.0, +1.0, // 0
            -1.0, -1.0, +1.0, // 1
            +1.0, -1.0, +1.0, // 2
            +1.0, -1.0, -1.0, // 6
            +1.0, -1.0, -1.0, // 6
            -1.0, -1.0, -1.0, // 5
            -1.0, -1.0, +1.0, // 1
        };

        // This is a 20 sided polygon that fits over the unit sphere:
        const GLfloat sphere_coords[] =
        {
            -0.6616f, +1.0705f, +0.0000f, // Vertex 0 from index 10
            +0.0000f, +0.6616f, +1.0705f, // Vertex 1 from index 4
            +0.6616f, +1.0705f, +0.0000f, // Vertex 2 from index 8
            +0.6616f, +1.0705f, +0.0000f, // Vertex 3 from index 8
            +0.0000f, +0.6616f, -1.0705f, // Vertex 4 from index 5
            -0.6616f, +1.0705f, +0.0000f, // Vertex 5 from index 10
            -0.6616f, +1.0705f, +0.0000f, // Vertex 6 from index 10
            +0.0000f, +0.6616f, -1.0705f, // Vertex 7 from index 5
            -1.0705f, +0.0000f, -0.6616f, // Vertex 8 from index 3
            -1.0705f, +0.0000f, -0.6616f, // Vertex 9 from index 3
            -1.0705f, +0.0000f, +0.6616f, // Vertex 10 from index 2
            -0.6616f, +1.0705f, +0.0000f, // Vertex 11 from index 10
            -0.6616f, +1.0705f, +0.0000f, // Vertex 12 from index 10
            -1.0705f, +0.0000f, +0.6616f, // Vertex 13 from index 2
            +0.0000f, +0.6616f, +1.0705f, // Vertex 14 from index 4
            +0.6616f, -1.0705f, +0.0000f, // Vertex 15 from index 9
            +1.0705f, +0.0000f, +0.6616f, // Vertex 16 from index 0
            +0.0000f, -0.6616f, +1.0705f, // Vertex 17 from index 6
            +0.0000f, -0.6616f, +1.0705f, // Vertex 18 from index 6
            -0.6616f, -1.0705f, +0.0000f, // Vertex 19 from index 11
            +0.6616f, -1.0705f, +0.0000f, // Vertex 20 from index 9
            +0.6616f, -1.0705f, +0.0000f, // Vertex 21 from index 9
            -0.6616f, -1.0705f, +0.0000f, // Vertex 22 from index 11
            +0.0000f, -0.6616f, -1.0705f, // Vertex 23 from index 7
            +0.0000f, -0.6616f, -1.0705f, // Vertex 24 from index 7
            +1.0705f, +0.0000f, -0.6616f, // Vertex 25 from index 1
            +0.6616f, -1.0705f, +0.0000f, // Vertex 26 from index 9
            +0.6616f, -1.0705f, +0.0000f, // Vertex 27 from index 9
            +1.0705f, +0.0000f, -0.6616f, // Vertex 28 from index 1
            +1.0705f, +0.0000f, +0.6616f, // Vertex 29 from index 0
            +1.0705f, +0.0000f, +0.6616f, // Vertex 30 from index 0
            +0.0000f, +0.6616f, +1.0705f, // Vertex 31 from index 4
            +0.0000f, -0.6616f, +1.0705f, // Vertex 32 from index 6
            +1.0705f, +0.0000f, +0.6616f, // Vertex 33 from index 0
            +0.6616f, +1.0705f, +0.0000f, // Vertex 34 from index 8
            +0.0000f, +0.6616f, +1.0705f, // Vertex 35 from index 4
            +0.0000f, +0.6616f, +1.0705f, // Vertex 36 from index 4
            -1.0705f, +0.0000f, +0.6616f, // Vertex 37 from index 2
            +0.0000f, -0.6616f, +1.0705f, // Vertex 38 from index 6
            +0.0000f, -0.6616f, +1.0705f, // Vertex 39 from index 6
            -1.0705f, +0.0000f, +0.6616f, // Vertex 40 from index 2
            -0.6616f, -1.0705f, +0.0000f, // Vertex 41 from index 11
            -0.6616f, -1.0705f, +0.0000f, // Vertex 42 from index 11
            -1.0705f, +0.0000f, +0.6616f, // Vertex 43 from index 2
            -1.0705f, +0.0000f, -0.6616f, // Vertex 44 from index 3
            -1.0705f, +0.0000f, -0.6616f, // Vertex 45 from index 3
            +0.0000f, -0.6616f, -1.0705f, // Vertex 46 from index 7
            -0.6616f, -1.0705f, +0.0000f, // Vertex 47 from index 11
            -1.0705f, +0.0000f, -0.6616f, // Vertex 48 from index 3
            +0.0000f, +0.6616f, -1.0705f, // Vertex 49 from index 5
            +0.0000f, -0.6616f, -1.0705f, // Vertex 50 from index 7
            +0.0000f, -0.6616f, -1.0705f, // Vertex 51 from index 7
            +0.0000f, +0.6616f, -1.0705f, // Vertex 52 from index 5
            +1.0705f, +0.0000f, -0.6616f, // Vertex 53 from index 1
            +1.0705f, +0.0000f, -0.6616f, // Vertex 54 from index 1
            +0.0000f, +0.6616f, -1.0705f, // Vertex 55 from index 5
            +0.6616f, +1.0705f, +0.0000f, // Vertex 56 from index 8
            +0.6616f, +1.0705f, +0.0000f, // Vertex 57 from index 8
            +1.0705f, +0.0000f, +0.6616f, // Vertex 58 from index 0
            +1.0705f, +0.0000f, -0.6616f, // Vertex 59 from index 1
        };

        static_assert( sizeof( sphere_coords ) / ( 3 * sizeof( float ) ) == LIGHT_SPHERE_VCOUNT,
                       "LIGHT_SPHERE_VCOUNT has incorrect number of vertices." );

        static_assert( sizeof( cube_coords ) / ( 3 * sizeof( float ) ) == SKYBOX_VCOUNT,
                       "SKYBOX_VCOUNT has incorrect number of vertices." );

        const GLsizeiptr bufferSize = sizeof( quad_coords ) + sizeof( sphere_coords ) + sizeof( cube_coords );

        glBindBuffer( GL_ARRAY_BUFFER, staticGeometryVBO_ );
        glBufferData( GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW );
        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( quad_coords ), quad_coords );
        glBufferSubData( GL_ARRAY_BUFFER, sizeof( quad_coords ), sizeof( sphere_coords ), sphere_coords );
        glBufferSubData( GL_ARRAY_BUFFER,
                         sizeof( quad_coords ) + sizeof( sphere_coords ),
                         sizeof( cube_coords ),
                         cube_coords );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

        assert_opengl_ok( __LINE__ );

        // VAO for the fullscreen quad:
        {
            glGenVertexArrays( 1, &fullscreenquadVAO_ );
            glBindVertexArray( fullscreenquadVAO_ );

            // Bind the VBO and setup pointers for the VAO
            glBindBuffer( GL_ARRAY_BUFFER, staticGeometryVBO_ );

            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, SS_BUFFER_OFFSET( 0 ) );

            glEnableVertexAttribArray( 0 );

            for( GLuint i = 1; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
            {
                glDisableVertexAttribArray( i );
            }

            assert_opengl_ok( __LINE__ );

            for( GLuint i = 0; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
            {
                glVertexAttribDivisor( i, 0 );
            }

            // Finished with fullscreen VAO:
            glBindVertexArray( 0 );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            assert_opengl_ok( __LINE__ );
        }

        // VAO for the light sphere:
        {
            glGenVertexArrays( 1, &lightSphereVAO_ );
            glBindVertexArray( lightSphereVAO_ );

            // Bind the VBO and setup pointers for the VAO
            glBindBuffer( GL_ARRAY_BUFFER, staticGeometryVBO_ );

            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, SS_BUFFER_OFFSET( sizeof( quad_coords ) ) );

            glEnableVertexAttribArray( 0 );

            for( GLuint i = 1; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
            {
                glDisableVertexAttribArray( i );
            }

            assert_opengl_ok( __LINE__ );

            for( GLuint i = 0; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
            {
                glVertexAttribDivisor( i, 0 );
            }

            // Finished with fullscreen VAO:
            glBindVertexArray( 0 );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

            assert_opengl_ok( __LINE__ );
        }

        // VAO for the cubemap:
        {
            glGenVertexArrays( 1, &skyboxVAO_ );
            glBindVertexArray( skyboxVAO_ );

            // Bind the VBO and setup pointers for the VAO
            glBindBuffer( GL_ARRAY_BUFFER, staticGeometryVBO_ );
            glVertexAttribPointer( 0,
                                   3,
                                   GL_FLOAT,
                                   GL_FALSE,
                                   0,
                                   SS_BUFFER_OFFSET( sizeof( quad_coords ) + sizeof( sphere_coords ) ) );

            glEnableVertexAttribArray( 0 );

            for( GLuint i = 1; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
            {
                glDisableVertexAttribArray( i );
            }

            assert_opengl_ok( __LINE__ );

            for( GLuint i = 0; i < MINIMUM_REQUIRED_VERTEX_ATTRIBS; ++i )
            {
                glVertexAttribDivisor( i, 0 );
            }

            // Finished with fullscreen VAO:
            glBindVertexArray( 0 );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

            validate_opengl_ok( __LINE__ );
        }
    }

    //! Called after setup and prior to beginning everything. Should
    //! not interfere with any existing setup states.
    void deferred_renderer::internalrenderer::setup_states()
    {
        validate_opengl_ok( __LINE__ );

        switch( qualityLevel_ )
        {
            case deferred_renderer::LowQuality:
                glHint( GL_TEXTURE_COMPRESSION_HINT, GL_FASTEST );
                break;

            case deferred_renderer::MediumQuality:
                glHint( GL_TEXTURE_COMPRESSION_HINT, GL_DONT_CARE );
                break;

            case deferred_renderer::HighQuality:
            case deferred_renderer::HighestQuality:
                glHint( GL_TEXTURE_COMPRESSION_HINT, GL_NICEST );
                break;
        }

        assert_opengl_ok( __LINE__ );

        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        glDisable( GL_LINE_SMOOTH );
        glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE );

        glDisable( GL_POLYGON_SMOOTH );
        glHint( GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE );

        // Try to stop flimmering in suns.
        glPolygonOffset( -4.0f, -2.0f );

        glReadBuffer( GL_BACK );
        glDrawBuffer( GL_BACK );

        glEnable( GL_CULL_FACE );
        glEnable( GL_DEPTH_TEST );

        glDisable( GL_STENCIL_TEST );
        glDisable( GL_BLEND );
        glDisable( GL_MULTISAMPLE );

        glDepthFunc( GL_LESS );
        glDepthMask( GL_TRUE );
        assert( opengl_zwrite_is_enabled( true ) );

        glStencilMask( 0xFFFFFFFF );
        glStencilFunc( GL_EQUAL, 0x00000000, 0x00000001 );
        glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

        glFrontFace( GL_CCW );
        glCullFace( GL_BACK );

        // The deferred renderer relies on the clear colours being zero.
        glClearColor( DEFAULT_CLEAR_RED, DEFAULT_CLEAR_GREEN, DEFAULT_CLEAR_BLUE, DEFAULT_CLEAR_ALPHA );

        glClearDepth( 1.0 );
        glClearStencil( 0 );

        assert_opengl_ok( __LINE__ );

        // Tell OpenGL we will be using this as the uniform buffer:
        glBindBuffer( GL_UNIFORM_BUFFER, uniformDataBuffer_ );

        // Always set triangle mode to strips: this is a little redundant
        // but for now its a low cost operation. Later we might assume this
        // to be the permanent state of the deferred_renderer.
        primitiveRestartEnabled_ = true;
        glEnable( GL_PRIMITIVE_RESTART );
        glPrimitiveRestartIndex( static_cast<GLuint>( imeshgeometry::StripRestartMarker ) );

        validate_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::setup_vsynch_preferences()
    {
        // -1 == v-sync with tearing, 0 == no v-sync, 1 == v-sync (default).
        const char* const vsynchNames[] = { "v-sync with tearing", "v-sync disabled", "vynch enabled" };

        // Medium quality settings:
        int interval1stPreference = SS_VSYNCH_WHEN_POSSIBLE;
        int interval2ndPreference = SS_VSYNCH_OFF;

        switch( qualityLevel_ )
        {
            // Low quality = no vysnch.
            case deferred_renderer::LowQuality:
                interval1stPreference = SS_VSYNCH_OFF;
                interval2ndPreference = SS_VSYNCH_WHEN_POSSIBLE;
                break;

            // Medium, v-sync with tear, or no v-sync.
            case deferred_renderer::MediumQuality:
                // interval1stPreference already set to SS_VSYNCH_WHEN_POSSIBLE.
                // interval2ndPreference already set to SS_VSYNCH_OFF.
                break;

            // Highs, tearing still allowed.
            case deferred_renderer::HighQuality:
                // interval1stPreference already set to SS_VSYNCH_WHEN_POSSIBLE.
                interval2ndPreference = SS_VSYNCH_ON;
                break;

            // Prefer v-sync.
            case deferred_renderer::HighestQuality:
                interval1stPreference = SS_VSYNCH_ON;            // Prefer v-sync.
                interval2ndPreference = SS_VSYNCH_WHEN_POSSIBLE; // Sync, or tear.
                break;
        }

        if( set_vsynch( interval1stPreference ) )
        {
            ss_log( "Vsynch set to ", vsynchNames[1 + interval1stPreference] );
        }
        else if( set_vsynch( interval2ndPreference ) )
        {
            ss_log( "Vsynch set to ", vsynchNames[1 + interval2ndPreference] );
        }
        else
        {
            ss_wrn( "Unable to set v-sync preferences." );
        }
    }

    void deferred_renderer::internalrenderer::clear_meshs()
    {
        assert_opengl_ok( __LINE__ );
        meshmap_t tmp;
        swap( tmp, meshes_ );
        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::clear_quads_batch()
    {
        screen_quads_current_batchindex_ = NO_SCREEN_QUADS_THIS_FRAME;
        // We guarantee this to never be empty by inserting one in ctor.
        screen_quads_quads_.back().first = 0;
    }

    void deferred_renderer::internalrenderer::clear_lights_batch()
    {
        point_lights_.clear();
        directional_lights_.clear();
    }

    void deferred_renderer::internalrenderer::clear_states()
    {
        assert_opengl_ok( __LINE__ );

        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
        glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );

        glUseProgram( 0 );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_UNIFORM_BUFFER, 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

        assert_opengl_ok( __LINE__ );

        glBindVertexArray( 0 );

        assert_opengl_ok( __LINE__ );

        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glReadBuffer( GL_BACK );
        glDrawBuffer( GL_BACK );

        assert_opengl_ok( __LINE__ );

        glEnable( GL_CULL_FACE );
        glEnable( GL_DEPTH_TEST );

        assert_opengl_ok( __LINE__ );
        enable_primitive_restart( false );
        assert_opengl_ok( __LINE__ );

        glDisable( GL_STENCIL_TEST );
        glDisable( GL_BLEND );
        glDisable( GL_MULTISAMPLE );

        glDepthFunc( GL_LESS );
        glDepthMask( GL_TRUE );
        assert( opengl_zwrite_is_enabled( true ) );

        assert_opengl_ok( __LINE__ );

        glStencilMask( 0xFFFFFFFF );
        glStencilFunc( GL_EQUAL, 0x00000000, 0x00000001 );
        glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

        glFrontFace( GL_CCW );
        glCullFace( GL_BACK );

        glClearColor( DEFAULT_CLEAR_RED, DEFAULT_CLEAR_GREEN, DEFAULT_CLEAR_BLUE, DEFAULT_CLEAR_ALPHA );
        glClearDepth( 1.0 );
        glClearStencil( 0 );

        assert_opengl_ok( __LINE__ );
    }

    //! Returns true if the requirements of the deferred_renderer are met. Add
    //! any requirements checks here to catch problems as early as possible.
    bool deferred_renderer::internalrenderer::are_opengl_requirements_met() const
    {
        // Check OpenGL state here. We could throw if OpenGL state was
        // not OK but this may be very restrictive behaviour for users.
        assert( glGetError() == GL_NO_ERROR );

        // Check version:
        int major;
        int minor;
        glGetIntegerv( GL_MAJOR_VERSION, &major );
        glGetIntegerv( GL_MINOR_VERSION, &minor );

        if( major < MINIMUM_OPENGL_MAJOR
                || ( major == MINIMUM_OPENGL_MAJOR && minor < MINIMUM_OPENGL_MINOR ) )
        {
            return false;
        }

        // Our list of minimum requirements of the OpenGL context:
        struct requirement
        {
            GLenum property;
            GLint minimum;
        };

        const requirement requirements[] =
        {
            { GL_MAX_DRAW_BUFFERS,              MINIMUM_REQUIRED_DRAW_BUFFERS },
            { GL_MAX_UNIFORM_BUFFER_BINDINGS,   MINIMUM_REQUIRED_UNIFORM_BUFFER_BINDINGS },
            { GL_MAX_COLOR_ATTACHMENTS,         MINIMUM_REQUIRED_COLOR_ATTACHMENTS },
            { GL_MAX_TEXTURE_IMAGE_UNITS,       MINIMUM_REQUIRED_TEXTURE_UNITS },
            { GL_MAX_VERTEX_ATTRIBS,            MINIMUM_REQUIRED_VERTEX_ATTRIBS },
            { GL_MAX_UNIFORM_BLOCK_SIZE,        MINIMUM_REQUIRED_UNIFORM_BLOCK_SIZE }
        };

        for( size_t i = 0; i < sizeof( requirements ) / sizeof( requirement ); ++i )
        {
            GLint answer = -1;
            glGetIntegerv( requirements[i].property, &answer );

            if( glGetError() != GL_NO_ERROR )
            {
                return false;
            }

            if( answer < requirements[i].minimum )
            {
                return false;
            }
        }

        return true;
    }

    //! Internal method that frees the dynbuffer resources. This can be called
    //! when the user frees a dynbuffer that is not in the render queue, or when
    //! the rendering completes and it notices the dynbuffer is no longer needed.
    void deferred_renderer::internalrenderer::really_free_dynbuffer( const internaldynbuffer* const d )
    {
        assert( d->db_queuecount_ == 0 );
        assert( !d->db_wanted_ );
        assert( d->db_unlocked_ );

        auto i = activedynbuffers_.begin();
        while( i != activedynbuffers_.end() )
        {
            if( i->get() == d )
            {
                activedynbuffers_.erase( i );
                return;
            }
        }

        assert( i != activedynbuffers_.end() );
    }

    //! Frees all resources associated with the dynbuffer, and removes any it
    //! from the queue. This will throw if the dynbuffer is locked at the time
    //! of calling.
    void deferred_renderer::internalrenderer::free_dynbuffer( internaldynbuffer* d )
    {
        assert( d && d->db_unlocked_ );

        if( d && d->db_unlocked_ )
        {
            if( d->db_wanted_ )
            {
                d->db_wanted_ = false;
            }

            if( d->db_queuecount_ == 0 )
            {
                really_free_dynbuffer( d );
            }
        }
        else
        {
            ss_throw( "Attempt made to free a locked (or null) dynbuffer." );
        }
    }

    dynbuffer* deferred_renderer::internalrenderer::alloc_dynbuffer( unsigned int nverts, unsigned int nindices )
    {
        assert( nverts > 0 && nindices > 0 );

        unique_ptr<internaldynbuffer> d( new internaldynbuffer( nverts, nindices ) );
        activedynbuffers_.push_back( move( d ) );
        return static_cast<dynbuffer*>( activedynbuffers_.back().get() );
    }

    //! Once a buffer is locked it must be unlocked again before it can be used
    //! or drawn with.
    void deferred_renderer::internalrenderer::lock_dynbuffer( internaldynbuffer* d )
    {
        if( d->db_unlocked_ && d->db_wanted_ )
        {
            d->db_unlocked_ = false;

            if( d->db_inVRAM_ )
            {
                d->db_inVRAM_ = false;
            }
        }
    }

    //! Unlocks the buffer and loads the specified number of vertex elements, the
    //! specified number of texture coordinate elements, and the specified number
    //! of indices to VRAM. This call must be made for the VRAM buffer to contain
    //! any content.
    //!
    //! @param nv The number of vertex elements to be uploaded. The same number
    //! of texture coordinate elements will also be uploaded. If this value is
    //! zero then no changes are made to the current contents of VRAM (even if
    //! the user has changed the contents in the buffer itself). This number
    //! must not exceed the original buffer vertex count.
    //!
    //! @param ni The number of indices to be uploaded. If this value is
    //! zero then no changes are made to the current contents of VRAM (even if
    //! the user has changed the contents in the buffer itself).This number
    //! must not exceed the original buffer index count.
    void deferred_renderer::internalrenderer::unlock_dynbuffer( internaldynbuffer* d,
                                                                const unsigned int nv,
                                                                const unsigned int ni,
                                                                const unsigned int nc )
    {
        assert( d );
        assert( nv <= d->vertex_count() );
        assert( ni <= d->index_count() );
        assert( nc <= d->colours_count() );
        assert( !d->db_inVRAM_ );
        assert( !d->db_unlocked_ );
        assert( d->db_queuecount_ == 0 );

        if( !d->db_unlocked_ && d->db_wanted_ && ( nv || ni || nc ) )
        {
            // This is potentially a stall in the pipeline as we wait for the last
            // batch to complete.

            if( nv )
            {
                // Uploaded new vertex and texture coords to the locations
                // specified in the VAO.
                assert_opengl_ok( __LINE__ );

                glBindBuffer( GL_ARRAY_BUFFER, d->db_vuv_vbo_ );
                {
                    // Buffer remains the same size, as the VAO has offsets into it.
                    const GLsizeiptr vertsSize = static_cast<GLsizeiptr>( d->vertex_count() * sizeof( dynbuffervertex ) );
                    const GLsizeiptr uvsSize   = static_cast<GLsizeiptr>( d->vertex_count() * sizeof( dynbufferuv ) );

                    glBufferData( GL_ARRAY_BUFFER, vertsSize + uvsSize, NULL, GL_STREAM_DRAW );

                    // Upload changed amount of vertices to offset zero:
                    glBufferSubData( GL_ARRAY_BUFFER, 0, nv * sizeof( dynbuffervertex ), d->vertices() );

                    // Upload changed amount of texture coords to offset at start of
                    // tex coords:
                    glBufferSubData( GL_ARRAY_BUFFER, vertsSize, nv * sizeof( dynbufferuv ), d->texcoords() );
                }
                glBindBuffer( GL_ARRAY_BUFFER, 0 );

                assert_opengl_ok( __LINE__ );
            }

            if( nc )
            {
                const GLsizeiptr colorsSize = static_cast<GLsizeiptr>( d->colours_count() * sizeof( dynbufferRGBA ) );

                // Uploaded new colors to the locations
                // specified in the VAO.
                assert_opengl_ok( __LINE__ );

                glBindBuffer( GL_ARRAY_BUFFER, d->db_clr_vbo_ );
                {
                    // Upload new colours, buffer remains constant size.
                    glBufferData( GL_ARRAY_BUFFER, colorsSize, NULL, GL_STREAM_DRAW );
                    glBufferSubData( GL_ARRAY_BUFFER, 0, nc * sizeof( dynbufferRGBA ), d->colours() );
                }
                glBindBuffer( GL_ARRAY_BUFFER, 0 );

                assert_opengl_ok( __LINE__ );
            }

            if( ni )
            {
                assert_opengl_ok( __LINE__ );

                // Upload new indices, buffer remains constant size.
                glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, d->db_ibo_ );
                {
                    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                                  static_cast<GLsizeiptr>( d->index_count() * sizeof( GLushort ) ),
                                  nullptr,
                                  GL_STREAM_DRAW );

                    glBufferSubData( GL_ELEMENT_ARRAY_BUFFER,
                                     0,
                                     static_cast<GLsizeiptr>( ni * sizeof( GLushort ) ),
                                     d->indices() );
                }

                glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

                assert_opengl_ok( __LINE__ );
            }

            d->db_unlocked_ = true;
            d->db_inVRAM_ = true;
            assert_opengl_ok( __LINE__ );
        }
    }

    //! Adds the dynbuffer to the draw queue only if not locked and it has
    //! valid content and has been uploaded to VRAM via the locking/fill/unlocking
    //! process.
    void deferred_renderer::internalrenderer::draw_dynbuffer(
        const deferred_renderer::dynbufferdrawparams& params )
    {
        assert( state_ == SceneOpen );

        if( state_ == SceneOpen )
        {
            assert( static_cast<const internaldynbuffer*>( params.buffer )->db_inVRAM_ );

            if( static_cast<const internaldynbuffer*>( params.buffer )->db_unlocked_
                    && static_cast<const internaldynbuffer*>( params.buffer )->db_wanted_
                    && static_cast<const internaldynbuffer*>( params.buffer )->db_inVRAM_
                    && params.texture != texturehandle_t() && params.num_indices_to_draw > 0 )
            {
                pendingdynbuffers_.push_back( params );
                ++static_cast<internaldynbuffer*>( pendingdynbuffers_.back().buffer )->db_queuecount_;
                scene_triangle_count_ += ( params.num_indices_to_draw / 3 );
            }
        }
    }

    texturehandle_t deferred_renderer::internalrenderer::load_sun_colour_ramp_texture( const iimgBGR_ptr& colour_ramp )
    {
        return load_texture( colour_ramp, false, true );
    }

    GLuint deferred_renderer::internalrenderer::load_cubemap( const cubemap& sb )
    {
        GLuint cubeTex = 0;

        assert_opengl_ok( __LINE__ );

        bool allSquare = true;
        for( size_t i = 0; i < 6; ++i )
        {
            allSquare = allSquare && ( sb[0].get() != nullptr )
                        && ( sb[0]->img_width() == sb[0]->img_height() );
        }

        bool allSameSize = allSquare;
        for( size_t i = 1; i < 6; ++i )
        {
            allSameSize = allSameSize && ( sb[0]->img_width() == sb[i]->img_width() )
                          && ( sb[0]->img_height() == sb[i]->img_height() );
        }

        if( !allSquare )
        {
            ss_err( "Unable to load cubemap - all textures were not square." );
        }

        if( !allSameSize )
        {
            ss_err( "Unable to load cubemap - all textures were not same size." );
        }

        if( allSquare && allSameSize )
        {
            glGenTextures( 1, &cubeTex );

            glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
            assert_opengl_ok( __LINE__ );

            glBindTexture( GL_TEXTURE_CUBE_MAP, cubeTex );
            glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
            glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

            // Only compress in lowest formats as its very noticeable.
            const bool compress = ( qualityLevel_ == deferred_renderer::LowQuality )
                                  || ( qualityLevel_ == deferred_renderer::MediumQuality );
            const GLint internalFormat = compress ? GL_COMPRESSED_RGB : GL_RGB8;

            // Use proxy to test we can create cubemap:
            upload_cubemap( sb, internalFormat, true );

            GLint proxyWidth = 0xFFFF;
            glGetTexLevelParameteriv( GL_PROXY_TEXTURE_CUBE_MAP, 0, GL_TEXTURE_WIDTH, &proxyWidth );
            assert_opengl_ok( __LINE__ );

            if( proxyWidth > 0 )
            {
                // We can upload cubemap.
                upload_cubemap( sb, internalFormat, false );
            }
            else
            {
                ss_err( "Proxy test of cubemap failed." );
                delete_texture( cubeTex );
            }

            glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
            assert_opengl_ok( __LINE__ );
        }

        assert_opengl_ok( __LINE__ );

        return cubeTex;
    }

    void deferred_renderer::internalrenderer::load_skybox( const cubemap& sb )
    {
        GLuint cubeTex = load_cubemap( sb );

        if( cubeTex )
        {
            if( skyboxTexture_ )
            {
                unload_skybox();
            }

            skyboxTexture_ = cubeTex;
        }
        else
        {
            ss_err( "Failed to load skybox." );
        }

        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::unload_skybox()
    {
        if( skyboxTexture_ )
        {
            delete_texture( skyboxTexture_ );
            show_skybox_ = false;
        }
        assert( show_skybox_ == false );
    }

    void deferred_renderer::internalrenderer::enable_skybox( const bool show )
    {
        show_skybox_ = show;
    }

    //! Loads a BGR image in as a texture. Returns a handle to the texture.
    texturehandle_t deferred_renderer::internalrenderer::load_texture(
        const shared_ptr<iimgBGR>& pic,
        bool mipmap,
        bool clamptoedge )
    {
        validate_opengl_ok( __LINE__ );

        if( state_ != SceneClosed )
        {
            return texturehandle_t();
        }

        if( pic.get() == nullptr || 0 == pic->img_width() || 0 == pic->img_height() )
        {
            return texturehandle_t();
        }

        GLuint tex = 0;

        try
        {
            tex = make_rgb_tex( qualityLevel_,
                                pic->img_bgr_pixels(),
                                static_cast<GLsizei>( pic->img_width() ),
                                static_cast<GLsizei>( pic->img_height() ),
                                mipmap );

            set_texture_mipmap_settings( tex, qualityLevel_, mipmap, clamptoedge );
        }
        catch( const SS_EXCEPTION_TYPE& )
        {
            assert( !"Failed to create requested 'loaded' RGB texture." );
        }
        catch( ... )
        {
            throw;
        }

        if( tex != 0 )
        {
            loaded_textures_.push_back( tex );
#ifndef NDEBUG
            loaded_texture_sources_.push_back( "Unknown RGB texture" );
#endif
            // Remember we are returning index+1!
            return static_cast<texturehandle_t>( loaded_textures_.size() );
        }
        else
        {
            return texturehandle_t();
        }
    }

    //! Loads a BGRA image in as a source texture for screen_quads rendering.
    //! Returns a handle to the screen_quads src texture.
    texturehandle_t deferred_renderer::internalrenderer::load_texture(
        const shared_ptr<iimgsrc>& pixels )
    {
        validate_opengl_ok( __LINE__ );

        if( pixels.get() == nullptr || state_ != SceneClosed )
        {
            return texturehandle_t();
        }

        auto pic = pixels->get_image();

        if( pic.get() == nullptr || 0 == pic->img_width() || 0 == pic->img_height() )
        {
            return texturehandle_t();
        }

        GLuint tex = 0;

        try
        {
            tex = make_rgba_tex( qualityLevel_,
                                 pic->img_bgra_pixels(),
                                 static_cast<GLsizei>( pic->img_width() ),
                                 static_cast<GLsizei>( pic->img_height() ),
                                 true );

            set_texture_mipmap_settings( tex, qualityLevel_, true, false );
        }
        catch( const SS_EXCEPTION_TYPE& )
        {
            assert( !"Failed to create requested 'loaded' RGBA texture." );
        }
        catch( ... )
        {
            throw;
        }

        if( tex != 0 )
        {
            loaded_textures_.push_back( tex );
#ifndef NDEBUG
            loaded_texture_sources_.push_back( pixels->get_image_name() );
#endif
            // Remember we are returning index+1!
            return static_cast<texturehandle_t>( loaded_textures_.size() );
        }
        else
        {
            return texturehandle_t();
        }
    }

    meshhandle_t deferred_renderer::internalrenderer::load_mesh( const shared_ptr<imesh>& msh )
    {
        // If this is called during rendering, or with an empty mesh, return the
        // invalid handle:
        if( state_ != SceneClosed || msh.get() == nullptr )
        {
            return meshhandle_t();
        }

        // Check we do not have any buffers bound, as this might cause errors
        // in vertex array states etc (if we have made mistakes!).
        assert( is_bound_array_buffer( 0 ) );

        // We need to already have created the matrix VBO for the instances.
        assert( instanceMatrixVBO_ );

        // Get handle and create mesh before making any internal changes.
        auto newhandle = get_new_meshhandle();

        bool showMipMaps = false;

#ifdef SS_DEBUGGING_RENDERER
        showMipMaps = ( debugRenderMode_ == ShowMipMapLevels );
#endif

        // No, it is a new mesh:
        shared_ptr<internalmesh> newmesh = make_shared<internalmesh>( msh, instanceMatrixVBO_, qualityLevel_, showMipMaps );

        assert( meshes_.find( newhandle ) == meshes_.end() );

        meshes_[newhandle] = newmesh;

        return newhandle;
    }

    internalmesh* deferred_renderer::internalrenderer::get_mesh( meshhandle_t h ) const
    {
#ifndef NDEBUG
        auto m = meshes_.find( h );

        if( m == meshes_.end() )
        {
            ss_throw( "meshhandle_t not found." );
        }

        return m->second.get();
#else
        // Serious error if handle is not present in map.
        return meshes_.find( h )->second.get();
#endif
    }

    //! Defers effect until start of next scene.
    void deferred_renderer::internalrenderer::set_viewport( int x,
                                                            int y,
                                                            unsigned int width,
                                                            unsigned int height )
    {
        pendingViewportX_ = x;
        pendingViewportY_ = y;
        pendingViewportWidth_ = static_cast<GLint>( width );
        pendingViewportHeight_ = static_cast<GLint>( height );
    }

    void deferred_renderer::internalrenderer::set_renderviewport_size()
    {
        assert( buffersWidth_ > 0 && buffersHeight_ > 0 );

        switch( qualityLevel_ )
        {
            case deferred_renderer::LowQuality:
            case deferred_renderer::MediumQuality:
                // No anti-aliasing, we draw what we show.
                renderViewportWidth_ = min( buffersWidth_, viewportWidth_ );
                renderViewportHeight_ = min( buffersHeight_, viewportHeight_ );
                break;

            case deferred_renderer::HighQuality:
            case deferred_renderer::HighestQuality:
                // Anti-alias if possible. In highest quality this will always
                // anti alias. Only anti-alias with a buffer that is exactly twice
                // the dimensions of the viewport.
                if( ( ( 2 * viewportWidth_ ) <= buffersWidth_ ) && ( ( 2 * viewportHeight_ ) <= buffersHeight_ ) )
                {
                    renderViewportWidth_ = 2 * viewportWidth_;
                    renderViewportHeight_ = 2 * viewportHeight_;
                }
                else
                {
                    // No 2x anti-aliasing possible, we draw what we show.
                    renderViewportWidth_ = min( buffersWidth_, viewportWidth_ );
                    renderViewportHeight_ = min( buffersHeight_, viewportHeight_ );
                }
                break;
        }
    }

    void deferred_renderer::internalrenderer::get_view_matrix( float* unaliased m4x4 ) const
    {
        memcpy( m4x4, matrices_.view_matrix(), 16 * sizeof( float ) );
    }

    void deferred_renderer::internalrenderer::get_inv_view_matrix( float* unaliased m4x4 ) const
    {
        memcpy( m4x4, matrices_.inv_view_matrix(), 16 * sizeof( float ) );
    }

    //! This can only be called outside of the begin/end scene calls.
    void deferred_renderer::internalrenderer::set_view_matrix( const float* unaliased view )
    {
        assert( state_ == SceneClosed );

        if( state_ == SceneClosed )
        {
            matrices_.set_view_matrix( view );
            need_to_update_uniform_block_ = true;
        }
    }

    //! This can only be called outside of the begin/end scene calls.
    void deferred_renderer::internalrenderer::set_proj_matrix( const float* unaliased proj, float zNear )
    {
        assert( state_ == SceneClosed );

        if( state_ == SceneClosed )
        {
            matrices_.set_proj_matrix( proj, zNear );
            need_to_update_uniform_block_ = true;
        }
    }

    void deferred_renderer::internalrenderer::set_skybox_3x3_matrix( const float* unaliased r3x3 )
    {
        memcpy( skyboxRotation3x3_, r3x3, sizeof( float ) * 3 * 3 );
    }

#ifdef SS_DEBUGGING_RENDERER

    //! When SS_DEBUGGING_RENDERER is defined, changes the render mode
    //! to one of the available debugging visualisations.
    void deferred_renderer::internalrenderer::set_debug_rendermode( deferred_renderer::DebugRenderMode m )
    {
        if( m < deferred_renderer::SceneRenderMode || m > deferred_renderer::ShowMipMapLevels )
        {
            ss_throw( "Invalid deferred_renderer::DebugRenderMode" );
        }
        debugRenderMode_ = m;
    }

#else

    void deferred_renderer::internalrenderer::set_debug_rendermode( deferred_renderer::DebugRenderMode )
    {
        // NOP
    }

#endif

    //! Uses the composite projection and view matrix to back project
    //! a screen coordinate to a world coordinate.
    void deferred_renderer::internalrenderer::un_project( unsigned int x,
                                                          unsigned int y,
                                                          float Z,
                                                          float* unaliased resultXYZ ) const
    {
        const int windowWidth = pendingViewportWidth_;
        const int windowHeight = pendingViewportHeight_;
        assert( x <= static_cast<unsigned int>( windowWidth ) );
        assert( y <= static_cast<unsigned int>( windowHeight ) );
        assert( Z >= 0.0f && Z <= 1.0f );

        // Remember in OpenGL (0,0) is at the lower left corner.
        // Convert screen x/y/z to world OpenGL x/y/z (range -1 to 1).
        const float inputs[4] =
        {
            2.0f * ( static_cast<float>( x ) / windowWidth ) - 1.0f,
            2.0f * ( ( windowHeight - static_cast<float>( y ) ) / windowHeight ) - 1.0f,
            2.0f * Z - 1.0f,
            1.0f
        };

        assert( inputs[0] >= -1.0f && inputs[0] <= 1.0f );
        assert( inputs[1] >= -1.0f && inputs[1] <= 1.0f );
        assert( inputs[2] >= -1.0f && inputs[2] <= 1.0f );

        float outputs[4];

        transform_4x1( inputs, matrices_.inv_viewproj_matrix(), outputs );

        resultXYZ[0] = outputs[0] / outputs[3];
        resultXYZ[1] = outputs[1] / outputs[3];
        resultXYZ[2] = outputs[2] / outputs[3];
    }

    unsigned int deferred_renderer::internalrenderer::scene_triangle_count() const
    {
        return scene_triangle_count_;
    }

    void deferred_renderer::internalrenderer::start_scene()
    {

        assert_opengl_ok( __LINE__ );

        assert( state_ == SceneClosed );
        assert( is_bound_array_buffer( 0 ) );

        if( state_ == SceneOpen )
        {
            return;
        }

        scene_triangle_count_ = 0u;

        // If the viewport was changed while a scene was being built, the
        // change is scheduled and is applied here at start of next scene.
        if( viewportX_ != pendingViewportX_ || viewportY_ != pendingViewportY_
                || viewportWidth_ != pendingViewportWidth_ || viewportHeight_ != pendingViewportHeight_ )
        {
            viewportX_ = pendingViewportX_;
            viewportY_ = pendingViewportY_;
            viewportWidth_ = pendingViewportWidth_;
            viewportHeight_ = pendingViewportHeight_;

            set_renderviewport_size();

            need_to_update_uniform_block_ = true;
        }

        clear_lights_batch();
        clear_quads_batch();

        state_ = SceneOpen;

        glEnable( GL_DEPTH_TEST );
        glDepthMask( GL_TRUE );
        assert( opengl_zwrite_is_enabled( true ) );

        glDepthFunc( GL_LESS );

        assert( renderViewportWidth_ <= buffersWidth_ );
        assert( renderViewportHeight_ <= buffersHeight_ );

        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, drawModelsFBO_ );
        glViewport( 0, 0, renderViewportWidth_, renderViewportHeight_ );

        assert_opengl_ok( __LINE__ );

        // Enable stencil testing, and write zeroes where we draw meshes:
        glEnable( GL_STENCIL_TEST );
        glClearStencil( 0xFF );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
        glStencilOp( GL_KEEP, GL_KEEP, GL_ZERO );
        glStencilFunc( GL_ALWAYS, 0, 0xFF );

        assert_opengl_ok( __LINE__ );

        glUseProgram( meshProgram_ );
        glUniform1i( uniformLocationModelDiffuseBGRSpecASampler_,  TEX_INPUT_INDEX_MODEL_DIFFSPEC );
        glUniform1i( uniformLocationModelNormalsBGEmissRASampler_, TEX_INPUT_INDEX_MODEL_NRMLEMISS );

#ifdef SS_DEBUGGING_RENDERER
        glUniform1ui( uniformLocationModelDebugRenderMode_,
                      static_cast<GLuint>( debugRenderMode_ ) );
#endif

        if( need_to_update_uniform_block_ )
        {
            update_uniform_block();
        }

        assert( framebuffer_is_complete( GL_DRAW_FRAMEBUFFER ) );
        assert_opengl_ok( __LINE__ );
    }

    //! Renders 'n' instances of the mesh specified, each mesh being rendered
    //! using its own particular instance data. Calling with n = 0 results in
    //! no instances being deferred_renderer.
    void deferred_renderer::internalrenderer::add_mesh_instances(
        meshhandle_t meshType,
        const meshinstancedata* const instancesInfo,
        GLsizei n )
    {
        assert_opengl_ok( __LINE__ );
        assert( state_ == SceneOpen && "Adding meshs before starting scene?" );
        assert( n > 0 );
        assert( meshType != 0 );

        if( meshType > 0 && n > 0 && state_ == SceneOpen )
        {
#ifdef SS_DEBUGGING_RENDERER
            if( debugRenderMode_ == deferred_renderer::WireFrameRenderMode )
            {
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            }
#endif
            const internalmesh* const mesh = get_mesh( meshType );
            const internalmesh::details meshInfo = mesh->setup_texture_states();

            scene_triangle_count_ += mesh->tricount() * n;

            // Check if we are using strips:
            enable_primitive_restart( GL_TRIANGLE_STRIP == meshInfo.layout );

            assert_opengl_ok( __LINE__ );

            // Render in batches of instances. In most cases it will probably be
            // a single batch of instances rendered.
            for( GLsizei i = 0; i < n; i += MAX_INSTANCES_PER_BATCH )
            {
                const GLsizei renderCount = n - i <= MAX_INSTANCES_PER_BATCH ? n - i
                                            : MAX_INSTANCES_PER_BATCH;

                // This is potentially a stall in the pipeline as we wait for the
                // last batch to complete.
                glBindBuffer( GL_ARRAY_BUFFER, instanceMatrixVBO_ );
                {
                    const GLsizeiptr dataSize =
                        static_cast<GLsizeiptr>( renderCount ) *
                        static_cast<GLsizeiptr>( sizeof( meshinstancedata ) );

                    // Calling this with NULL informs the driver that we do not care
                    // about the previous contents:
                    glBufferData(
                        GL_ARRAY_BUFFER,
                        dataSize,
                        NULL,
                        GL_STREAM_DRAW );

                    // Note:
                    // Clients must align data elements consistent with the
                    // requirements of the client platform, with an additional
                    // base-level requirement that an offset within a buffer to
                    // a datum comprising N bytes be a multiple of N.
                    glBufferSubData(
                        GL_ARRAY_BUFFER,
                        0,
                        dataSize,
                        instancesInfo + i );
                }
                glBindBuffer( GL_ARRAY_BUFFER, 0 );

                glBindVertexArray( meshInfo.vertexArrayObject );

                glDrawElementsInstanced(
                    meshInfo.layout,
                    meshInfo.indicesCount,
                    GL_UNSIGNED_SHORT,
                    0,
                    renderCount );

                assert_opengl_ok( __LINE__ );
            }

#ifdef SS_DEBUGGING_RENDERER
            if( debugRenderMode_ == deferred_renderer::WireFrameRenderMode )
            {
                // Undo previous change.
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            }
#endif
        }

        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::set_ambiant_light( float red, float green, float blue )
    {
        assert_opengl_ok( __LINE__ );
        assert( state_ == SceneClosed && "Cannot set ambient between scene begin and end." );
        assert( red >= 0.0f && green >= 0.0f && blue >= 0.0f );
        assert( red <= 1.0f && green <= 1.0f && blue <= 1.0f );

        if( state_ == SceneClosed )
        {
            ambientRGBA_[0] = red;
            ambientRGBA_[1] = green;
            ambientRGBA_[2] = blue;

            ambient_changed_ = true;
        }
    }

    void deferred_renderer::internalrenderer::render_point_light( const float viewCentreAndRadius[4],
                                                                  const float worldCentreAndRadius[4],
                                                                  const float colourAndIsPoint[4] )
    {
        assert_opengl_ok( __LINE__ );
        assert( state_ == SceneOpen );

        // Update lighting info for shader:
        glUniform4fv( uniformLocationLightingLightViewCentreAndRadius_, 1, viewCentreAndRadius );
        glUniform4fv( uniformLocationLightingLightColourAndIsPoint_,    1, colourAndIsPoint );
        glUniform4fv( uniformLocationLightingLightCentreAndRadius_,     1, worldCentreAndRadius );

        assert_opengl_ok( __LINE__ );

        // Draw back faces, and wherever we hit a "mesh" pixel ( less than our light
        // number), and whenever the surface is hidden (z test fails), place the light number
        // in the stencil buffer.

        assert( lightStencilNumber_ > 0 && lightStencilNumber_ < 0xFF );
        assert( glIsEnabled( GL_DEPTH_CLAMP ) );
        assert( glIsEnabled( GL_DEPTH_TEST ) );
        assert( glIsEnabled( GL_STENCIL_TEST ) );
        assert( colourAndIsPoint[3] == 1.0f );

        // Light is outside frustum if Radius > (Z Distance to Light + Near)
        const bool outsideLight = viewCentreAndRadius[2] > ( viewCentreAndRadius[3] + matrices_.zNear() );

        if( outsideLight )
        {
            // Draw back faces:
            glDepthFunc( GL_GEQUAL );
            glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
            glStencilFunc( GL_GREATER, lightStencilNumber_, 0xFF );
            glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
            glFrontFace( GL_CW );
            glDrawArrays( GL_TRIANGLES, 0, LIGHT_SPHERE_VCOUNT );

            assert_opengl_ok( __LINE__ );

            // Draw front faces, only where we find our light number in the stencil.
            glDepthFunc( GL_LEQUAL );
            glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
            glStencilFunc( GL_EQUAL, lightStencilNumber_, 0xFF );
            glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
            glFrontFace( GL_CCW );
            glDrawArrays( GL_TRIANGLES, 0, LIGHT_SPHERE_VCOUNT );
        }
        else
        {
            // Draw back faces:
            glDepthFunc( GL_GEQUAL );
            glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
            glStencilFunc( GL_GREATER, lightStencilNumber_, 0xFF );
            glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
            glFrontFace( GL_CW );
            glDrawArrays( GL_TRIANGLES, 0, LIGHT_SPHERE_VCOUNT );
            assert_opengl_ok( __LINE__ );

            // Reset facing
            glFrontFace( GL_CCW );
        }

        assert_opengl_ok( __LINE__ );

        // Update light stencil number:
        if( lightStencilNumber_ < MAX_LIGHT_STENCIL_NUMBER )
        {
            ++lightStencilNumber_;
        }

        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::draw_directional_light(
        const deferred_renderer::directional_light& light )
    {
        directional_lights_.push_back( light );
    }

    void deferred_renderer::internalrenderer::draw_point_light( const deferred_renderer::point_light& light )
    {
        point_lights_.push_back( light );
    }

    bool deferred_renderer::internalrenderer::is_valid_screen_quads_pagehandle( texturehandle_t g ) const
    {
        return g > 0 && ( g - 1 ) < loaded_textures_.size();
    }

    //! Returns the current quadbatch. The renderer can check to see if this uses
    //! the same quadpage and just add more entries to it.
    pair<unsigned int, vector<screen_quad_in_vb_t>>*
                                                 deferred_renderer::internalrenderer::get_current_quadbatch()
    {
        pair<unsigned int, vector<screen_quad_in_vb_t>>* batch;

        if( NO_SCREEN_QUADS_THIS_FRAME == screen_quads_current_batchindex_ )
        {
            screen_quads_current_batchindex_ = 0;
        }

        if( screen_quads_current_batchindex_ < screen_quads_quads_.size() )
        {
            // We have an existing element to try using:
            auto it = screen_quads_quads_.begin();
            advance( it, screen_quads_current_batchindex_ );
            batch = &*it;
        }
        else
        {
            // We need to expand the batch size. This expansion is permanent over
            // the lifetime of the deferred_renderer. We will re-use this container.
            screen_quads_quads_.push_back( pair<unsigned int, vector<screen_quad_in_vb_t>>() );
            batch = &screen_quads_quads_.back();
            batch->first = texturehandle_t();
            screen_quads_current_batchindex_ = screen_quads_quads_.size() - 1;
        }

        return batch;
    }

    //! Returns a pointer to the next batch of quads to use. The
    // screen_quads_current_batchindex_
    //! is incremented each time this is called.
    pair<unsigned int, vector<screen_quad_in_vb_t>>* deferred_renderer::internalrenderer::get_next_quadbatch()
    {
        assert( state_ == SceneOpen );
        assert( screen_quads_current_batchindex_ != NO_SCREEN_QUADS_THIS_FRAME );

        ++screen_quads_current_batchindex_;

        auto batch = get_current_quadbatch();

        batch->first = texturehandle_t();

        assert( screen_quads_current_batchindex_ <= screen_quads_quads_.size() );

        return batch;
    }

    //! Screen quads are rendered in the order they are specified, which will determine
    //! their layering on the final image. Background quads should be specified before
    //! foreground quads. Similarly, elements of pages will be rendered in batches in
    //! the order they are specified. This may only be called while the scene is open.
    void deferred_renderer::internalrenderer::draw_screen_quads( texturehandle_t htex,
                                                                 const deferred_renderer::screen_quad* unaliased quads,
                                                                 unsigned int n )
    {
        assert( state_ == SceneOpen );
        assert( htex != texturehandle_t() );
        assert( is_valid_screen_quads_pagehandle( htex ) );

        if( state_ == SceneOpen && quads && n > 0 && is_valid_screen_quads_pagehandle( htex ) )
        {
            scene_triangle_count_ += n * 2;

            auto batch = get_current_quadbatch();
            size_t batchstart = 0;

            if( batch->first == htex )
            {
                // Already uses same texture, append quads to end of batch.
                batchstart = batch->second.size();
                batch->second.resize( batchstart + n );
            }
            else
            {
                if( batch->first == texturehandle_t() )
                {
                    // Append quads to end of batch.
                    batch->first = htex;
                    batch->second.resize( n );
                }
                else
                {
                    // A different texture, get a new batch.
                    batch = get_next_quadbatch();
                    assert( batch->first == 0 );
                    batch->first = htex;
                    batch->second.resize( n );
                }
            }

            assert( n + batchstart <= batch->second.size() );
            transform_quads_to_quadbuffer( quads, n, &( batch->second[batchstart] ) );
        }
    }

    void deferred_renderer::internalrenderer::draw_sun( texturehandle_t h,
                                                        const sun_noise& n,
                                                        const sun_location_and_radius& xyzr )
    {
        if( h != texturehandle_t() )
        {
            // The sun handle is just a cast of a texture handle and it refers to
            // the ramp texture used to colour the sun.
            assert( h <= loaded_textures_.size() );

            assert( sun_instance_count_ <= sun_instances_.size() );

            point4d sunxyzr( xyzr.world_location_[0],
                             xyzr.world_location_[1],
                             xyzr.world_location_[2],
                             xyzr.radius_ );

            if( sun_instance_count_ == sun_instances_.size() )
            {
                sun_instances_.emplace_back( new suninstance( sunxyzr, h, n ) );
            }
            else
            {
                sun_instances_[sun_instance_count_]->load_suninstance( sunxyzr, h, n );
            }

            ++sun_instance_count_;
        }
    }

    void deferred_renderer::internalrenderer::draw_sfx(
        bool emissive,
        texturehandle_t t1,
        texturehandle_t t2,
        const colour& c,
        const fxtriangle* fx,
        const unsigned int n )
    {
        scene_triangle_count_ += n;

        GLuint tex1 = loaded_textures_.at( t1 - 1 );
        GLuint tex2 = loaded_textures_.at( t2 - 1 );

        assert( emissive_sfxcount_ <= emissive_sfx_.size() );
        assert( diffuse_sfxcount_ <= diffuse_sfx_.size() );

        if( emissive )
        {
            if( emissive_sfxcount_ == emissive_sfx_.size() )
            {
                emissive_sfx_.emplace_back( new internalsfx( tex1, tex2, c, fx, n ) );
            }
            else
            {
                emissive_sfx_[emissive_sfxcount_]->load( tex1, tex2, c, fx, n );
            }

            ++emissive_sfxcount_;
        }
        else
        {
            if( diffuse_sfxcount_ == diffuse_sfx_.size() )
            {
                diffuse_sfx_.emplace_back( new internalsfx( tex1, tex2, c, fx, n ) );
            }
            else
            {
                diffuse_sfx_[diffuse_sfxcount_]->load( tex1, tex2, c, fx, n );
            }

            ++diffuse_sfxcount_;
        }
    }

    void deferred_renderer::internalrenderer::render_directional_lights()
    {
        assert_opengl_ok( __LINE__ );
        assert( opengl_zwrite_is_enabled( false ) );

        // We only light where have a number other then 0xFF in the stencil buffer
        // 0xFF means nothing has been written there / no mesh.
        glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
        glStencilFunc( GL_NOTEQUAL, 0xFF, 0xFF );

        for( size_t i = 0; i < directional_lights_.size(); ++i )
        {
            // Calculate light view-space direction. Note zero:
            float lightDir[4] =
            {
                -directional_lights_[i].directionX,
                -directional_lights_[i].directionY,
                -directional_lights_[i].directionZ,
                0.0f
            };

            normalise3( lightDir );

            float lightViewDir[4];
            transform_4x1( lightDir, matrices_.view_matrix(), lightViewDir );

            // Set w value to 1 so we can use it directly in shader.
            lightViewDir[3] = 1.0f;
            assert( 0.0f == lightDir[3] );

            normalise3( lightViewDir );

            // Note zero
            const float colourAndIsPoint[4] =
            {
                directional_lights_[i].red,
                directional_lights_[i].green,
                directional_lights_[i].blue,
                0.0f
            };

            assert_opengl_ok( __LINE__ );
            assert( state_ == SceneOpen );

            // Update lighting info for shader:
            glUniform4fv( uniformLocationLightingLightViewCentreAndRadius_, 1, lightViewDir );
            glUniform4fv( uniformLocationLightingLightColourAndIsPoint_, 1, colourAndIsPoint );
            glUniform4fv( uniformLocationLightingLightCentreAndRadius_, 1, lightDir );

            // Draw fullscreen light:
            glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
        }

        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::render_point_lights()
    {
        assert_opengl_ok( __LINE__ );

        // Prepare to draw spherical light volume:
        glBindVertexArray( lightSphereVAO_ );

        for( size_t i = 0; i < point_lights_.size(); ++i )
        {
            // Calculate light view-space centre:
            float lightPos[4]
                = { point_lights_[i].worldX, point_lights_[i].worldY, point_lights_[i].worldZ, 1.0f };

            float lightViewPos[4];

            transform_4x1( lightPos, matrices_.view_matrix(), lightViewPos );

            lightViewPos[3] = lightPos[3] = point_lights_[i].radius;

            const float colourAndIsPoint[4]
                = { point_lights_[i].red, point_lights_[i].green, point_lights_[i].blue, 1.0f };

            render_point_light( lightViewPos, lightPos, colourAndIsPoint );
        }

        // Reset depth func.
        glDepthFunc( GL_LEQUAL );

        assert_opengl_ok( __LINE__ );
    }

    //! Due to ambient light, this stage is still required even if there are
    //! no lights in the scene.
    void deferred_renderer::internalrenderer::move_to_lighting_stage()
    {
        assert_opengl_ok( __LINE__ );
        assert( state_ == SceneOpen );

        lightStencilNumber_ = INITIAL_LIGHT_STENCIL_NUMBER;

        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, drawLightingFBO_ );
        glViewport( 0, 0, renderViewportWidth_, renderViewportHeight_ );

        // Clear lighting planes:
        glClear( GL_COLOR_BUFFER_BIT );

        // Lighting is additive:
        glEnable( GL_BLEND );
        glBlendFunc( GL_ONE, GL_ONE );

        // No changing of the z buffer:
        glDepthMask( GL_FALSE );
        assert( opengl_zwrite_is_enabled( false ) );

        // Wrap light volumes to closed surfaces:
        glEnable( GL_DEPTH_CLAMP );

        // Lighting program & texture inputs:
        glUseProgram( lightingProgram_ );
        glUniform1i( uniformLocationLightingNormalsSpecSampler_, TEX_INPUT_INDEX_LIGHTING_NORMALSSPEC );
        glUniform1i( uniformLocationLightingDepthSampler_, TEX_INPUT_INDEX_LIGHTING_DEPTH );

#ifdef SS_DEBUGGING_RENDERER
        glUniform1ui( uniformLocationLightingDebugRenderMode_,
                      static_cast<GLuint>( debugRenderMode_ ) );
#endif

        // Setup normal/spec power and depth textures as inputs:
        glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_LIGHTING_NORMALSSPEC );
        glBindTexture( GL_TEXTURE_2D, meshNormalsSpecANDCompositionPassRenderTargetTexture_ );

        glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_LIGHTING_DEPTH );
        glBindTexture( GL_TEXTURE_2D, depthTexture_ );

        assert( framebuffer_is_complete( GL_DRAW_FRAMEBUFFER ) );
        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::render_lights()
    {
        assert_opengl_ok( __LINE__ );
        assert( need_to_update_uniform_block_ == false );
        assert( state_ == SceneOpen );
        assert( fullscreenquadVAO_ );

        move_to_lighting_stage();

        render_point_lights();

        // Set this here always as its used also in next phase.
        glBindVertexArray( fullscreenquadVAO_ );

        // These states are shared with composition pass:
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_DEPTH_CLAMP );

        render_directional_lights();

        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::render_sun_godrays()
    {
        assert_opengl_ok( __LINE__ );

        const size_t suncount = sun_instance_count_;

        if( suncount > 0u )
        {
            sun_vbo_data_.clear();

            const float z_offset = 0.0f;

            for( size_t i = 0u; i < suncount; ++i )
            {
                auto xyzr = sun_instances_[i]->position_and_radius();
                solosnake::fxtriangle billboard[2];
                initialise_offset_billboard( billboard,
                                             xyzr[0],
                                             xyzr[1],
                                             xyzr[2],
                                             2.0f * xyzr[3],
                                             2.0f * xyzr[3],
                                             z_offset,
                                             matrices_.inv_view_matrix() );

                for( size_t j = 0u; j < 2u; ++j )
                {
                    for( size_t v = 0u; v < 3u; ++v )
                    {
                        // x y z
                        sun_vbo_data_.push_back( billboard[j].vert[v].x );
                        sun_vbo_data_.push_back( billboard[j].vert[v].y );
                        sun_vbo_data_.push_back( billboard[j].vert[v].z );

                        // Centre x y z
                        sun_vbo_data_.push_back( xyzr[0] );
                        sun_vbo_data_.push_back( xyzr[1] );
                        sun_vbo_data_.push_back( xyzr[2] );

                        // U V
                        sun_vbo_data_.push_back( billboard[j].vert[v].u );
                        sun_vbo_data_.push_back( billboard[j].vert[v].v );
                    }
                }
            }

            glDepthMask( GL_FALSE );  // Disable z writes.
            glDisable( GL_DEPTH_TEST ); // Enable z tests.

            glEnable( GL_BLEND );
            glBlendFunc( GL_ONE, GL_ONE );

            glEnable( GL_POLYGON_OFFSET_FILL );

            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, sunsCoronaFBO_ );
            glViewport( 0, 0, renderViewportWidth_, renderViewportHeight_ );

            assert_opengl_ok( __LINE__ );

            glUseProgram( sunsCoronasProgram_ );
            // glUniform1i(uniformLocationSunsCoronaErgbDrSampler_,
            // TEX_INPUT_INDEX_SUN_CORONA_EMISSIVE);

            assert_opengl_ok( __LINE__ );

            glBindVertexArray( sunCoronasVAO_ );
            glBindBuffer( GL_ARRAY_BUFFER, sunCoronasVBO_ );

            glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_SUN_CORONA_EMISSIVE );
            glBindTexture( GL_TEXTURE_2D, sceneEmissRGBDiffR_ );

            glBufferData( GL_ARRAY_BUFFER, sun_vbo_data_.size() * sizeof( float ), NULL, GL_STREAM_DRAW );
            glBufferSubData(
                GL_ARRAY_BUFFER, 0, sun_vbo_data_.size() * sizeof( float ), sun_vbo_data_.data() );

            assert_opengl_ok( __LINE__ );

            // Each sun billboard is 2 triangles.
            glDrawArrays( GL_TRIANGLES, 0, static_cast<GLsizei>( suncount * 6u ) );

            glBindTexture( GL_TEXTURE_2D, 0 );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glDepthFunc( GL_LESS );
            glDisable( GL_POLYGON_OFFSET_FILL );
            glDisable( GL_BLEND );
            glEnable( GL_DEPTH_TEST ); // Enable z tests.
        }

        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::render_sun_spheres()
    {
        assert_opengl_ok( __LINE__ );

        const size_t suncount = sun_instance_count_;

        if( suncount > 0u )
        {
            // Build sun Vertex buffer data. Draw one set of suns per ramp texture
            sort( sun_instances_.begin(),
                  sun_instances_.begin() + suncount,
                  []( const unique_ptr<suninstance>& lhs, const unique_ptr<suninstance>& rhs )
            { return lhs->sun_texture() < rhs->sun_texture(); } );

            // If flag is GL_FALSE, depth buffer writing is disabled.
            glDepthMask( GL_TRUE );
            glEnable( GL_DEPTH_TEST );

            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, sunsSpheresFBO_ );
            glViewport( 0, 0, renderViewportWidth_, renderViewportHeight_ );

            glUseProgram( sunsSpheresProgram_ );

            glUniform1i( uniformLocationSunsNoiseSampler_, TEX_INPUT_INDEX_SUN_SPHERE_NOISE );
            glUniform1i( uniformLocationSunsRampSampler_, TEX_INPUT_INDEX_SUN_SPHERE_RAMP );

            glBindVertexArray( sunSpheresVAO_ );
            glBindBuffer( GL_ARRAY_BUFFER, sunSpheresVBO_ );

            glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_SUN_SPHERE_NOISE );
            glBindTexture( GL_TEXTURE_3D, noiseTexture_ );

            glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_SUN_SPHERE_RAMP );

            size_t total_suns_rendered_count = 0u;

            while( total_suns_rendered_count < suncount )
            {
                sun_vbo_data_.clear();

                texturehandle_t suntex = sun_instances_[total_suns_rendered_count]->sun_texture();

                for( size_t i = total_suns_rendered_count; i < suncount; ++i )
                {
                    const suninstance* sun = sun_instances_[i].get();

                    if( sun->sun_texture() == suntex )
                    {
                        auto xyzr = sun->position_and_radius();
                        solosnake::fxtriangle billboard[2];
                        initialise_billboard( billboard,
                                              xyzr[0],
                                              xyzr[1],
                                              xyzr[2],
                                              xyzr[3],
                                              xyzr[3],
                                              matrices_.inv_view_matrix() );

                        for( size_t j = 0u; j < 2u; ++j )
                        {
                            for( size_t v = 0u; v < 3u; ++v )
                            {
                                // x y z angle
                                sun_vbo_data_.push_back( billboard[j].vert[v].x );
                                sun_vbo_data_.push_back( billboard[j].vert[v].y );
                                sun_vbo_data_.push_back( billboard[j].vert[v].z );
                                sun_vbo_data_.push_back( sun->angle() );

                                // u v noise-move xy
                                sun_vbo_data_.push_back( billboard[j].vert[v].u );
                                sun_vbo_data_.push_back( billboard[j].vert[v].v );
                                sun_vbo_data_.push_back( sun->noise_move_x() );
                                sun_vbo_data_.push_back( sun->noise_move_y() );
                            }
                        }
                    }
                }

                const GLsizei suns_rendered_this_loop = static_cast<GLsizei>( sun_vbo_data_.size() / ( 6u * FLOATS_PER_SUN_VERTEX ) );
                total_suns_rendered_count += suns_rendered_this_loop;

                // Draw current batch.
                glBindTexture( GL_TEXTURE_2D, loaded_textures_[suntex - 1] );
                glBufferData( GL_ARRAY_BUFFER, sun_vbo_data_.size() * sizeof( float ), NULL, GL_STREAM_DRAW );
                glBufferSubData( GL_ARRAY_BUFFER, 0, sun_vbo_data_.size() * sizeof( float ), sun_vbo_data_.data() );
                glDrawArrays( GL_TRIANGLES, 0, suns_rendered_this_loop * 6 );

                scene_triangle_count_ += suns_rendered_this_loop * 2;
            }

            // Do we need to unbind these?
            glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_SUN_SPHERE_RAMP );
            glBindTexture( GL_TEXTURE_2D, 0 );

            glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_SUN_SPHERE_NOISE );
            glBindTexture( GL_TEXTURE_3D, 0 );

            glBindBuffer( GL_ARRAY_BUFFER, 0 );

            glDepthMask( GL_FALSE );
        }

        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::cleanup_pending_dynbuffers()
    {
        for( size_t i = 0; i < pendingdynbuffers_.size(); ++i )
        {
            const internaldynbuffer* buffer = static_cast<internaldynbuffer*>( pendingdynbuffers_[i].buffer );
            if( 0u == buffer->db_queuecount_ && !buffer->db_wanted_ )
            {
                really_free_dynbuffer( buffer );
            }
        }

        pendingdynbuffers_.clear();
    }

    void deferred_renderer::internalrenderer::render_diffuse_sfx()
    {
        if( diffuse_sfxcount_ > 0u )
        {
            // Sets the diffuse sfx counter to zero. We do not
            // touch the array itself.
            const size_t n = diffuse_sfxcount_;
            diffuse_sfxcount_ = 0u;

            // Only sort the subset we intend to draw.
            sort( diffuse_sfx_.begin(),
                  diffuse_sfx_.begin() + n,
                  [ = ]( const unique_ptr<internalsfx>& lhs, const unique_ptr<internalsfx>& rhs )
            { return *lhs < *rhs; } );

            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, diffuseSfxFBO_ );
            glViewport( 0, 0, renderViewportWidth_, renderViewportHeight_ );
            glUseProgram( sfxProgram_ );

            sfxrenderer_->start_sfx();

            for( size_t i = 0u; i < n; ++i )
            {
                sfxrenderer_->render_sfx( *diffuse_sfx_[i] );
            }

            sfxrenderer_->end_sfx();
        }
    }

    void deferred_renderer::internalrenderer::render_emissive_sfx()
    {
        if( emissive_sfxcount_ > 0u )
        {
            // Sets the emissive sfx counter to zero. We do not
            // touch the array itself.
            const size_t n = emissive_sfxcount_;
            emissive_sfxcount_ = 0u;

            // Only sort the subset we intend to draw.
            sort( emissive_sfx_.begin(),
                  emissive_sfx_.begin() + n,
                  [ = ]( const unique_ptr<internalsfx>& lhs, const unique_ptr<internalsfx>& rhs )
            { return *lhs < *rhs; } );

            glBindFramebuffer( GL_DRAW_FRAMEBUFFER, emissiveSfxFBO_ );
            glViewport( 0, 0, renderViewportWidth_, renderViewportHeight_ );
            glUseProgram( sfxProgram_ );

            sfxrenderer_->start_sfx();

            for( size_t i = 0u; i < n; ++i )
            {
                sfxrenderer_->render_sfx( *emissive_sfx_[i] );
            }

            sfxrenderer_->end_sfx();
        }
    }

    void deferred_renderer::internalrenderer::render_dynbuffers()
    {
        if( !pendingdynbuffers_.empty() )
        {
            // Setup defaults:
            dynbufferdrawparams defaults;

            if( defaults.alphaBlend )
            {
                glEnable( GL_BLEND );
            }
            else
            {
                glDisable( GL_BLEND );
            }

            // The classic and recommended alpha transparency setting:
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

            // Enable/disable writing to Z:
            glDepthMask( defaults.writeZ ? GL_TRUE : GL_FALSE );

            // Enable/disable Z testing:
            if( defaults.readZ )
            {
                glEnable( GL_DEPTH_TEST );
            }
            else
            {
                glDisable( GL_DEPTH_TEST );
            }

            enable_primitive_restart( true );

            glUseProgram( dynbufferProgram_ );
            glUniform1i( uniformLocationDynbufferSampler_, TEX_INPUT_INDEX_DYNBUFFERS );

#ifdef SS_DEBUGGING_RENDERER
            glUniform1ui( uniformLocationDynbufferDebugRenderMode_,
                          static_cast<GLuint>( debugRenderMode_ ) );
#endif

            glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_DYNBUFFERS );

            for( size_t i = 0; i < pendingdynbuffers_.size(); ++i )
            {
                render_dynbuffer( pendingdynbuffers_[i] );
            }

            // Restore defaults.
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glEnable( GL_DEPTH_TEST );
            glDisable( GL_BLEND );
        }
    }

    void deferred_renderer::internalrenderer::render_dynbuffer(
        const deferred_renderer::dynbufferdrawparams& d )
    {
        assert( d.texture > 0 );
        assert( d.texture <= loaded_textures_.size() );
        assert( d.buffer != nullptr );

        matrix4x4_t t4x4;

        if( d.applyView && d.applyProj )
        {
            mul_4x4( matrices_.viewproj_matrix(), d.matrix, t4x4 );
        }
        else if( d.applyView )
        {
            mul_4x4( matrices_.view_matrix(), d.matrix, t4x4 );
        }
        else if( d.applyProj )
        {
            mul_4x4( matrices_.proj_matrix(), d.matrix, t4x4 );
        }
        else
        {
            copy_4x4( d.matrix, t4x4 );
        }

        // We will always come into this with the default settings already
        // enabled:
        const deferred_renderer::dynbufferdrawparams defaults;
        if( defaults.alphaBlend != d.alphaBlend )
        {
            // Different to defaults:
            if( d.alphaBlend )
            {
                glEnable( GL_BLEND );
            }
            else
            {
                glDisable( GL_BLEND );
            }
        }

        // Enable/disable writing to Z:
        if( defaults.writeZ != d.writeZ )
        {
            glDepthMask( d.writeZ ? GL_TRUE : GL_FALSE );
        }

        // Enable/disable Z testing:
        if( defaults.readZ != d.readZ )
        {
            if( d.readZ )
            {
                glEnable( GL_DEPTH_TEST );
            }
            else
            {
                glDisable( GL_DEPTH_TEST );
            }
        }

        glUniformMatrix4fv( uniformLocationDynbufferTransform_, 1, GL_FALSE, t4x4 );
        glUniform4fv( uniformLocationDynbufferBlendColour_, 1, d.rgba );
        glBindTexture( GL_TEXTURE_2D, loaded_textures_.at( d.texture - 1 ) );

        internaldynbuffer* idyn = static_cast<internaldynbuffer*>( d.buffer );
        assert( idyn->db_inVRAM_ && idyn->db_unlocked_ && idyn->db_queuecount_ > 0 );

        // Decrement the queue count. This is checked later and all with zero
        // are deleted.
        --idyn->db_queuecount_;

        glBindVertexArray( idyn->db_vao_ );

        glDrawElements( d.strips ? GL_TRIANGLE_STRIP : GL_TRIANGLES,
                        static_cast<GLsizei>( d.num_indices_to_draw ),
                        GL_UNSIGNED_SHORT,
                        0 );

        // Restore defaults that were changed:
        if( defaults.alphaBlend != d.alphaBlend )
        {
            if( defaults.alphaBlend )
            {
                glEnable( GL_BLEND );
            }
            else
            {
                glDisable( GL_BLEND );
            }
        }

        // Enable/disable writing to Z:
        if( defaults.writeZ != d.writeZ )
        {
            glDepthMask( defaults.writeZ ? GL_TRUE : GL_FALSE );
        }

        // Enable/disable Z testing:
        if( defaults.readZ != d.readZ )
        {
            if( defaults.readZ )
            {
                glEnable( GL_DEPTH_TEST );
            }
            else
            {
                glDisable( GL_DEPTH_TEST );
            }
        }

        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::render_screen_quads()
    {
        if( screen_quads_current_batchindex_ == NO_SCREEN_QUADS_THIS_FRAME
                || screen_quads_quads_.empty() )
        {
            return;
        }

        assert_opengl_ok( __LINE__ );

        // Setup OpenGL for screen_quads.
        glDisable( GL_DEPTH_TEST );
        glEnable( GL_BLEND );

        // We switch this on but do not switch off. The mesh rendering will switch
        // this
        // off it it needs it off.
        enable_primitive_restart( true );

        // The classic and recommended alpha transparency setting:
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        // Lighting program & texture inputs:
        glUseProgram( screen_quadsProgram_ );
        glUniform1i( uniformLocationQuadsSampler_, TEX_INPUT_INDEX_GUI );

#ifdef SS_DEBUGGING_RENDERER
        glUniform1ui( uniformLocationQuadsDebugRenderMode_, static_cast<GLuint>( debugRenderMode_ ) );
#endif

        // Setup indicated screen_quads texture as inputs:
        glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_GUI );

        auto q = screen_quads_quads_.begin();
        for( size_t i = 0; i <= screen_quads_current_batchindex_; ++i )
        {
            assert( q != screen_quads_quads_.end() );
            render_screen_quads( *q );
            // Reset page type.
            q->first = texturehandle_t();
            ++q;
        };

        glDisable( GL_BLEND );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

        assert_opengl_ok( __LINE__ );
    }

    void deferred_renderer::internalrenderer::render_screen_quads(
        const pair<unsigned int, vector<screen_quad_in_vb_t>>& quads )
    {
        assert_opengl_ok( __LINE__ );
        assert( quads.second.size() > 0 );
        assert( quads.first > 0 );
        assert( quads.first <= loaded_textures_.size() );

        const size_t totalQuadsToDraw = quads.second.size();

        size_t numQuadsDrawn = 0;

        // -1 to change handle to index!
        glBindTexture( GL_TEXTURE_2D, loaded_textures_.at( quads.first - 1 ) );

        // Slice the incoming quads into renderable batches. This is based on
        // the MAX_QUADS_PER_BATCH which is linked to the index buffer used.
        do
        {
            const size_t numToDrawThisBatch = ( totalQuadsToDraw - numQuadsDrawn ) > MAX_QUADS_PER_BATCH
                                              ? MAX_QUADS_PER_BATCH
                                              : totalQuadsToDraw - numQuadsDrawn;

            // Use a different buffer to last one.
            currentQuadVAOIndex_ = currentQuadVAOIndex_ ? 0 : 1;

            glBindBuffer( GL_ARRAY_BUFFER, screen_quadsQuadsVBOs_[currentQuadVAOIndex_] );
            {
                // Calling this with NULL informs the driver that we do not care
                // about the previous contents:
                glBufferData( GL_ARRAY_BUFFER,
                              static_cast<GLsizeiptr>( numToDrawThisBatch * sizeof( screen_quad_in_vb_t ) ),
                              NULL,
                              GL_STREAM_DRAW );

                glBufferSubData( GL_ARRAY_BUFFER,
                                 0,
                                 static_cast
                                 <GLsizeiptr>( numToDrawThisBatch * sizeof( screen_quad_in_vb_t ) ),
                                 &quads.second[numQuadsDrawn] );
            }
            glBindBuffer( GL_ARRAY_BUFFER, 0 );

            glBindVertexArray( screen_quadsQuadsVAOs_[currentQuadVAOIndex_] );

            glDrawElements(
                GL_TRIANGLE_STRIP, static_cast<GLsizei>( 5 * numToDrawThisBatch ), GL_UNSIGNED_SHORT, 0 );

            assert_opengl_ok( __LINE__ );

            numQuadsDrawn += numToDrawThisBatch;
        }
        while( numQuadsDrawn < totalQuadsToDraw );
    }

    void deferred_renderer::internalrenderer::end_scene()
    {
        assert_opengl_ok( __LINE__ );
        assert( fullscreenquadVAO_ );
        assert( state_ == SceneOpen );

        if( state_ == SceneClosed )
        {
            return;
        }

        render_lights();

        glDisable( GL_STENCIL_TEST );
        // Lighting used additive blending.
        glDisable( GL_BLEND );

        assert_opengl_ok( __LINE__ );

        // Enable depth testing for the following block:
        glEnable( GL_DEPTH_TEST );

        //
        // Render suns spheres to composition textures
        //
        render_sun_spheres();
        render_sun_godrays();

        //
        // Draw emissive SFX here so that their output is added to emissive
        // before the blurs take place.
        //
        render_emissive_sfx();

        //
        // Assemble into final scene.
        //
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, compositionFBO_ );
        glViewport( 0, 0, renderViewportWidth_, renderViewportHeight_ );
        assert_opengl_ok( __LINE__ );

        // Clear composition frame buffer.
        glClear( GL_COLOR_BUFFER_BIT );
        assert_opengl_ok( __LINE__ );

        //
        // Draw cubemap cube.
        //
        if( show_skybox_ && skyboxTexture_ )
        {
            assert_opengl_ok( __LINE__ );

            glDepthFunc( GL_LEQUAL );
            assert( opengl_zwrite_is_enabled( false ) );
            assert( opengl_ztest_is_enabled( true ) );

            glUseProgram( skyboxProgram_ );
            glUniform1i( uniformLocationSkyboxSampler_, TEX_INPUT_INDEX_SKYBOX );
            glUniformMatrix3fv( uniformLocationSkyboxMat3x3_, 1, false, skyboxRotation3x3_ );

            glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_SKYBOX );
            glBindTexture( GL_TEXTURE_CUBE_MAP, skyboxTexture_ );

            glBindVertexArray( skyboxVAO_ );
            glDrawArrays( GL_TRIANGLES, 0, SKYBOX_VCOUNT );

            glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
            glDepthFunc( GL_LESS );

            assert_opengl_ok( __LINE__ );
        }

        //
        // Draw user buffers here.
        //
        render_dynbuffers();

        // End block with depth testing enabled.
        glDisable( GL_DEPTH_TEST );

        glUseProgram( compositionProgram_ );
        assert_opengl_ok( __LINE__ );

        // We use these 4 textures to compose the final image.
        glUniform1i( uniformLocationCompositionLightingDiffSampler_,
                     TEX_INPUT_INDEX_COMPOSITION_SCENE_LITDIFF );
        glUniform1i( uniformLocationCompositionLightingSpecSampler_,
                     TEX_INPUT_INDEX_COMPOSITION_SCENE_LITSPEC );
        glUniform1i( uniformLocationCompositionModelErgbDrSampler_,
                     TEX_INPUT_INDEX_COMPOSITION_SCENE_ERGBDR );
        glUniform1i( uniformLocationCompositionModelDgbWxySampler_,
                     TEX_INPUT_INDEX_COMPOSITION_SCENE_DGBWXY );

#ifdef SS_DEBUGGING_RENDERER
        glUniform1ui( uniformLocationCompositionDebugRenderMode_,
                      static_cast<GLuint>( debugRenderMode_ ) );
#endif

        if( ambient_changed_ )
        {
            glUniform4fv( uniformLocationCompositionAmbientLight_, 1, ambientRGBA_ );
            ambient_changed_ = false;
        }

        assert_opengl_ok( __LINE__ );

        // Setup Diffuse and Specular light component textures:
        glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_COMPOSITION_SCENE_LITDIFF );
        glBindTexture( GL_TEXTURE_2D, lightingDiffComponentTexture_ );

        glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_COMPOSITION_SCENE_LITSPEC );
        glBindTexture( GL_TEXTURE_2D, lightingSpecComponentTexture_ );

        // Setup mesh Diffuse and mesh Emissive textures:
        glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_COMPOSITION_SCENE_DGBWXY );
        glBindTexture( GL_TEXTURE_2D, sceneDiffGBWarpXY_ );

        glActiveTexture( GL_TEXTURE0 + TEX_INPUT_INDEX_COMPOSITION_SCENE_ERGBDR );
        glBindTexture( GL_TEXTURE_2D, sceneEmissRGBDiffR_ );

        assert_opengl_ok( __LINE__ );
        assert( opengl_zwrite_is_enabled( false ) );
        assert( opengl_ztest_is_enabled( false ) );

        // Draw fullscreen quad: VAO has been set in lighting phase already for us.

        // This blend just adds the composition output colour onto the
        // back-buffer (containing cubemap etc).
        glEnable( GL_BLEND );
        glBlendFunc( GL_ONE, GL_ONE );
        glBindVertexArray( fullscreenquadVAO_ );
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
        glDisable( GL_BLEND );

        assert_opengl_ok( __LINE__ );

        //
        // Draw diffuse fxtriangles. These are drawn after and into the
        // composed scene. They also write to the warp xy which is used
        // in next pass.
        //
        render_diffuse_sfx();

        //
        // Apply post effects to the scene.
        //
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, posteffectFBO_ );
        glViewport( 0, 0, renderViewportWidth_, renderViewportHeight_ );
        assert_opengl_ok( __LINE__ );
        glUseProgram( posteffectProgram_ );

#ifdef SS_DEBUGGING_RENDERER
        glUniform1ui( uniformLocationPosteffectDebugRenderMode_,
                      static_cast<GLuint>( debugRenderMode_ ) );
#endif

        glUniform1i( uniformLocationPosteffectSceneSampler_, TEX_INPUT_POSTEFFECT_SCENE_SRC );
        glUniform1i( uniformLocationPosteffectWarpSampler_, TEX_INPUT_POSTEFFECT_WARP_SRC );
        glActiveTexture( GL_TEXTURE0 + TEX_INPUT_POSTEFFECT_SCENE_SRC );
        glBindTexture( GL_TEXTURE_2D, meshNormalsSpecANDCompositionPassRenderTargetTexture_ );
        glActiveTexture( GL_TEXTURE0 + TEX_INPUT_POSTEFFECT_WARP_SRC );
        glBindTexture( GL_TEXTURE_2D, sceneDiffGBWarpXY_ );
        glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneEmissRGBDiffR_, 0 );
        glBindVertexArray( fullscreenquadVAO_ );
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

        assert_opengl_ok( __LINE__ );

        // Finally we draw the screen_quads on top of the scene.
        render_screen_quads();

        assert_opengl_ok( __LINE__ );

        glBindVertexArray( 0 );

        assert_opengl_ok( __LINE__ );

        // Finish scene : blit to front buffer:

#ifdef SS_DEBUGGING_RENDERER
        switch( debugRenderMode_ )
        {
            // Default FBO mode:
            case deferred_renderer::SceneRenderMode:
            case deferred_renderer::NoSpecularRenderMode:
            case deferred_renderer::NoDiffuseRenderMode:
            case deferred_renderer::ShowMipMapLevels:
            case deferred_renderer::LitVolumes:
                glBindFramebuffer( GL_READ_FRAMEBUFFER, posteffectFBO_ );
                break;

            // Debugging FBO modes:
            case deferred_renderer::DiffuseTexRenderMode:
            case deferred_renderer::NormalTexRenderMode:
            case deferred_renderer::EmissiveTex0RenderMode:
            case deferred_renderer::EmissiveTex1RenderMode:
            case deferred_renderer::EmissiveTexRenderMode:
            case deferred_renderer::SpecularChannelRenderMode:
            case deferred_renderer::LightingNormalsRenderMode:
            case deferred_renderer::WireFrameRenderMode:
                //  case deferred_renderer::LitVolumes:
                glBindFramebuffer( GL_READ_FRAMEBUFFER, drawModelsFBO_ );
                break;

            default:
                assert( !"Unhandled debugRenderMode_" );
                break;
        }
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
#else
        glBindFramebuffer( GL_READ_FRAMEBUFFER, posteffectFBO_ );
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
#endif

        assert_opengl_ok( __LINE__ );

#ifndef NDEBUG
        if( glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        {
            ss_err( "Incomplete Read Framebuffer" );
        }

        if( glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        {
            ss_err( "Incomplete Draw Framebuffer" );
        }
        assert_opengl_ok( __LINE__ );
#endif

        glBlitFramebuffer( 0,
                           0,
                           renderViewportWidth_,
                           renderViewportHeight_,
                           viewportX_,
                           viewportY_,
                           ( viewportX_ + viewportWidth_ ),
                           ( viewportY_ + viewportHeight_ ),
                           GL_COLOR_BUFFER_BIT,
                           GL_LINEAR );

        assert_opengl_ok( __LINE__ );

        // Do this now to give buffers that are in use a time to flush.
        cleanup_pending_dynbuffers();

        sun_instance_count_ = 0u;
        state_ = SceneClosed;

        // DAIRE ss_log( "Triangles in scene: ~", scene_triangle_count_ );
    }

    //-------------------------------------------------------------------------
    //      Public facing deferred_renderer calls into implementation class:
    //-------------------------------------------------------------------------

    dynbuffer* deferred_renderer::alloc_dynbuffer( const unsigned int nverts, const unsigned int nindices )
    {
        return r_->alloc_dynbuffer( nverts, nindices );
    }

    void deferred_renderer::free_dynbuffer( dynbuffer* d )
    {
        if( d )
        {
            r_->free_dynbuffer( static_cast<internaldynbuffer*>( d ) );
        }
    }

    void deferred_renderer::lock_dynbuffer( dynbuffer* d )
    {
        r_->lock_dynbuffer( static_cast<internaldynbuffer*>( d ) );
    }

    //! Unlocks the buffer and loads the specified number of vertex elements, the
    //! specified number of texture coordinate elements, and the specified number
    //! of indices to VRAM. This call must be made for the VRAM buffer to contain
    //! any content.
    //! @param nv The number of vertex elements to be uploaded. The same number
    //! of texture coordinate elements will also be uploaded. If this value is
    //! zero then no changes are made to the current contents of VRAM (even if
    //! the user has changed the contents in the buffer itself). This number
    //! must not exceed the original buffer vertex count.
    //! @param ni The number of indices to be uploaded. If this value is
    //! zero then no changes are made to the current contents of VRAM (even if
    //! the user has changed the contents in the buffer itself).This number
    //! must not exceed the original buffer index count.
    void deferred_renderer::unlock_dynbuffer( dynbuffer* d, const unsigned int newvcount, const unsigned int newicount )
    {
        r_->unlock_dynbuffer( static_cast<internaldynbuffer*>( d ), newvcount, newicount, newvcount );
    }

    //! Unlocks the buffer and loads the updated colour elements  to VRAM.
    //! This call assumes a previous call to unlock_dynbuffer was made which
    //! will have populated the VRAM buffer with the vertex, uv and index
    //! information: this call only updates the color array.
    //! This call requires that the dynbuffer remains the same size as
    //! when it was last unlocked.
    void deferred_renderer::unlock_dynbuffer_after_colour_change( dynbuffer* d )
    {
        auto idb = static_cast<internaldynbuffer*>( d );
        r_->unlock_dynbuffer( idb, 0, 0, idb->colours_count() );
    }

    //! Adds the dynbuffer to the draw queue only if not locked and it has
    //! valid content and has been uploaded to VRAM via the locking/fill/unlocking
    //! process.
    void deferred_renderer::draw_dynbuffer( const dynbufferdrawparams& d )
    {
        r_->draw_dynbuffer( d );
    }

    void deferred_renderer::set_viewport( int x, int y, unsigned int width, unsigned int height )
    {
        r_->set_viewport( x, y, width, height );
    }

    void deferred_renderer::set_view_matrix( const float* unaliased v )
    {
        r_->set_view_matrix( v );
    }

    void deferred_renderer::set_proj_matrix( const float* unaliased p, const float zNear )
    {
        r_->set_proj_matrix( p, zNear );
    }

    void deferred_renderer::set_skybox_3x3_matrix( const float* unaliased r3x3 )
    {
        r_->set_skybox_3x3_matrix( r3x3 );
    }

    void deferred_renderer::get_view_matrix( float* m4x4 ) const
    {
        assert( m4x4 );
        r_->get_view_matrix( m4x4 );
    }

    void deferred_renderer::get_inv_view_matrix( float* m4x4 ) const
    {
        assert( m4x4 );
        r_->get_inv_view_matrix( m4x4 );
    }

    void deferred_renderer::start_scene()
    {
        r_->start_scene();
    }

    void deferred_renderer::draw_sun( texturehandle_t h, const sun_noise& n, const sun_location_and_radius& xyzr )
    {
        r_->draw_sun( h, n, xyzr );
    }

    void deferred_renderer::draw_point_light( const point_light& light )
    {
        r_->draw_point_light( light );
    }

    void deferred_renderer::draw_directional_light( const directional_light& light )
    {
        r_->draw_directional_light( light );
    }

    //! Quads quads are rendered in the order they are specified, which will determine their
    //! layering on the final image. Background quads should be specified before foreground
    //! quads. Similarly, elements of pages will be rendered in batches in the order they
    //! are specified.
    void deferred_renderer::draw_screen_quads( texturehandle_t g, const screen_quad* quads, const unsigned int n )
    {
        r_->draw_screen_quads( g, quads, n );
    }

    void deferred_renderer::draw_sfx( SfxType tfx,
                                      texturehandle_t t1,
                                      texturehandle_t t2,
                                      const colour& c,
                                      const fxtriangle* fx,
                                      const unsigned int n )
    {
        r_->draw_sfx( tfx == SfxEmissive, t1, t2, c, fx, n );
    }

    void deferred_renderer::set_ambiant_light( float red, float green, float blue )
    {
        r_->set_ambiant_light( red, green, blue );
    }

    void deferred_renderer::set_debug_rendermode( DebugRenderMode m )
    {
        r_->set_debug_rendermode( m );
    }

    void deferred_renderer::end_scene()
    {
        r_->end_scene();
    }

    //! Renders 'n' instances of the mesh specified, each mesh being rendered
    //! using its own particular instance data.
    void deferred_renderer::draw_mesh_instances( meshhandle_t meshType,
                                                 const meshinstancedata* const instancesInfo,
                                                 const size_t n )
    {
        r_->add_mesh_instances( meshType, instancesInfo, static_cast<GLsizei>( n ) );
    }

    deferred_renderer::deferred_renderer( const unsigned int screen_width,
                                          const unsigned int screen_height,
                                          const Quality quality )
        : r_( nullptr )
    {
        unique_ptr<internalrenderer> p(
            new internalrenderer( static_cast<GLsizei>( screen_width ),
                                  static_cast<GLsizei>( screen_height ),
                                  quality ) );

        r_.swap( p );
    }

    deferred_renderer::~deferred_renderer()
    {
        r_.reset( nullptr );
    }

    meshhandle_t deferred_renderer::load_mesh( const shared_ptr<imesh>& m )
    {
        return r_->load_mesh( m );
    }

    texturehandle_t deferred_renderer::load_texture( const shared_ptr<iimgsrc>& pixels )
    {
        return r_->load_texture( pixels );
    }

    void deferred_renderer::load_skybox( const cubemap& sb )
    {
        r_->load_skybox( sb );
    }

    texturehandle_t deferred_renderer::load_sun_colour_ramp_texture( const iimgBGR_ptr& s )
    {
        return r_->load_sun_colour_ramp_texture( s );
    }

    unsigned int deferred_renderer::scene_triangle_count() const
    {
        return r_->scene_triangle_count();
    }

    void deferred_renderer::enable_skybox( const bool show )
    {
        r_->enable_skybox( show );
    }

    deferred_renderer::Quality deferred_renderer::quality_level() const
    {
        return r_->quality_level();
    }

    //! Returns the world position of a Window based x/y/Z location, where x and
    //! y are points within the Window rectangle, x being between 0 and Width and
    //! y being between 0 and Height, and Z between 0 and 1.0, which will return
    //! values in the range of the Z near and Z far of the currently set
    //! projection matrix. Using a value of 1.0 is recommended - this will return
    //! pick points on the far plane, and together with the known eye point a line
    //! from eye to the end of the scene can be constructed and used to find
    //! where the pick point lands in the scene.
    void deferred_renderer::un_project( unsigned int x,
                                        unsigned int y,
                                        float Z,
                                        float* unaliased worldpoint ) const
    {
        r_->un_project( x, y, Z, worldpoint );
    }

    void deferred_renderer::load_perspective(
        float FOVradians,
        size_t windowWidth,
        size_t windowHeight,
        float zNear,
        float zFar,
        float* unaliased m4x4 )
    {
        assert( m4x4 );

        solosnake::load_perspective( solosnake::radians( FOVradians ),
                                     static_cast<float>( windowWidth ) / windowHeight,
                                     zNear,
                                     zFar,
                                     m4x4 );
    }

    void deferred_renderer::load_orthographic(
        float left,
        float right,
        float bottom,
        float top,
        float nearVal,
        float farVal,
        float* unaliased m4x4 )
    {
        assert( m4x4 );
        solosnake::load_ortho( left, right, bottom, top, nearVal, farVal, m4x4 );
    }

    //-------------------------------------------------------------------------

    texture::texture() : bgra_(), width_( 0 )
    {
    }

    texture::texture( size_t w, size_t h, const unsigned char* bgraData )
        : bgra_( w* h * 4, 0u ), width_( w )
    {
        if( bgra_.empty() )
        {
            ss_throw( "Invalid width or height during texture creation." );
        }

        if( bgraData == nullptr )
        {
            ss_throw( "Cannot initialise a texture with nullptr data." );
        }

        memcpy( &bgra_.at( 0 ), bgraData, bgra_.size() );
    }

    const unsigned char* texture::bgra_bytes() const
    {
        return bgra_.data();
    }

    size_t texture::width() const
    {
        return width_;
    }

    size_t texture::height() const
    {
        return bgra_.empty() ? 0 : bgra_.size() / ( 4 * width_ );
    }

    //-------------------------------------------------------------------------

    /*
        Listing 10-6 shows a code fragment that downloads a rectangular texture that
       uses cached memory. Your
        application processes data between the glCopyTexSubImage2D and glGetTexImage
       calls. How much
        processing? Enough so that your application does not need to wait for the
       GPU.
        Listing 10-6 Code that downloads texture data
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, myTextureName);
        glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
        glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, sizex, sizey, 0, GL_BGRA,
       GL_UNSIGNED_INT_8_8_8_8_REV, myImagePtr);
        glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, 0, 0, image_width,
       image_height);
        glFlush();
        // Do other work processing here, using a double or triple buffer
        glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, 0, GL_BGRA,
       GL_UNSIGNED_INT_8_8_8_8_REV, pixels);

        */
}

#if 0
These are not used by application...

//! Returns the OpenGL enum type as its string name. May not have the
//! complete set of GLenum.
static string opengl_enum_to_string( GLenum type )
{
    switch( type )
    {
        case GL_FLOAT:                                       return "GL_FLOAT";
        case GL_FLOAT_VEC2:                                  return "GL_FLOAT_VEC2";
        case GL_FLOAT_VEC3:                                  return "GL_FLOAT_VEC3";
        case GL_FLOAT_VEC4:                                  return "GL_FLOAT_VEC4";
        case GL_INT:                                         return "GL_INT";
        case GL_INT_VEC2:                                    return "GL_INT_VEC2";
        case GL_INT_VEC3:                                    return "GL_INT_VEC3";
        case GL_INT_VEC4:                                    return "GL_INT_VEC4";
        case GL_BOOL:                                        return "GL_BOOL";
        case GL_BOOL_VEC2:                                   return "GL_BOOL_VEC2";
        case GL_BOOL_VEC3:                                   return "GL_BOOL_VEC3";
        case GL_BOOL_VEC4:                                   return "GL_BOOL_VEC4";
        case GL_FLOAT_MAT2:                                  return "GL_FLOAT_MAT2";
        case GL_FLOAT_MAT3:                                  return "GL_FLOAT_MAT3";
        case GL_FLOAT_MAT4:                                  return "GL_FLOAT_MAT4";
        case GL_FLOAT_MAT2x3:                                return "GL_FLOAT_MAT2x3";
        case GL_FLOAT_MAT2x4:                                return "GL_FLOAT_MAT2x4";
        case GL_FLOAT_MAT3x2:                                return "GL_FLOAT_MAT3x2";
        case GL_FLOAT_MAT3x4:                                return "GL_FLOAT_MAT3x4";
        case GL_FLOAT_MAT4x2:                                return "GL_FLOAT_MAT4x2";
        case GL_FLOAT_MAT4x3:                                return "GL_FLOAT_MAT4x3";
        case GL_SAMPLER_1D:                                  return "GL_SAMPLER_1D";
        case GL_SAMPLER_2D:                                  return "GL_SAMPLER_2D";
        case GL_SAMPLER_3D:                                  return "GL_SAMPLER_3D";
        case GL_SAMPLER_CUBE:                                return "GL_SAMPLER_CUBE";
        case GL_SAMPLER_1D_SHADOW:                           return "GL_SAMPLER_1D_SHADOW";
        case GL_SAMPLER_2D_SHADOW:                           return "GL_SAMPLER_2D_SHADOW";
        case GL_UNIFORM_BLOCK_BINDING:                       return "GL_UNIFORM_BLOCK_BINDING";
        case GL_UNIFORM_BLOCK_DATA_SIZE:                     return "GL_UNIFORM_BLOCK_DATA_SIZE";
        case GL_UNIFORM_BLOCK_NAME_LENGTH:                   return "GL_UNIFORM_BLOCK_NAME_LENGTH";
        case GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS:               return "GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS";
        case GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES:        return "GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES";
        case GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER:   return "GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER";
        case GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER: return "GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER";
        case GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER: return "GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER";

        default: break;
    }

    ostringstream os;
    os << "Unknown OpenGL enum : " << type;
    return os.str();
}

//! Writes information about OpenGL ACTIVE_UNIFORM usage to log.
static void log_program_uniform_use( GLuint program )
{
    validate_opengl_ok( __LINE__ );

    GLint numActiveUniforms = -1;
    glGetProgramiv( program, GL_ACTIVE_UNIFORMS, &numActiveUniforms );
    ss_log( "Num active uniforms is ", numActiveUniforms );

    GLint numActiveBlocks = -1;
    glGetProgramiv( program, GL_ACTIVE_UNIFORM_BLOCKS, &numActiveBlocks );
    validate_opengl_ok( __LINE__ );
    ss_log( "Num active blocks is ", numActiveUniforms );

    const GLenum blockParams[] =
    {
        GL_UNIFORM_BLOCK_BINDING,
        GL_UNIFORM_BLOCK_DATA_SIZE,
        GL_UNIFORM_BLOCK_NAME_LENGTH,
        GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER,
        GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER,
        GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER
    };

    for( GLint block = 0; block < numActiveBlocks; ++block )
    {
        ss_log( "Block ", block );
        for( size_t i = 0; i < ( sizeof( blockParams ) / sizeof( GLenum ) ); ++i )
        {
            GLint param = -1;
            string enumName = opengl_enum_to_string( blockParams[i] );
            glGetActiveUniformBlockiv( program, block, blockParams[i], &param );
            validate_opengl_ok( __LINE__ );
            ss_log( "Param : ", enumName.c_str(), " = ", param );
        }

        GLint numActiveUniformsInBlock = -1;
        glGetActiveUniformBlockiv( program, block, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numActiveUniformsInBlock );
        validate_opengl_ok( __LINE__ );
        ss_log( "Param : GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS = ", numActiveUniformsInBlock );

        vector<GLint> uniformIndices( 1 + size_t( numActiveUniformsInBlock ), 0 );
        glGetActiveUniformBlockiv( program, block, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, &uniformIndices[0] );
        validate_opengl_ok( __LINE__ );
        ss_log( "Param : GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES = " );
        for( GLint i = 0; i < numActiveUniformsInBlock; ++i )
        {
            ss_log( uniformIndices[i], ", " );
        }
    }

    GLint nameBufferSize = 0;
    glGetProgramiv( program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameBufferSize );
    validate_opengl_ok( __LINE__ );

    GLint uniformSize = -1;
    GLenum uniformType = -1;
    vector<GLchar> buffer( nameBufferSize + 1, 0 );
    for( GLint i = 0; i < numActiveUniforms; ++i )
    {
        glGetActiveUniform( program, i, nameBufferSize, NULL, &uniformSize, &uniformType, &buffer[0] );
        validate_opengl_ok( __LINE__ );
        string enumName = opengl_enum_to_string( uniformType );
        ss_log( "Uniform : ", &buffer[0], " ", enumName.c_str(), " size = ", uniformSize );
    }
}

//! Returns the contents of the named file as a string.
static string read_file_to_string( const filepath& f )
{
    stringstream is;
    std::filesystem::ifstream ifile( f );

    if( false == ifile.is_open() )
    {
        ss_throw( "Unable to open shader file." );
    }

    is << ifile.rdbuf();

    return is.str();
}
#endif // Unused code.
