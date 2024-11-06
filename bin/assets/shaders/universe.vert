#version 450 core

layout (location=0) in vec4 vertex_xyz_radius;

uniform mat4    model_view_projection_matrix;
uniform float   height_of_near_plane;

const float point_scale = 100.0;

void main(void)
{
    gl_Position  = model_view_projection_matrix * vec4(vertex_xyz_radius.xyz, 1.0f);;
    gl_PointSize = (point_scale * height_of_near_plane * vertex_xyz_radius.w) / gl_Position.w;
}
