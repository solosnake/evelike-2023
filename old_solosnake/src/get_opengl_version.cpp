#include "solosnake/get_opengl_version.hpp"
#include "solosnake/throw.hpp"
#include <cstring>

namespace solosnake
{
    std::pair<int, int> get_opengl_version( const std::string& options )
    {
        int opengl_major = -1;
        int opengl_minor = -1;

        if( options.empty() || options == "opengl" )
        {
            // Default opengl is 2.1
            opengl_major = 2;
            opengl_minor = 1;
        }
        else if( 0 == options.compare( 0, 7, "opengl " ) && options.length() >= std::strlen( "opengl X.Y" ) )
        {
            // Locations of X.Y in string.
            const size_t major = 7;
            const size_t minor = 9;

            // Starts with opengl<space>...
            switch( options[major] )
            {
                case '1':

                    // We support 1.0, 1.1, 1.2, 1.3, 1.4, 1.5.
                    if( options[minor] > '0' && options[minor] <= '5' )
                    {
                        opengl_major = 1;
                        opengl_minor = options[minor] - '0';
                    }
                    break;

                case '2':
                    // 2.0 and 2.1
                    if( options[minor] > '0' || options[minor] == '1' )
                    {
                        opengl_major = 2;
                        opengl_minor = options[minor] - '0';
                    }
                    break;

                case '3':
                    // 3.0 3.1 3.2 3.3
                    if( options[minor] > '0' && options[minor] <= '3' )
                    {
                        opengl_major = 3;
                        opengl_minor = options[minor] - '0';
                    }
                    break;

                case '4':
                    // 4.n
                    if( options[minor] > '0' || options[minor] <= '9' )
                    {
                        opengl_major = 4;
                        opengl_minor = options[minor] - '0';
                    }
                    break;

                default:
                    ss_throw( "Unsupported, unknown or invalid OpenGL major version was "
                              "specified." );
                    break;
            }
        }

        return std::pair<int, int>( opengl_major, opengl_minor );
    }
}
