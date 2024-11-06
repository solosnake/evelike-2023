#ifndef SOLOSNAKE_OPENGL_HPP
#define SOLOSNAKE_OPENGL_HPP

#include <cstdint>
#include <functional>

namespace solosnake
{
    /// Class to mirror OpenGL state to minimise state changes.
    /// Correct usage assumes only one state used.
    ///
    ///        +Y
    ///          |
    ///          |__ __ +X
    ///         /
    ///    +Z /
    ///
    ///
    /// OpenGL is right-handed: the positive x and y axes point right and up,
    /// and the negative z axis points forward. Positive rotation is counter
    /// -clockwise about the axis of rotation.
    class OpenGL
    {
    public:

        struct Viewport { int x, y, width, height; };

        enum class Mode : std::int32_t
        {
            TRIANGLES       = 0x0004,
            TRIANGLE_STRIP  = 0x0005
        };

        enum class ShaderType : std::uint32_t
        {
            FRAGMENT        = 0x8B30,
            VERTEX          = 0x8B31
        };

        enum Bitfield : std::int32_t
        {
            COLOR_BUFFER_BIT = 0x00004000,
            DEPTH_BUFFER_BIT = 0x00000100
        };

        enum class ProgramParam : std::uint32_t
        {
            LINK_STATUS = 0x8B82
        };

        enum class ShaderParam : std::uint32_t
        {
            COMPILE_STATUS  = 0x8B81,
            INFO_LOG_LENGTH = 0x8B84
        };

        enum class Target : std::uint32_t
        {
            ARRAY_BUFFER         = 0x8892,
            ELEMENT_ARRAY_BUFFER = 0x8893
        };

        enum class Usage : std::uint32_t
        {
            STATIC_DRAW = 0x88E4
        };

        enum class TextureTarget : std::uint32_t
        {
            TEXTURE0 = 0x84C0,
            TEXTURE1 = 0x84C1,
            TEXTURE2 = 0x84C2,
            TEXTURE3 = 0x84C3
        };

        enum class TextureType : std::uint32_t
        {
            TEXTURE_2D          = 0x0DE1,
            TEXTURE_CUBE_MAP    = 0x8513
        };

        enum class DataType : std::uint32_t
        {
            FLOAT               = 0x1406
        };

        enum class IndexType : std::uint32_t
        {
            UNSIGNED_SHORT      = 0x1403,
            UNSIGNED_INT        = 0x1405
        };

        enum class Blending : std::int32_t
        {
            ZERO                = 0x0000,
            ONE                 = 0x0001,
            SRC_ALPHA           = 0x0302,
            ONE_MINUS_SRC_ALPHA = 0x0303
        };

        enum class DepthAlgo : std::int32_t
        {
            LESS                = 0x0201,
            LEQUAL              = 0x0203,
            ALWAYS              = 0x0207
        };

        OpenGL();

        void ActiveTexture(TextureTarget) noexcept;

        void AttachShader(unsigned int program, unsigned int shader ) noexcept;

        void BindBuffer(Target, unsigned int buffer) noexcept;

        void BindTexture(TextureType type, unsigned int texture) noexcept;

        void BindVertexArray(unsigned int vao) noexcept;

        void BufferData(Target, std::ptrdiff_t size, const void * data, Usage) noexcept;

        void BlendFunc(Blending src, Blending dst) noexcept;

        void Clear( unsigned int flags ) noexcept;

        void ClearColor(float r, float g, float b, float a) noexcept;

        void CompileShader(unsigned int shader) noexcept;

        void CompileShader(const char* source, unsigned int shader);

        unsigned int CreateProgram() noexcept;

        unsigned int CreateShader(ShaderType) noexcept;

        void DeleteBuffers(int n, const unsigned int* buffers) noexcept;

        void DeleteProgram(unsigned int program) noexcept;

        void DeleteShader(unsigned int shader) noexcept;

        void DeleteTexture(unsigned int* texture) noexcept;

        void DeleteTextures(int n, const unsigned int* textures) noexcept;

        void DeleteVertexArrays(int n, const unsigned int* arrays) noexcept;

        void DepthFunc(DepthAlgo);

        void DisableDebugOutput() noexcept;

        void DrawArrays(Mode, int first, unsigned int count) noexcept;

        void DrawElements(Mode, unsigned int count, IndexType, const void * indices) noexcept;

        void DrawIndexedPoints(unsigned int point_count, const std::uint16_t* indices) noexcept;

        void DrawPointArrays(unsigned int point_count) noexcept;

        void EnableBackFaceCulling(bool) noexcept;

        void EnableBlending(bool) noexcept;

        void EnableDebugOutput() noexcept;

        void EnableDepthClamp(bool) noexcept;

        void EnableDepthWrite(bool) noexcept;

        void EnableDepthTest(bool) noexcept;

        void EnableLinePolygonMode(bool) noexcept;

        void EnableProgramPointSize(bool) noexcept;

        void EnableVertexAttribArray(unsigned int index) noexcept;

        void GenBuffers(int n, unsigned int* buffers) noexcept;

        void GenVertexArrays(int n, unsigned int* arrays) noexcept;

        int  GetError() noexcept;

        void GetProgramiv(unsigned int program, ProgramParam, int* params) noexcept;

        void GetShaderiv(unsigned int shader, ShaderParam, int* params) noexcept;

        void GetShaderInfoLog(unsigned int shader, int maxLength, int* length, char* infoLog) noexcept;

        int  GetUniformLocation(unsigned int program, const char* name) noexcept;

        Viewport GetViewport() noexcept;

        void LinkProgram( unsigned int program ) noexcept;

        void LinkShader(unsigned int shader);

        /// Creates an RGB(A) OpenGL texture from a BGR(A) source.
        /// @param texels_have_alpha_channel True to indicate the source texels
        ///        have an alpha channel (BGRA), false to indicate BGR.
        unsigned int MakeTexture(const void* bgr_texels,
                                 int width,
                                 int height,
                                 bool texels_have_alpha_channel,
                                 bool make_mip_maps);

        /// Creates an RGB OpenGL cubemap texture from 6 square BGR sources.
        /// The user must provide a callback function (which will not be stored
        /// and which will only be used during the scope of this function's
        /// execution) which returns each side's BGR pixel buffer. This callback
        /// will be called in the order POSITIVE_X, NEGATIVE_X, POSITIVE_Y,
        /// NEGATIVE_Y, POSITIVE_Z and NEGATIVE_Z (the OpenGL default order) and
        /// the user should return each side's respective BGR pixel buffer as a
        /// square of pixels of size `side_length` * `side_length`.
        /// Returns the OpenGL texture ID.
        unsigned int MakeCubeMapTexture(int side_length,
                                        std::function<const std::uint8_t*()> next_side_bgr);

        void SaveTGAScreenshot(const char* filename);

        void SetViewport(int x, int y, int width, int height) noexcept;

        void ShaderSource(unsigned int shader, int count, const char **texts, const int *length) noexcept;

        void ToggleLinePolygonMode() noexcept;

        void Uniform1i(int location, int v0) noexcept;

        void Uniform1f(int location, float value) noexcept;

        void Uniform4fv(int location, int count, const float* value) noexcept;

        void UniformMatrix4fv(int location, int count, bool transpose, const float* value) noexcept;

        void UseProgram(unsigned int program_object) noexcept;

        void VerifyNoError(const char* msg);

        void VertexAttribPointer(unsigned int index, int size, DataType,
                                 bool normalized, int stride, const void * pointer) noexcept;

    private:

        unsigned int            m_program_object;
        unsigned int            m_vertex_array_object;
        unsigned int            m_enabled_flags;
        int                     m_blend_src_rgb;
        int                     m_blend_src_alpha;
        int                     m_blend_dst_rgb;
        int                     m_blend_dst_alpha;
        int                     m_depth_func;
        float                   m_clear_color[4];
        bool                    m_depth_testing;
        bool                    m_depth_clamping;
        bool                    m_depth_writing;
        bool                    m_back_face_culling_enabled;
        bool                    m_wireframe;
        bool                    m_point_size;
        bool                    m_blending;
    };
}

#endif
