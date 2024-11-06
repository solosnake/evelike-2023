#define RUNTESTS
#ifdef RUNTESTS

#include "solosnake/testing/testing.hpp"
#include <memory>
#include "solosnake/utf8text.hpp"
#include "solosnake/throw.hpp"

using namespace solosnake;

TEST(ss_utf8text, is_empty)
{
    {
        utf8text notxt;
        EXPECT_TRUE( notxt.is_empty() );

        utf8text txt( "Hello" );
        EXPECT_FALSE( txt.is_empty() );
    }
}

TEST(ss_utf8text, ascii)
{
    {
        try
        {
            utf8text t("hello world");
        }
        catch (...)
        {
            ADD_FAILURE("Unexpected exception.");
        }
    }
}

TEST(ss_utf8text, lt_ascii)
{
    {
        try
        {
            {
                std::string hw("Hello World");
                utf8text t0(hw);
                utf8text t1(hw);
                EXPECT_TRUE(!(t0 < t1));
            }

            {
                const char txt1[] = { 'a', 'b', 0, 'c' };
                const char txt2[] = { 'a', 'b', 0, 'd' };
                std::string st1(txt1);
                std::string st2(txt2);
                utf8text s0(st1);
                utf8text s1(st2);
                EXPECT_TRUE(!(s0 < s1));
            }
        }
        catch (...)
        {
            ADD_FAILURE("Unexpected exception.");
        }
    }
}

TEST(ss_utf8text, 2chars)
{
    {
        try
        {
            utf8text t("\xe6\x97\xa5\xd1\x88");
            EXPECT_TRUE(t.number_of_characters() == 2);
        }
        catch (...)
        {
            ADD_FAILURE("Unexpected exception.");
        }
    }
}

TEST(ss_utf8text, inspect3chars0)
{
    {
        try
        {
            // See http://utfcpp.sourceforge.net/
            utf8text t("\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88");
            EXPECT_TRUE(t.number_of_characters() == 3);
            EXPECT_TRUE(t[0] == 0x10346);
        }
        catch (...)
        {
            ADD_FAILURE("Unexpected exception.");
        }
    }
}

TEST(ss_utf8text, inspect3chars1)
{
    {
        try
        {
            // See http://utfcpp.sourceforge.net/
            utf8text t("\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88");
            auto char1 = t[1];
            EXPECT_TRUE(t.number_of_characters() == 3);
            EXPECT_TRUE(char1 == 0x65e5);
        }
        catch (...)
        {
            ADD_FAILURE("Unexpected exception.");
        }
    }
}

TEST(ss_utf8text, inspect3chars2)
{
    {
        try
        {
            // See http://utfcpp.sourceforge.net/
            utf8text t("\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88");
            auto char2 = t[2];
            EXPECT_TRUE(t.number_of_characters() == 3);
            EXPECT_TRUE(char2 == 0x0448);
        }
        catch (...)
        {
            ADD_FAILURE("Unexpected exception.");
        }
    }
}

#endif
