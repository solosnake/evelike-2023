#include "solosnake/testing/testing.hpp"
#include "solosnake/range.hpp"

TEST( Range, Range )
{
    auto e = solosnake::Range<float>();

    std::vector<float> v;
    auto r = solosnake::Range(v);
    EXPECT_EQ(r, e);
    EXPECT_EQ(r, r);
    EXPECT_EQ(r.size(), v.size());

    v = std::vector<float>{ 1.0f, 2.0f, 3.0f, 4.0f };
    r = solosnake::Range(v);
    EXPECT_EQ(r.size(), v.size());

    auto v2 = std::array<float, 4>{ 1.0f, 2.0f, 3.0f, 4.0f };
    auto r2 = solosnake::Range(v2);
    EXPECT_EQ(r2, r);

    const float f[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    auto r3 = solosnake::Range(f, 4u);
    EXPECT_EQ(r2, r3);

    const float f4[] = { 1.0f, 2.0f, 3.0f, 5.0f };
    auto r4 = solosnake::Range(f4, 4u);
    EXPECT_NEQ(r2, r4);
}

