#include "solosnake/datadirectory.hpp"
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/load_meshes.hpp"
#include "solosnake/foreachtriangle.hpp"
#include "solosnake/image.hpp"
#include "solosnake/point.hpp"

using namespace solosnake;

namespace
{

void get_tri_normal(const for_each_triangle::triangle& tri, float* n)
{
    float v0[3] = { tri.verts[1].x - tri.verts[0].x, tri.verts[1].y - tri.verts[0].y,
                    tri.verts[1].z - tri.verts[0].z };

    float v1[3] = { tri.verts[2].x - tri.verts[0].x, tri.verts[2].y - tri.verts[0].y,
                    tri.verts[2].z - tri.verts[0].z };

    cross3(v0, v1, n);
    normalise3(n);
}

bgr normal_to_colour(const float* n)
{
    return bgr(static_cast<unsigned char>((n[2] + 1.0f) * 127.5f), // Blue is  Z
               static_cast<unsigned char>((n[1] + 1.0f) * 127.5f), // Green is Y
               static_cast<unsigned char>((n[0] + 1.0f) * 127.5f)  // Red is   X
               );
}

bgr get_normal_colour(const for_each_triangle::triangle& tri)
{
    float n[3];
    get_tri_normal(tri, n);
    return normal_to_colour(n);
}
}

void generate_model_normal_tex(unsigned char material)
{
    auto assetsDir = std::make_shared<datadirectory>(BLUEDATADIR "/meshes");
    auto url = assetsDir->get_file("thruster.msh");
    auto meshes = load_mesh_datas(url, assetsDir);
    auto tex = meshes.at(0)->get_meshgeometrytexinfo();

    image normals(tex->tex_normals_and_emiss()->img_width(),
                  tex->tex_normals_and_emiss()->img_height(),
                  solosnake::image::format_bgra);

    auto f = [&](const imeshgeometry&, const for_each_triangle::triangle& tri)
    {
        const bgra normalColour(get_normal_colour(tri), material);

        // Only two channels of the normal are used by the renderer,
        // and the third is reconstructed in the shader. The 3rd and 4th
        // channels are used as emissive on/off channels.

        // Omit the B channel (always positive) and store only the
        // G and R channels.
        bgra normlpix(normalColour.green(), normalColour.red(), 0, 0);

        normals.draw_triangle(tri.verts[0].u,
                              tri.verts[0].v,
                              normlpix,
                              tri.verts[1].u,
                              tri.verts[1].v,
                              normlpix,
                              tri.verts[2].u,
                              tri.verts[2].v,
                              normlpix);
    };

    for_each_triangle::iterate(*meshes.at(0)->get_meshgeometry(), *tex, f);

    normals.save(BLUEDATADIR "/meshes/gui.bmp");
}