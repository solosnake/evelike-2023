#include "solosnake/testing/testing.hpp"
#include "solosnake/colourtriangle.hpp"

using namespace solosnake;

TEST(colourtriangle, colourtriangle)
{
    point2_t verts[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f } };
    bgra colours[] = { bgra(0, 0, 255, 0), bgra(0, 255, 0, 0), bgra(255, 0, 0, 0) };

    colourtriangle2d tri(verts[0], colours[0], verts[1], colours[1], verts[2], colours[2]);
}