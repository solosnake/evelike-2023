#define RUN_TESTS

#ifdef RUN_TESTS

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "solosnake/testing/testing.hpp"
#include "solosnake/external/glew.hpp"
#include "solosnake/glutwindow.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/matrix3d.hpp"

using namespace solosnake;

namespace
{

class imodel3d
{
public:
    virtual ~imodel3d();

    unsigned int primitiveType() const; // GL_TRIANGLES normally.

    unsigned int vertexCount() const;

    const float* vertices() const;

    const float* normals() const;

    const float* textures0() const;

    unsigned int indexCount() const;

    const unsigned short* indices() const;

private:
    virtual unsigned int get_primitiveType() const = 0;

    virtual unsigned int get_vertexCount() const = 0;

    virtual const float* get_vertices() const = 0;

    virtual const float* get_normals() const = 0;

    virtual const float* get_textures0() const = 0;

    virtual unsigned int get_indexCount() const = 0;

    virtual const unsigned short* get_indices() const = 0;
};

//-------------------------------------------------------------------------

imodel3d::~imodel3d()
{
    static_assert(sizeof(GLushort) == sizeof(unsigned short),
                  "unsigned short and OpenGL GLushort are not same types.");
}

unsigned int imodel3d::primitiveType() const
{
    return this->get_primitiveType();
}

unsigned int imodel3d::vertexCount() const
{
    return this->get_vertexCount();
}

const float* imodel3d::vertices() const
{
    return this->get_vertices();
}

const float* imodel3d::normals() const
{
    return this->get_normals();
}

const float* imodel3d::textures0() const
{
    return this->get_textures0();
}

unsigned int imodel3d::indexCount() const
{
    return this->get_indexCount();
}

const unsigned short* imodel3d::indices() const
{
    return this->get_indices();
}

//-------------------------------------------------------------------------

class cubemodel : public imodel3d
{
    static const unsigned int vertex_count_
        = 6 * 2 * 3; // 6 faces, 2 triangles each face, 3 vertex per triangle.
    static const unsigned int primitiveType_ = GL_TRIANGLES;
    static const float vertices_[vertex_count_ * 3];
    static const float normals_[vertex_count_ * 3];
    static const float textures0_[vertex_count_ * 2];
    static const unsigned short indices_[vertex_count_];

    virtual unsigned int get_primitiveType() const
    {
        return cubemodel::primitiveType_;
    }

    virtual unsigned int get_vertexCount() const
    {
        return cubemodel::vertex_count_;
    }

    virtual const float* get_vertices() const
    {
        return cubemodel::vertices_;
    }

    virtual const float* get_normals() const
    {
        return cubemodel::normals_;
    }

    virtual const float* get_textures0() const
    {
        return cubemodel::textures0_;
    }

    virtual unsigned int get_indexCount() const
    {
        return cubemodel::vertex_count_;
    }

    virtual const unsigned short* get_indices() const
    {
        return cubemodel::indices_;
    }
};

//-------------------------------------------------------------------------

const unsigned int cubemodel::vertex_count_;

const unsigned int cubemodel::primitiveType_;

#define A -0.5f, +0.5f, +0.5f
#define B -0.5f, +0.5f, -0.5f
#define C +0.5f, +0.5f, -0.5f
#define D +0.5f, +0.5f, +0.5f

#define E -0.5f, -0.5f, +0.5f
#define F -0.5f, -0.5f, -0.5f
#define G +0.5f, -0.5f, -0.5f
#define H +0.5f, -0.5f, +0.5f

#define NYu +0.0f, +1.0f, +0.0f
#define NYd +0.0f, -1.0f, +0.0f
#define NXu +1.0f, +0.0f, +0.0f
#define NXd -1.0f, +0.0f, +0.0f
#define NZu +0.0f, +0.0f, +1.0f
#define NZd +0.0f, +0.0f, -1.0f

#define T0 0.0f, 0.0f
#define T1 1.0f, 0.0f
#define T2 0.0f, 1.0f
#define T3 1.0f, 1.0f

/// 36 x 3 = 108?
const float cubemodel::vertices_[vertex_count_ * 3] = { B, A, D, D, C, B, // 0
                                                        C, D, H, C, H, G, // 1
                                                        G, H, E, G, E, F, // 2
                                                        F, E, B, B, E, A, // 3
                                                        A, E, D, D, E, H, // 4
                                                        B, C, G, B, G, F  // 5
};

const float cubemodel::normals_[vertex_count_ * 3]
    = { NYu, NYu, NYu, NYu, NYu, NYu, NXu, NXu, NXu, NXu, NXu, NXu, NYd, NYd, NYd, NYd, NYd, NYd,
        NXd, NXd, NXd, NXd, NXd, NXd, NZu, NZu, NZu, NZu, NZu, NZu, NZd, NZd, NZd, NZd, NZd, NZd, };

const float cubemodel::textures0_[vertex_count_ * 2]
    = { T0, T1, T2, T2, T1, T3, T0, T1, T2, T2, T1, T3, T0, T1, T2, T2, T1, T3,
        T0, T1, T2, T2, T1, T3, T0, T1, T2, T2, T1, T3, T0, T1, T2, T2, T1, T3, };

// 36
const unsigned short cubemodel::indices_[6 * 2 * 3]
    = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, };

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

struct TVertex3
{
    float x, y, z;
};

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

int ProjectionModelviewMatrix_Loc[1]; // The location of
// ProjectionModelviewMatrix in the
// shaders

GLuint fbo = 0;
GLuint rboRGBA = 0;
GLuint rboDepth = 0;

//[VERTEX SHADER]
static const char Shader2vert[]
    = "#version 150\n"
      "\n"
      "in vec3 InVertex;\n"
      "in vec3 InNormal;\n"
      "\n"
      "\n"
      "smooth out vec3 LightVector0;\n"
      "smooth out vec3 EyeNormal;\n"
      "\n"
      "uniform mat4 ProjectionModelviewMatrix;\n"
      "\n"
      "\n"
      "void main()\n"
      "{\n"
      "   gl_Position = ProjectionModelviewMatrix * vec4(InVertex, 1.0);\n"
      "   LightVector0 = vec3(1.0, 1.0, 1.0);\n"
      "   EyeNormal = InNormal;\n"
      "}\n";

//[FRAGMENT SHADER]
static const char Shader2frag[] = "#version 150\n"
                                  "\n"
                                  "smooth in vec3 LightVector0;\n"
                                  "smooth in vec3 EyeNormal;\n"
                                  "\n"
                                  "out vec4 FragColor;\n"
                                  "\n"
                                  "\n"
                                  "void main()\n"
                                  "{\n"
                                  "   vec3 eyeNormal;\n"
                                  "   vec3 lightVector;\n"
                                  "   float dotProduct;\n"
                                  "\n"
                                  "   eyeNormal = normalize(EyeNormal);\n"
                                  "   lightVector = normalize(LightVector0);\n"
                                  "\n"
                                  "   dotProduct = dot(eyeNormal, lightVector);\n"
                                  "\n"
                                  "   FragColor = vec4(dotProduct);\n"
                                  "}\n";

// loadFile - loads text file into char* fname
// allocates memory - so need to delete after use
// size of file returned in fSize
std::string loadFile(const char* fname)
{
    // FAKE LOADING HERE.

    if (0 == strcmp(fname, "Shader2.vert"))
    {
        return Shader2vert;
    }

    if (0 == strcmp(fname, "Shader2.frag"))
    {
        return Shader2frag;
    }

    /*std::ifstream file(fname);
        if(!file.is_open())
        {
            std::cout << "Unable to open file " << fname << std::endl;
            exit(1);
        }

        std::stringstream fileData;
        fileData << file.rdbuf();
        file.close();

        return fileData.str();*/

    return std::string();
}

// printShaderInfoLog
// From OpenGL Shading Language 3rd Edition, p215-216
// Display (hopefully) useful error messages if shader fails to compile
void printShaderInfoLog(GLint shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0)
    {
        GLchar* infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
        std::cout << "InfoLog : " << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
}

void InitGLStates()
{
    assert(GL_NO_ERROR == glGetError());

    glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glReadBuffer(GL_BACK);
    glDrawBuffer(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    glStencilMask(0xFFFFFFFF);
    glStencilFunc(GL_EQUAL, 0x00000000, 0x00000001);

    assert(GL_NO_ERROR == glGetError());

    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glClearColor(1.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glClearStencil(0);

    assert(GL_NO_ERROR == glGetError());

    glDisable(GL_BLEND);

    assert(GL_NO_ERROR == glGetError());

    if (GL_TRUE == glIsEnabled(GL_ALPHA_TEST))
    {
        glDisable(GL_ALPHA_TEST);
    }

    assert(GL_NO_ERROR == glGetError());

    glDisable(GL_DITHER);

    assert(GL_NO_ERROR == glGetError());

    glActiveTexture(GL_TEXTURE0);

    assert(GL_NO_ERROR == glGetError());
}

int LoadShader(const char* pfilePath_vs,
               const char* pfilePath_fs,
               bool bindTexCoord0,
               bool bindNormal,
               bool bindColor,
               GLuint& shaderProgram,
               GLuint& vertexShader,
               GLuint& fragmentShader)
{
    assert(GL_NO_ERROR == glGetError());

    shaderProgram = 0;
    vertexShader = 0;
    fragmentShader = 0;

    // load shaders & get length of each
    std::string vertexShaderString = loadFile(pfilePath_vs);
    std::string fragmentShaderString = loadFile(pfilePath_fs);

    if (vertexShaderString.empty())
    {
        return -1;
    }

    if (fragmentShaderString.empty())
    {
        return -1;
    }

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* vertexShaderCStr = vertexShaderString.c_str();
    const char* fragmentShaderCStr = fragmentShaderString.c_str();
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexShaderCStr, NULL);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentShaderCStr, NULL);

    GLint compiled;

    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE)
    {
        std::cout << "Vertex shader not compiled." << std::endl;
        printShaderInfoLog(vertexShader);

        glDeleteShader(vertexShader);
        vertexShader = 0;
        glDeleteShader(fragmentShader);
        fragmentShader = 0;

        return -1;
    }

    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE)
    {
        std::cout << "Fragment shader not compiled." << std::endl;
        printShaderInfoLog(fragmentShader);

        glDeleteShader(vertexShader);
        vertexShader = 0;
        glDeleteShader(fragmentShader);
        fragmentShader = 0;

        return -1;
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glBindAttribLocation(shaderProgram, 0, "InVertex");

    if (bindTexCoord0)
        glBindAttribLocation(shaderProgram, 1, "InTexCoord0");

    if (bindNormal)
        glBindAttribLocation(shaderProgram, 2, "InNormal");

    if (bindColor)
        glBindAttribLocation(shaderProgram, 3, "InColor");

    glLinkProgram(shaderProgram);

    GLint IsLinked;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, (GLint*)&IsLinked);
    if (IsLinked == GL_FALSE)
    {
        std::cout << "Failed to link shader." << std::endl;

        GLint maxLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
        if (maxLength > 0)
        {
            char* pLinkInfoLog = new char[maxLength];
            glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, pLinkInfoLog);
            std::cout << pLinkInfoLog << std::endl;
            delete[] pLinkInfoLog;
        }

        glDetachShader(shaderProgram, vertexShader);
        glDetachShader(shaderProgram, fragmentShader);
        glDeleteShader(vertexShader);
        vertexShader = 0;
        glDeleteShader(fragmentShader);
        fragmentShader = 0;
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;

        return -1;
    }

    assert(GL_NO_ERROR == glGetError());

    return 1; // Success
}

void CreateFBO()
{
    assert(GL_NO_ERROR == glGetError());

    glGenRenderbuffers(1, &rboRGBA);
    glBindRenderbuffer(GL_RENDERBUFFER, rboRGBA);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, 640, 480);

    assert(GL_NO_ERROR == glGetError());

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, 640, 480);

    assert(GL_NO_ERROR == glGetError());

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboRGBA);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    assert(GL_NO_ERROR == glGetError());
}

void CreateGeometry()
{
    assert(GL_NO_ERROR == glGetError());

    cubemodel c;

    start = 0;
    end = c.indexCount() - 1;
    count = c.indexCount();

    // Create the IBO for the cube
    // 16 bit indices
    glGenBuffers(1, &IBCUBE[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBCUBE[0]);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, c.indexCount() * sizeof(GLushort), c.indices(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create VBO for the cube
    glGenBuffers(1, &VBCUBE[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBCUBE[0]);
    glBufferData(GL_ARRAY_BUFFER, c.vertexCount() * sizeof(TVertex3), c.vertices(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VAO for the quad
    glGenVertexArrays(1, &VAOCUBE[0]);
    glBindVertexArray(VAOCUBE[0]);

    // Bind the VBO and setup pointers for the VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBCUBE[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TVertex3), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);

    // Bind the IBO for the VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBCUBE[0]);

    // Just testing
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    assert(GL_NO_ERROR == glGetError());
}

void display()
{
    GLenum fboBindings = { GL_COLOR_ATTACHMENT0 }; //{ GL_FRONT_LEFT }; //
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffers(1, &fboBindings);

    assert(GL_NO_ERROR == glGetError());

    // Clear all the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));

    assert(GL_NO_ERROR == glGetError());

    static float ry = 0.0f;
    ry += 0.1f;

    load_xyz_ry_4x4(0.0f, 0.0f, -10.0f, radians(ry), mv);

    matrix4x4_t projmv;
    mul_4x4(mv, proj, projmv);

    // Bind the shader that we want to use
    glUseProgram(ShaderProgram[0]);
    // Setup all uniforms for your shader
    glUniformMatrix4fv(ProjectionModelviewMatrix_Loc[0], 1, GL_FALSE, projmv);
    // Bind the VAO
    glBindVertexArray(VAOCUBE[0]);
    // At this point, we would bind textures but we aren't using textures in
    // this example

    // Draw command
    // The first to last vertex is 0 to 3
    // 6 indices will be used to render the 2 triangles. This make our quad.
    // The last parameter is the start address in the IBO => zero
    glDrawRangeElements(GL_TRIANGLES, start, end, count, GL_UNSIGNED_SHORT, NULL);

    assert(GL_NO_ERROR == glGetError());

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, 640, 480, 0, 0, 640, 480, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    assert(GL_NO_ERROR == glGetError());
}

void reshape(int w, int h)
{
    assert(GL_NO_ERROR == glGetError());

    load_perspective(solosnake::radians(PI_RADIANS * 0.25f), float(w) / h, 0.1f, 100.0f, proj);

    glViewport(0, 0, w, h);

    assert(GL_NO_ERROR == glGetError());
}

void ExitFunction(int value)
{
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glUseProgram(0);

    glDeleteBuffers(1, &VAOCUBE[0]);
    glDeleteBuffers(1, &VBCUBE[0]);
    glDeleteVertexArrays(1, &VAOCUBE[0]);

    glDetachShader(ShaderProgram[0], VertexShader[0]);
    glDetachShader(ShaderProgram[0], FragmentShader[0]);
    glDeleteShader(VertexShader[0]);
    glDeleteShader(FragmentShader[0]);
    glDeleteProgram(ShaderProgram[0]);
}
}

TEST( opengl, renderbuffer )
{
    {
        auto glut = glutWindow::make_shared(3, 3, 640, 480);

        assert(GL_NO_ERROR == glGetError());

        int OpenGLVersion[2];

        glut->glutExitFunc(ExitFunction);

        // Currently, GLEW uses glGetString(GL_EXTENSIONS) which is not legal
        // code
        // in GL 3.3, therefore GLEW would fail if we don't set this to GL_TRUE.
        // GLEW will avoid looking for extensions and will just get function
        // pointers for all GL functions.
        glewExperimental = GL_TRUE;

        assert(GL_NO_ERROR == glGetError());

        GLenum err = glewInit();
        if (err != GLEW_OK)
        {
            // Problem: glewInit failed, something is seriously wrong.
            ADD_FAILURE("glewInit failed, aborting.");
            return;
        }

        // GLEW 1.9 has error after glewInit :(
        glGetError();

        assert(GL_NO_ERROR == glGetError());

        // This is the new way for getting the GL version.
        // It returns integers. Much better than the old
        // glGetString(GL_VERSION).
        glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
        glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
        EXPECT_EQ(3, OpenGLVersion[0]);
        EXPECT_EQ(3, OpenGLVersion[1]);

        assert(GL_NO_ERROR == glGetError());

        InitGLStates();

        if (LoadShader("Shader2.vert",
                       "Shader2.frag",
                       false,
                       false,
                       true,
                       ShaderProgram[0],
                       VertexShader[0],
                       FragmentShader[0]) == -1)
        {
            ADD_FAILURE("LoadShader failed");
            return;
        }
        else
        {
            ProjectionModelviewMatrix_Loc[0]
                = glGetUniformLocation(ShaderProgram[0], "ProjectionModelviewMatrix");
        }

        CreateGeometry();

        CreateFBO();

        glut->glutDisplayFunc(display);
        glut->glutReshapeFunc(reshape);

        glut->glutMainLoop();
    }
}

#endif
