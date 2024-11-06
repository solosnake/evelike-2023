#include "solosnake/testing/testing.hpp"
#include "solosnake/decode-64.hpp"

TEST( decode64, decode64 )
{
    auto result = solosnake::decode_64("TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCAuLi4=");
    std::string text(result.begin(), result.end());
    std::string expected("Man is distinguished, not only by his reason, but ...");
    bool same = text == expected;
    EXPECT_TRUE(same);
}