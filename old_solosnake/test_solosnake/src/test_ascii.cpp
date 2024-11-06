#include "solosnake/testing/testing.hpp"
#include "solosnake/utf8_to_unicode.hpp"
#include "solosnake/unicode_to_utf8.hpp"

#pragma warning(disable : 4428) // Use of unicode in string.

using namespace solosnake;

TEST(unicode_to_utf8, two_char_unicode_to_UTF8)
{
    {
        // These all expand to 3(?) characters
        size_t n = 12;
        std::wstring test(n, wchar_t(0x0958));
        std::string utf8A = unicode_to_utf8(test);
        EXPECT_TRUE(utf8A.length() == n * 3);
    }
}

TEST(unicode_to_utf8, three_char_unicode_to_UTF8)
{
    {
        // These expand to 3 characters
        size_t n = 12;
        std::wstring test(n, wchar_t(0xfb2c));
        std::string utf8A = unicode_to_utf8(test);
        EXPECT_TRUE(utf8A.length() == n * 3);
    }
}

TEST(unicode_to_utf8, two_char_unicode_to_UTF8_1)
{
    {
        // These expand to 2 characters
        size_t n = 12;
        std::wstring test(n, wchar_t(0xf0));
        std::string utf8A = unicode_to_utf8(test);
        EXPECT_TRUE(utf8A.length() == n * 2);
    }
}

TEST(unicode_to_utf8, two_char_unicode_to_UTF8_2)
{
    {
        // These expand to 2 characters
        size_t n = 12;
        std::wstring test(n, wchar_t(0x90));
        std::string utf8A = unicode_to_utf8(test);
        EXPECT_TRUE(utf8A.length() == n * 2);
    }
}

TEST(unicode_to_utf8, two_char_unicode_to_UTF8_3)
{
    {
        // These expand to 2 characters
        size_t n = 12;
        std::wstring test(n, wchar_t(0x8c));
        std::string utf8A = unicode_to_utf8(test);
        EXPECT_TRUE(utf8A.length() == n * 2);
    }
}

TEST(unicode_to_utf8, example_string_toutf8)
{
    {
        std::string utf8A = unicode_to_utf8(L"T\u00e8st string \uFF54\uFF4F n\u00f8rm\u00e4lize");
        EXPECT_TRUE(!utf8A.empty());
    }
}

TEST(unicode_to_utf8, nulls_unicode_to_UTF8)
{
    {
        std::string utf8A = unicode_to_utf8(L"\0\0\0\0\0\0");
        EXPECT_TRUE(utf8A.empty());
    }
}

TEST(ascii, utf8_to_unicode_empty)
{
    {
        const char utf8[] = { 0 };
        std::wstring u = utf8_to_unicode(utf8);
        EXPECT_TRUE(u.empty());
    }
}

TEST(ascii, utf8_to_unicode)
{
    {
        const char utf8[] = { 0 };
        std::wstring u = utf8_to_unicode(utf8);
        EXPECT_TRUE( u.empty() );
    }
}