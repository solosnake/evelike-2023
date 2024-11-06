#define RUNTESTS
#ifdef RUNTESTS

#include "solosnake/testing/testing.hpp"
#include <memory>
#include <numeric>
#include <sstream>
#include <fstream>
#include "solosnake/make_font.hpp"
#include "solosnake/fontletters.hpp"
#include "solosnake/image.hpp"

using namespace solosnake;

// BLUEDATADIR is defined by CMake.
#define TESTFONT_NAME BLUEDATADIR "/fonts/FSEX300.ttf"

namespace
{
    //!Returns just the letter A and a.
    std::vector<std::uint32_t> aaa()
    {
        std::vector<std::uint32_t> printables;
        printables.push_back( 'a' );
        printables.push_back( 'A' );
        return printables;
    }

    //! Returns a sorted vector of the printable ascii characters.
    std::vector<std::uint32_t> ascii()
    {
        std::vector<std::uint32_t> printables( 1 + 0x7E - 0x21 );
        std::iota( printables.begin(), printables.end(), 0x21 );
        return printables;
    }

    //! Returns a sorted vector of the printable ascii characters.
    std::vector<std::uint32_t> abcdefg()
    {
        std::vector<std::uint32_t> printables( 7 );
        std::iota( printables.begin(), printables.end(), 'A' );
        return printables;
    }

    //! Returns a sorted vector of the printable ascii characters.
    std::vector<std::uint32_t> abcdefgy()
    {
        std::vector<std::uint32_t> printables;
        // printables.push_back( 'a' );
        // printables.push_back( 'A' );
        // printables.push_back( 'b' );
        // printables.push_back( 'B' );
        // printables.push_back( 'c' );
        // printables.push_back( 'C' );
        // printables.push_back( 'd' );
        // printables.push_back( 'D' );
        // printables.push_back( 'e' );
        // printables.push_back( 'E' );
        // printables.push_back( 'f' );
        // printables.push_back( 'F' );
        // printables.push_back( 'g' );
        // printables.push_back( 'G' );

        printables.push_back( 'y' );
        printables.push_back( 'Y' );
        return printables;
    }

    /** Returns A-G characters plus some swedish ones. */
    std::vector<std::uint32_t> abcdefg_svensk()
    {
        std::vector<std::uint32_t> printables( 7 );
        std::iota( printables.begin(), printables.end(), 'A' );

        printables.push_back( 0x00E4 );
        printables.push_back( 0x00F6 );

        return printables;
    }

    //! Returns a sorted vector of the printable ascii characters plus some
    //! Swedish ones.
    std::vector<std::uint32_t> svensk()
    {
        std::vector<std::uint32_t> printables = ascii();

        printables.push_back( 0x00C4 );
        printables.push_back( 0x00C5 );
        printables.push_back( 0x00D6 );

        return printables;
    }

    std::string ttf_filename()
    {
        return TESTFONT_NAME;
    }

    std::pair<std::shared_ptr<image>, std::shared_ptr<fontletters>>
            make_test_font( unsigned int fontsize,
                            const std::vector<std::uint32_t>& chars )
    {
        std::ostringstream os;
        os << "testfont_pt" << fontsize << "_n" << chars.size();

        unsigned int padding = 1;
        bool verbose = false;

        auto fnt = make_font( ttf_filename(), os.str() + ".bmp", chars, fontsize, padding, verbose );

        fnt.first->save( fnt.second->texture_filename().c_str() );

        std::ofstream ofile( os.str() + ".xml" );
        fnt.second->save( ofile );

        return fnt;
    }
}

TEST( make_font, make_abcdefg_8 )
{
    try
    {
        auto fnt = make_test_font( 8, abcdefg() );

        EXPECT_TRUE( fnt.first != nullptr );
        EXPECT_TRUE( fnt.second != nullptr );
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

TEST( make_font, make_abcdefg_12 )
{
    try
    {
        auto fnt = make_test_font( 12, abcdefg() );

        EXPECT_TRUE( fnt.first != nullptr );
        EXPECT_TRUE( fnt.second != nullptr );
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

TEST( make_font, make_abcdefg_20 )
{
    try
    {
        auto fnt = make_test_font( 20, abcdefg() );

        EXPECT_TRUE( fnt.first != nullptr );
        EXPECT_TRUE( fnt.second != nullptr );
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

TEST( make_font, make_abcdefgy_12 )
{
    try
    {
        auto fnt = make_test_font( 12, abcdefgy() );

        EXPECT_TRUE( fnt.first != nullptr );
        EXPECT_TRUE( fnt.second != nullptr );
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

TEST( make_font, make_abcdefgy_18 )
{
    try
    {
        auto fnt = make_test_font( 18, abcdefgy() );

        EXPECT_TRUE( fnt.first != nullptr );
        EXPECT_TRUE( fnt.second != nullptr );
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

TEST( make_font, make_abcdefgy_24 )
{
    try
    {
        auto fnt = make_test_font( 24, abcdefgy() );

        EXPECT_TRUE( fnt.first != nullptr );
        EXPECT_TRUE( fnt.second != nullptr );
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

TEST( make_font, make_abcdefg_svensk_30 )
{
    try
    {
        auto fnt = make_test_font( 30, abcdefg_svensk() );

        EXPECT_TRUE( fnt.first != nullptr );
        EXPECT_TRUE( fnt.second != nullptr );
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

TEST( make_font, make_abcdefg_svensk_24 )
{
    try
    {
        auto fnt = make_test_font( 24, svensk() );

        EXPECT_TRUE( fnt.first != nullptr );
        EXPECT_TRUE( fnt.second != nullptr );
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

TEST( make_font, make_ascii_12 )
{
    try
    {
        auto fnt = make_test_font( 12, ascii() );

        EXPECT_TRUE( fnt.first != nullptr );
        EXPECT_TRUE( fnt.second != nullptr );
        EXPECT_EQ( ascii().size(), fnt.second->letters_count() );
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

TEST( make_font, make_ascii_16 )
{
    try
    {
        auto fnt = make_test_font( 16, ascii() );

        EXPECT_TRUE( fnt.first != nullptr );
        EXPECT_TRUE( fnt.second != nullptr );
        EXPECT_EQ( ascii().size(), fnt.second->letters_count() );
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

TEST( make_font, make_AAA )
{
    try
    {
        auto fnt = make_test_font( 16, aaa() );

        EXPECT_TRUE( fnt.first != nullptr );
        EXPECT_TRUE( fnt.second != nullptr );
        EXPECT_EQ( aaa().size(), fnt.second->letters_count() );

        fnt.first->save( fnt.second->texture_filename().c_str() );

        std::ofstream ofile( "aaa.xml" );
        fnt.second->save( ofile );
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

TEST( make_font, make_ascii_8 )
{
    try
    {
        auto fnt = make_test_font( 8, ascii() );

        EXPECT_TRUE( fnt.first != nullptr );
        EXPECT_TRUE( fnt.second != nullptr );
        EXPECT_EQ( ascii().size(), fnt.second->letters_count() );
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

TEST( make_font, make_ascii_20 )
{
    try
    {
        auto fnt = make_test_font( 20, ascii() );

        EXPECT_TRUE( fnt.first != nullptr );
        EXPECT_TRUE( fnt.second != nullptr );
        EXPECT_EQ( ascii().size(), fnt.second->letters_count() );
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

#endif
