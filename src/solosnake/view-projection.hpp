#ifndef SOLOSNAKE_VIEW_PROJECTION_HPP
#define SOLOSNAKE_VIEW_PROJECTION_HPP

#include <array>

namespace solosnake
{
    /// Class representing the view (the user/camera location in the scene and
    /// what it is looking at, and the projection (a 3D perspective transform
    /// incorporating the viewport size, the field of view, and the near and far
    /// planes.
    ///
    /// Changing various settings in the `ViewProjection` have no effect on the
    /// generated matrices until the `update` method is called.
    ///
    /// Remember to set the viewport dimensions correctly before using.
    class ViewProjection
    {
    public:

        using Mat4x4 = std::array<float,16>;
        using Vec3   = std::array<float,3>;

        ViewProjection();

        void  update();

        void  set_viewport(unsigned int width, unsigned int height) noexcept;

        void  set_field_of_view_degrees(float) noexcept;

        void  set_field_of_view_radians(float) noexcept;

        void  set_z_near_far(float, float) noexcept;

        void  set_z_far(float) noexcept;

        void  set_camera_position(const float*) noexcept;

        void  set_camera_look_at(const float*) noexcept;

        void  set_camera_position(float x, float y, float z) noexcept;

        void  set_camera_look_at(float x, float y, float z) noexcept;

        void  translate_camera_position(float dx, float dy, float dz) noexcept;

        void  translate_camera_look_at(float dx, float dy, float dz) noexcept;

        float get_field_of_view_degrees() const noexcept;

        float get_field_of_view_radians() const noexcept;

        float get_near_plane_height() const noexcept;

        float get_z_near() const noexcept;

        float get_z_far() const noexcept;

        Vec3  get_camera_position() const noexcept;

        Vec3  get_camera_look_at() const noexcept;

        Mat4x4 get_model_view(const Mat4x4& model) const noexcept;

        Mat4x4 get_model_view(const float* model) const noexcept;

        Mat4x4 get_model_view_projection(const Mat4x4& model) const noexcept;

        Mat4x4 get_model_view_projection(const float* model) const noexcept;

        const float* view() const noexcept;

        const float* projection() const noexcept;

        const float* view_projection() const noexcept;

    private:

        Mat4x4          m_view;
        Mat4x4          m_projection;
        Mat4x4          m_view_projection;
        Vec3            m_camera_position;
        Vec3            m_camera_look_at;
        unsigned int    m_viewport_width;
        unsigned int    m_viewport_height;
        float           m_field_of_view_radians;
        float           m_z_near;
        float           m_z_far;
    };


//---------------------------------- inlines -----------------------------------

inline void ViewProjection::set_viewport(const unsigned int width,
                                         const unsigned int height) noexcept
{
    m_viewport_width  = width;
    m_viewport_height = height;
}

inline void ViewProjection::set_z_near_far(const float z_near,
                                           const float z_far) noexcept
{
    m_z_near = z_near;
    m_z_far  = z_far;
}

inline void ViewProjection::set_z_far(const float z_far) noexcept
{
    m_z_far = z_far;
}

inline void ViewProjection::set_camera_position(const float* p) noexcept
{
    set_camera_position( p[0], p[1], p[2] );
}

inline void ViewProjection::set_camera_look_at(const float* p) noexcept
{
    set_camera_look_at( p[0], p[1], p[2] );
}

inline void ViewProjection::set_camera_position(const float x,
                                                const float y,
                                                const float z) noexcept
{
    m_camera_position = Vec3{x,y,z};
}

inline void ViewProjection::set_camera_look_at(const float x,
                                               const float y,
                                               const float z) noexcept
{
    m_camera_look_at = Vec3{x,y,z};
}

inline void ViewProjection::translate_camera_position(const float dx,
                                                      const float dy,
                                                      const float dz) noexcept
{
    m_camera_position[0] += dx;
    m_camera_position[1] += dy;
    m_camera_position[2] += dz;
}

inline void ViewProjection::translate_camera_look_at(const float dx,
                                                     const float dy,
                                                     const float dz) noexcept
{
    m_camera_look_at[0] += dx;
    m_camera_look_at[1] += dy;
    m_camera_look_at[2] += dz;
}

inline float ViewProjection::get_field_of_view_radians() const noexcept
{
    return m_field_of_view_radians;
}

inline float ViewProjection::get_z_near() const noexcept
{
    return m_z_near;
}

inline float ViewProjection::get_z_far() const noexcept
{
    return m_z_far;
}

inline ViewProjection::Vec3 ViewProjection::get_camera_position() const noexcept
{
    return m_camera_position;
}

inline ViewProjection::Vec3 ViewProjection::get_camera_look_at() const noexcept
{
    return m_camera_look_at;
}

inline void ViewProjection::set_field_of_view_radians(const float fov) noexcept
{
    m_field_of_view_radians = fov;
}

inline const float* ViewProjection::projection() const noexcept
{
    return m_projection.data();
}

inline const float* ViewProjection::view() const noexcept
{
    return m_view.data();
}

inline const float* ViewProjection::view_projection() const noexcept
{
    return m_view_projection.data();
}

} // solosnake
#endif