#include "solosnake/testing/testing.hpp"
#include "solosnake/case_insensitive_compare.hpp"

using namespace solosnake;

TEST(case_insensitive_compare, HelloWorldSame)
{
    EXPECT_TRUE( 
        case_insensitive_compare("Hello World", 
                                 "Hello World"));
}

TEST(case_insensitive_compare, HelloWorldCased)
{
    EXPECT_TRUE( 
        case_insensitive_compare("Hello World", 
                                 "HELLO WORLD") );
    EXPECT_TRUE( 
        case_insensitive_compare("HELLO WORLD", 
                                 "Hello World") );
}

TEST(case_insensitive_compare, DifferentStrings)
{
    EXPECT_FALSE( 
        case_insensitive_compare("Hello World", 
                                 "Hi there !!") );
}

TEST(case_insensitive_compare, NullPtrs)
{
    EXPECT_TRUE( 
        case_insensitive_compare(nullptr, nullptr) );
}

TEST(case_insensitive_compare, StringAndNullPtr)
{
    EXPECT_FALSE( 
        case_insensitive_compare("Hello", nullptr) );
}

TEST(case_insensitive_compare, NullPtrAndString)
{
    EXPECT_FALSE( 
        case_insensitive_compare(nullptr, "Hello") );
}

TEST(case_insensitive_compare, EmptyStringLhs)
{
    EXPECT_FALSE( 
        case_insensitive_compare("", "Hello") );
}

TEST(case_insensitive_compare, EmptyStringRhs)
{
    EXPECT_FALSE( 
        case_insensitive_compare("HELLO", "") );
}

TEST(case_insensitive_compare, EmptyStrings)
{
    EXPECT_TRUE( 
        case_insensitive_compare("", "") );
}

TEST(case_insensitive_compare, SingleChars)
{
    EXPECT_TRUE( 
        case_insensitive_compare("a", 
                                 "A") );
}

TEST(case_insensitive_compare, SingleCharsFails)
{
    EXPECT_FALSE( 
        case_insensitive_compare("a", 
                                 "B") );
}

TEST(case_insensitive_compare, SameLongStrings)
{
    EXPECT_TRUE( 
        case_insensitive_compare(
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
            "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut "
            "enim ad minim veniam, quis nostrud exercitation ullamco laboris "
            "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor "
            "in reprehenderit in voluptate velit esse cillum dolore eu fugiat "
            "nulla pariatur. Excepteur sint occaecat cupidatat non proident, "
            "sunt in culpa qui officia deserunt mollit anim id est laborum.",
                "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
            "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut "
            "enim ad minim veniam, quis nostrud exercitation ullamco laboris "
            "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor "
            "in reprehenderit in voluptate velit esse cillum dolore eu fugiat "
            "nulla pariatur. Excepteur sint occaecat cupidatat non proident, "
            "sunt in culpa qui officia deserunt mollit anim id est laborum.") );
}