#version 420 core

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec4 vertex_tangent;
layout (location = 3) in vec2 vertex_uv;

// Model-view matrix. Transforms a model vertex to view space.
uniform mat4 model_view_matrix;

// Model-view-project transform. Transforms model to normalized screen coordinates.
uniform mat4 model_view_projection_matrix;

// Light position in view space.
uniform vec4 light_view_position;

// Debug setting: controls how the model is rendered.
uniform int debug_output;

out VS_DATA
{
    vec3  tbn_light_vector;      // Vector towards light in TBN space.
    vec3  tbn_eye_vector;        // Vector towards eye in TBN space.
    vec2  geometry_uv_coords;    // The geometry's texture coordinates.
    float distance_to_light;     // How far away light is.
} vs_data;

// Returns the transposed TBN matrix which will transform view-space vectors
// to TBN-space vectors.
mat3 inverse_tbn()
{
    // Gram-Schmidt: re-orthogonalize T with respect to N
    vec3 T1 = normalize(vec3(model_view_matrix * vec4(vertex_tangent.xyz, 0.0)));

    vec3 N = normalize(vec3(model_view_matrix * vec4(vertex_normal,      0.0)));
    vec3 T = normalize(T1 - dot(T1, N) * N);
    vec3 B = cross(N, T) * vertex_tangent.w;

    return transpose(mat3(T, B, N));
}

// The entry point for our vertex shader.
void main()
{
    // Position of vertex in view-space (negated, makes maths faster!)
    vec3 view_vector_to_vertex = -vec3(model_view_matrix * vertex_position);

    // Vector to light from position, in view-space.
    vec3 view_vector_to_light   = view_vector_to_vertex + light_view_position.xyz;

    // Matrix to transform from view to TBN space.
    mat3 invTBN = inverse_tbn();

    vs_data.geometry_uv_coords = vertex_uv;
    vs_data.tbn_eye_vector     = invTBN * normalize(view_vector_to_vertex);
    vs_data.tbn_light_vector   = invTBN * normalize(view_vector_to_light);
    vs_data.distance_to_light  = length(view_vector_to_light);

    // gl_Position is a special variable used to store the final position.
    gl_Position = model_view_projection_matrix * vertex_position;
}
