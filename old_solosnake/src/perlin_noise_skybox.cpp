#include <algorithm>
#include <cmath>
#include <map>
#include <iostream>
#include <valarray>

#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/image.hpp"
#include "solosnake/perlin_noise_skybox.hpp"
#include "solosnake/point.hpp"
#include "solosnake/make_iimg.hpp"
#include "solosnake/unreachable.hpp"

using namespace std;

namespace solosnake
{
    namespace
    {

        class ClassicNoise
        {
        public:

            class NoiseData 
            { 
                public: 
                    int operator [] (const size_t n) const { return p[n]; }
                    int p[512]; 
            };

        private:

            // Classic Perlin noise in 3D, for comparison

            static const int grad3[12][3];

            static const int p[];

            // To remove the need for index wrapping, float the permutation table
            // length
            static const NoiseData perm;

            // This method is a *lot* faster than using (int)Math.floor(x)
            static int fastfloor( float x )
            {
                return x > 0.0f ? ( int )x : ( int )x - 1;
            }

            static float dot( const int g[], float x, float y, float z )
            {
                return g[0] * x + g[1] * y + g[2] * z;
            }

            static float mix( float a, float b, float t )
            {
                return ( 1 - t ) * a + t * b;
            }

            static float fade( float t )
            {
                return t * t * t * ( t * ( t * 6 - 15 ) + 10 );
            }

        public:

            static NoiseData make_initial_noise()
            {
                NoiseData d;

                for( int i = 0; i < 512; i++ )
                {
                    d.p[i] = p[i & 255];
                }

                return d;
            }

            // Classic Perlin noise, 3D version
            static float noise( float x, float y, float z )
            {
                // Find unit grid cell containing point
                int X = fastfloor( x );
                int Y = fastfloor( y );
                int Z = fastfloor( z );
                // Get relative xyz coordinates of point within that cell
                x = x - X;
                y = y - Y;
                z = z - Z;
                // Wrap the integer cells at 255 (smaller integer period can be
                // introduced here)
                X = X & 255;
                Y = Y & 255;
                Z = Z & 255;
                // Calculate a set of eight hashed gradient indices
                const int gi000 = perm[ X + perm[Y + perm[Z]] ]             % 12;
                const int gi001 = perm[ X + perm[Y + perm[Z + 1]] ]         % 12;
                const int gi010 = perm[ X + perm[Y + 1 + perm[Z]] ]         % 12;
                const int gi011 = perm[ X + perm[Y + 1 + perm[Z + 1]] ]     % 12;
                const int gi100 = perm[ X + 1 + perm[Y + perm[Z]] ]         % 12;
                const int gi101 = perm[ X + 1 + perm[Y + perm[Z + 1]] ]     % 12;
                const int gi110 = perm[ X + 1 + perm[Y + 1 + perm[Z]] ]     % 12;
                const int gi111 = perm[ X + 1 + perm[Y + 1 + perm[Z + 1]] ] % 12;
                // The gradients of each corner are now:
                // g000 = grad3[gi000];
                // g001 = grad3[gi001];
                // g010 = grad3[gi010];
                // g011 = grad3[gi011];
                // g100 = grad3[gi100];
                // g101 = grad3[gi101];
                // g110 = grad3[gi110];
                // g111 = grad3[gi111];
                // Calculate noise contributions from each of the eight corners
                const float n000 = dot( grad3[gi000], x, y, z );
                const float n100 = dot( grad3[gi100], x - 1, y, z );
                const float n010 = dot( grad3[gi010], x, y - 1, z );
                const float n110 = dot( grad3[gi110], x - 1, y - 1, z );
                const float n001 = dot( grad3[gi001], x, y, z - 1 );
                const float n101 = dot( grad3[gi101], x - 1, y, z - 1 );
                const float n011 = dot( grad3[gi011], x, y - 1, z - 1 );
                const float n111 = dot( grad3[gi111], x - 1, y - 1, z - 1 );
                // Compute the fade curve value for each of x, y, z
                const float u = fade( x );
                const float v = fade( y );
                const float w = fade( z );
                // Interpolate along x the contributions from each of the corners
                const float nx00 = mix( n000, n100, u );
                const float nx01 = mix( n001, n101, u );
                const float nx10 = mix( n010, n110, u );
                const float nx11 = mix( n011, n111, u );
                // Interpolate the four results along y
                const float nxy0 = mix( nx00, nx10, v );
                const float nxy1 = mix( nx01, nx11, v );
                // Interpolate the two last results along z
                const float nxyz = mix( nxy0, nxy1, w );
                return nxyz;
            }
        };

        const int ClassicNoise::grad3[12][3] = 
        {   
          {  1,  1,  0 },
          { -1,  1,  0 },
          {  1, -1,  0 },
          { -1, -1,  0 },
          {  1,  0,  1 },
          { -1,  0,  1 },
          {  1,  0, -1 },
          { -1,  0, -1 },
          {  0,  1,  1 },
          {  0, -1,  1 },
          {  0,  1, -1 },
          {  0, -1, -1 }
        };

        const int ClassicNoise::p[] =
        { 
          151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,   225,
          140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190, 6,   148,
          247, 120, 234, 75,  0,   26,  197, 62,  94,  252, 219, 203, 117, 35,  11,  32,
          57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125, 136, 171, 168, 68,  175,
          74,  165, 71,  134, 139, 48,  27,  166, 77,  146, 158, 231, 83,  111, 229, 122,
          60,  211, 133, 230, 220, 105, 92,  41,  55,  46,  245, 40,  244, 102, 143, 54,
          65,  25,  63,  161, 1,   216, 80,  73,  209, 76,  132, 187, 208, 89,  18,  169,
          200, 196, 135, 130, 116, 188, 159, 86,  164, 100, 109, 198, 173, 186, 3,   64,
          52,  217, 226, 250, 124, 123, 5,   202, 38,  147, 118, 126, 255, 82,  85,  212,
          207, 206, 59,  227, 47,  16,  58,  17,  182, 189, 28,  42,  223, 183, 170, 213,
          119, 248, 152, 2,   44,  154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,
          129, 22,  39,  253, 19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104,
          218, 246, 97,  228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241,
          81,  51,  145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157,
          184, 84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205, 93,
          222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,  215, 61,  156, 180
        };

        const ClassicNoise::NoiseData ClassicNoise::perm = ClassicNoise::make_initial_noise();

        enum OpenGLCubeMapEnums
        {
            TEXTURE_CUBEMAP_POSITIVE_X,
            TEXTURE_CUBEMAP_NEGATIVE_X,
            TEXTURE_CUBEMAP_POSITIVE_Y,
            TEXTURE_CUBEMAP_NEGATIVE_Y,
            TEXTURE_CUBEMAP_POSITIVE_Z,
            TEXTURE_CUBEMAP_NEGATIVE_Z
        };

        void get_world_xyz( float* xyz, unsigned int face, float x, float y, float fhalfsize )
        {
            switch( face )
            {
                case TEXTURE_CUBEMAP_NEGATIVE_Z:
                    xyz[0] = -x;
                    xyz[1] = -y;
                    xyz[2] = -fhalfsize;
                    break;

                case TEXTURE_CUBEMAP_NEGATIVE_X:
                    xyz[0] = -fhalfsize;
                    xyz[1] = -y;
                    xyz[2] = +x;
                    break;

                case TEXTURE_CUBEMAP_POSITIVE_Z:
                    xyz[0] = +x;
                    xyz[1] = -y;
                    xyz[2] = +fhalfsize;
                    break;

                case TEXTURE_CUBEMAP_POSITIVE_X:
                    xyz[0] = +fhalfsize;
                    xyz[1] = -y;
                    xyz[2] = -x;
                    break;

                case TEXTURE_CUBEMAP_POSITIVE_Y:
                    xyz[0] = +x;
                    xyz[1] = +fhalfsize;
                    xyz[2] = +y;
                    break;

                case TEXTURE_CUBEMAP_NEGATIVE_Y:
                    xyz[0] = +x;
                    xyz[1] = -fhalfsize;
                    xyz[2] = -y;
                    break;

                default:
                    ss_unreachable;
            }
        }

        // @param sharpness Controls how fuzzy or sharp the clouds are. Lower
        // values give sharper, denser clouds, and values
        // closer to 1.0 give fuzzier, thinner clouds. Do not
        // use values any greater than 1.0.
        static float curve( float v, float coverage, float sharpness )
        {
            // v = 0.1, coverage = 0.7 (70%) 0.7 - 0.1 = 0.6
            // v = 0.3, coverage = 0.7 (70%) 0.7 - 0.3 = 0.4
            // v = 0.6, coverage = 0.7 (70%) 0.7 - 0.6 = 0.1
            //
            // 0.5 ^ 0.1 = 0.933033
            // 0.5 ^ 0.4 = 0.757858
            // 0.5 ^ 0.6 = 0.659754
            //
            float c = coverage > v ? coverage - v : 0.0f;
            c /= coverage;
            return 1.0f - static_cast<float>( pow( sharpness, c ) );
        }
    }

    cubemap make_perlin_skybox( const unsigned int size, 
                                const int // seed
                              )
    {
        image bmps[6] = { image( size, size, bgr( 255, 0, 0 ) ), // Blue         +X
                          image( size, size, bgr( 0, 255, 0 ) ), // Green        -X
                          image( size, size, bgr( 0, 0, 255 ) ), // Red          +Y
                          image( size, size, bgr( 255, 255, 255 ) ), // White        -Y
                          image( size, size, bgr( 255, 0, 255 ) ), // Pinky purple +Z
                          image( size, size, bgr( 0, 255, 255 ) ) // Yellow       -Z
                        };

#ifndef NDEBUG
        image ps[8];
        for( int p = 0; p < 8; ++p )
        {
            ps[p] = image( size, size, bgr( 255, 0, 0 ) );
        }
#endif

        // For each face.
        const float fsize = static_cast<float>( size );
        const float fhalfsize = fsize * 0.5f;
        const float pixeloffset = 0.5f / fsize;

        for( unsigned int i = 0; i < 6; ++i )
        {
            for( unsigned int y = 0; y < size; ++y )
            {
                const float worldY = fsize * ( -0.5f + pixeloffset + ( y / fsize ) );

                for( unsigned int x = 0; x < size; ++x )
                {
                    float worldX = fsize * ( -0.5f + pixeloffset + ( x / fsize ) );

                    float xyz[3];
                    get_world_xyz( xyz, i, worldX, worldY, fhalfsize );
                    normalise3( xyz );

                    float sum = 0.0f;
                    const size_t octaveCount = 5;
                    for( size_t octave = 1; octave <= octaveCount; ++octave )
                    {
                        float pv = ClassicNoise::noise( xyz[0] * octave * octave,
                                                        xyz[1] * octave * octave,
                                                        xyz[2] * octave * octave );
                        pv = 0.5f + 0.5f * pv;
#ifndef NDEBUG
                        ps[octave - 1].set_pixel( x, y, bgra::from_floats( pv, pv, pv, 1.0f ) );
#endif
                        sum += pv;
                    }

                    sum /= octaveCount;
                    // sum = 0.1f + 0.9f * sum;

                    float c1 = curve( sum, 0.8f, 0.6f );
                    float c2 = curve( sum, 0.5f, 0.09f );
                    float c3 = curve( sum, 0.5f, 0.3f );

                    float b = 6.0f;
                    float g = 2.0f;
                    float r = 2.0f;

                    b += 60.0f * c1;
                    g += 20.0f * c1;
                    r += 20.0f * c1;

                    b += 60.0f * c2;
                    g += 50.0f * c2;

                    b += 60.0f * c3;
                    g += 60.0f * c3;
                    r += 10.0f * c3;

                    b /= 255.0f;
                    g /= 255.0f;
                    r /= 255.0f;

                    bmps[i].set_pixel( x, y, bgra::from_floats( b, g, r, 1.0f ) );
                }
            }
        }

        const bgra starColors[]
            = { bgra( 255, 255, 255 ), bgra( 255, 255, 255 ), bgra( 255, 255, 255 ), bgra( 255, 255, 255 ),
                bgra( 255, 255, 255 ), bgra( 255, 255, 255 ), bgra( 0, 255, 255 ),   bgra( 255, 255, 0 ),
                bgra( 0, 128, 255 ),   bgra( 192, 128, 255 ), bgra( 128, 255, 255 )
              };

        const unsigned int starsCount = 10000u;
        for( unsigned int n = 0; n < starsCount; ++n )
        {
            const float halfPi = 3.14159265359f * 0.50f;
            const float quarterPi = 3.14159265359f * 0.25f;
            const float rX = rand() / static_cast<float>( RAND_MAX );
            const float rY = rand() / static_cast<float>( RAND_MAX );
            const float starX = rX > 0.0001f ? 0.5f + 0.5f / tan( quarterPi + rX * halfPi ) : 0.5f;
            const float starY = rY > 0.0001f ? 0.5f + 0.5f / tan( quarterPi + rY * halfPi ) : 0.5f;
            unsigned int direction = rand() % 6;

            const size_t pixelX = static_cast<size_t>( starX * bmps[direction].width() );
            const size_t pixelY = static_cast<size_t>( starY * bmps[direction].height() );
            const bgra starColour = starColors[rand() % sizeof( starColors ) / sizeof( bgra )];
            const float starIntensity = 0.75f * rand() / static_cast<float>( RAND_MAX );
            bmps[direction].set_pixel( pixelX, pixelY, starColour * starIntensity );
        }

        //#ifndef NDEBUG

        const char* texture_names[6]
            = { "positive_x", "negative_x", "positive_y", "negative_y", "positive_z", "negative_z" };

        for( unsigned int i = 0; i < 6; ++i )
        {
            bmps[i].save( std::string( texture_names[i] ) + std::string( ".bmp" ) );
        }

        //#endif

        cubemap sb;
        for( unsigned int i = 0; i < 6; ++i )
        {
            sb[i] = make_iimgBGR( move( bmps[i] ) );
        }

        return sb;
    }
}
