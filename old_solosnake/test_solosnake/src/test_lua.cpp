#include "solosnake/testing/testing.hpp"
#include "solosnake/external/lua.hpp"

using namespace solosnake;

TEST(lua, ctor)
{
    {
        auto L = solosnake::lua::create();
        EXPECT_TRUE(L != nullptr);
    }
}
