#ifndef blue_xml_board_hpp
#define blue_xml_board_hpp

#include <memory>
#include <vector>
#include "solosnake/external/xml_fwd.hpp"

namespace blue
{
    class Asteroid;
    class Sun;
    class Machine;
    class Hex_grid;

    //! Finds the hexgrid child node of @a docRoot and reads the hexgrid contained
    //! in it.
    Hex_grid read_hexgrid( TiXmlElement& docRoot );

    //! Finds the suns child node of @a docRoot and reads the array of asteroids
    //! contained in it.
    std::vector<Sun> read_suns( TiXmlElement& docRoot );

    //! Finds the asteroids child node of @a docRoot and reads the array of
    //! asteroids contained in it.
    std::vector<Asteroid> read_asteroids( TiXmlElement& docRoot );

    //! Finds the machines child node of @a docRoot and reads the array of machines
    //! contained in it.
    std::vector<std::unique_ptr<Machine>> read_machines( TiXmlElement& docRoot );

    //! Writes the hexgrid @a grid as an child element of @a docRoot.
    void write_hexgrid( const Hex_grid& grid, TiXmlElement& docRoot );

    //! Writes the array of suns as an child element of @a docRoot.
    void write_suns( const std::vector<Sun>& suns, TiXmlElement& docRoot );

    //! Writes the array of asteroids as a child element of @a docRoot.
    void write_asteroids( const std::vector<Asteroid>& asteroids, TiXmlElement& docRoot );

}

#endif
