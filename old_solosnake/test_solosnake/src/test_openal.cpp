#define RUN_TESTS

// BLUEDATADIR is defined by CMake.

#ifdef RUN_TESTS

#include <thread>
#include <chrono>
#include "solosnake/testing/testing.hpp"
#include "solosnake/external/openal.hpp"
#include "solosnake/datadirectory.hpp"

#ifdef WIN32
#   ifdef _MSC_VER
#       include "solosnake/win32/includewindows.hpp"
#       pragma comment(lib, "OpenAL32.lib") // OpenAL lib (32/64)
#   endif
#endif

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
}

TEST(openal, helloworld)
{
    ALuint helloBuffer, helloSource;
    EXPECT_TRUE(0 != alutInit(NULL, NULL));
    helloBuffer = alutCreateBufferHelloWorld();
    alGenSources(1, &helloSource);
    alSourcei(helloSource, AL_BUFFER, helloBuffer);
    alSourcePlay(helloSource);

    sleep_this_thread(2000);

    EXPECT_TRUE(0 != alutExit());
}

TEST(openal, playfile)
{
    EXPECT_TRUE(0 != alutInit(NULL, NULL));

    ALuint buffer;
    ALuint source;
    ALenum error;
    ALint status;

    const solosnake::datadirectory sounddir(BLUEDATADIR "/sounds");
    auto url = sounddir.get_file("test.wav");

    /* Create an AL buffer from the given sound file. */
    buffer = alutCreateBufferFromFile(url.string().c_str());

    if (buffer == AL_NONE)
    {
        alutExit();
        ADD_FAILURE("alutCreateBufferFromFile failed.");
    }

    /* Generate a single source, attach the buffer to it and start playing. */
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcePlay(source);

    /* Normally nothing should go wrong above, but one never knows... */
    error = alGetError();
    if (error != ALUT_ERROR_NO_ERROR)
    {
        alutExit();
        ADD_FAILURE("alGetError problem.");
    }

    /* Check every 0.1 seconds if the sound is still playing. */
    do
    {
        sleep_this_thread(100);
        alGetSourcei(source, AL_SOURCE_STATE, &status);
    } while (status == AL_PLAYING);

    EXPECT_TRUE(0 != alutExit());
}

#endif // RUN_TESTS

