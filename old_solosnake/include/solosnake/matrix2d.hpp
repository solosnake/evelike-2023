#ifndef solosnake_matrix2d_hpp
#define solosnake_matrix2d_hpp

#include <cassert>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <memory>
#include "solosnake/angles.hpp"
#include "solosnake/point.hpp"
#include "solosnake/unaliased.hpp"
#include "solosnake/inline.hpp"

namespace solosnake
{
    typedef float matrix2x2_t[4];

    SS_INLINE void transform_2x1( const float* unaliased src,
                                  const float* unaliased m2x2,
                                  float* unaliased dst )
    {
        ASSERT_UNALIASED( src, dst, 2, 2 );
        ASSERT_UNALIASED( src, m2x2, 2, 4 );
        dst[0] = src[0] * m2x2[0] + src[1] * m2x2[2];
        dst[1] = src[0] * m2x2[1] + src[1] * m2x2[3];
    }
}

#endif
