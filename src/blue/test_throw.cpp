#define RUNTESTS
#ifdef RUNTESTS

#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_throw.hpp"
#include <memory>

TEST(ss_throw, throwing)
{
    {
        bool threw = false;
        try
        {
            ss_throw("Test message");
        }
        catch (const SS_EXCEPTION_TYPE& e)
        {
            threw = true;
            std::clog << e.what() << std::endl;
        }

        EXPECT_TRUE(threw);
    }
}

#endif
