#define RUN_TESTS
#ifdef RUN_TESTS

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "solosnake/testing/testing.hpp"
#include "meshsrc.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/image.hpp"
#include "solosnake/glutwindow.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/point.hpp"
#include "solosnake/image.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/load_meshes.hpp"
#include "solosnake/foreachtriangle.hpp"
#include "solosnake/img.hpp"

// BLUEDATADIR is defined by CMake.

using namespace solosnake;

#define WINDOW_W 1024
#define WINDOW_H 768
// Lets say 100fps = 20s
#define MAX_TEST_LOOPS (10 * 4)

namespace
{
std::shared_ptr<deferred_renderer> g_rr;
meshhandle_t g_h = 0;
texturehandle_t g_t1 = 0;
texturehandle_t g_t2 = 0;
std::shared_ptr<imesh> g_msrc;
matrix4x4_t g_mv;
matrix4x4_t g_proj;
bool g_stop = false;
size_t g_loopcounter = 0;
deferred_renderer::dynbufferdrawparams drawDyn;
const float g_zNear = 0.1f;

//-------------------------------------------------------------------------

void create_quad_tex()
{
    const size_t w = 128;
    const size_t g_h = 128;

    solosnake::image img1(w, g_h, solosnake::image::format_bgra);

    solosnake::image::clear_layer_to(img1, 0, 0xFF);
    solosnake::image::clear_layer_to(img1, 1, 0x00);
    solosnake::image::clear_layer_to(img1, 2, 0x00);
    solosnake::image::clear_layer_to(img1, 3, 0x40);

    img1.save(BLUEDATADIR "/meshes/quad.bmp");
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

    img1.save(BLUEDATADIR "/meshes/dynbuffertex.bmp");
}

void init(const rect& screenRect)
{
    // const unsigned char material = 128;
    // generate_model_normal_tex( material );

    std::vector<std::string> directories;
    directories.push_back(BLUEDATADIR "/meshes");
    directories.push_back(BLUEDATADIR "/textures");

    auto assetsDir = std::make_shared<datadirectories>(directories);
    auto url = assetsDir->get_file("thruster.msh");

    auto meshes = load_mesh_datas(url, assetsDir);
    ASSERT_TRUE(meshes.size() == 1);

    auto msh = meshes.at(0)->get_meshgeometry();
    auto tex = meshes.at(0)->get_meshgeometrytexinfo();

    g_msrc = meshes.at(0);

    g_rr = std::make_shared<deferred_renderer>(
        screenRect.width(), screenRect.height(), deferred_renderer::HighQuality); // 1920 x 1080

    g_h = g_rr->load_mesh(g_msrc);

    // Quads
    {
        create_quad_tex();
        auto tex1 = assetsDir->get_file("quad.bmp");
        g_t1 = g_rr->load_texture(std::make_shared<solosnake::img::src>(tex1));
    }

    // Dynbuffer
    {
        create_dynbuffer_tex();
        auto tex2 = assetsDir->get_file("dynbuffertex.bmp");
        g_t2 = g_rr->load_texture(std::make_shared<solosnake::img::src>(tex2));
        drawDyn.buffer = g_rr->alloc_dynbuffer(3, 6);
        drawDyn.texture = g_t2;
        drawDyn.num_indices_to_draw = 6;
    }
}

// Display 1 stationary cube and 2 nearby rotating cubes.
void displayFn()
{
    static float cameraDistance = 10.0f;
    static float cameraY = 2.50f;
    static float change = -0.01f;

    static float varier = -1.0f;
    static float dv = 0.01f;

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

    varier += dv;

    static const size_t grid = 5;
    static const size_t N = grid * grid;

    const float spacing = 2.50f;

    cameraDistance += change;

    if (cameraDistance < (2) || cameraDistance > (grid + grid * spacing))
    {
        change = -change;
    }

    // Model view - "camera".
    load_translation_4x4(0.0f, -cameraY, -cameraDistance, g_mv);

    // 9 cubes of radius 2, in a grid, spaced 4 units apart, hard-coded.

    // Our cube spans from 1,1,1 to -1,-1,-1. Find the sphere that fits it
    // perfectly:

    deferred_renderer::point_light light[N];
    meshinstancedata cubes[N];

    const float colours[N][3] = 
    { 
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
        { 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
    };

    float start = grid * spacing * 0.5f;

    // Initialise model instance data:
    for (size_t i = 0; i < grid; ++i)
    {
        for (size_t j = 0; j < grid; ++j)
        {
            auto ij = grid * j + i;

            load_translation_4x4(
                -start + i * spacing, 0.0f, -start + j * spacing, cubes[ij].location_4x4());

            cubes[ij].set_team_hue_shift(static_cast<float>(i) / grid);

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

    g_rr->set_proj_matrix(g_proj, g_zNear);
    g_rr->set_view_matrix(g_mv);
    g_rr->set_ambiant_light(0.25f, 0.25f, 0.25f);

    // Fill dyn buffer.
    g_rr->lock_dynbuffer(drawDyn.buffer);
    {
        drawDyn.buffer->vertex(0).x = 0.0f;
        drawDyn.buffer->vertex(0).y = 0.0f;
        drawDyn.buffer->vertex(0).z = 0.0f;
        drawDyn.buffer->texcoord(0).u = 0.0f;
        drawDyn.buffer->texcoord(0).v = 0.0f;

        drawDyn.buffer->vertex(1).x = 5.0f;
        drawDyn.buffer->vertex(1).y = 5.0f;
        drawDyn.buffer->vertex(1).z = 0.0f;
        drawDyn.buffer->texcoord(1).u = 1.0f;
        drawDyn.buffer->texcoord(1).v = 1.0f;

        drawDyn.buffer->vertex(2).x = 0.0f;
        drawDyn.buffer->vertex(2).y = 5.0f;
        drawDyn.buffer->vertex(2).z = 0.0f;
        drawDyn.buffer->texcoord(2).u = 0.0f;
        drawDyn.buffer->texcoord(2).v = 1.0f;

        drawDyn.buffer->indice(0) = 0;
        drawDyn.buffer->indice(1) = 1;
        drawDyn.buffer->indice(2) = 2;

        drawDyn.buffer->indice(3) = 0;
        drawDyn.buffer->indice(4) = 2;
        drawDyn.buffer->indice(5) = 1;
    }
    g_rr->unlock_dynbuffer(drawDyn.buffer, 3, 6);

    g_rr->start_scene();
    g_rr->draw_mesh_instances(g_h, cubes, N);

    // Test screen quads:

    deferred_renderer::screen_quad quads[4] = {};

    bgra color1(0xFF, 0, 0, 0xFF);
    bgra color2(0, 0xFF, 0, 0x1F);

    quads[0].screenCoord.x_ = 0.0f;
    quads[0].screenCoord.y_ = 0.0f;
    quads[0].screenCoord.width_ = 0.5f;
    quads[0].screenCoord.height_ = 0.5f;
    quads[0].texCoord.x0 = 0.0f;
    quads[0].texCoord.y0 = 0.0f;
    quads[0].texCoord.x1 = 0.1f;
    quads[0].texCoord.y1 = 0.1f;
    quads[0].cornerColours[0] = color1;
    quads[0].cornerColours[1] = color1;
    quads[0].cornerColours[2] = color2;
    quads[0].cornerColours[3] = color2;

    quads[1].screenCoord.x_ = 0.5f;
    quads[1].screenCoord.y_ = 0.5f;
    quads[1].screenCoord.width_ = 0.25f;
    quads[1].screenCoord.height_ = 0.25f;
    quads[1].texCoord.x0 = 0.0f;
    quads[1].texCoord.y0 = 0.0f;
    quads[1].texCoord.x1 = 0.1f;
    quads[1].texCoord.y1 = 0.1f;
    quads[1].cornerColours[0] = color1;
    quads[1].cornerColours[1] = color1;
    quads[1].cornerColours[2] = color2;
    quads[1].cornerColours[3] = color2;

    g_rr->draw_screen_quads(g_t1, quads, 2);

    for (size_t i = 0; i < N; ++i)
    {
        if (i != 4 && i != 5)
        {
            g_rr->draw_point_light(light[i]);
        }
    }

    // Test dynbuffers:
    g_rr->draw_dynbuffer(drawDyn);

    // Finished:
    g_rr->end_scene();

    if (++g_loopcounter == MAX_TEST_LOOPS)
    {
        g_stop = true;
    }
}

void reshapeFn(int w, int h)
{
    load_perspective(solosnake::radians(PI_RADIANS * 0.25f), float(w) / h, g_zNear, 100.0f, g_proj);
    g_rr->set_viewport(0, 0, w, h);
}

void exitFn(int)
{
    g_rr.reset();
}
}

TEST(deferred_renderer, simpledisplayloop)
{
    solosnake::g_ssLogLvl = SS_LOG_ALL;
    solosnake::close_log();

    try
    {
        bool debugWindow = true;
        auto glut = glutWindow::make_shared(
            3, 3, WINDOW_W, WINDOW_H, glutWindow::CoreProfileOnly, debugWindow);

        EXPECT_TRUE(GL_NO_ERROR == glGetError());

        auto displayLoop = [&]()
        {
            displayFn();
            if (g_stop)
            {
                glut->end_display_loop();
            }
        };

        glut->glutExitFunc(exitFn);
        glut->glutDisplayFunc(displayLoop);
        glut->glutReshapeFunc(reshapeFn);

        init(glut->get_screen_rect());

        glut->glutMainLoop();
    }
    catch (...)
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

#endif
