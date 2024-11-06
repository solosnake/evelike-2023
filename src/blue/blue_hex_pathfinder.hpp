#ifndef blue_hexpathfinder_hpp
#define blue_hexpathfinder_hpp

#include <memory>
#include <stack>
#include <vector>
#include "solosnake/blue/blue_hexfacingname.hpp"

namespace blue
{
    struct Hex_coord;

    class Game_hex_grid;
    class Hex_path_search;
    class Hex_path_costs;
    class Instruction;
    class Machine_grid_view;

    /// A* search algorithm manager for use with the hexagonal game grid. The
    /// user can request searches to be performed - these may occur on another
    /// thread - and then request the search results. If no path can be found,
    /// the empty path is returned. If a path is found then an array of coords
    /// is returned, and it will always contain both the start and end coords.
    /// If the requested search was the identity search (to move zero steps to
    /// the tile it starts at) then a path containing only one tile is returned.
    class Hex_pathfinder
    {
    public:

        Hex_pathfinder();

        ~Hex_pathfinder();

        /// Searches for a path from a towards b, with no coord in the
        /// returned path being further from a than @a search_radius. During
        /// the A* search no coords will be considered if they are further from
        /// a than search_radius and so this may mean the search will not find
        /// paths.
        ///
        /// If no route could be found than any path or path Instructions
        /// queried for will be empty.
        ///
        /// If @a path_limit is non zero then any returned path towards b will
        /// be limited to containing 1 + path_limit coords.
        ///
        /// @param search_centre Points further away from this point than
        /// search_radius will not be considered when looking for a path. Zero
        /// search_radius will mean no path can be detected.
        Hex_path_search* search_for_path_between(
            const Hex_coord a,
            const Hex_coord b,
            const Machine_grid_view grid,
            const Hex_path_costs& costs,
            const HexFacingName facing,
            const Hex_coord search_centre,
            const std::uint16_t search_radius,
            const std::uint16_t path_limit );

        /// Returns the Instructions to navigate the path requested - returns
        /// empty instruction vector if no path could be found.
        const std::vector<Instruction>& get_path_instructions( Hex_path_search*& );

        /// Returns the path found - this will be empty if no path could be
        /// found. If a path was found it will contain the start and end (a
        /// and b) locations.
        const std::vector<Hex_coord>& get_path( Hex_path_search*& );

        void cancel( Hex_path_search*& );

        bool operator == (const Hex_pathfinder& rhs) const noexcept;

    private:

        std::vector<std::unique_ptr<Hex_path_search>> tasks_;
        std::stack<Hex_path_search*>                  idle_tasks_;
    };

    //-------------------------------------------------------------------------

    /// Sets the contents of @a code to be the Instructions needed to
    /// navigate @a path. path[0] is assumed to be the starting point.
    void make_path_instructions(
        const std::vector<Hex_coord>& path,
        const HexFacingName initial_facing,
        std::vector<Instruction>& code );
}

#endif
