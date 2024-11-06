#include "solosnake/external/openal.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/logging.hpp"
#include <cassert>

#ifdef SS_OS_WINDOWS
#ifdef _MSC_VER
#pragma comment(lib, "OpenAL32.lib")
#endif
#endif

#define SS_STRINGIFY(x) #x
#define SS_TOSTRING(x)  SS_STRINGIFY(x)
#define SS_ERR_LOCATION __FILE__ ":" SS_TOSTRING(__LINE__)

namespace solosnake
{
    namespace
    {
        static size_t g_alut_open_count = 0;
    }

    //! Initialises the ALUT, the OpenAL helper library, or throws. Can be called
    //! repeatedly, and each call must be matched by a similar alut_release call.
    void alut_init()
    {
        ss_log( "alut_init, g_alut_open_count is ", g_alut_open_count );

        if( g_alut_open_count == 0 )
        {
            // Clear error code:
            ALenum e = alutGetError();
            assert( e == ALUT_ERROR_NO_ERROR );

            if( e != ALUT_ERROR_NO_ERROR )
            {
                ss_err( SS_ERR_LOCATION "OpenAL error : ", alutGetErrorString( e ) );
            }

            if( false == alutInit( NULL, NULL ) )
            {
                e = alutGetError();
                if( e == ALUT_ERROR_NO_ERROR || e == ALUT_ERROR_INVALID_OPERATION )
                {
                    // We can skip these, they are expected.
                    ++g_alut_open_count;
                }
                else
                {
                    ss_err( SS_ERR_LOCATION "OpenAL error : ", alutGetErrorString( e ) );
                    ss_throw( "Failed to initialise ALUT." );
                }
            }
            else
            {
                ++g_alut_open_count;

#ifdef ALUT_API_MAJOR_VERSION
                if( alutGetMajorVersion() != ALUT_API_MAJOR_VERSION || alutGetMinorVersion()
                        != ALUT_API_MINOR_VERSION )
                {
                    ss_throw( "The ALUT header and the ALUT library are different "
                              "revisions." );
                }
#else
#error Linking against an ALUT 0.x.x header file?
#endif
            }
        }
    }

    //! Decrements the ALUT reference counter, which is incremented by each call
    //! to alut_init. When the counter is zero the global ALUT resources are
    //! closed. This does not throw in the event of an error. It will just log
    //! details if possible.
    void alut_release()
    {
        ss_log( "alut_release, g_alut_open_count is ", g_alut_open_count );

        assert( alutGetError() == ALUT_ERROR_NO_ERROR );
        assert( alGetError() == AL_NO_ERROR );
        assert( g_alut_open_count > 0 );
        assert( g_alut_open_count == 0 || g_alut_open_count == 1 );

        if( g_alut_open_count > 0 )
        {
            --g_alut_open_count;

            if( 0 == g_alut_open_count )
            {
                if( false == alutExit() )
                {
                    ss_err( "OpenAL error: ", alutGetErrorString( alutGetError() ) );
                    assert( ! "alutExit returned false." );
                }
            }
        }
    }
}
