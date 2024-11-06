#include "solosnake/quickmodel.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include "solosnake/image.hpp"
#include "solosnake/glutwindow.hpp"
#include "solosnake/external/glew.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/logging.hpp"

using namespace solosnake;

namespace
{
#define BUFFER_OFFSET(i) ((char*)NULL + (i))

    // Globals
    // 1 VAO for the cube
    GLuint VAOCUBE[1];
    // 1 IBO for the cube
    GLuint IBCUBE[1];
    // 1 VBO cube
    GLuint VBCUBE[1];
    GLuint start = 0;
    GLuint end = 0;
    GLuint count = 0;

    matrix4x4_t mv;
    matrix4x4_t proj;

    // 1 shader for the quad
    // 1 shader for the triangle
    GLuint ShaderProgram[1];
    GLuint VertexShader[1];
    GLuint FragmentShader[1];

    GLuint texDiffuse = 0;

    int ProjectionModelviewMatrix_Loc[1]; // The location of
    // ProjectionModelviewMatrix in the
    // shaders
    int Texture0_Loc[1];

    /*
        Shader1.vert : notice that we have 1 uniform called
       ProjectionModelviewMatrix. We set this to identity in our C++ code.
        The color has the smooth qualifier which will instruct the GPU to do
       bilinear interpolation across the polygon.
        InVertex and InColor are attributes which are setup with a call to
       glBindAttribLocation. After that, you must
        link the shader and check for success (glLinkProgram and
       glGetProgramiv(shaderProgram, GL_LINK_STATUS, (GLint *)&IsLinked))
        */
    //[VERTEX SHADER]
    static const char shader1vert[]
        = "#version 150\n"
          "\n"
          "in vec3 InVertex;\n"
          "in vec2 InTexCoord0;\n"
          "\n"
          "smooth out vec2 texCoords;\n"
          "\n"
          "uniform mat4 ProjectionModelviewMatrix;\n"
          "\n"
          "void main()\n"
          "{\n"
          "    gl_Position = ProjectionModelviewMatrix * vec4(InVertex, 1.0);\n"
          "    texCoords = InTexCoord0;\n"
          "}\n\n";

    /*
        Shader1.frag : here we have the input Color. We just write it out to
       FragColor. FragColor is our own output variable
        which GL automatically bind to output 0, therefore we don't need to set it
       up from our C++ side of the code.
        */
    //[FRAGMENT SHADER]
    static const char Shader1frag[] = "#version 150\n"
                                      "\n"
                                      "uniform sampler2D texDiffuse;\n"
                                      "\n"
                                      "smooth in vec2 texCoords;\n"
                                      "\n"
                                      "out vec4 FragColor;\n"
                                      "\n"
                                      "void main()\n"
                                      "{\n"
                                      //"    FragColor = vec4(0.0, 1.0, 0.5, 0);\n"
                                      // "    FragColor = vec4(texCoords.s, texCoords.t, 0, 0);\n"
                                      "    FragColor = texture(texDiffuse,texCoords);\n"
                                      "}\n";

    // loadFile - loads text file into char* fname
    // allocates memory - so need to delete after use
    // size of file returned in fSize
    std::string loadFile( const char* fname )
    {
        // FAKE LOADING HERE.

        if( 0 == strcmp( fname, "Shader1.vert" ) )
        {
            return shader1vert;
        }

        if( 0 == strcmp( fname, "Shader1.frag" ) )
        {
            return Shader1frag;
        }

        return std::string();
    }

    // printShaderInfoLog
    // From OpenGL Shading Language 3rd Edition, p215-216
    // Display (hopefully) useful error messages if shader fails to compile
    void printShaderInfoLog( GLint shader )
    {
        int infoLogLen = 0;
        int charsWritten = 0;

        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLogLen );

        if( infoLogLen > 0 )
        {
            GLchar* infoLog = new GLchar[infoLogLen];
            // error check for fail to allocate memory omitted
            glGetShaderInfoLog( shader, infoLogLen, &charsWritten, infoLog );
            ss_log( "InfoLog : ", infoLog );
            delete[] infoLog;
        }
    }

    void InitGLStates()
    {
        assert( GL_NO_ERROR == glGetError() );

        //glShadeModel( GL_SMOOTH );
        assert( GL_NO_ERROR == glGetError() );
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        assert( GL_NO_ERROR == glGetError() );
        glReadBuffer( GL_BACK );
        assert( GL_NO_ERROR == glGetError() );
        glDrawBuffer( GL_BACK );
        assert( GL_NO_ERROR == glGetError() );
        glEnable( GL_DEPTH_TEST );
        assert( GL_NO_ERROR == glGetError() );
        glDepthFunc( GL_LEQUAL );
        assert( GL_NO_ERROR == glGetError() );
        glDepthMask( GL_TRUE );
        assert( GL_NO_ERROR == glGetError() );
        glDisable( GL_STENCIL_TEST );
        assert( GL_NO_ERROR == glGetError() );
        glStencilMask( 0xFFFFFFFF );
        assert( GL_NO_ERROR == glGetError() );
        glStencilFunc( GL_EQUAL, 0x00000000, 0x00000001 );

        assert( GL_NO_ERROR == glGetError() );

        glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
        glFrontFace( GL_CCW );
        glCullFace( GL_BACK );
        glEnable( GL_CULL_FACE );
        glClearColor( 0.0, 0.0, 0.5, 0.0 );
        glClearDepth( 1.0 );
        glClearStencil( 0 );

        assert( GL_NO_ERROR == glGetError() );

        glDisable( GL_BLEND );

        assert( GL_NO_ERROR == glGetError() );

        //if( GL_TRUE == glIsEnabled( GL_ALPHA_TEST ) )
        //{
        //    glDisable( GL_ALPHA_TEST );
        //}

        //assert( GL_NO_ERROR == glGetError() );

        glDisable( GL_DITHER );

        assert( GL_NO_ERROR == glGetError() );

        glActiveTexture( GL_TEXTURE0 );

        assert( GL_NO_ERROR == glGetError() );
    }

    int LoadShader( const char* pfilePath_vs,
                    const char* pfilePath_fs,
                    bool bindTexCoord0,
                    bool bindNormal,
                    bool bindColor,
                    GLuint& shaderProgram,
                    GLuint& vertexShader,
                    GLuint& fragmentShader )
    {
        assert( GL_NO_ERROR == glGetError() );

        shaderProgram = 0;
        vertexShader = 0;
        fragmentShader = 0;

        // load shaders
        std::string vertexShaderString = loadFile( pfilePath_vs );
        std::string fragmentShaderString = loadFile( pfilePath_fs );

        if( vertexShaderString.empty() )
        {
            return -1;
        }

        if( fragmentShaderString.empty() )
        {
            return -1;
        }

        vertexShader = glCreateShader( GL_VERTEX_SHADER );
        fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

        const char* vertexShaderCStr = vertexShaderString.c_str();
        const char* fragmentShaderCStr = fragmentShaderString.c_str();
        glShaderSource( vertexShader, 1, ( const GLchar** )&vertexShaderCStr, NULL );
        glShaderSource( fragmentShader, 1, ( const GLchar** )&fragmentShaderCStr, NULL );

        GLint compiled;

        glCompileShader( vertexShader );
        glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compiled );
        if( compiled == GL_FALSE )
        {
            ss_err( "Vertex shader not compiled." );
            printShaderInfoLog( vertexShader );

            glDeleteShader( vertexShader );
            vertexShader = 0;
            glDeleteShader( fragmentShader );
            fragmentShader = 0;

            return -1;
        }

        glCompileShader( fragmentShader );
        glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &compiled );
        if( compiled == GL_FALSE )
        {
            ss_err( "Fragment shader not compiled." );
            printShaderInfoLog( fragmentShader );

            glDeleteShader( vertexShader );
            vertexShader = 0;
            glDeleteShader( fragmentShader );
            fragmentShader = 0;

            return -1;
        }

        shaderProgram = glCreateProgram();

        glAttachShader( shaderProgram, vertexShader );
        glAttachShader( shaderProgram, fragmentShader );

        glBindAttribLocation( shaderProgram, 0, "InVertex" );

        if( bindTexCoord0 )
        {
            glBindAttribLocation( shaderProgram, 1, "InTexCoord0" );
        }

        if( bindNormal )
        {
            glBindAttribLocation( shaderProgram, 2, "InNormal" );
        }

        if( bindColor )
        {
            glBindAttribLocation( shaderProgram, 3, "InColor" );
        }

        glLinkProgram( shaderProgram );

        GLint IsLinked;
        glGetProgramiv( shaderProgram, GL_LINK_STATUS, ( GLint* )&IsLinked );
        if( IsLinked == GL_FALSE )
        {
            ss_err( "Failed to link shader." );

            GLint maxLength;
            glGetProgramiv( shaderProgram, GL_INFO_LOG_LENGTH, &maxLength );
            if( maxLength > 0 )
            {
                char* pLinkInfoLog = new char[maxLength];
                glGetProgramInfoLog( shaderProgram, maxLength, &maxLength, pLinkInfoLog );
                ss_log( pLinkInfoLog );
                delete[] pLinkInfoLog;
            }

            glDetachShader( shaderProgram, vertexShader );
            glDetachShader( shaderProgram, fragmentShader );
            glDeleteShader( vertexShader );
            vertexShader = 0;
            glDeleteShader( fragmentShader );
            fragmentShader = 0;
            glDeleteProgram( shaderProgram );
            shaderProgram = 0;

            return -1;
        }

        assert( GL_NO_ERROR == glGetError() );

        return 1; // Success
    }

    void CreateGeometry( size_t indexCount,
                         const unsigned short* indices,
                         size_t vertexCount,
                         const float* vertexSrc,
                         const float* textureUVsSrc )
    {
        assert( GL_NO_ERROR == glGetError() );

        start = 0;
        end = static_cast<GLuint>( indexCount - 1 );
        count = static_cast<GLuint>( indexCount );

        // Create the IBO for the cube
        // 16 bit indices
        glGenBuffers( 1, &IBCUBE[0] );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBCUBE[0] );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof( GLushort ), indices, GL_STATIC_DRAW );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

        std::vector<float> buffer( vertexCount * 5 );

        float* vertices = &buffer[0];
        float* uvs = vertices + ( vertexCount * 3 );

        std::memcpy( vertices, vertexSrc, vertexCount * 3 * sizeof( float ) );
        std::memcpy( uvs, textureUVsSrc, vertexCount * 2 * sizeof( float ) );

        // Create VBO for the cube
        glGenBuffers( 1, &VBCUBE[0] );
        glBindBuffer( GL_ARRAY_BUFFER, VBCUBE[0] );
        glBufferData( GL_ARRAY_BUFFER, buffer.size() * sizeof( float ), vertices, GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

        // VAO for the quad
        glGenVertexArrays( 1, &VAOCUBE[0] );
        glBindVertexArray( VAOCUBE[0] );

        // Bind the VBO and setup pointers for the VAO
        glBindBuffer( GL_ARRAY_BUFFER, VBCUBE[0] );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( 0 ) );
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( sizeof( float ) * ( uvs - vertices ) ) );
        glEnableVertexAttribArray( 0 );
        glEnableVertexAttribArray( 1 );
        glDisableVertexAttribArray( 2 );
        glDisableVertexAttribArray( 3 );

        // Bind the IBO for the VAO
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBCUBE[0] );

        // Just testing
        glBindVertexArray( 0 );
        glDisableVertexAttribArray( 0 );
        glDisableVertexAttribArray( 1 );
        glDisableVertexAttribArray( 2 );
        glDisableVertexAttribArray( 3 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

        assert( GL_NO_ERROR == glGetError() );

        // Texture
        const size_t dim = 128;
        image diffImg( dim, dim, image::format_bgra );

        const bgra blue( 0xFF, 0, 0, 0 );
        const bgra white( 0xFF, 0xFF, 0xFF, 0xFF );

        for( size_t i = 0; i < 4; ++i )
        {
            float x = 0.25f * i;

            for( size_t j = 0; j < 4; ++j )
            {
                bgra c = ( ( i % 2 ) || ( j % 2 ) ) && ( !( ( i % 2 ) && ( j % 2 ) ) ) ? blue : white;

                float y = 0.25f * j;
                diffImg.draw_triangle( x, y, c, x, y + 0.25f, c, x + 0.25f, y + 0.25f, c );
                diffImg.draw_triangle( x, y, c, x + 0.25f, y, c, x + 0.25f, y + 0.25f, c );
            }
        }

        glGenTextures( 1, &texDiffuse );
        glBindTexture( GL_TEXTURE_2D, texDiffuse );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

        assert( GL_NO_ERROR == glGetError() );

        glTexImage2D( GL_TEXTURE_2D,
                      0,
                      GL_RGB8,
                      diffImg.width(),
                      diffImg.height(),
                      0,
                      GL_BGRA,
                      GL_UNSIGNED_BYTE,
                      diffImg.data() );

        assert( GL_NO_ERROR == glGetError() );

        glActiveTexture( GL_TEXTURE0 );

        assert( GL_NO_ERROR == glGetError() );

        glGenerateMipmap( GL_TEXTURE_2D );

        assert( GL_NO_ERROR == glGetError() );
    }

    void display()
    {
        assert( GL_NO_ERROR == glGetError() );

        static float ry = 0.0f;
        ry += 0.01f;

        load_xyz_ry_4x4( 0.0f, 2.0f, -10.0f, radians( ry ), mv );

        matrix4x4_t projmv;
        mul_4x4( mv, proj, projmv );

        // Clear all the buffers
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

        // Bind the shader that we want to use
        glUseProgram( ShaderProgram[0] );
        glBindTexture( GL_TEXTURE_2D, texDiffuse );

        // Setup all uniforms for your shader
        glUniformMatrix4fv( ProjectionModelviewMatrix_Loc[0], 1, GL_FALSE, projmv );
        glUniform1i( Texture0_Loc[0], 0 );

        // Bind the VAO
        glBindVertexArray( VAOCUBE[0] );

        // Draw command
        // The first to last vertex is 0 to 3
        // 6 indices will be used to render the 2 triangles. This make our quad.
        // The last parameter is the start address in the IBO => zero
        glDrawRangeElements( GL_TRIANGLES, start, end, count, GL_UNSIGNED_SHORT, NULL );

        assert( GL_NO_ERROR == glGetError() );
    }

    void reshape( int w, int h )
    {
        assert( GL_NO_ERROR == glGetError() );
        load_perspective( solosnake::radians( PI_RADIANS * 0.25f ), float( w ) / h, 0.1f, 100.0f, proj );
        glViewport( 0, 0, w, h );
        assert( GL_NO_ERROR == glGetError() );
    }

    void ExitFunction( int )
    {
        glDeleteTextures( 1, &texDiffuse );
        texDiffuse = 0;

        glBindVertexArray( 0 );
        glDisableVertexAttribArray( 0 );
        glDisableVertexAttribArray( 1 );
        glDisableVertexAttribArray( 2 );
        glDisableVertexAttribArray( 3 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

        glUseProgram( 0 );

        glDeleteBuffers( 1, &VAOCUBE[0] );
        glDeleteBuffers( 1, &VBCUBE[0] );
        glDeleteVertexArrays( 1, &VAOCUBE[0] );

        glDetachShader( ShaderProgram[0], VertexShader[0] );
        glDetachShader( ShaderProgram[0], FragmentShader[0] );
        glDeleteShader( VertexShader[0] );
        glDeleteShader( FragmentShader[0] );
        glDeleteProgram( ShaderProgram[0] );
    }
}

namespace solosnake
{
    bool quickmodel( size_t indexCount,
                     const unsigned short* indices,
                     size_t vertexCount,
                     const float* vertexSrc,
                     const float* textureUVsSrc,
                     const unsigned int framesToShowFor )
    {
        auto glut = glutWindow::make_shared( 3, 3, 1280, 960 );

        assert( GL_NO_ERROR == glGetError() );

        int NumberOfExtensions;
        int OpenGLVersion[2];

        glut->glutExitFunc( ExitFunction );

        // Currently, GLEW uses glGetString(GL_EXTENSIONS) which is not legal code
        // in GL 3.3, therefore GLEW would fail if we don't set this to TRUE.
        // GLEW will avoid looking for extensions and will just get function
        // pointers for all GL functions.
        glewExperimental = GL_TRUE;

        assert( GL_NO_ERROR == glGetError() );

        GLenum err = glewInit();

        if( GLEW_OK != err )
        {
            ss_err( "glewInit error : ", glewGetErrorString( err ) );
            return false;
        }

        // GLEW 1.9 has error after glewInit :(
        glGetError();

        assert( GL_NO_ERROR == glGetError() );

        // This is the new way for getting the GL version.
        // It returns integers. Much better than the old glGetString(GL_VERSION).
        glGetIntegerv( GL_MAJOR_VERSION, &OpenGLVersion[0] );
        glGetIntegerv( GL_MINOR_VERSION, &OpenGLVersion[1] );

        ss_log( "OpenGL major version = ", OpenGLVersion[0] );
        ss_log( "OpenGL minor version = ", OpenGLVersion[1] );

        assert( GL_NO_ERROR == glGetError() );

        // The old method to get the extension list is obsolete.
        // You must use glGetIntegerv and glGetStringi
        glGetIntegerv( GL_NUM_EXTENSIONS, &NumberOfExtensions );

        InitGLStates();

        if( LoadShader( "Shader1.vert",
                        "Shader1.frag",
                        true,
                        false,
                        false,
                        ShaderProgram[0],
                        VertexShader[0],
                        FragmentShader[0] ) == -1 )
        {
            return false;
        }
        else
        {
            ProjectionModelviewMatrix_Loc[0]
                = glGetUniformLocation( ShaderProgram[0], "ProjectionModelviewMatrix" );
            Texture0_Loc[0] = glGetUniformLocation( ShaderProgram[0], "texDiffuse" );
        }

        CreateGeometry(
            indexCount, indices, vertexCount, vertexSrc, textureUVsSrc ? textureUVsSrc : vertexSrc );

        unsigned int displayCallCounter = 0; // Captured by display functor.
        glutWindow* gw = glut.get();         // Capture by pointer only.
        auto displayfunc = [&]()
        {
            display();
            if( ++displayCallCounter == framesToShowFor )
            {
                gw->end_display_loop();
            }
        };

        glut->glutDisplayFunc( displayfunc );
        glut->glutReshapeFunc( reshape );
        glut->glutMainLoop();

        return true;
    }
}
