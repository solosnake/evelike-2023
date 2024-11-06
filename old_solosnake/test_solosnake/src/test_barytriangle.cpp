#include "solosnake/testing/testing.hpp"
#include "solosnake/barytriangle.hpp"

using namespace solosnake;

TEST(barytriangle, barytriangle)
{
    point2_t verts[] = { { 1.0f, 1.0f }, { 2.0f, 1.0f }, { 1.0f, 2.0f } };

    barytriangle2d tri(verts[0], verts[1], verts[2]);

    point3_t b1 = {};
    point3_t b2 = {};
    point3_t b3 = {};

    tri.get_barycentric_coords(1.0f, 1.0f, b1);
    tri.get_barycentric_coords(2.0f, 1.0f, b2);
    tri.get_barycentric_coords(1.0f, 2.0f, b3);

    EXPECT_TRUE(b1[0] * verts[0][0] + b2[0] * verts[1][0] + b3[0] * verts[2][0] == 1.0f);
}