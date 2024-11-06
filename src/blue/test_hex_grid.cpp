#include <memory>
#include <iostream>
#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_hexcontentflags.hpp"
#include "solosnake/blue/blue_game_hex_grid.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

namespace
{
    constexpr auto BLUE_TEST_INVALID_TILE_FLAG  {HEX_FLAGS_OFF_BOARD};
    constexpr auto BLUE_TEST_NO_CONTENT         {0};
    constexpr auto BLUE_TEST_CONTENT            {HEX_FLAGS_BOT};

    std::ostream& operator<<( std::ostream& os, const blue::HexFacingName f )
    {
        os << std::int16_t( f );
        return os;
    }

    void print_grid( size_t w, size_t h, const std::vector<Hex_grid::Hex_tile>& tiles )
    {
    }
}

TEST( Hex_grid, count_of_hexagons_inside_ring )
{
    EXPECT_EQ( 1, Hex_grid::count_of_hexagons_inside_ring( 0 ) );
    EXPECT_EQ( 7, Hex_grid::count_of_hexagons_inside_ring( 1 ) );
    EXPECT_EQ( 19, Hex_grid::count_of_hexagons_inside_ring( 2 ) );
    EXPECT_EQ( 37, Hex_grid::count_of_hexagons_inside_ring( 3 ) );
    EXPECT_EQ( 61, Hex_grid::count_of_hexagons_inside_ring( 4 ) );
    EXPECT_EQ( 91, Hex_grid::count_of_hexagons_inside_ring( 5 ) );
    EXPECT_EQ( 127, Hex_grid::count_of_hexagons_inside_ring( 6 ) );
    EXPECT_EQ( 169, Hex_grid::count_of_hexagons_inside_ring( 7 ) );
    EXPECT_EQ( 169, 1 + ( 6 ) + ( 2 * 6 ) + ( 3 * 6 ) + ( 4 * 6 ) + ( 5 * 6 ) + ( 6 * 6 ) + ( 7 * 6 ) );
}

TEST( Hex_grid, get_circle_contents_0_00 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 0, 0 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 0, tiles, 0u );
    EXPECT_EQ( 0, tiles.at( 0 ).tile_coordinate.x );
    EXPECT_EQ( 0, tiles.at( 0 ).tile_coordinate.y );
    EXPECT_EQ( BLUE_TEST_NO_CONTENT, tiles.at( 0 ).tile_contents );
}

TEST( Hex_grid, get_circle_contents_1_11 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 1, 1 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 1, tiles, 0u );

    EXPECT_EQ( 1, tiles.at( 0 ).tile_coordinate.x );
    EXPECT_EQ( 1, tiles.at( 0 ).tile_coordinate.y );

    EXPECT_EQ( 0, tiles.at( 1 ).tile_coordinate.x );
    EXPECT_EQ( 2, tiles.at( 1 ).tile_coordinate.y );

    EXPECT_EQ( 1, tiles.at( 2 ).tile_coordinate.x );
    EXPECT_EQ( 2, tiles.at( 2 ).tile_coordinate.y );

    EXPECT_EQ( 2, tiles.at( 3 ).tile_coordinate.x );
    EXPECT_EQ( 2, tiles.at( 3 ).tile_coordinate.y );

    EXPECT_EQ( 2, tiles.at( 4 ).tile_coordinate.x );
    EXPECT_EQ( 1, tiles.at( 4 ).tile_coordinate.y );

    EXPECT_EQ( 1, tiles.at( 5 ).tile_coordinate.x );
    EXPECT_EQ( 0, tiles.at( 5 ).tile_coordinate.y );

    EXPECT_EQ( 0, tiles.at( 6 ).tile_coordinate.x );
    EXPECT_EQ( 1, tiles.at( 6 ).tile_coordinate.y );

    EXPECT_EQ( 7, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_like_drawing_2_33 )
{
    Hex_grid g( 9, 9, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 3, 3 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 2, tiles, 0u );
    print_grid( 9, 9, tiles );
    EXPECT_EQ( 19, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_like_drawing_2_34 )
{
    Hex_grid g( 9, 9, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 3, 4 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 2, tiles, 0u );
    print_grid( 9, 9, tiles );
    EXPECT_EQ( 19, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_2_33 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 3, 3 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 2, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 19, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_1_00 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 0, 0 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 1, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 3, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_1_10 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 1, 0 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 1, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 6, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_1_01 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 0, 1 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 1, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 5, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_1_02 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 0, 2 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 1, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 5, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_2_00 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 0, 0 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 2, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 7, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_1_99 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 9, 9 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 1, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 3, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_1_06 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 0, 6 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 1, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 5, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_1_07 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 0, 7 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 1, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 5, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_0_07 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 0, 7 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 0, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 1, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_1_18 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 1, 8 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 1, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 7, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_1_09 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 0, 9 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 1, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 4, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_1_90 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 9, 0 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 1, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 4, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_radius_4_22 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 2, 2 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 4, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 41, tiles.size() );
}

TEST( Hex_grid, get_circle_contents_offgrid )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 10, 10 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 1, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( 0, tiles.size() );
}

TEST( Hex_grid, get_sorted_circle_contents_radius_1_90 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 9, 0 };
    std::vector<Hex_grid::Hex_tile> tiles, sortedtiles;
    g.get_sorted_circle_contents( xy, 1, tiles, 0u );
    print_grid( 10, 10, tiles );
    g.get_sorted_circle_contents( xy, 1, sortedtiles, 0u );
    EXPECT_EQ( 4, sortedtiles.size() );
    print_grid( 10, 10, sortedtiles );
}

TEST( Hex_grid, get_circle_contents_encompass_grid )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 5, 5 };
    std::vector<Hex_grid::Hex_tile> tiles;
    g.get_sorted_circle_contents( xy, 10, tiles, 0u );
    print_grid( 10, 10, tiles );
    EXPECT_EQ( g.grid_size(), tiles.size() );
}

TEST( Hex_grid, get_neighbours_of_11 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 1, 1 };
    Hex_grid::Neighbours n = g.get_neighbours( xy, 0u );

    // Check that the tiles are laid out as we require and document.
    EXPECT_TRUE( n.size() == 6u );
    EXPECT_TRUE( n[0].tile_coordinate == Hex_coord::make_coord( 1, 0 ) );
    EXPECT_TRUE( n[1].tile_coordinate == Hex_coord::make_coord( 2, 1 ) );
    EXPECT_TRUE( n[2].tile_coordinate == Hex_coord::make_coord( 2, 2 ) );
    EXPECT_TRUE( n[3].tile_coordinate == Hex_coord::make_coord( 1, 2 ) );
    EXPECT_TRUE( n[4].tile_coordinate == Hex_coord::make_coord( 0, 2 ) );
    EXPECT_TRUE( n[5].tile_coordinate == Hex_coord::make_coord( 0, 1 ) );
}

TEST( Hex_grid, get_neighbours_of_34 )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 3, 4 };
    Hex_grid::Neighbours n = g.get_neighbours( xy, 0u );

    // Check that the tiles are laid out as we require and document.
    EXPECT_TRUE( n.size() == 6u );
    EXPECT_TRUE( n[0].tile_coordinate == Hex_coord::make_coord( 3, 3 ) );
    EXPECT_TRUE( n[1].tile_coordinate == Hex_coord::make_coord( 4, 4 ) );
    EXPECT_TRUE( n[2].tile_coordinate == Hex_coord::make_coord( 4, 5 ) );
    EXPECT_TRUE( n[3].tile_coordinate == Hex_coord::make_coord( 3, 5 ) );
    EXPECT_TRUE( n[4].tile_coordinate == Hex_coord::make_coord( 2, 5 ) );
    EXPECT_TRUE( n[5].tile_coordinate == Hex_coord::make_coord( 2, 4 ) );
}

TEST( Hex_grid, get_neighbours_3_00 )
{
    Hex_grid g( 3, 3, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 0, 0 };

    Hex_grid::Neighbours n = g.get_neighbours( xy, 0u );

    EXPECT_EQ( n.size(), 2 );

    EXPECT_EQ( std::int16_t( n[0].facing() ), std::int16_t( 2 ) );
    EXPECT_EQ( n[0].tile_coordinate.x, 1 );
    EXPECT_EQ( n[0].tile_coordinate.y, 0 );

    EXPECT_EQ( std::int16_t( n[1].facing() ), std::int16_t( 3 ) );
    EXPECT_EQ( n[1].tile_coordinate.x, 0 );
    EXPECT_EQ( n[1].tile_coordinate.y, 1 );
}

TEST( Hex_grid, get_neighbours_3_01 )
{
    Hex_grid g( 3, 3, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 0, 1 };

    Hex_grid::Neighbours n = g.get_neighbours( xy, 0u );

    EXPECT_EQ( n.size(), 4 );
    EXPECT_EQ( std::int16_t( n[0].facing() ), std::int16_t( 0 ) );
    EXPECT_EQ( std::int16_t( n[1].facing() ), std::int16_t( 1 ) );
    EXPECT_EQ( std::int16_t( n[2].facing() ), std::int16_t( 2 ) );
    EXPECT_EQ( std::int16_t( n[3].facing() ), std::int16_t( 3 ) );
}

TEST( Hex_grid, get_neighbours_3_20 )
{
    Hex_grid g( 3, 3, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 2, 0 };

    Hex_grid::Neighbours n = g.get_neighbours( xy, 0u );

    EXPECT_EQ( n.size(), 2 );
    EXPECT_EQ( std::int16_t( n[0].facing() ), std::int16_t( 3 ) );
    EXPECT_EQ( std::int16_t( n[1].facing() ), std::int16_t( 4 ) );
}

TEST( Hex_grid, get_neighbours_3_11 )
{
    Hex_grid g( 3, 3, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 1, 1 };

    Hex_grid::Neighbours n = g.get_neighbours( xy, 0u );

    EXPECT_EQ( n.size(), 6 );
    EXPECT_EQ( std::int16_t( n[0].facing() ), std::int16_t( 0 ) );
    EXPECT_EQ( std::int16_t( n[1].facing() ), std::int16_t( 1 ) );
    EXPECT_EQ( std::int16_t( n[2].facing() ), std::int16_t( 2 ) );
    EXPECT_EQ( std::int16_t( n[3].facing() ), std::int16_t( 3 ) );
    EXPECT_EQ( std::int16_t( n[4].facing() ), std::int16_t( 4 ) );
    EXPECT_EQ( std::int16_t( n[5].facing() ), std::int16_t( 5 ) );
}

TEST( Hex_grid, get_neighbours_3_21 )
{
    Hex_grid g( 3, 3, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 2, 1 };

    Hex_grid::Neighbours n = g.get_neighbours( xy, 0u );

    EXPECT_EQ( n.size(), 4 );
    EXPECT_EQ( std::int16_t( n[0].facing() ), 0 );
    EXPECT_EQ( std::int16_t( n[1].facing() ), 3 );
    EXPECT_EQ( std::int16_t( n[2].facing() ), 4 );
    EXPECT_EQ( std::int16_t( n[3].facing() ), 5 );
}

TEST( Hex_grid, get_neighbours_3_22 )
{
    Hex_grid g( 3, 3, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 2, 2 };

    Hex_grid::Neighbours n = g.get_neighbours( xy, 0u );

    EXPECT_EQ( n.size(), 3 );
    EXPECT_EQ( std::int16_t( n[0].facing() ), 0 );
    EXPECT_EQ( std::int16_t( n[1].facing() ), 4 );
    EXPECT_EQ( std::int16_t( n[2].facing() ), 5 );
}

TEST( Hex_grid, get_neighbours_3_02 )
{
    Hex_grid g( 3, 3, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 0, 2 };

    Hex_grid::Neighbours n = g.get_neighbours( xy, 0u );

    EXPECT_EQ( n.size(), 3 );
    EXPECT_EQ( std::int16_t( n[0].facing() ), 0 );
    EXPECT_EQ( std::int16_t( n[1].facing() ), 1 );
    EXPECT_EQ( std::int16_t( n[2].facing() ), 2 );
}

TEST( Hex_grid, get_neighbours_colours5_22 )
{
    Hex_grid g( 5, 5, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_coord xy = { 2, 2 };

    Hex_grid::Neighbours n = g.get_neighbours( xy, 0u );

    EXPECT_EQ( n.size(), 6 );

    EXPECT_EQ( std::int16_t( n[0].facing() ), 0 );
    EXPECT_EQ( std::int16_t( n[1].facing() ), 1 );
    EXPECT_EQ( std::int16_t( n[2].facing() ), 2 );
    EXPECT_EQ( std::int16_t( n[3].facing() ), 3 );
    EXPECT_EQ( std::int16_t( n[4].facing() ), 4 );
    EXPECT_EQ( std::int16_t( n[5].facing() ), 5 );

    EXPECT_EQ( 2, n[0].tile_coordinate.x );
    EXPECT_EQ( 1, n[0].tile_coordinate.y );

    EXPECT_EQ( n[1].tile_coordinate.x, 3 );
    EXPECT_EQ( n[1].tile_coordinate.y, 1 );

    EXPECT_EQ( n[2].tile_coordinate.x, 3 );
    EXPECT_EQ( n[2].tile_coordinate.y, 2 );

    EXPECT_EQ( n[3].tile_coordinate.x, 2 );
    EXPECT_EQ( n[3].tile_coordinate.y, 3 );

    EXPECT_EQ( n[4].tile_coordinate.x, 1 );
    EXPECT_EQ( n[4].tile_coordinate.y, 2 );

    EXPECT_EQ( n[5].tile_coordinate.x, 1 );
    EXPECT_EQ( n[5].tile_coordinate.y, 1 );
}

TEST( Hex_grid, coord_to_uint16 )
{
    {
        Hex_coord c = { 0, 0 };
        auto x = Hex_coord::to_uint16( c );
        auto xc = Hex_coord::from_uint16( x );
        EXPECT_EQ( xc.x, c.x );
        EXPECT_EQ( xc.y, c.y );
    }

    {
        Hex_coord c = { 0, 100 };
        auto x = Hex_coord::to_uint16( c );
        auto xc = Hex_coord::from_uint16( x );
        EXPECT_EQ( xc.x, c.x );
        EXPECT_EQ( xc.y, c.y );
    }

    {
        Hex_coord c = { 100, 0 };
        auto x = Hex_coord::to_uint16( c );
        auto xc = Hex_coord::from_uint16( x );
        EXPECT_EQ( xc.x, c.x );
        EXPECT_EQ( xc.y, c.y );
    }

    {
        Hex_coord c = { 101, 125 };
        auto x = Hex_coord::to_uint16( c );
        auto xc = Hex_coord::from_uint16( x );
        EXPECT_EQ( xc.x, c.x );
        EXPECT_EQ( xc.y, c.y );
    }
}

TEST( Hex_grid, coord_from_uint16 )
{
    {
        std::uint16_t i = 0x0;
        auto c = Hex_coord::from_uint16( i );
        auto ic = Hex_coord::to_uint16( c );
        EXPECT_EQ( i, ic );
    }

    {
        std::uint16_t i = 0x1234;
        auto c = Hex_coord::from_uint16( i );
        auto ic = Hex_coord::to_uint16( c );
        EXPECT_EQ( i, ic );
    }

    {
        std::uint16_t i = 0xFFFF;
        auto c = Hex_coord::from_uint16( i );
        auto ic = Hex_coord::to_uint16( c );
        EXPECT_EQ( i, ic );
    }
}

TEST( Hex_grid, equals )
{
    Hex_grid a( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_grid b( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_grid c( 10,  9, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_grid d(  9, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );

    EXPECT_TRUE( a == b );
    EXPECT_FALSE( a == c );
    EXPECT_FALSE( a == d );
    EXPECT_FALSE( c == d );

    auto xy = Hex_coord::make_coord( 0, 1 );
    a.set_contents( xy, BLUE_TEST_CONTENT );
    EXPECT_FALSE( a == b );
}

TEST( Hex_grid, not_equals )
{
    Hex_grid a( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_grid b( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_grid c( 10,  9, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    Hex_grid d(  9, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );

    EXPECT_FALSE( a != b );
    EXPECT_TRUE( a != c );
    EXPECT_TRUE( a != d );
    EXPECT_TRUE( c != d );

    auto xy = Hex_coord::make_coord( 0, 1 );
    a.set_contents( xy, BLUE_TEST_CONTENT );
    EXPECT_TRUE( a != b );
}

TEST( Hex_grid, step_distance_between_same_coords_is_zero )
{
    for(int8_t y = 0; y < Hex_grid::MaxPermittedGridWidth; ++y)
    {
        for(int8_t x = 0; x < Hex_grid::MaxPermittedGridHeight; ++x)
        {
            auto xy = Hex_coord::make_coord(x,y);
            auto d1 = Hex_grid::get_step_distance_between(xy, xy);
            EXPECT_TRUE(d1 == 0);
        }
    }
}

TEST( Hex_grid, set_contents_and_get_contents )
{
    Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );

    int tested = 0;

    for( std::uint8_t x = 0u; x < 10u; ++x )
    {
        for( std::uint8_t y = 0u; y < 10u; ++y )
        {
            auto xy = Hex_coord::make_coord( x, y );
            if( g.is_in_board_bounds( xy ) )
            {
                EXPECT_FALSE( g.contents( xy ) == BLUE_TEST_CONTENT );
                g.set_contents( xy, BLUE_TEST_CONTENT );
                EXPECT_TRUE( g.contents( xy ) == BLUE_TEST_CONTENT );
                g.set_contents( xy, BLUE_TEST_NO_CONTENT );
                EXPECT_TRUE( g.contents( xy ) == BLUE_TEST_NO_CONTENT );
                ++tested;
            }
        }
    }

    EXPECT_TRUE( tested > 0 );
}

namespace
{
    void start_to_end_in_min_step( std::int8_t x0, std::int8_t y0, std::int8_t x1, std::int8_t y1 )
    {
        Hex_grid g( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );

        Hex_coord start = { x0, y0 };
        Hex_coord end = { x1, y1 };
        uint16_t maxSteps = Hex_grid::get_step_distance_between( start, end );

        g.set_contents( start, BLUE_TEST_CONTENT );
        g.set_contents( end, BLUE_TEST_CONTENT );

        auto n = g.get_neighbours( start, 0 );

        Game_hex_grid gg( std::move( g ) );

        ASSERT_TRUE( gg.contents( start ) == BLUE_TEST_CONTENT );
        ASSERT_TRUE( gg.contents( end )   == BLUE_TEST_CONTENT );

        std::vector<Hex_coord> hits;
        gg.contents_under_line( start, end, maxSteps, hits );

        ASSERT_TRUE( !hits.empty() );
        // ASSERT_TRUE( end == hits.at( 0 ) );
    }
}

TEST( Hex_grid, lines_in_minimum_steps )
{
    start_to_end_in_min_step( 1, 1, 0, 1 );
    start_to_end_in_min_step( 0, 0, 1, 0 );
    start_to_end_in_min_step( 0, 0, 2, 1 );
    start_to_end_in_min_step( 0, 0, 3, 1 );
}

TEST( Hex_grid, to_from_json )
{
    Hex_grid h( 10, 10, BLUE_TEST_NO_CONTENT, BLUE_TEST_INVALID_TILE_FLAG );
    h.set_contents_at( Hex_coord::make_coord(0,0), BLUE_TEST_CONTENT );
    h.set_contents_at( Hex_coord::make_coord(2,1), BLUE_TEST_CONTENT );
    h.set_contents_at( Hex_coord::make_coord(1,2), BLUE_TEST_CONTENT );
    h.set_contents_at( Hex_coord::make_coord(5,6), BLUE_TEST_CONTENT );
    h.set_contents_at( Hex_coord::make_coord(9,9), BLUE_TEST_CONTENT );

    nlohmann::json j = h;
    auto h2 = j.get<blue::Hex_grid>();
    EXPECT_EQ( h, h2 );
}

