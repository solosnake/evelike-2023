#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_compiler.hpp"

TEST( compiler, empty_string )
{
    auto t = blue::predict_text( "" );
    EXPECT_TRUE( t.empty() );
}

TEST( compiler, get_sense_result_type )
{
    auto t0 = blue::predict_text( "g" );
    auto t1 = blue::predict_text( "get_" );
    auto t2 = blue::predict_text( "get_sense_" );
    auto t3 = blue::predict_text( "get_sense_result_" );
    auto t4 = blue::predict_text( "get_sense_result_type" );
    EXPECT_TRUE( t0.size() >= t1.size() );
    EXPECT_TRUE( t1.size() > t2.size() );
    EXPECT_TRUE( t2.size() > t3.size() );
    EXPECT_TRUE( t3.size() > t4.size() );
    EXPECT_TRUE( 1u == t4.size() );
}

TEST( compiler, special_chars )
{
    auto t0 = blue::predict_text( ">" );
    EXPECT_TRUE( ! t0.empty() );
}

TEST( compiler, if_text )
{
    auto t0 = blue::predict_text( "if" );
    EXPECT_FALSE( t0.empty() );
}

TEST( compiler, syntax_of_assignment )
{
    auto c0 = blue::try_compile( "[0] = 1" );
    EXPECT_TRUE( c0.was_successful() );

    auto c1 = blue::try_compile( "" );
    EXPECT_TRUE( c1.was_successful() );
}

TEST( compiler, compile_lines )
{
    blue::compile("sense(5)");
    blue::compile("[1] = get_sense_result_count()");
    blue::compile("[4] = get_distance_between([2],[5])");
    blue::compile("print(\"DIST\")");
    blue::compile( "if [0] goto [2]" );
    blue::compile( "if ! [0] goto [2]" );
    blue::compile( "if [0] != [1] goto 2" );
    blue::compile( "if [0] != [1] goto [2]" );
    blue::compile( "if [0] == [1] goto [2]" );
    blue::compile( "if [0] <= [1] goto [2]" );
    blue::compile( "if [0] >= [1] goto [2]" );
    blue::compile( "if [0] >  [1] goto [2]" );
    blue::compile( "if [0] <  [1] goto [2]" );
}

TEST( compiler, compile_code )
{
    std::vector<std::string> vector_of_lines;
    vector_of_lines.push_back("sense(5)");
    vector_of_lines.push_back("[1] = get_sense_result_count()");
    vector_of_lines.push_back("[4] = get_distance_between([2],[5])");
    vector_of_lines.push_back("print(\"DIST\")");
    vector_of_lines.push_back("if [0] goto [2]");

    auto c1 = blue::compile_code( vector_of_lines );

    // Same code, in single block of text.
    constexpr std::string_view code =
        "sense(5)\n"
        "[1] = get_sense_result_count()\n"
        "[4] = get_distance_between([2],[5])\n"
        "print(\"DIST\")\n"
        "if [0] goto [2]"
        ;

    auto c2 = blue::compile_code( code );

    EXPECT_EQ( c1, c2 );
    EXPECT_EQ( c1.size(), 5u );
}


TEST( compiler, nop )
{
    auto iC  = blue::compile("");
    auto nop = blue::Instruction::nop();
    EXPECT_TRUE( iC == nop );
}
