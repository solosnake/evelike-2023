#ifndef solosnake_quickmodel_hpp
#define solosnake_quickmodel_hpp

#include <memory>

namespace solosnake
{
    bool quickmodel( size_t indexCount,
                     const unsigned short* indices,
                     size_t vertexCount,
                     const float* vertexSrc,
                     const float* textureUVsSrc = nullptr,
                     const unsigned int framesToShowFor = 0 );
}

#endif
