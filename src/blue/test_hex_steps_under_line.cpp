#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_hex_steps.hpp"
#include "solosnake/blue/blue_game_hex_grid.hpp"

using namespace blue;

// When we had algorithmic bugs the hexagon never converged on the end condition
// (reaching its goal) so this is a limiter on how far we walk before agreeing
// that there is some problem.
#define SS_TOO_MANY_ADDED  (10u)

namespace
{
    struct hexagon
    {
        hexagon() = default;

        hexagon( int i, int j ) : x( i ), y( j )
        {
        }

        int x = 0;
        int y = 0;
    };

    bool operator== ( const hexagon& lhs, const hexagon& rhs )
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    bool operator!= ( const hexagon& lhs, const hexagon& rhs )
    {
        return !( lhs == rhs );
    }

    std::vector<hexagon> get_hexes_under_line( const hexagon& start, const hexagon& end )
    {
        std::vector<hexagon> tiles;

        auto fn = [&]( int x, int y )->bool
        {
            tiles.push_back( hexagon( x, y ) );
            return tiles.size() < SS_TOO_MANY_ADDED;
        };

        hex_steps_under_line( start.x, start.y, end.x, end.y, fn );

        ASSERT_TRUE( tiles.back() == end );

        return tiles;
    }
}

//////////////////////////////////////////////////////////////////////////

// Circle around 24

TEST( hex_steps_under_line, full_circle_around_24_radius_1 )
{
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 2, 3 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 3, 3 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 3, 4 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 2, 5 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 1, 4 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 1, 3 ) );
}

TEST( hex_steps_under_line, full_circle_around_24_radius_2 )
{
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 2, 2 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 3, 2 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 4, 3 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 4, 4 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 4, 5 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 3, 5 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 2, 6 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 1, 5 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 0, 5 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 0, 4 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 0, 3 ) );
    get_hexes_under_line( hexagon( 2, 4 ), hexagon( 1, 2 ) );
}

TEST( hex_steps_under_line, full_circle_around_34_radius_1 )
{
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 3, 3 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 4, 4 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 4, 5 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 3, 5 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 2, 5 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 2, 4 ) );
}

TEST( hex_steps_under_line, full_circle_around_34_radius_2 )
{
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 3, 2 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 4, 3 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 5, 3 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 5, 4 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 5, 5 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 4, 6 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 3, 6 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 2, 6 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 1, 5 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 1, 4 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 1, 3 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 2, 3 ) );
}


TEST( circle_around_24, from_24_to_23 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 2, 3 ) );
}

TEST( circle_around_24, from_24_to_33 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 3, 3 ) );
}

TEST( circle_around_24, from_24_to_34 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 3, 4 ) );
}

TEST( circle_around_24, from_24_to_25 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 2, 5 ) );
}

TEST( circle_around_24, from_24_to_14 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 1, 4 ) );
}

TEST( circle_around_24, from_24_to_13 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 1, 3 ) );
}



//////////////////////////////////////////////////////////////////////////

// Note: the column makes a difference to the algorithm so repeat
// test in each column (24 is in a different column to 34).



TEST( circle_around_34, from_34_to_33 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 3, 3 ) );
}

TEST( circle_around_34, from_34_to_44 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 4, 4 ) );
}

TEST( circle_around_34, from_34_to_45 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 4, 5 ) );
}

TEST( circle_around_34, from_34_to_35 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 3, 5 ) );
}

TEST( circle_around_34, from_34_to_25 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 2, 5 ) );
}

TEST( circle_around_34, from_34_to_24 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 2, 4 ) );
}

///    _ _ _ _ _               _ _ _ _ _              _ _ _ _ _          //
///   /         \             /         \            /         \         //
///  /           \           /           \          /           \        //
/// /     0,0     \_ _ _ _ _/     2,0     \_ _ _ _ /     4,0     \_ _ _ _//
/// \             /         \             /        \             /       //
///  \           /           \           /          \           /        //
///   \_ _ _ _ _/     1,0     \_ _ _ _ _/     3,0    \_ _ _ _ _/     5,0 //
///   /         \             /         \            /         \         //
///  /           \           /           \          /           \        //
/// /     0,1     \_ _ _ _ _/     2,1     \_ _ _ _ /     4,1     \_ _ _ _//
/// \             /         \             /        \             /       //
///  \           /           \           /          \           /        //
///   \_ _ _ _ _/     1,1     \_ _ _ _ _/     3,1    \_ _ _ _ _/     5,1 //
///   /         \             /         \            /         \         //
///  /           \           /           \          /           \        //
/// /     0,2     \_ _ _ _ _/     2,2     \_ _ _ _ /     4,2     \_ _ _ _//
/// \             /         \             /        \             /       //
///  \           /           \           /          \           /        //
///   \_ _ _ _ _/     1,2     \_ _ _ _ _/     3,2    \_ _ _ _ _/     5,2 //
///   /         \             /         \            /         \         //
///  /           \           /           \          /           \        //
/// /     0,3     \_ _ _ _ _/     2,3     \_ _ _ _ /     4,3     \_ _ _ _//
/// \             /         \             /        \             /       //
///  \           /           \           /          \           /        //
///   \_ _ _ _ _/     1,3     \_ _ _ _ _/     3,3    \_ _ _ _ _/     5,3 //
///   /         \             /         \            /         \         //
///  /           \           /           \          /           \        //
/// /     0,4     \_ _ _ _ _/     2,4     \_ _ _ _ /     4,4     \_ _ _ _//

TEST( hex_steps_under_line, step_from_11_to_01 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 1, 1, 0, 1, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 0, 1 ) );
}

TEST( hex_steps_under_line, step_from_12_to_02 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 1, 2, 0, 2, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 0, 2 ) );
}

TEST( hex_steps_under_line, step_from_01_to_11 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 0, 1, 1, 1, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 1, 1 ) );
}

TEST( hex_steps_under_line, step_from_11_to_10 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 1, 1, 1, 0, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 1, 0 ) );
}

TEST( hex_steps_under_line, step_from_10_to_11 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 1, 0, 1, 1, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 1, 1 ) );
}

TEST( hex_steps_under_line, step_from_10_to_20 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 1, 0, 2, 0, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 2, 0 ) );
}

TEST( hex_steps_under_line, step_from_30_to_40 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 3, 0, 4, 0, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 4, 0 ) );
}

TEST( hex_steps_under_line, step_from_01_to_10 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 0, 1, 1, 0, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 1, 0 ) );
}

TEST( hex_steps_under_line, step_from_11_to_21 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 1, 1, 2, 1, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 2, 1 ) );
}

TEST( hex_steps_under_line, step_from_21_to_30 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 2, 1, 3, 0, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 3, 0 ) );
}

TEST( hex_steps_under_line, step_from_12_to_22 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 1, 2, 2, 2, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 2, 2 ) );
}

TEST( hex_steps_under_line, step_from_13_to_23 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 1, 3, 2, 3, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 2, 3 ) );
}

TEST( hex_steps_under_line, step_from_21_to_11 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 2, 1, 1, 1, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 1, 1 ) );
}

TEST( hex_steps_under_line, step_from_11_to_22 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 1, 1, 2, 2, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 2, 2 ) );
}

TEST( hex_steps_under_line, step_from_22_to_11 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 2, 2, 1, 1, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 1, 1 ) );
}

TEST( hex_steps_under_line, step_from_11_to_12 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 1, 1, 1, 2, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 1, 2 ) );
}

TEST( hex_steps_under_line, step_from_12_to_11 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 1, 2, 1, 1, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 1, 1 ) );
}

TEST( hex_steps_under_line, step_from_11_to_02 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 1, 1, 0, 2, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 0, 2 ) );
}

TEST( hex_steps_under_line, step_from_02_to_11 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 0, 2, 1, 1, fn );

    EXPECT_TRUE( tiles.size() == 1u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 1, 1 ) );
}

//////////////////////////////////////////////////////////////////////////
TEST( circle_around, from_24_to_32 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 3, 2 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 3, 3 ) ) && ( tiles.at( 1 ) == hexagon( 2, 3 ) );
    const bool pathB = ( tiles.at( 0 ) == hexagon( 2, 3 ) ) && ( tiles.at( 1 ) == hexagon( 3, 3 ) );
    EXPECT_TRUE( pathA || pathB );
}

TEST( circle_around, from_24_to_43 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 4, 3 ) );
    EXPECT_TRUE( tiles.size() == 2u );
}

TEST( circle_around, from_24_to_44 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 4, 4 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 3, 3 ) ) && ( tiles.at( 1 ) == hexagon( 3, 4 ) );
    const bool pathB = ( tiles.at( 0 ) == hexagon( 3, 4 ) ) && ( tiles.at( 1 ) == hexagon( 3, 3 ) );
    EXPECT_TRUE( pathA || pathB );
}

TEST( circle_around, from_24_to_45 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 4, 5 ) );
    EXPECT_TRUE( tiles.size() == 2u );
}

TEST( circle_around, from_24_to_35 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 3, 5 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 3, 4 ) ) && ( tiles.at( 1 ) == hexagon( 2, 5 ) );
    const bool pathB = ( tiles.at( 0 ) == hexagon( 2, 5 ) ) && ( tiles.at( 1 ) == hexagon( 3, 4 ) );
    EXPECT_TRUE( pathA || pathB );
}

TEST( circle_around, from_24_to_26 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 2, 6 ) );
    EXPECT_TRUE( tiles.size() == 2u );
}

TEST( circle_around, from_24_to_15 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 1, 5 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 1, 4 ) ) && ( tiles.at( 1 ) == hexagon( 2, 5 ) );
    const bool pathB = ( tiles.at( 0 ) == hexagon( 2, 5 ) ) && ( tiles.at( 1 ) == hexagon( 1, 4 ) );
    EXPECT_TRUE( pathA || pathB );
}

TEST( circle_around, from_24_to_05 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 0, 5 ) );
    EXPECT_TRUE( tiles.size() == 2u );
}

TEST( circle_around, from_24_to_04 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 0, 4 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 1, 4 ) ) && ( tiles.at( 1 ) == hexagon( 1, 3 ) );
    const bool pathB = ( tiles.at( 0 ) == hexagon( 1, 3 ) ) && ( tiles.at( 1 ) == hexagon( 1, 4 ) );
    EXPECT_TRUE( pathA || pathB );
}

TEST( circle_around, from_24_to_03 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 0, 3 ) );
    EXPECT_TRUE( tiles.size() == 2u );
}

TEST( circle_around, from_24_to_12 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 1, 2 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 2, 3 ) ) && ( tiles.at( 1 ) == hexagon( 1, 3 ) );
    const bool pathB = ( tiles.at( 0 ) == hexagon( 1, 3 ) ) && ( tiles.at( 1 ) == hexagon( 2, 3 ) );
    EXPECT_TRUE( pathA || pathB );
}

TEST( circle_around, from_24_to_02 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 0, 2 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 1, 3 ) ) && ( tiles.at( 1 ) == hexagon( 1, 2 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_24_to_31 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 3, 1 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 2, 3 ) ) && ( tiles.at( 1 ) == hexagon( 3, 2 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_24_to_42 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 4, 2 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 3, 3 ) ) && ( tiles.at( 1 ) == hexagon( 3, 2 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_24_to_52 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 5, 2 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 3, 3 ) ) && ( tiles.at( 1 ) == hexagon( 4, 3 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_24_to_53 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 5, 3 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 3, 3 ) ) && ( tiles.at( 1 ) == hexagon( 4, 4 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_24_to_54 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 5, 4 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 3, 4 ) ) && ( tiles.at( 1 ) == hexagon( 4, 4 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_24_to_46 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 4, 6 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 3, 4 ) ) && ( tiles.at( 1 ) == hexagon( 3, 5 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_24_to_36 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 3, 6 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 2, 5 ) ) && ( tiles.at( 1 ) == hexagon( 3, 5 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_24_to_27 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 2, 7 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 2, 5 ) ) && ( tiles.at( 1 ) == hexagon( 2, 6 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_24_to_16 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 1, 6 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 2, 5 ) ) && ( tiles.at( 1 ) == hexagon( 1, 5 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_24_to_06 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 0, 6 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 1, 4 ) ) && ( tiles.at( 1 ) == hexagon( 1, 5 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_44_to_15 )
{
    auto tiles = get_hexes_under_line( hexagon( 4, 4 ), hexagon( 1, 5 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 3, 4 ) ) && ( tiles.at( 1 ) == hexagon( 2, 5 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_44_to_14 )
{
    auto tiles = get_hexes_under_line( hexagon( 4, 4 ), hexagon( 1, 4 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 3, 4 ) ) && ( tiles.at( 1 ) == hexagon( 2, 4 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_44_to_13 )
{
    auto tiles = get_hexes_under_line( hexagon( 4, 4 ), hexagon( 1, 3 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 3, 3 ) ) && ( tiles.at( 1 ) == hexagon( 2, 4 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_44_to_12 )
{
    auto tiles = get_hexes_under_line( hexagon( 4, 4 ), hexagon( 1, 2 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 3, 3 ) ) && ( tiles.at( 1 ) == hexagon( 2, 3 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_44_to_22 )
{
    auto tiles = get_hexes_under_line( hexagon( 4, 4 ), hexagon( 2, 2 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 3, 3 ) ) && ( tiles.at( 1 ) == hexagon( 3, 2 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_44_to_31 )
{
    auto tiles = get_hexes_under_line( hexagon( 4, 4 ), hexagon( 3, 1 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 4, 3 ) ) && ( tiles.at( 1 ) == hexagon( 3, 2 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_24_to_11 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 1, 1 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 2, 3 ) ) && ( tiles.at( 1 ) == hexagon( 1, 2 ) );
    EXPECT_TRUE( pathA );
}

TEST( circle_around, from_24_to_21 )
{
    auto tiles = get_hexes_under_line( hexagon( 2, 4 ), hexagon( 2, 1 ) );
    EXPECT_TRUE( tiles.size() == 3u );
    const bool pathA = ( tiles.at( 0 ) == hexagon( 2, 3 ) ) && ( tiles.at( 1 ) == hexagon( 2, 2 ) );
    EXPECT_TRUE( pathA );
}

//////////////////////////////////////////////////////////////////////////

TEST( circle_around, from_34_to_32 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 3, 2 ) );
    EXPECT_TRUE( tiles.size() == 2u );
}

TEST( circle_around, from_34_to_43 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 4, 3 ) );
    EXPECT_TRUE( tiles.size() == 3u );
}

TEST( circle_around, from_34_to_53 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 5, 3 ) );
    EXPECT_TRUE( tiles.size() == 2u );
}

TEST( circle_around, from_34_to_54 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 5, 4 ) );
    EXPECT_TRUE( tiles.size() == 3u );
}

TEST( circle_around, from_34_to_55 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 5, 5 ) );
    EXPECT_TRUE( tiles.size() == 2u );
}

TEST( circle_around, from_34_to_46 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 4, 6 ) );
    EXPECT_TRUE( tiles.size() == 3u );
}

TEST( circle_around, from_34_to_36 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 3, 6 ) );
    EXPECT_TRUE( tiles.size() == 2u );
}

TEST( circle_around, from_34_to_26 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 2, 6 ) );
    EXPECT_TRUE( tiles.size() == 3u );
}

TEST( circle_around, from_34_to_15 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 1, 5 ) );
    EXPECT_TRUE( tiles.size() == 2u );
}

TEST( circle_around, from_34_to_14 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 1, 4 ) );
    EXPECT_TRUE( tiles.size() == 3u );
}

TEST( circle_around, from_34_to_13 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 1, 3 ) );
    EXPECT_TRUE( tiles.size() == 2u );
    EXPECT_TRUE( tiles.at(0) == hexagon( 2, 4 ) );
    EXPECT_TRUE( tiles.at(1) == hexagon( 1, 3 ) );
}

TEST( circle_around, from_34_to_23 )
{
    auto tiles = get_hexes_under_line( hexagon( 3, 4 ), hexagon( 2, 3 ) );
    EXPECT_TRUE( tiles.size() == 3u );
}

//////////////////////////////////////////////////////////////////////////

TEST( hex_steps_under_line, bug_2_January_2015 )
{
    // All of these were observed to fail.
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 2, 4 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 4, 4 ) );
    get_hexes_under_line( hexagon( 3, 4 ), hexagon( 1, 3 ) );
    get_hexes_under_line( hexagon( 1, 1 ), hexagon( 0, 1 ) );
    get_hexes_under_line( hexagon( 1, 2 ), hexagon( 0, 2 ) );
    get_hexes_under_line( hexagon( 1, 0 ), hexagon( 2, 0 ) );
    get_hexes_under_line( hexagon( 1, 1 ), hexagon( 2, 1 ) );
    get_hexes_under_line( hexagon( 1, 2 ), hexagon( 2, 2 ) );
    get_hexes_under_line( hexagon( 1, 3 ), hexagon( 2, 3 ) );
    get_hexes_under_line( hexagon( 1, 1 ), hexagon( 2, 2 ) );
    get_hexes_under_line( hexagon( 1, 1 ), hexagon( 3, 2 ) );
}

TEST( hex_steps_under_line, bug_3_January_2015 )
{
    // All of these were observed to fail.
    get_hexes_under_line( hexagon( 1, 1 ), hexagon( 2, 2 ) );
    get_hexes_under_line( hexagon( 1, 1 ), hexagon( 3, 2 ) );
    get_hexes_under_line( hexagon( 2, 2 ), hexagon( 3, 2 ) );
}

//////////////////////////////////////////////////////////////////////////

TEST( hex_steps_under_line, step_from_01_to_03 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 0, 1, 0, 3, fn );

    EXPECT_TRUE( tiles.size() == 2u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 0, 2 ) );
    EXPECT_TRUE( tiles.at( 1 ) == hexagon( 0, 3 ) );
}

/// The correct path joining the centers of 0,0 and 1,7 touches the      //
/// following hexes:                                                     //
///                                                                      //
///    _ _ _ _ _                                                         //
///   /         \                                                        //
///  /           \                                                       //
/// /     0,0     \                                                      //
/// \             /                                                      //
///  \           /                                                       //
///   \_ _ _ _ _/                                                        //
///   /         \                                                        //
///  /           \                                                       //
/// /     0,1     \                                                      //
/// \             /                                                      //
///  \           /                                                       //
///   \_ _ _ _ _/                                                        //
///   /         \                                                        //
///  /           \                                                       //
/// /     0,2     \                                                      //
/// \             /                                                      //
///  \           /                                                       //
///   \_ _ _ _ _/                                                        //
///   /         \                                                        //
///  /           \                                                       //
/// /     0,3     \_ _ _ _ _                                             //
/// \             /         \                                            //
///  \           /           \                                           //
///   \_ _ _ _ _/     1,3     \                                          //
///   /         \             /                                          //
///  /           \           /                                           //
/// /     0,4     \_ _ _ _ _/                                            //
/// \             /         \                                            //
///  \           /           \                                           //
///   \_ _ _ _ _/     1,4     \                                          //
///             \             /                                          //
///              \           /                                           //
///               \_ _ _ _ _/                                            //
///               /         \                                            //
///              /           \                                           //
///             /     1,5     \                                          //
///             \             /                                          //
///              \           /                                           //
///               \_ _ _ _ _/                                            //
///               /         \                                            //
///              /           \                                           //
///             /     1,6     \                                          //
///             \             /                                          //
///              \           /                                           //
///               \_ _ _ _ _/                                            //
///               /         \                                            //
///              /           \                                           //
///             /     1,7     \                                          //
///             \             /                                          //
///              \           /                                           //
///               \_ _ _ _ _/                                            //
TEST( hex_steps_under_line, step_from_00_to_17 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < 20u;
    };

    hex_steps_under_line( 0, 0, 1, 7, fn );

    EXPECT_TRUE( tiles.size() == 9u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 0, 1 ) );
    EXPECT_TRUE( tiles.at( 1 ) == hexagon( 0, 2 ) );
    EXPECT_TRUE( tiles.at( 2 ) == hexagon( 0, 3 ) );
    EXPECT_TRUE( tiles.at( 3 ) == hexagon( 1, 3 ) );
    EXPECT_TRUE( tiles.at( 4 ) == hexagon( 0, 4 ) );
    EXPECT_TRUE( tiles.at( 5 ) == hexagon( 1, 4 ) );
    EXPECT_TRUE( tiles.at( 6 ) == hexagon( 1, 5 ) );
    EXPECT_TRUE( tiles.at( 7 ) == hexagon( 1, 6 ) );
    EXPECT_TRUE( tiles.at( 8 ) == hexagon( 1, 7 ) );
}

TEST( hex_steps_under_line, step_from_17_to_00 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 1, 7, 0, 0, fn );

    EXPECT_TRUE( tiles.size() == 9u );
    EXPECT_TRUE( tiles.at( 8 ) == hexagon( 0, 0 ) );
    EXPECT_TRUE( tiles.at( 7 ) == hexagon( 0, 1 ) );
    EXPECT_TRUE( tiles.at( 6 ) == hexagon( 0, 2 ) );
    EXPECT_TRUE( tiles.at( 5 ) == hexagon( 0, 3 ) );
    EXPECT_TRUE( tiles.at( 4 ) == hexagon( 1, 3 ) );
    EXPECT_TRUE( tiles.at( 3 ) == hexagon( 0, 4 ) );
    EXPECT_TRUE( tiles.at( 2 ) == hexagon( 1, 4 ) );
    EXPECT_TRUE( tiles.at( 1 ) == hexagon( 1, 5 ) );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 1, 6 ) );
}

/// The correct path between 0,1 and 4,3:
///    _ _ _ _ _                                                         //
///   /         \                                                        //
///  /           \                                                       //
/// /     0,1     \_ _ _ _ _                                             //
/// \             /         \                                            //
///  \           /           \                                           //
///   \_ _ _ _ _/     1,1     \_ _ _ _ _                                 //
///   /         \             /         \                                //
///  /           \           /           \                               //
/// /             \_ _ _ _ _/     2,2     \_ _ _ _                       //
/// \             /         \             /        \                     //
///  \           /           \           /          \                    //
///   \_ _ _ _ _/             \_ _ _ _ _/     3,2    \_ _ _ _ _          //
///   /         \             /         \            /         \         //
///  /           \           /           \          /           \        //
/// /             \_ _ _ _ _/             \_ _ _ _ /     4,3     \       //
/// \             /         \             /        \             /       //
///  \           /           \           /          \           /        //
///   \_ _ _ _ _/             \_ _ _ _ _/            \_ _ _ _ _/         //
TEST( hex_steps_under_line, step_from_01_to_43 )
{
    std::vector<hexagon> tiles;

    auto fn = [&]( int x, int y )->bool
    {
        tiles.push_back( hexagon( x, y ) );
        return tiles.size() < SS_TOO_MANY_ADDED;
    };

    hex_steps_under_line( 0, 1, 4, 3, fn );

    EXPECT_TRUE( tiles.size() == 4u );
    EXPECT_TRUE( tiles.at( 0 ) == hexagon( 1, 1 ) );
    EXPECT_TRUE( tiles.at( 1 ) == hexagon( 2, 2 ) );
    EXPECT_TRUE( tiles.at( 2 ) == hexagon( 3, 2 ) );
    EXPECT_TRUE( tiles.at( 3 ) == hexagon( 4, 3 ) );
}
