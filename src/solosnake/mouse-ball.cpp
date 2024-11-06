#include <algorithm>
#include <cassert>
#include <cmath>
#include "solosnake/mouse-ball.hpp"

// Rotation matrix from axis-angle:
// See https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
//
// Modified Gram-Schmidt:
// https://en.wikipedia.org/wiki/Gram%E2%80%93Schmidt_process

namespace solosnake
{
    namespace
    {
        /// Shorthand for the 16 float array.
        using Mat4 = std::array<float, 16>;

        /// Quick and dirty 3D vector struct.
        struct Vec3f { float x; float y; float z; };

        /// Returns the 4x4 identity matrix.
        constexpr Mat4 Identity() noexcept
        {
            return {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };
        }

        /// Return x squared.
        inline float square(const float x) noexcept
        {
            return x * x;
        }

        inline Vec3f operator - (const Vec3f& lhs, const Vec3f& rhs) noexcept
        {
            return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
        }

        inline Vec3f operator * (const Vec3f& lhs, const float rhs) noexcept
        {
            return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
        }

        /// Returns (x,y,z) as a unit length vector. Uses assert (debug only)
        /// to warn about possible zero (or near zero) length vectors.
        Vec3f unit3f(const float x, const float y, const float z) noexcept
        {
            const auto L = std::sqrt(square(x) + square(y) + square(z) );
            assert(L > 0.0001);
            return { x/L, y/L, z/L };
        }

        inline Vec3f normalize(const Vec3f& v) noexcept
        {
            return unit3f(v.x, v.y, v.z);
        }

        /// Returns the dot product of the two vectors a and b.
        /// a.b = |a||b|Cos(A)
        inline float dot(const Vec3f& a, const Vec3f& b) noexcept
        {
            return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
        }

        /// Returns the right-hand-rule unit length vector orthogonal
        /// to the vectors a and b.
        inline Vec3f cross(const Vec3f& a, const Vec3f& b) noexcept
        {
            return unit3f( a.y * b.z - a.z * b.y,
                           a.z * b.x - a.x * b.z,
                           a.x * b.y - a.y * b.x );
        }

        /// Modifed gram-schmidt ortho-normalises a 3D matrix in a numerically
        /// stable manner. Its expensive, but it helps ensure that we do not
        /// accumulate drift as we use the mouse to move the scene contents
        /// again and again.
        Mat4 modified_gram_schmidt(const Mat4& m)
        {
            Vec3f c0 = unit3f( m[0], m[4], m[8] );
            Vec3f c1{ m[1], m[5], m[9] };
            Vec3f c2{ m[2], m[6], m[10] };
            c1 = normalize( c1 - c0 * dot(c1, c0) );
            c2 = c2 - c0 * dot(c2, c0);
            c2 = c2 - c1 * dot(c2, c1);
            c2 = normalize(c2);
            return {
                c0.x, c1.x, c2.x, 0.0f,
                c0.y, c1.y, c2.y, 0.0f,
                c0.z, c1.z, c2.z, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };
        }

        /// Returns the OpenGL compatible 4x4 matrix representing a
        /// rotation of A radians around the unit length axis (x,y,z).
        Mat4 rotation_mat4(const float x,
                           const float y,
                           const float z,
                           const float A) noexcept
        {
            const auto cA  = std::cos(A);
            const auto sA  = std::sin(A);
            const auto NcA = 1.0f - cA;

            return {
                (x * x * NcA) +       cA,  (y * x * NcA) - (z * sA),  (z * x * NcA) + (y * sA),  0.0f,
                (x * y * NcA) + (z * sA),  (y * y * NcA) +       cA,  (z * y * NcA) - (x * sA),  0.0f,
                (x * z * NcA) - (y * sA),  (y * z * NcA) + (x * sA),  (z * z * NcA) +       cA,  0.0f,
                                    0.0f,                      0.0f,                      0.0f,  1.0f
            };
        }

        /// Returns the OpenGL compatible 4x4 matrix representing a
        /// rotation of A radians around the unit length axis.
        inline std::array<float, 16> rotation_mat4(const Vec3f& axis,
                                                   const float A) noexcept
        {
            return rotation_mat4(axis.x, axis.y, axis.z, A);
        }
    }

    void Mouseball::set_window_size(const unsigned int width,
                                    const unsigned int height) noexcept
    {
        m_window_width  = width;
        m_window_height = height;
    }

    void Mouseball::start_drag(const unsigned int x0,
                               const unsigned int y0) noexcept
    {
        m_click_x0   = x0;
        m_click_y0   = y0;
        m_dragged_x1 = x0;
        m_dragged_y1 = y0;
        m_dragging   = true;
    }

    std::array<float, 16> Mouseball::get_drag_rotation() const noexcept
    {
        // Check for a drag, or no rotation, or zero area window.
        const bool calculate = m_dragging and
                               0u <  m_window_width  and
                               0u <  m_window_height and
                               m_click_x0   <= m_window_width and
                               m_click_y0   <= m_window_height and
                               m_dragged_x1 <= m_window_width and
                               m_dragged_y1 <= m_window_height and
                               (m_dragged_x1 != m_click_x0 or m_dragged_y1 != m_click_y0);

        if( calculate )
        {
            // Local calculation values:
            const auto click_x = m_click_x0;
            const auto drag_x  = m_dragged_x1;

            // Correct for window y being upside down.
            const auto click_y = m_window_height - m_click_y0;
            const auto drag_y  = m_window_height - m_dragged_y1;

            // Calculate the sphere radius based on distance to
            // a window corner from the centre of the screen.
            const auto half_w = static_cast<float>(m_window_width)  / 2.0f;
            const auto half_h = static_cast<float>(m_window_height) / 2.0f;

            // Clamp drag point to window bounds. Rotating stops when the
            // mouse leaves the window bounds.
            const auto dx1 = std::min( m_window_width,  drag_x );
            const auto dy1 = std::min( m_window_height, drag_y );

            // Transform points so that (0,0,0) is centre of screen.
            const auto ax = static_cast<float>(click_x) - half_w;
            const auto ay = static_cast<float>(click_y) - half_h;
            const auto bx = static_cast<float>(dx1) - half_w;
            const auto by = static_cast<float>(dy1) - half_h;

            // Calculate Rs, the square of the sphere's radius `R`,
            // based on the window width and height.
            const auto Rs = square(half_w) + square(half_h);

            // A sphere is x² + y² + z² = R²
            // Find az and bz by using z² = R² - x² - y²
            const auto az = std::sqrt( Rs - square(ax) - square(ay) );
            const auto bz = std::sqrt( Rs - square(bx) - square(by) );

            // Create unit vectors to points A (ax, ay, az) and B (bx, by, bz).
            const auto A = unit3f(ax, ay, az);
            const auto B = unit3f(bx, by, bz);

            // We now have two vectors, A and B, to points on sphere's surface.
            // Find the axis orthogonal to them using cross product.
            const auto axis = cross(B,A);

            // Find the angle between the two vectors.
            const auto angle = std::acos( dot(B,A) );

            // Transform the axis-angle to a 4x4 rotation matrix.
            return modified_gram_schmidt( rotation_mat4( axis, angle ) );
        }
        else
        {
            // Either cannot, or will-not, calculate the rotation.
            return Identity();
        }
    }
}
