#define RUN_TESTS
#ifdef RUN_TESTS

#include "solosnake/testing/testing.hpp"
#include <memory>
#include "solosnake/external/xml.hpp"
#include "solosnake/blue/blue_asteroid.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_sun.hpp"
#include "solosnake/blue/blue_xml_board.hpp"

using namespace blue;
using namespace solosnake;

namespace
{
    std::unique_ptr<TiXmlElement> make_element( const char* const name )
    {
        return std::unique_ptr<TiXmlElement>( new TiXmlElement( name ) );
    }

    void save_element_to_xml_doc( std::unique_ptr<TiXmlElement> e, const char* const filename )
    {
        std::unique_ptr<TiXmlDocument> doc( new TiXmlDocument() );
        doc->LinkEndChild( new TiXmlDeclaration( "1.0", "", "" ) );
        doc->LinkEndChild( e.release() );
        doc->SaveFile( filename );
    }
}

TEST( read_and_write_hexgrid, read_and_write_small_grid )
{
    const auto filename = "write_small_grid.xml";
    const auto rootelement = "board";

    hexgrid grid( 1, 1, 0u, 0xF );
    auto b = make_element( rootelement );
    write_hexgrid( grid, *b );
    save_element_to_xml_doc( std::move( b ), filename );

    TiXmlDocument doc( filename );
    EXPECT_TRUE( doc.LoadFile() );

    auto root = solosnake::get_child_element( doc, rootelement );

    const hexgrid loadedgrid = read_hexgrid( root );

    EXPECT_TRUE( loadedgrid == grid );
}

TEST( read_and_write_hexgrid, read_and_write_bigger_grid )
{
    const auto filename = "write_bigger_grid.xml";
    const auto rootelement = "board";

    const HexContentFlags invalid = HEX_FLAGS_OFFBOARD;
    const HexContentFlags empty   = 0u;
    const HexContentFlags content = HEX_FLAGS_BOT;

    hexgrid grid( hexgrid::MaxPermittedGridWidth, hexgrid::MaxPermittedGridHeight, empty, invalid );

    // Add some contents around the grid.
    for( std::int8_t x = 0; x < grid.grid_height(); ++x )
    {
        for( std::int8_t y = 0; y <  grid.grid_width(); ++y )
        {
            auto xy = hexcoord::make_coord( x, y );

            EXPECT_TRUE( grid.is_in_board_bounds( xy ) );

            if( x % 3 && y % 2 )
            {
                grid.set_contents( xy, content );
            }
            else if( x % 2 )
            {
                grid.set_contents( xy, invalid );
            }
        }
    }

    auto b = make_element( rootelement );
    write_hexgrid( grid, *b );
    save_element_to_xml_doc( std::move( b ), filename );

    TiXmlDocument doc( filename );
    EXPECT_TRUE( doc.LoadFile() );

    auto root = solosnake::get_child_element( doc, rootelement );
    const hexgrid loadedgrid = read_hexgrid( root );
    EXPECT_TRUE( loadedgrid == grid );
}

TEST( read_and_write_asteroids, asteroids )
{
    const auto filename = "asteroids.xml";
    const auto rootelement = "test";
    auto b = make_element( rootelement );

    std::vector<asteroid> asteroids;
    asteroids.push_back( asteroid(PanguiteOre,  0x000Fu, hexcoord::make_coord(1,2) ) );
    asteroids.push_back( asteroid(KamaciteOre,  0x00FFu, hexcoord::make_coord(3,4) ) );
    asteroids.push_back( asteroid(AtaxiteOre,   0x0FFFu, hexcoord::make_coord(5,6) ) );
    asteroids.push_back( asteroid(ChondriteOre, 0xFFFFu, hexcoord::make_coord(7,8) ) );
    asteroids.back().set_as_removed_from_board();

    write_asteroids( asteroids, *b );
    save_element_to_xml_doc( std::move( b ), filename );

    TiXmlDocument doc( filename );
    EXPECT_TRUE( doc.LoadFile() );

    auto root = solosnake::get_child_element( doc, rootelement );
    const auto loadedasteroids = read_asteroids( root );
    EXPECT_TRUE( loadedasteroids == asteroids );
}

TEST( read_and_write_suns, suns )
{
    const auto filename = "suns.xml";
    const auto rootelement = "test";
    auto b = make_element( rootelement );

    std::vector<sun> suns;
    suns.push_back( sun(SunStrength0, SunType3, hexcoord::make_coord(1,2) ) );
    suns.push_back( sun(SunStrength1, SunType2, hexcoord::make_coord(3,4) ) );
    suns.push_back( sun(SunStrength2, SunType1, hexcoord::make_coord(5,6) ) );
    suns.push_back( sun(SunStrength3, SunType0, hexcoord::make_coord(7,8) ) );

    write_suns( suns, *b );
    save_element_to_xml_doc( std::move( b ), filename );

    TiXmlDocument doc( filename );
    EXPECT_TRUE( doc.LoadFile() );

    auto root = solosnake::get_child_element( doc, rootelement );
    const auto loadedsuns = read_suns( root );
    EXPECT_TRUE( loadedsuns == suns );
}


#endif
