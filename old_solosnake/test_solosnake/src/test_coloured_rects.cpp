#define RUN_TESTS
#ifdef RUN_TESTS

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "solosnake/testing/testing.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/image.hpp"
#include "solosnake/colour.hpp"
#include "solosnake/glutwindow.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/point.hpp"
#include "solosnake/image.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/load_models.hpp"
#include "solosnake/foreachtriangle.hpp"
#include "solosnake/img.hpp"
#include "solosnake/font.hpp"
#include "solosnake/fontprinter.hpp"
#include "solosnake/utf8text.hpp"
#include "solosnake/img_cache.hpp"
#include "solosnake/itextureloader.hpp"

using namespace solosnake;

#define WINDOW_W 1024
#define WINDOW_H 768
// Lets say 100fps = 20s
#define MAX_TEST_LOOPS (100 * 20)

namespace
{
std::shared_ptr<deferred_renderer> g_rr;
fontprinter_ptr g_printer;
std::shared_ptr<imesh> g_msrc;
std::string g_fontname = "basic24.xml";
deferred_renderer::meshhandle g_h = 0;
deferred_renderer::texturehandle g_t1 = 0;
deferred_renderer::texturehandle g_white = 0;
bool g_stop = false;
size_t g_loopcounter = 0;
unsigned int g_window_width = WINDOW_W;
unsigned int g_window_height = WINDOW_H;
matrix4x4_t g_mv;
matrix4x4_t g_proj;

class quickloader : public itextureloader
{
public:
    quickloader(std::shared_ptr<deferred_renderer> g_rr, std::shared_ptr<iimg_cache> imgs)
        : renderer_(g_rr), images_(imgs)
    {
    }

    virtual deferred_renderer::texturehandle load_texture(const std::string& texname)
    {
        return renderer_->load_texture(images_->get_img_src(texname));
    }

private:
    std::shared_ptr<deferred_renderer> renderer_;
    std::shared_ptr<iimg_cache> images_;
};

class modelsrc : public imesh
{
public:
    modelsrc(std::shared_ptr<meshgeometry> g, std::shared_ptr<imeshtextureinfo> t)
        : geometry_(g), texInfo_(t)
    {
    }
    virtual std::string get_mesh_name() const
    {
        return geometry_->name();
    }
    virtual std::shared_ptr<meshgeometry> get_meshgeometry()
    {
        return geometry_;
    }
    virtual std::shared_ptr<imeshtextureinfo> get_meshgeometrytexinfo()
    {
        return texInfo_;
    }

private:
    std::shared_ptr<meshgeometry> geometry_;
    std::shared_ptr<imeshtextureinfo> texInfo_;
};

//////////////////////////////////////////////////////////////////////////

void create_quad_tex()
{
    const size_t w = 128;
    const size_t g_h = 128;

    solosnake::image img1(w, g_h, solosnake::image::format_bgra);

    solosnake::image::clear_layer_to(img1, 0, 0xFF);
    solosnake::image::clear_layer_to(img1, 1, 0x00);
    solosnake::image::clear_layer_to(img1, 2, 0x00);
    solosnake::image::clear_layer_to(img1, 3, 0x40);

    img1.save(BLUEDATADIR "/test_assets/quad2.bmp");
}

void create_white_quad_tex()
{
    const size_t w = 128;
    const size_t g_h = 128;

    solosnake::image img1(w, g_h, solosnake::image::format_bgra);

    solosnake::image::clear_layer_to(img1, 0, 0xFF);
    solosnake::image::clear_layer_to(img1, 1, 0xFF);
    solosnake::image::clear_layer_to(img1, 2, 0xFF);
    solosnake::image::clear_layer_to(img1, 3, 0xFF);

    img1.save(BLUEDATADIR "/test_assets/white.bmp");
}

void create_dynbuffer_tex()
{
    const size_t w = 128;
    const size_t g_h = 128;

    solosnake::image img1(w, g_h, solosnake::image::format_bgra);

    solosnake::image::clear_layer_to(img1, 0, 0x0F);
    solosnake::image::clear_layer_to(img1, 1, 0x00);
    solosnake::image::clear_layer_to(img1, 2, 0xF0);
    solosnake::image::clear_layer_to(img1, 3, 0x40);

    img1.save(BLUEDATADIR "/test_assets/dynbuffertex2.bmp");
}

void get_tri_normal(const for_each_triangle::triangle& tri, float* n)
{
    float v0[3] = { tri.verts[1].x - tri.verts[0].x, tri.verts[1].y - tri.verts[0].y,
                    tri.verts[1].z - tri.verts[0].z };

    float v1[3] = { tri.verts[2].x - tri.verts[0].x, tri.verts[2].y - tri.verts[0].y,
                    tri.verts[2].z - tri.verts[0].z };

    cross3(v0, v1, n);
    normalise3(n);
}

solosnake::bgr normal_to_colour(const float* n)
{
    return solosnake::bgr(static_cast<unsigned char>((n[2] + 1.0f) * 127.5f), // Blue is  Z
                          static_cast<unsigned char>((n[1] + 1.0f) * 127.5f), // Green is Y
                          static_cast<unsigned char>((n[0] + 1.0f) * 127.5f)  // Red is   X
                          );
}

solosnake::bgr get_normal_colour(const for_each_triangle::triangle& tri)
{
    float n[3];
    get_tri_normal(tri, n);
    return normal_to_colour(n);
}

void generate_model_normal_tex(unsigned char material)
{
    solosnake::datadirectory assetsDir(BLUEDATADIR "/test_assets");

    auto url = assetsDir.get_file("cube.lua");
    auto meshes = load_meshes(url);
    auto texinfos = load_texinfos(url, assetsDir);
    auto tex = texinfos.at(0);

    solosnake::image normals(tex->tex_normals_and_spec()->img_width(),
                             tex->tex_normals_and_spec()->img_height(),
                             solosnake::image::format_bgra);

    auto f = [&](const meshgeometry&, const for_each_triangle::triangle& tri)
    {
        const bgra normalColour(get_normal_colour(tri), material);

        normals.draw_triangle(tri.verts[0].u,
                              tri.verts[0].v,
                              normalColour,
                              tri.verts[1].u,
                              tri.verts[1].v,
                              normalColour,
                              tri.verts[2].u,
                              tri.verts[2].v,
                              normalColour);
    };

    for_each_triangle::iterate(*meshes.at(0), *tex, f);

    normals.save(BLUEDATADIR "/test_assets/gui.bmp");
}

void init()
{
    const unsigned char material = 128;

    generate_model_normal_tex(material);

    auto assetsDir = std::make_shared<datadirectory>(BLUEDATADIR "/test_assets");

    auto url = assetsDir->get_file("cube.lua");
    auto meshes = load_meshes(url);
    auto texinfos = load_texinfos(url, *assetsDir);

    EXPECT_TRUE(meshes.size() == 1);
    EXPECT_TRUE(texinfos.size() == 1);

    g_msrc.reset(new modelsrc(meshes[0], texinfos[0]));

    g_rr = deferred_renderer::create(
        WINDOW_W, WINDOW_H, deferred_renderer::HighQuality); // 1920 x 1080
    g_h = g_rr->load_mesh(g_msrc);

    // Quads
    {
        create_quad_tex();
        auto tex1 = assetsDir->get_file("quad2.bmp");
        g_t1 = g_rr->load_texture(std::make_shared<solosnake::img::src>(tex1));

        create_white_quad_tex();
        auto white = assetsDir->get_file("white.bmp");
        g_white = g_rr->load_texture(std::make_shared<solosnake::img::src>(white));
    }

    // Font g_printer
    {
        datadirectory fontfiles(BLUEDATADIR "/fonts");

        auto fontURL = fontfiles.get_file(g_fontname);
        auto texDir = std::make_shared<datadirectory>(BLUEDATADIR "/textures");
        auto images = std::make_shared<img_cache>(texDir);
        auto texloader = std::make_shared<quickloader>(g_rr, images);
        auto fnt = std::make_shared<font>(fontURL, *texloader);

        g_printer = std::make_shared
            <fontprinter>(dimension2d<unsigned int>(WINDOW_W, WINDOW_H), g_rr, fnt);
    }
}

// Converts a float in the range -1 to +1 to range 0 to 1
float range_to_0_1(float f)
{
    assert(((f * 0.5f) + 0.5f) <= 1.0f);
    assert(((f * 0.5f) + 0.5f) >= 0.0f);
    return (f * 0.5f) + 0.5f;
}

// Display 1 stationary cube and 2 nearby rotating cubes.
void displayFn()
{
    static float cameraDistance = 10.0f;
    static float cameraY = 2.50f;
    static float change = -0.01f;

    static float varier = -1.0f;
    static float varyingAlpha = 1.0f;
    static float dv = 0.01f;

    varier += dv;
    varyingAlpha *= 0.98f;

    if (varyingAlpha < 0.05f)
    {
        varyingAlpha = 1.0f;
    }

    if (varier < -1.0f)
    {
        dv = 0.001f;
        varier = -1.0f;
    }
    else if (varier > 1.0f)
    {
        dv = -0.001f;
        varier = 1.0f;
    }

    static const size_t grid = 5;
    static const size_t N = grid * grid;

    const float spacing = 2.50f;

    cameraDistance += change;

    if (cameraDistance < (2) || cameraDistance > (grid + grid * spacing))
    {
        change = -change;
    }

    // Model view - "camera".
    load_translation(0.0f, -cameraY, -cameraDistance, g_mv);

    // 9 cubes of radius 2, in a grid, spaced 4 units apart, hard-coded.

    // Our cube spans from 1,1,1 to -1,-1,-1. Find the sphere that fits it
    // perfectly:
    float corner[3] = { 1, 1, 1 };

    deferred_renderer::point_light light[N];
    deferred_renderer::meshinstancedata cubes[N];
    memset(cubes, 0, N * sizeof(deferred_renderer::meshinstancedata));

    const float colours[N][3] = { { 1.0f, 0.0f, 0.0f },
                                  { 0.0f, 1.0f, 0.0f },
                                  { 0.0f, 0.0f, 1.0f },
                                  { 0.0f, 1.0f, 0.0f },
                                  { 0.0f, 0.0f, 1.0f },
                                  { 1.0f, 0.0f, 0.0f },
                                  { 0.0f, 1.0f, 0.0f },
                                  { 0.0f, 0.0f, 1.0f },
                                  { 0.0f, 1.0f, 0.0f },
                                  { 0.0f, 0.0f, 1.0f },
                                  { 1.0f, 0.0f, 0.0f },
                                  { 0.0f, 1.0f, 0.0f },
                                  { 0.0f, 0.0f, 1.0f },
                                  { 0.0f, 1.0f, 0.0f },
                                  { 0.0f, 0.0f, 1.0f },
                                  { 1.0f, 0.0f, 0.0f },
                                  { 0.0f, 1.0f, 0.0f },
                                  { 0.0f, 0.0f, 1.0f },
                                  { 0.0f, 1.0f, 0.0f },
                                  { 0.0f, 0.0f, 1.0f },
                                  { 1.0f, 0.0f, 0.0f },
                                  { 0.0f, 1.0f, 0.0f },
                                  { 0.0f, 0.0f, 1.0f },
                                  { 0.0f, 1.0f, 0.0f },
                                  { 0.0f, 0.0f, 1.0f }, };

    float start = grid * spacing * 0.5f;

    // Initialise model instance data:
    for (size_t i = 0; i < grid; ++i)
    {
        for (size_t j = 0; j < grid; ++j)
        {
            auto ij = grid * j + i;

            load_translation(-start + i * spacing, 0.0f, -start + j * spacing, cubes[ij].transform);

            cubes[ij].rgbTeam[0] = static_cast<float>((i + 1) % 2);
            cubes[ij].rgbTeam[1] = static_cast<float>((j + 1) % 2);
            cubes[ij].rgbTeam[2] = static_cast<float>((i + j + 1) % 2);

            light[ij].worldX = -start + i * spacing;
            light[ij].worldY = 0.0f + varier * 2.0f;
            light[ij].worldZ = -start + j * spacing;

            light[ij].radius = 2.0f;

            if (i == 1)
            {
                // light[ij].radius *= 1;
            }

            light[ij].red = colours[ij][0];
            light[ij].green = colours[ij][1];
            light[ij].blue = colours[ij][2];
        }
    }

    g_rr->set_proj_matrix(g_proj);
    g_rr->set_view_matrix(g_mv);
    g_rr->set_ambiant_light(0.1f, 0.1f, 0.1f);

    g_rr->start_scene();
    g_rr->add_mesh_instances(g_h, cubes, N);

    for (size_t i = 0; i < N; ++i)
    {
        if (i != 4 && i != 5)
        {
            g_rr->add_point_light(light[i]);
        }
    }

    const dimension2d<unsigned int> windowSize(g_window_width, g_window_height);

    const quad printableArea(.0f, .0f, 1.0f, 0.5f);

    g_printer->print_text(utf8text("To be, or not to be, that is the question:"
                                   "Whether 'tis Nobler in the mind to suffer "
                                   "The Slings and Arrows of outrageous Fortune, "
                                   "Or to take Arms against a Sea of troubles, "
                                   "And by opposing end them: to die, to sleep "
                                   "No more; and by a sleep, to say we end "
                                   "The heart-ache, and the thousand Natural shocks "
                                   "That Flesh is heir to? 'Tis a consummation "
                                   "Devoutly to be wished. To die to sleep, "
                                   "To sleep, perchance to Dream; Ay, there's the rub, "
                                   "For in that sleep of death, what dreams may come, "
                                   "When we have shuffled off this mortal coil, "
                                   "Must give us pause. "),
                          printableArea);

    static const size_t QuadsCount = 5;
    static const float Border = 0.05f;

    static const colour Colours[]
        = { colour::Red,   colour::Green,  colour::Blue,   colour::Pink,   colour::Black,
            colour::White, colour::Yellow, colour::Orange, colour::Purple, colour::Grey };

    static const size_t ColoursCount = sizeof(Colours) / sizeof(colour);

    for (size_t i = 0; i < QuadsCount; ++i)
    {
        // Go across x
        const float x = (Border * (1.0f / QuadsCount)) + i * (1.0f / QuadsCount);

        for (size_t j = 0; j < QuadsCount; ++j)
        {
            const size_t colourIndex = (j + i * QuadsCount) % (ColoursCount);

            const float y = (Border * (1.0f / QuadsCount)) + j * (1.0f / QuadsCount);
            deferred_renderer::screen_quad q;
            colour qColour(Colours[colourIndex], varyingAlpha);
            q.set_colour(qColour);
            q.screenCoord.x_ = x;
            q.screenCoord.y_ = y;
            q.screenCoord.width_ = (1.0f - Border * 2) * (1.0f / QuadsCount);
            q.screenCoord.height_ = (1.0f - Border * 2) * (1.0f / QuadsCount);
            q.texCoord.x0 = 0.0f;
            q.texCoord.y0 = 0.0f;
            q.texCoord.x1 = 1.0f;
            q.texCoord.y1 = 1.0f;
            g_rr->add_screen_quads(g_white, &q, 1);
        }
    }

    // Finished:
    g_rr->end_scene();

    if (++g_loopcounter == MAX_TEST_LOOPS)
    {
        g_stop = true;
    }
}

void reshapeFn(int w, int g_h)
{
    g_window_width = static_cast<unsigned int>(w);
    load_perspective(
        solosnake::radians(solosnake::radians::pi * 0.25f), float(w) / g_h, 0.1f, 100.0f, g_proj);
    g_printer->set_screen_size(
        dimension2d<unsigned int>(static_cast<unsigned int>(w), static_cast<unsigned int>(g_h)));
}

void exitFn(int)
{
    g_msrc.reset();
    g_printer.reset();
    g_rr.reset();
}
}

TEST(fontprinter, coloured_rects)
{
    try
    {
        bool debugWindow = true;
        auto glut = glutWindow::create(3, 3, WINDOW_W, WINDOW_H, debugWindow);

        EXPECT_TRUE(GL_NO_ERROR == glGetError());

        glut->stopDisplayBool(&g_stop);
        glut->glutExitFunc(exitFn);
        glut->glutDisplayFunc(displayFn);
        glut->glutReshapeFunc(reshapeFn);

        init();

        glut->glutMainLoop();
    }
    catch (const std::exception& e)
    {
        ADD_FAILURE(e.what());
    }
    catch (...)
    {
        ADD_FAILURE("Unexpected exception.");
    }

    exitFn(0);
}

#endif
