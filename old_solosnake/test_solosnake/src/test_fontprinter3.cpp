#define RUN_TESTS
#ifdef RUN_TESTS

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "solosnake/testing/testing.hpp"
#include "quickloader.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/font.hpp"
#include "solosnake/fontprinter.hpp"
#include "solosnake/fontletters.hpp"
#include "solosnake/glutwindow.hpp"
#include "solosnake/img.hpp"
#include "solosnake/img_cache.hpp"
#include "solosnake/itextureloader.hpp"
#include "solosnake/load_meshes.hpp"
#include "solosnake/make_font.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/utf8text.hpp"

using namespace solosnake;

#define WINDOW_W 1024
#define WINDOW_H 768
// Lets say 100fps = 20s
#define MAX_TEST_LOOPS (10 * 20)

namespace
{
const solosnake::bgra red(0x00, 0x00, 0xFF, 0xFF);

std::shared_ptr<deferred_renderer> g_rr;
std::shared_ptr<fontprinter> g_printer;
meshhandle_t g_h = 0;
bool g_stop = false;
size_t g_loopcounter = 0;
unsigned int g_window_width = WINDOW_W;
unsigned int g_window_height = WINDOW_H;
std::shared_ptr<imesh> g_msrc;
matrix4x4_t g_mv;
matrix4x4_t g_proj;
const float g_zNear = 0.1f;

//-------------------------------------------------------------------------

image create_screensized_image(unsigned int w, unsigned int h)
{
    image img1(w, h, solosnake::image::format_bgra);
    img1.clear_to_bgra(0xFF, 0xFF, 0xFF, 0xFF);

    for (unsigned int y = 0; y < h; ++y)
    {
        for (unsigned int x = 0; x < w; ++x)
        {
            if ((x + y) % 2 == 0)
            {
                *reinterpret_cast<bgra*>(image::pixel(x, y, img1, image::format_bgra)) = red;
            }
        }
    }

    return img1;
}

solosnake::bgra normal_to_colour(const float* n)
{
    return bgra(static_cast<unsigned char>((n[2] + 1.0f) * 127.5f), // Blue is Z
                static_cast<unsigned char>((n[1] + 1.0f) * 127.5f), // Green is Y
                static_cast<unsigned char>((n[0] + 1.0f) * 127.5f), // Red is X
                0x00);
}

std::shared_ptr<fontprinter> make_test_fontprinter(std::shared_ptr<deferred_renderer> g_rr)
{
    datadirectory fontfiles(BLUEDATADIR "/fonts");

    auto fontimg = create_screensized_image(WINDOW_W, WINDOW_H);
    fontimg.save("./fonttestimg.bmp");

    fontglyph windowsizeGlyph( texquad(), WINDOW_W, WINDOW_H, WINDOW_W, WINDOW_H);

    std::map<std::uint32_t, fontglyph> codepoints;
    codepoints['A'] = windowsizeGlyph;

    fontglyph unknown = windowsizeGlyph;

    const unsigned int whitespaceSize = 12;

    fontletters letters("fonttestimg.bmp", codepoints, unknown, WINDOW_H, whitespaceSize);

    auto texDir = std::make_shared<datadirectory>(".");
    auto images = std::make_shared<img_cache>(texDir);
    auto texloader = std::make_shared<solosnake_test::quickloader>(g_rr, images);
    auto fnt = std::make_shared<font>(letters, *texloader);

    return std::make_shared<fontprinter>(dimension2d<unsigned int>(WINDOW_W, WINDOW_H), g_rr, fnt);
}

void init(const rect& screenRect)
{
    solosnake::g_ssLogLvl = SS_LOG_ALL;
    solosnake::close_log();

    // const unsigned char material = 128;
    // generate_model_normal_tex( material );

    std::vector<std::string> directories;
    directories.push_back(BLUEDATADIR "/meshes");
    directories.push_back(BLUEDATADIR "/textures");

    auto assetsDir = std::make_shared<datadirectories>(directories);
    auto url = assetsDir->get_file("thruster.msh");
    auto meshes = load_mesh_datas(url, assetsDir);

    ASSERT_TRUE(meshes.size() == 1);

    g_msrc = meshes.at(0);

    g_rr = std::make_shared<deferred_renderer>(
        screenRect.width(), screenRect.height(), deferred_renderer::HighQuality); // 1920 x 1080

    g_h = g_rr->load_mesh(g_msrc);

    g_printer = make_test_fontprinter(g_rr);
}

// Display 1 stationary cube and 2 nearby rotating cubes.
void displayFn()
{
    static float cameraDistance = 10.0f;
    static float cameraY = 2.50f;
    static float change = -0.01f;

    static float varier = -1.0f;
    static float dv = 0.01f;

    varier += dv;

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
    load_translation_4x4(0.0f, -cameraY, -cameraDistance, g_mv);

    // 9 cubes of radius 2, in a grid, spaced 4 units apart, hard-coded.

    // Our cube spans from 1,1,1 to -1,-1,-1. Find the sphere that fits it
    // perfectly:

    deferred_renderer::point_light light[N];
    meshinstancedata cubes[N];

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

    g_rr->start_scene();
    g_rr->draw_mesh_instances(g_h, cubes, N);

    for (size_t i = 0; i < N; ++i)
    {
        if (i != 4 && i != 5)
        {
            g_rr->draw_point_light(light[i]);
        }
    }

    const dimension2d<unsigned int> windowSize(g_window_width, g_window_height);

    g_printer->print_text(utf8text("A"),
                          position2df(0.5f, 0.0f),
                          dimension2df(),
                          HorizontalAlignCentre,
                          VerticalAlignTop);

    // Finished:
    g_rr->end_scene();

    if (++g_loopcounter == MAX_TEST_LOOPS)
    {
        g_stop = true;
    }
}

void reshapeFn(int w, int h)
{
    g_window_height = static_cast<unsigned int>(h);
    g_window_width = static_cast<unsigned int>(w);
    load_perspective(solosnake::radians(PI_RADIANS * 0.25f), float(w) / h, 0.1f, 100.0f, g_proj);
    g_printer->set_screen_size(
        dimension2d<unsigned int>(static_cast<unsigned int>(w), static_cast<unsigned int>(h)));
    g_rr->set_viewport(0, 0, w, h);
}

void exitFn(int)
{
    g_printer.reset();
    g_rr.reset();
}
}

TEST(fontprinter, fullscreensized_pink_check_text)
{
    try
    {
        auto glut = glutWindow::make_shared(3, 3, WINDOW_W, WINDOW_H);

        EXPECT_TRUE(GL_NO_ERROR == glGetError());

        auto displayLoop = [&]()
        {
            displayFn();
            if (g_stop)
            {
                glut->end_display_loop();
            }
        };

        glut->glutDisplayFunc(displayLoop);
        glut->glutExitFunc(exitFn);
        glut->glutReshapeFunc(reshapeFn);

        init(glut->get_screen_rect());

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
