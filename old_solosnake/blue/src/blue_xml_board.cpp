#include <cassert>
#include <cstdint>
#include <string>
#include "solosnake/blue/blue_asteroid.hpp"
#include "solosnake/blue/blue_hexcontentflags.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_sun.hpp"
#include "solosnake/blue/blue_sunstrength.hpp"
#include "solosnake/blue/blue_tradables.hpp"
#include "solosnake/blue/blue_xml_board.hpp"
#include "solosnake/external/xml.hpp"
#include "solosnake/throw.hpp"

#define BLUE_HEXGRID_ELEMENT_NAME               "hexgrid"
#define     BLUE_HEXGRID_WIDTH_ATTRIB_NAME      "width"
#define     BLUE_HEXGRID_HEIGHT_ATTRIB_NAME     "height"
#define     BLUE_HEXGRID_INVALID_ATTRIB_NAME    "invalid"
#define BLUE_TILE_ELEMENT_NAME                  "hex"
#define BLUE_COORD_ATTRIB_NAME                  "xy"
#define BLUE_ASTEROIDS_ELEMENT_NAME             "asteroids"
#define BLUE_ASTEROID_ELEMENT_NAME              "asteroid"
#define     BLUE_ASTEROID_ORE_ATTRIB_NAME       "ore"
#define     BLUE_ASTEROID_VOLUME_ATTRIB_NAME    "volume"
#define     BLUE_ASTEROID_ONBOARD_ATTRIB_NAME   "onboard"
#define BLUE_SUNS_ELEMENT_NAME                  "suns"
#define     BLUE_SUN_ELEMENT_NAME               "sun"
#define     BLUE_SUN_TYPE_ATTRIB_NAME           "type"
#define     BLUE_SUN_STRENGTH_ATTRIB_NAME       "strength"



using namespace solosnake;
using namespace std;

//! As a space optimisation, we do not write any contents with this value, and we initialise
//! any newly created grid which is about to be filled from XML to have this value by default.
//! This we only need to read and write values which differ from this.
#define BLUE_DEFAULT_XML_GRID_CONTENT   (0u)

namespace blue
{

    //---------------------------------------------------------------------------------------------

    hexgrid read_hexgrid( TiXmlElement& docRoot )
    {
        TiXmlElement& gridElement = get_child_element( docRoot, BLUE_HEXGRID_ELEMENT_NAME );

        unsigned int width  = 0u;
        unsigned int height = 0u;
        HexContentFlags invalidTile    = 0u;

        read_attribute( gridElement, BLUE_HEXGRID_WIDTH_ATTRIB_NAME,   width,       false );
        read_attribute( gridElement, BLUE_HEXGRID_HEIGHT_ATTRIB_NAME,  height,      false );
        read_attribute( gridElement, BLUE_HEXGRID_INVALID_ATTRIB_NAME, invalidTile, false );

        const HexContentFlags defaultContent = BLUE_DEFAULT_XML_GRID_CONTENT;
        hexgrid grid( width, height, defaultContent, invalidTile );

        TiXmlElement* tileElement = gridElement.FirstChildElement( BLUE_TILE_ELEMENT_NAME );

        while( tileElement )
        {
            uint16_t xy   = 0u;
            uint8_t flags = 0u;
            read_attribute( *tileElement, BLUE_COORD_ATTRIB_NAME, xy, false );
            flags = static_cast<HexContentFlags>( stoul( tileElement->GetText() ) );
            assert( grid.contents( Hex_coord::from_uint16( xy ) ) == defaultContent );
            grid.set_contents( Hex_coord::from_uint16( xy ), flags );
            tileElement = tileElement->NextSiblingElement( BLUE_TILE_ELEMENT_NAME );
        }

        return grid;
    }


    void write_hexgrid( const hexgrid& grid, TiXmlElement& docRoot )
    {
        auto gridElement = make_unique<TiXmlElement>( BLUE_HEXGRID_ELEMENT_NAME );

        gridElement->SetAttribute( BLUE_HEXGRID_WIDTH_ATTRIB_NAME,   grid.grid_width() );
        gridElement->SetAttribute( BLUE_HEXGRID_HEIGHT_ATTRIB_NAME,  grid.grid_height() );
        gridElement->SetAttribute( BLUE_HEXGRID_INVALID_ATTRIB_NAME, grid.invalid_tile_value() );

        TiXmlComment comment( "Each '" BLUE_TILE_ELEMENT_NAME "' is a grid tile. '"
                              BLUE_COORD_ATTRIB_NAME "' is the encoded coordinate." );

        gridElement->InsertEndChild( comment );

        for( int8_t y = 0; y < grid.grid_height(); ++y )
        {
            for( int8_t x = 0; x < grid.grid_width(); ++x )
            {
                const auto xy = Hex_coord::make_coord( x, y );
                const auto contents = grid.contents( xy );

                if( contents != BLUE_DEFAULT_XML_GRID_CONTENT )
                {
                    const auto content = static_cast<unsigned long long>( grid.contents( xy ) );
                    TiXmlElement tile( BLUE_TILE_ELEMENT_NAME );
                    tile.SetAttribute( BLUE_COORD_ATTRIB_NAME, Hex_coord::to_uint16( xy ) );
                    tile.InsertEndChild( TiXmlText( to_string( content ) ) );
                    gridElement->InsertEndChild( tile );
                }
            }
        }

        docRoot.LinkEndChild( gridElement.release() );
    }

    //---------------------------------------------------------------------------------------------

    vector<asteroid> read_asteroids( TiXmlElement& docRoot )
    {
        vector<asteroid> asteroids;

        auto& asteroidsElement = get_child_element( docRoot, BLUE_ASTEROIDS_ELEMENT_NAME );

        auto* asteroidElement = asteroidsElement.FirstChildElement( BLUE_ASTEROID_ELEMENT_NAME );

        while( asteroidElement )
        {
            uint8_t  ore  = 0u;
            uint16_t vol  = 0u;
            uint16_t xy   = 0u;
            bool onboard  = true;
            read_attribute( *asteroidElement, BLUE_COORD_ATTRIB_NAME,           xy,  false );
            read_attribute( *asteroidElement, BLUE_ASTEROID_ORE_ATTRIB_NAME,    ore, false );
            read_attribute( *asteroidElement, BLUE_ASTEROID_VOLUME_ATTRIB_NAME, vol, false );
            read_attribute( *asteroidElement, BLUE_ASTEROID_ONBOARD_ATTRIB_NAME, onboard, true );

            if( ! is_ore( ore ) )
            {
                ss_throw( "Invalid '" BLUE_ASTEROID_ORE_ATTRIB_NAME "' value." );
            }

            asteroids.emplace_back(
                asteroid( static_cast<OreTypes>( ore ), vol, Hex_coord::from_uint16( xy ) ) );

            if( ! onboard )
            {
                asteroids.back().set_as_removed_from_board();
            }

            asteroidElement = asteroidElement->NextSiblingElement( BLUE_ASTEROID_ELEMENT_NAME );
        }

        return asteroids;
    }

    //! Writes the array of asteroids as a child element of @a docRoot.
    void write_asteroids( const vector<asteroid>& asteroids, TiXmlElement& docRoot )
    {
        auto asteroidsElement = make_unique<TiXmlElement>( BLUE_ASTEROIDS_ELEMENT_NAME );

        TiXmlElement asteroidElement( BLUE_ASTEROID_ELEMENT_NAME );

        for( size_t i = 0u; i < asteroids.size(); ++i )
        {
            const int ore  = static_cast<int>( asteroids[i].ore_type() );
            const int vol  = static_cast<int>( asteroids[i].volume() );
            const int xy   = static_cast<int>( Hex_coord::to_uint16( asteroids[i].board_xy() ) );

            asteroidElement.SetAttribute( BLUE_ASTEROID_ORE_ATTRIB_NAME,    ore );
            asteroidElement.SetAttribute( BLUE_ASTEROID_VOLUME_ATTRIB_NAME, vol );
            asteroidElement.SetAttribute( BLUE_COORD_ATTRIB_NAME,           xy );

            if( ! asteroids[i].is_on_board() )
            {
                asteroidElement.SetAttribute( BLUE_ASTEROID_ONBOARD_ATTRIB_NAME, "false" );
            }

            asteroidsElement->InsertEndChild( asteroidElement );
        }

        docRoot.LinkEndChild( asteroidsElement.release() );
    }

    //---------------------------------------------------------------------------------------------

    vector<sun> read_suns( TiXmlElement& docRoot )
    {
        vector<sun> suns;

        auto& sunsElement = get_child_element( docRoot, BLUE_SUNS_ELEMENT_NAME );

        auto* sunElement  = sunsElement.FirstChildElement( BLUE_SUN_ELEMENT_NAME );

        while( sunElement )
        {
            uint8_t  suntype     = 0u;
            uint16_t sunstrength = 0u;
            uint16_t xy          = 0u;
            read_attribute( *sunElement, BLUE_COORD_ATTRIB_NAME,        xy,          false );
            read_attribute( *sunElement, BLUE_SUN_TYPE_ATTRIB_NAME,     suntype,     false );
            read_attribute( *sunElement, BLUE_SUN_STRENGTH_ATTRIB_NAME, sunstrength, false );

            if( ! can_be_a_sun_strength( sunstrength ) )
            {
                ss_throw( "Value of '" BLUE_SUN_STRENGTH_ATTRIB_NAME "' out of range." );
            }

            if( ! can_be_a_suntype( suntype ) )
            {
                ss_throw( "Value of '" BLUE_SUN_TYPE_ATTRIB_NAME "' out of range." );
            }

            suns.emplace_back( sun( static_cast<SunStrength>( sunstrength ),
                                    static_cast<SunType>( suntype ),
                                    Hex_coord::from_uint16( xy ) ) );

            sunElement = sunElement->NextSiblingElement( BLUE_SUN_ELEMENT_NAME );
        }

        return suns;
    }

    void write_suns( const std::vector<sun>& suns, TiXmlElement& docRoot )
    {
        auto sunsElement = make_unique<TiXmlElement>( BLUE_SUNS_ELEMENT_NAME );

        TiXmlElement sunElement( BLUE_SUN_ELEMENT_NAME );

        for( size_t i = 0u; i < suns.size(); ++i )
        {
            const int suntype     = static_cast<int>( suns[i].sun_type() );
            const int sunstrength = static_cast<int>( suns[i].sun_strength() );
            const int xy          = static_cast<int>( Hex_coord::to_uint16( suns[i].board_xy() ) );

            sunElement.SetAttribute( BLUE_SUN_TYPE_ATTRIB_NAME,     suntype );
            sunElement.SetAttribute( BLUE_SUN_STRENGTH_ATTRIB_NAME, sunstrength );
            sunElement.SetAttribute( BLUE_COORD_ATTRIB_NAME,        xy );

            sunsElement->InsertEndChild( sunElement );
        }

        docRoot.LinkEndChild( sunsElement.release() );
    }

}
