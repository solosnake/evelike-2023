#define RUN_TESTS

// BLUEDATADIR is defined by CMake.

#ifdef RUN_TESTS

#include <thread>
#include <chrono>
#include <cmath>
#include "solosnake/testing/testing.hpp"
#include "solosnake/sound_renderer.hpp"
#include "solosnake/iworldobject.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/matrix3d.hpp"

#ifdef WIN32
#   ifdef _MSC_VER
#       include "solosnake/win32/includewindows.hpp"
#   endif
#endif

using namespace solosnake;

namespace
{
    void sleep_this_thread( const unsigned int ms )
    {
        #ifdef WIN32
        ::Sleep( ms );
        #else
        std::this_thread::sleep_for( std::chrono::milliseconds(ms) );
        #endif
    }

    class thing : public solosnake::iworldobject
    {
    public:
        point4_t& dirvector()
        {
            return dirvector_;
        }

        matrix4x4_t& orientation()
        {
            return orientation_;
        }
    };
}

TEST(sound_renderer, ctor)
{
    try
    {
        auto soundsDir = std::make_shared<datadirectory>(BLUEDATADIR "/meshes");

        auto sr = std::make_shared<sound_renderer>(soundsDir, sound_renderer::LinearFalloff, 1.0f, 16);

        EXPECT_TRUE(sr != nullptr);
    }
    catch (...)
    {
        ADD_FAILURE("Unhandled exception.");
    }
}

TEST(sound_renderer, playblip)
{
    try
    {
        auto soundsDir = std::make_shared<datadirectory>(BLUEDATADIR "/sounds");

        auto sr = std::make_shared<sound_renderer>(soundsDir, sound_renderer::LinearFalloff, 1.0f, 16);

        auto blip = sr->load_buffer("test.wav");

        EXPECT_TRUE(blip != 0);

        sr->play_buffer(blip);

        size_t x = 100;
        while (--x)
        {
            sr->update();
            sleep_this_thread(10);
        }
    }
    catch (...)
    {
        ADD_FAILURE("Unhandled exception.");
    }
}

TEST(sound_renderer, play_2_sounds)
{
    try
    {
        auto soundsDir = std::make_shared<datadirectory>(BLUEDATADIR "/sounds");

        auto sr = std::make_shared<sound_renderer>(soundsDir, sound_renderer::LinearFalloff, 1.0f, 16);

        auto a = sr->load_buffer("Footsteps.wav");
        auto b = sr->load_buffer("ding.wav");

        EXPECT_TRUE(a != 0);
        EXPECT_TRUE(b != 0);

        sr->play_buffer(a);
        sr->play_buffer(b);

        size_t x = 500;
        while (--x)
        {
            sr->update();
            sleep_this_thread(10);
        }
    }
    catch (...)
    {
        ADD_FAILURE("Unhandled exception.");
    }
}

TEST(sound_renderer, moving_sounds)
{
    try
    {
        auto soundsDir = std::make_shared<datadirectory>(BLUEDATADIR "/sounds");

        auto sr = std::make_shared<sound_renderer>(soundsDir, sound_renderer::LinearFalloff, 1.0f, 16);

        auto footsteps = sr->load_buffer("Footsteps.wav");

        std::shared_ptr<thing> walker = std::make_shared<thing>();

        float radius = 10.0f;

        solosnake::load_translation_4x4(radius * std::cos(6.28318f),
                                        0.0f,
                                        radius * std::sin(6.28318f),
                                        walker->orientation());

        sr->play_buffer(footsteps, walker, 1.0f, 2.0f * radius, true);

        size_t walkspeed = 10000;
        size_t t = 10000;
        while (--t)
        {
            float z = radius * std::sin(2 * 3.14159f * t / walkspeed);
            float x = radius * std::cos(2 * 3.14159f * t / walkspeed);
            solosnake::load_translation_4x4(x, 0.0f, z, walker->orientation());

            sr->update();
            sleep_this_thread(1);
        }
    }
    catch (...)
    {
        ADD_FAILURE("Unhandled exception.");
    }
}

TEST(sound_renderer, moving_listener)
{
    try
    {
        auto soundsDir = std::make_shared<datadirectory>(BLUEDATADIR "/sounds");

        auto sr = std::make_shared<sound_renderer>(soundsDir, sound_renderer::LinearFalloff, 1.0f, 16);

        auto engine = sr->load_buffer("c.wav");

        std::shared_ptr<thing> tank = std::make_shared<thing>();
        std::shared_ptr<thing> listener = std::make_shared<thing>();

        float distance = 15.0f;
        float bubble = 8.0f;

        // The tank is stationary.
        solosnake::load_translation_4x4(0.0f, 0.0f, 0.0f, tank->orientation());
        sr->play_buffer(engine, tank, 0.5f, bubble, true);

        // The listener moves. Higher is slower.
        size_t listenerspeed = 10000;
        solosnake::load_translation_4x4( -distance + (2.0f * distance), 0.0f, 0.0f, listener->orientation());

        size_t t = 10000;
        while (--t)
        {
            float x = -distance + (2.0f * distance * t / listenerspeed);
            float up[3] = { 0.0f, 1.0f, 0.0f };
            float forwards[3] = { 0.0f, 0.0f, 1.0f };
            float location[3] = { x, 0.0f, 0.0f };
            sr->set_listener_orientation(location, up, forwards);
            sr->update();
            sleep_this_thread(1);
        }
    }
    catch (...)
    {
        ADD_FAILURE("Unhandled exception.");
    }
}

TEST(sound_renderer, moving_sounds_x2)
{
    try
    {
        auto soundsDir = std::make_shared<datadirectory>(BLUEDATADIR "/sounds");

        auto sr = std::make_shared<sound_renderer>(soundsDir, sound_renderer::LinearFalloff, 1.0f, 16);

        auto footsteps = sr->load_buffer("Footsteps.wav");
        auto engine = sr->load_buffer("c.wav");

        std::shared_ptr<thing> walker = std::make_shared<thing>();
        std::shared_ptr<thing> tank = std::make_shared<thing>();

        float radius = 15.0f;

        solosnake::load_translation_4x4(radius * std::cos(6.28318f),
                                        0.0f,
                                        radius * std::sin(6.28318f),
                                        walker->orientation());
        solosnake::load_translation_4x4(
            radius * std::cos(6.28318f), 0.0f, radius * std::sin(6.28318f), tank->orientation());

        sr->play_buffer(footsteps, walker, 1.0f, 2.0f * radius, true);
        sr->play_buffer(engine, tank, 0.5f, 2.0f * radius, true);

        // Higher is slower.
        size_t walkspeed = 10000;
        size_t tankspeed = 30000;

        size_t t = 10000;
        while (--t)
        {
            float z = radius * std::sin(2 * 3.14159f * t / walkspeed);
            float x = radius * std::cos(2 * 3.14159f * t / walkspeed);
            solosnake::load_translation_4x4(x, 0.0f, z, walker->orientation());

            z = radius * std::sin(2 * 3.14159f * t / tankspeed);
            x = radius * std::cos(2 * 3.14159f * t / tankspeed);
            solosnake::load_translation_4x4(x, 0.0f, z, tank->orientation());

            sr->update();
            sleep_this_thread(1);
        }
    }
    catch (...)
    {
        ADD_FAILURE("Unhandled exception.");
    }
}

TEST(sound_renderer, passing_sound_x1)
{
    try
    {
        auto soundsDir = std::make_shared<datadirectory>(BLUEDATADIR "/sounds");

        auto sr = std::make_shared<sound_renderer>(soundsDir, sound_renderer::LinearFalloff, 1.0f, 16);

        auto engine = sr->load_buffer("c.wav");

        std::shared_ptr<thing> tank = std::make_shared<thing>();

        float distance = 15.0f;
        float bubble = 8.0f;

        solosnake::load_translation_4x4( -distance + (2.0f * distance), 0.0f, 0.0f, tank->orientation());

        sr->play_buffer(engine, tank, 0.5f, bubble, true);

        // Higher is slower.
        size_t tankspeed = 10000;

        size_t t = 10000;
        while (--t)
        {
            float x = -distance + (2.0f * distance * t / tankspeed);
            solosnake::load_translation_4x4(x, 0.0f, 0.0f, tank->orientation());
            sr->update();
            sleep_this_thread(1);
        }
    }
    catch (...)
    {
        ADD_FAILURE("Unhandled exception.");
    }
}

TEST(sound_renderer, passing_sound_x3)
{
    try
    {
        auto soundsDir = std::make_shared<datadirectory>(BLUEDATADIR "/sounds");

        auto sr = std::make_shared<sound_renderer>(soundsDir, sound_renderer::LinearFalloff, 1.0f, 16);

        auto engine1 = sr->load_buffer("c.wav");
        auto ding = sr->load_buffer("ding.wav");
        auto footsteps = sr->load_buffer("Footsteps.wav");

        std::shared_ptr<thing> tank = std::make_shared<thing>();
        std::shared_ptr<thing> walker = std::make_shared<thing>();
        std::shared_ptr<thing> lawnmower = std::make_shared<thing>();

        float distance = 15.0f;

        solosnake::load_translation_4x4(
            -distance + (2.0f * distance), 0.0f, 0.0f, tank->orientation());
        solosnake::load_translation_4x4(
            -distance + (2.0f * distance), 0.0f, 0.0f, walker->orientation());
        solosnake::load_translation_4x4(
            -distance + (2.0f * distance), 0.0f, 0.0f, lawnmower->orientation());

        sr->play_buffer(engine1, tank, 0.2f, 10.0f, true);
        sr->play_buffer(ding, lawnmower, 0.5f, 6.0f, true);
        sr->play_buffer(footsteps, walker, 1.0f, 8.0f, true);

        const size_t n = 10000;
        size_t t = n;

        float tankspeed = 1.0f / n;
        float lawnmowerspeed = 0.9f / n;
        float walkspeed = 0.85f / n;

        while (--t)
        {
            float x;

            x = -distance + (distance * tankspeed * (n - t));
            solosnake::load_translation_4x4(x, 0.0f, 0.0f, tank->orientation());

            x = -distance + (distance * lawnmowerspeed * (n - t));
            solosnake::load_translation_4x4(x, 0.0f, 0.0f, lawnmower->orientation());

            x = -distance + (distance * walkspeed * (n - t));
            solosnake::load_translation_4x4(x, 0.0f, 0.0f, walker->orientation());

            sr->update();
            sleep_this_thread(1);
        }
    }
    catch (...)
    {
        ADD_FAILURE("Unhandled exception.");
    }
}

#endif // RUN_TESTS

