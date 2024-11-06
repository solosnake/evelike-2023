#ifndef solosnake_clamp_hpp
#define solosnake_clamp_hpp

namespace solosnake
{
    template <typename T> inline T clamp(T x, T low, T high)
    {
        return (x < low) ? low : (x > high ? high : x);
    }
}

#endif
