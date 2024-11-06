#include <cassert>
#include <cmath>
#include <limits>
#include "solosnake/view-projection.hpp"

namespace solosnake
{
    namespace
    {
        constexpr ViewProjection::Mat4x4 Identity() noexcept
        {
            return {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };
        }

        constexpr float Default_FOV() noexcept
        {
            constexpr double Pi{3.1415926535897932384626433};
            return static_cast<float>(0.5f * Pi);
        }

        inline float aspect_ratio(const unsigned int width, const unsigned int height) noexcept
        {
            return static_cast<float>(width) / static_cast<float>(height);
        }

        struct V3f {
            float x, y, z;
        };

        struct M4x4f {
            float m11, m21, m31, m41, //  0  1  2  3
                  m12, m22, m32, m42, //  4  5  6  7
                  m13, m23, m33, m43, //  8  9 10 11
                  m14, m24, m34, m44; // 12 13 14 15
        };

        ViewProjection::Mat4x4 convert(const M4x4f& m) noexcept
        {
            return ViewProjection::Mat4x4{
                m.m11, m.m21, m.m31, m.m41,
                m.m12, m.m22, m.m32, m.m42,
                m.m13, m.m23, m.m33, m.m43,
                m.m14, m.m24, m.m34, m.m44
            };
        }

        M4x4f convert(const ViewProjection::Mat4x4& m) noexcept
        {
            return M4x4f{
                 m[0],  m[1],  m[2],  m[3],
                 m[4],  m[5],  m[6],  m[7],
                 m[8],  m[9], m[10], m[11],
                m[12], m[12], m[14], m[15],
            };
        }

        /// Returns lhs (4x4) x rhs (4x4)
        /// In terms of perceived effect, rhs happens after lhs.
        ViewProjection::Mat4x4 multiply( const M4x4f* __restrict lhs,
                                         const M4x4f* __restrict rhs) noexcept
        {
            assert( lhs != rhs );
            return ViewProjection::Mat4x4 {
                lhs->m11 * rhs->m11 + lhs->m12 * rhs->m21 + lhs->m13 * rhs->m31 + lhs->m14 * rhs->m41,
                lhs->m21 * rhs->m11 + lhs->m22 * rhs->m21 + lhs->m23 * rhs->m31 + lhs->m24 * rhs->m41,
                lhs->m31 * rhs->m11 + lhs->m32 * rhs->m21 + lhs->m33 * rhs->m31 + lhs->m34 * rhs->m41,
                lhs->m41 * rhs->m11 + lhs->m42 * rhs->m21 + lhs->m43 * rhs->m31 + lhs->m44 * rhs->m41,

                lhs->m11 * rhs->m12 + lhs->m12 * rhs->m22 + lhs->m13 * rhs->m32 + lhs->m14 * rhs->m42,
                lhs->m21 * rhs->m12 + lhs->m22 * rhs->m22 + lhs->m23 * rhs->m32 + lhs->m24 * rhs->m42,
                lhs->m31 * rhs->m12 + lhs->m32 * rhs->m22 + lhs->m33 * rhs->m32 + lhs->m34 * rhs->m42,
                lhs->m41 * rhs->m12 + lhs->m42 * rhs->m22 + lhs->m43 * rhs->m32 + lhs->m44 * rhs->m42,

                lhs->m11 * rhs->m13 + lhs->m12 * rhs->m23 + lhs->m13 * rhs->m33 + lhs->m14 * rhs->m43,
                lhs->m21 * rhs->m13 + lhs->m22 * rhs->m23 + lhs->m23 * rhs->m33 + lhs->m24 * rhs->m43,
                lhs->m31 * rhs->m13 + lhs->m32 * rhs->m23 + lhs->m33 * rhs->m33 + lhs->m34 * rhs->m43,
                lhs->m41 * rhs->m13 + lhs->m42 * rhs->m23 + lhs->m43 * rhs->m33 + lhs->m44 * rhs->m43,

                lhs->m11 * rhs->m14 + lhs->m12 * rhs->m24 + lhs->m13 * rhs->m34 + lhs->m14 * rhs->m44,
                lhs->m21 * rhs->m14 + lhs->m22 * rhs->m24 + lhs->m23 * rhs->m34 + lhs->m24 * rhs->m44,
                lhs->m31 * rhs->m14 + lhs->m32 * rhs->m24 + lhs->m33 * rhs->m34 + lhs->m34 * rhs->m44,
                lhs->m41 * rhs->m14 + lhs->m42 * rhs->m24 + lhs->m43 * rhs->m34 + lhs->m44 * rhs->m44,
            };
        }

        // dst4x4 (4x4) = lhs (4x4) x rhs (4x4)
        // In terms of perceived effect, rhs happens after lhs.
        void multiply( const M4x4f* __restrict lhs,
                       const M4x4f* __restrict rhs,
                       M4x4f* __restrict dst4x4 ) noexcept
        {
            assert( lhs != rhs );
            assert( lhs != dst4x4 );
            dst4x4->m11 = lhs->m11 * rhs->m11 + lhs->m12 * rhs->m21 + lhs->m13 * rhs->m31 + lhs->m14 * rhs->m41;
            dst4x4->m21 = lhs->m21 * rhs->m11 + lhs->m22 * rhs->m21 + lhs->m23 * rhs->m31 + lhs->m24 * rhs->m41;
            dst4x4->m31 = lhs->m31 * rhs->m11 + lhs->m32 * rhs->m21 + lhs->m33 * rhs->m31 + lhs->m34 * rhs->m41;
            dst4x4->m41 = lhs->m41 * rhs->m11 + lhs->m42 * rhs->m21 + lhs->m43 * rhs->m31 + lhs->m44 * rhs->m41;

            dst4x4->m12 = lhs->m11 * rhs->m12 + lhs->m12 * rhs->m22 + lhs->m13 * rhs->m32 + lhs->m14 * rhs->m42;
            dst4x4->m22 = lhs->m21 * rhs->m12 + lhs->m22 * rhs->m22 + lhs->m23 * rhs->m32 + lhs->m24 * rhs->m42;
            dst4x4->m32 = lhs->m31 * rhs->m12 + lhs->m32 * rhs->m22 + lhs->m33 * rhs->m32 + lhs->m34 * rhs->m42;
            dst4x4->m42 = lhs->m41 * rhs->m12 + lhs->m42 * rhs->m22 + lhs->m43 * rhs->m32 + lhs->m44 * rhs->m42;

            dst4x4->m13 = lhs->m11 * rhs->m13 + lhs->m12 * rhs->m23 + lhs->m13 * rhs->m33 + lhs->m14 * rhs->m43;
            dst4x4->m23 = lhs->m21 * rhs->m13 + lhs->m22 * rhs->m23 + lhs->m23 * rhs->m33 + lhs->m24 * rhs->m43;
            dst4x4->m33 = lhs->m31 * rhs->m13 + lhs->m32 * rhs->m23 + lhs->m33 * rhs->m33 + lhs->m34 * rhs->m43;
            dst4x4->m43 = lhs->m41 * rhs->m13 + lhs->m42 * rhs->m23 + lhs->m43 * rhs->m33 + lhs->m44 * rhs->m43;

            dst4x4->m14 = lhs->m11 * rhs->m14 + lhs->m12 * rhs->m24 + lhs->m13 * rhs->m34 + lhs->m14 * rhs->m44;
            dst4x4->m24 = lhs->m21 * rhs->m14 + lhs->m22 * rhs->m24 + lhs->m23 * rhs->m34 + lhs->m24 * rhs->m44;
            dst4x4->m34 = lhs->m31 * rhs->m14 + lhs->m32 * rhs->m24 + lhs->m33 * rhs->m34 + lhs->m34 * rhs->m44;
            dst4x4->m44 = lhs->m41 * rhs->m14 + lhs->m42 * rhs->m24 + lhs->m43 * rhs->m34 + lhs->m44 * rhs->m44;
        }

        // fov_y:   Field of view angle, in RADIANS, in the y direction.
        // aspect:  Aspect ratio that determines the field of view in the x direction.
        //          The aspect ratio is the ratio of x (width) to y (height) = (W/H)
        // zNear:   The distance from the viewer to the near clipping plane (always positive).
        // zFar:    The distance from the viewer to the far clipping plane (always positive).
        void make_projection( const float fov_y,
                              const float aspect,
                              const float zNear,
                              const float zFar,
                              M4x4f& dst4x4 ) noexcept
        {
            assert( zNear > 0.0f );
            assert( zFar > 0.0f );
            assert( std::abs( aspect ) > std::numeric_limits<float>::epsilon() );
            assert( fov_y > std::numeric_limits<float>::epsilon() );

            dst4x4.m22 = 1.0f / std::tan( fov_y * 0.5f );

            dst4x4.m11 = dst4x4.m22 / aspect;
            dst4x4.m21 = 0.0f;
            dst4x4.m31 = 0.0f;
            dst4x4.m41 = 0.0f;

            dst4x4.m12 = 0.0f;
            dst4x4.m32 = 0.0f;
            dst4x4.m42 = 0.0f;

            dst4x4.m13 = 0.0f;
            dst4x4.m23 = 0.0f;
            dst4x4.m33 = ( zFar + zNear ) / ( zNear - zFar );
            dst4x4.m43 = -1.0f;

            dst4x4.m14 = 0.0f;
            dst4x4.m24 = 0.0f;
            dst4x4.m34 = ( 2.0f * zFar * zNear ) / ( zNear - zFar );
            dst4x4.m44 = 0.0f;
        }

        void make_view_matrix( const V3f* __restrict look_at,
                               const V3f* __restrict eye_pos,
                               const V3f* __restrict up_vec,
                               M4x4f* __restrict view4x4 ) noexcept
        {
            V3f forward { look_at->x - eye_pos->x,
                          look_at->y - eye_pos->y,
                          look_at->z - eye_pos->z };

            // Normalise `forward`
            const auto f = std::sqrt( ( forward.x * forward.x )
                                    + ( forward.y * forward.y )
                                    + ( forward.z * forward.z ) );
            assert( f > 0.0001f );
            forward.x /= f;
            forward.y /= f;
            forward.z /= f;

            // Side = forward x up (cross product).
            V3f side { forward.y * up_vec->z - forward.z * up_vec->y,
                       forward.z * up_vec->x - forward.x * up_vec->z, // NOTE -( x * op1->z - z * op1->x )
                       forward.x * up_vec->y - forward.y * up_vec->x };

            // Normalise `side`
            const auto g = std::sqrt( ( side.x * side.x )
                                    + ( side.y * side.y )
                                    + ( side.z * side.z ) );
            assert( g > 0.0001f );
            side.x /= g;
            side.y /= g;
            side.z /= g;

            // Recompute up as: up2 = side x forward (cross product).
            V3f up2 { side.y * forward.z - side.z * forward.y,
                      side.z * forward.x - side.x * forward.z, // NOTE -( x * op1->z - z * op1->x )
                      side.x * forward.y - side.y * forward.x };

            // Normalise `up2` (it should be close to unit length already).
            const auto h = std::sqrt( ( up2.x * up2.x )
                                    + ( up2.y * up2.y )
                                    + ( up2.z * up2.z ) );
            assert( h > 0.0001f );
            up2.x /= h;
            up2.y /= h;
            up2.z /= h;

            M4x4f dst4x4 { side.x,  up2.x, -forward.x, 0.0f,
                           side.y,  up2.y, -forward.y, 0.0f,
                           side.z,  up2.z, -forward.z, 0.0f,
                              0.0,   0.0f,       0.0f, 1.0f };

            M4x4f trans {        1.0f,        0.0f,        0.0f, 0.0f,
                                 0.0f,        1.0f,        0.0f, 0.0f,
                                 0.0f,        0.0f,        1.0f, 0.0f,
                          -eye_pos->x, -eye_pos->y, -eye_pos->z, 1.0f };

            multiply( &dst4x4, &trans, view4x4 );
        }
    }

    //--------------------------------------------------------------------------

    ViewProjection::ViewProjection()
    : m_view(Identity())
    , m_projection(Identity())
    , m_view_projection(Identity())
    , m_camera_position{0.0f, 0.0f, 1.0f}
    , m_camera_look_at{0.0f, 0.0f, 0.0f}
    , m_viewport_width{1024u}
    , m_viewport_height{768u}
    , m_field_of_view_radians{Default_FOV()}
    , m_z_near{0.1f}
    , m_z_far{1000.0f}
    {
    }

    float ViewProjection::get_field_of_view_degrees() const noexcept
    {
        constexpr double Pi{3.1415926535897932384626433};
        return static_cast<float>(m_field_of_view_radians * 180.0 / Pi);
    }

    void ViewProjection::set_field_of_view_degrees(const float fov) noexcept
    {
        constexpr double Pi{3.1415926535897932384626433};
        set_field_of_view_radians( static_cast<float>(fov * Pi / 180.0) );
    }


    ViewProjection::Mat4x4 ViewProjection::get_model_view_projection(const Mat4x4& model) const noexcept
    {
        auto model_4x4  = convert(model);
        auto vp         = convert(m_view_projection);
        return multiply(&vp, &model_4x4);
    }

    ViewProjection::Mat4x4 ViewProjection::get_model_view_projection(const float* model) const noexcept
    {
        auto model_4x4  = M4x4f{
             model[0],  model[1],  model[2],  model[3],
             model[4],  model[5],  model[6],  model[7],
             model[8],  model[9], model[10], model[11],
            model[12], model[13], model[14], model[15],
        };
        auto vp = convert(m_view_projection);
        return multiply(&vp, &model_4x4);
    }

    ViewProjection::Mat4x4 ViewProjection::get_model_view(const Mat4x4& model) const noexcept
    {

        auto model_4x4  = convert(model);
        auto v = convert(m_view);
        return multiply(&v, &model_4x4);
    }

    ViewProjection::Mat4x4 ViewProjection::get_model_view(const float* model) const noexcept
    {
        auto model_4x4  = M4x4f{
             model[0],  model[1],  model[2],  model[3],
             model[4],  model[5],  model[6],  model[7],
             model[8],  model[9], model[10], model[11],
            model[12], model[13], model[14], model[15],
        };
        auto v = convert(m_view);
        return multiply(&v, &model_4x4);
    }

    void ViewProjection::update()
    {
        assert( m_z_near < m_z_far );
        assert( m_field_of_view_radians > 0.0f );

        V3f up      { 0.0f, 1.0f, 0.0f };
        V3f look_at { m_camera_look_at[0],  m_camera_look_at[1],  m_camera_look_at[2] };
        V3f position{ m_camera_position[0], m_camera_position[1], m_camera_position[2] };
        M4x4f view;
        M4x4f projection;

        make_view_matrix( &look_at, &position, &up, &view );

        make_projection(m_field_of_view_radians,
                         aspect_ratio(m_viewport_width, m_viewport_height),
                         m_z_near,
                         m_z_far,
                         projection);

        m_view            = convert(view);
        m_projection      = convert(projection);
        m_view_projection = multiply(&projection, &view);
    }

    float ViewProjection::get_near_plane_height() const noexcept
    {
        return static_cast<float>(m_viewport_height) / (2.0f * std::tan(0.5f * m_field_of_view_radians));
    }
}