#define RUNTESTS
#ifdef RUNTESTS

#include "solosnake/testing/testing.hpp"
#include <fstream>
#include <memory>
#include "solosnake/html.hpp"
#include "solosnake/utf8text.hpp"
#include "solosnake/fontletters.hpp"
#include "solosnake/itextureloader.hpp"
#include "solosnake/datadirectory.hpp"

using namespace solosnake;

// BLUEDATADIR is defined by CMake.

#define FONTFILENAME "basic24.xml"

TEST(fontletters, loading)
{
    {
        try
        {
            datadirectory fontfiles(BLUEDATADIR "/fonts");

            auto url = fontfiles.get_file(FONTFILENAME);

            fontletters testfont(url);
        }
        catch (...)
        {
            ADD_FAILURE("Unexpected exception.");
        }
    }
}

TEST(fontletters, moving)
{
    {
        try
        {
            datadirectory fontfiles(BLUEDATADIR "/fonts");

            auto url = fontfiles.get_file(FONTFILENAME);

            fontletters testfont(url);

            fontletters copyoffont = testfont;

            fontletters movedfont = std::move(testfont);

            EXPECT_TRUE(movedfont == copyoffont);
        }
        catch (...)
        {
            ADD_FAILURE("Unexpected exception.");
        }
    }
}
#endif
