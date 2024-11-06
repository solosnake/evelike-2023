#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_machine_name.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"


using namespace solosnake;
using namespace blue;

TEST( Machine_name, text )
{
    const Machine_name name("ABCD");
    std::string name_text = name.c_str();
    EXPECT_EQ( std::string("ABCD"), name_text );
}

TEST( Machine_name, version )
{
    const Machine_name name("1.2.345");
    std::string name_text = name.c_str();
    EXPECT_EQ( std::string("1.2.345"), name_text );
}

TEST( Machine_name, hyphen_version )
{
    const Machine_name name("a1-2-35");
    std::string name_text = name.c_str();
    EXPECT_EQ( std::string("a1-2-35"), name_text );
}

TEST( Machine_name, exact_length )
{
    const Machine_name name("0123456");
    std::string name_text = name.c_str();
    EXPECT_EQ( std::string("0123456"), name_text );
}

TEST( Machine_name, shorter_text )
{
    const Machine_name name("0123");
    std::string name_text = name.c_str();
    EXPECT_EQ( std::string("0123"), name_text );
}

TEST( Machine_name, longer_text )
{
    EXPECT_THROW( Machine_name("0123456789ABCDEF") );
}

TEST( Machine_name, empty_text )
{
    EXPECT_THROW( Machine_name("") );
}

TEST( Machine_name, default_ctor )
{
    const Machine_name name;
    std::string text = name.c_str();
    EXPECT_EQ( std::string(""), text );
}

TEST( Machine_name, from_string_view )
{
    const std::string text("A-B-C-7");
    const Machine_name name( text );
    const std::string name_text = name.c_str();
    EXPECT_EQ( text, name_text );
}

TEST( Machine_name, JSON )
{
    const Machine_name name( "ABC-123" );
    nlohmann::json j = name;
    auto name_from_json = j.get<Machine_name>();
    TEST_PRINT(j.dump(4));
    TEST_PRINT(name.c_str());
    TEST_PRINT(name_from_json.c_str());
    EXPECT_EQ( name, name_from_json );
}
