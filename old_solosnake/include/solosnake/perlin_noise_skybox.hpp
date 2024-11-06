#ifndef solosnake_perlin_noise_skybox_hpp
#define solosnake_perlin_noise_skybox_hpp

namespace solosnake
{
struct cubemap;

cubemap make_perlin_skybox(const unsigned int size, const int seed);
}

#endif
