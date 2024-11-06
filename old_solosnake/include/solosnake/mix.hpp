#ifndef solosnake_mix_hpp
#define solosnake_mix_hpp

namespace solosnake
{
    template <typename T> inline T mix( T x, T y, float a )
    {
        return x * ( 1.0f - a ) + y * a;
    }

    template <typename T> inline T mix( T x, T y, double a )
    {
        return x * ( 1.0 - a ) + y * a;
    }
}

#endif
