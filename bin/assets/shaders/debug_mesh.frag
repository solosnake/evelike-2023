#version 420 core

// Light position in view space.
uniform vec4 light_view_position;

// Debug setting: controls how the model is rendered.
uniform int debug_output;

// Textures.
layout(binding = 0) uniform sampler2D base_texture;
layout(binding = 1) uniform sampler2D normals_texture;
layout(binding = 2) uniform sampler2D orm_texture;
layout(binding = 3) uniform sampler2D emissive_team_texture;

// Interpolate data from vertex shader.
in VS_DATA
{
    vec3  tbn_light_vector;      // Vector towards light in TBN space.
    vec3  tbn_eye_vector;        // Vector towards eye in TBN space.
    vec2  geometry_uv_coords;    // The geometry's texture coordinates.
    float distance_to_light;     // How far away light is.
} vs_data;

// Final fragment color.
out vec4 final_rgba;

// The entry point for our fragment shader.
void main()
{
    vec3 base_rgb           = texture(base_texture,          vs_data.geometry_uv_coords).rgb;
    vec3 normals_rgb        = texture(normals_texture,       vs_data.geometry_uv_coords).rgb;
    vec3 orm_rgb            = texture(orm_texture,           vs_data.geometry_uv_coords).rgb;
    vec4 emissive_team_rgba = texture(emissive_team_texture, vs_data.geometry_uv_coords);

    const vec3  kRimColor = vec3(1.0, 0.0, 0.0);
    const vec3  kLightColor  = vec3(1.0, 1.0, 1.0);
    const float kPi = 3.14159265;
    const float kShinyFactor = 24.0;
    const float kRimLightPower = 4.0f;

    float occlusion = orm_rgb.r;
    float roughness = orm_rgb.g;
    float shininess = orm_rgb.b * kShinyFactor;

    // Normal (TBN space)
    vec3 normal = normalize(normals_rgb * 2.0 - 1.0);

    // Light direction (TBN space)
    vec3 light_dir = normalize(vs_data.tbn_light_vector);

    // View direction (TBN space)
    vec3 view_dir = normalize(vs_data.tbn_eye_vector);

    // Diffuse
    float diffuse = max(dot(normal, light_dir), 0.0);

    // Rim
    float rim     = pow(smoothstep(0.0, 1.0, 1.0 - dot(normal, view_dir)), kRimLightPower);

    // Specular (Blinn)
    vec3 halfway_dir = normalize(view_dir + light_dir);
    float energy_conservation = ( 8.0 + shininess ) / ( 8.0 * kPi );
    float spec = energy_conservation * pow(max(dot(normal, halfway_dir), 0.0), shininess);

    // Attenuation
    float max_light_effect = light_view_position.w;
    float light_effect = max(0.0, max_light_effect - vs_data.distance_to_light);

    // Lighting combinations
    vec3 spec_lighting     = kLightColor * spec * (1.0 - roughness);
    vec3 diffuse_lighting  = kLightColor * diffuse * base_rgb;
    vec3 combined_lighting = spec_lighting + diffuse_lighting;
    vec3 total_lighting    = light_effect * occlusion * combined_lighting;
    vec3 rim_lighting      = total_lighting * kRimColor * vec3(rim);
    vec3 total_and_rim     = total_lighting + rim_lighting;

    // Final color.
    final_rgba = vec4(total_and_rim, 1.0);

    // Ensure range of debug_output matches range in code.
    switch(debug_output)
    {
        default:
        case 0:
        break;

        case 1:
        // Base Texture
        final_rgba.rgb = base_rgb;
        break;

        case 2:
        // Normals texture
        final_rgba.rgb = normals_rgb;
        break;

        case 3:
        // Occlusion texture
        final_rgba.rgb = vec3(occlusion);
        break;

        case 4:
        // Roughness (lack of roughness)
        final_rgba.rgb = vec3(1-roughness);
        break;

        case 5:
        // Metallic
        final_rgba.rgb = vec3(orm_rgb.b);
        break;

        case 6:
        // emissive rgb
        final_rgba.rgb = emissive_team_rgba.rgb;
        break;

        case 7:
        // Team
        final_rgba.rgb = vec3(emissive_team_rgba.w);
        break;

        case 8:
        // Show diffuse contribution
        final_rgba.rgb = vec3(diffuse, diffuse, diffuse);
        break;

        case 9:
        // Rim lighting contribution
        final_rgba.rgb = vec3(rim);
        break;

        case 10:
        // Specular contribution
        final_rgba.rgb = vec3(spec);
        break;

        case 11:
        // Attenuation
        final_rgba.rgb = vec3(light_effect);
        break;

        case 12:
        final_rgba.rgb = spec_lighting;
        break;

        case 13:
        final_rgba.rgb = diffuse_lighting;
        break;

        case 14:
        final_rgba.rgb = total_lighting;
        break;

        case 15:
        final_rgba.rgb = rim_lighting;
        break;

        case 16:
        final_rgba.rgb = total_and_rim;
        break;

        case 17:
        // Show light direction vector. Hopefully smooth.
        final_rgba.rgb = 0.5 * (vec3(1) + normalize(vs_data.tbn_light_vector));
        break;
    }
}