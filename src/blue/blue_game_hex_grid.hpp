#ifndef blue_gamehexgrid_hpp
#define blue_gamehexgrid_hpp

#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_hexfacingname.hpp"
#include "solosnake/blue/blue_hexcontentflags.hpp"
#include "solosnake/blue/blue_rect.hpp"
#include "solosnake/blue/blue_sunstrength.hpp"

namespace blue
{
    class Line_hit;

    /// A hex grid which also knows about game concepts.
    /// A tile is reserved if it is occupied or pending occupation by a moving
    /// board piece. The grid lies with the 0,0 tile centred at the world
    /// location (0,0). Only valid tiles are considered part of the board.
    class Game_hex_grid
    {
    public:

        /// Constructs an empty 0 x 0 grid with invalid tile value zero.
        Game_hex_grid();

        explicit Game_hex_grid( Hex_grid&& );

        HexContentFlags invalid_tile_value() const noexcept;

        HexContentFlags contents( Hex_coord ) const;

        HexContentFlags get_neighbour_contents( const Hex_coord, const HexFacingName ) const;

        SunStrength tile_sun_strength( Hex_coord ) const;

        /// Throws if an Asteroid cannot be placed at coordinate. Asteroids can only
        /// be placed on empty unreserved tile locations.
        void place_asteroid_on_tile( const Hex_coord );

        /// Throws if a Sun cannot be placed at coordinate. Tiles suns are placed
        /// onto will become off-board tiles.
        void place_sun_on_tile( SunStrength strength, const Hex_coord );

        /// Sets up the tile as if a bot were added to it (as opposed to moved
        /// to it from a nearby tile).
        void place_bot_on_tile( const Hex_coord );

        void move_bot_from_tile_to_tile( const Hex_coord from, const Hex_coord to );

        /// Completely removes a bot from @a xy, leaving the tile empty.
        void remove_bot_from_tile( const Hex_coord );

        void remove_asteroid_from_tile( const Hex_coord );

        ///  A hex is reserved when a bot is on it or moving onto or off it, or
        ///  when it is somehow occupied by something.
        ///  It is an error to call this on a coordinate off the board or which
        ///  is invalid.
        void reserve_empty_tile( const Hex_coord );

        /// An unreserved tile is a tile free to be moved onto or upon which
        /// something can be placed. It has no occupant.
        void unreserve_empty_tile( const Hex_coord );

        /// A board is a 2D W x H grid of tiles. Returns the number of tiles wide
        // the board is.
        unsigned int grid_width() const noexcept;

        /// A board is a 2D W x H grid of tiles. Returns the number of tiles wide
        // the board is.
        unsigned int grid_height() const noexcept;

        bool is_on_board( const Hex_coord ) const noexcept;

        bool is_tile_reserved( const Hex_coord ) const noexcept;

        /// Returns true if the tile is on the board and available. Returns false
        /// if the tile is invalid, not on the board, or reserved.
        bool is_tile_empty_and_unreserved( const Hex_coord ) const noexcept;

        /// If the tile is on the board, returns true and fills in the coordinate, else
        // returns false.
        bool try_get_neighbour_tile( const Hex_coord, const HexFacingName, Hex_coord& ) const noexcept;

        /// Fills the array with details of any tile within the circle of radius r at the
        /// given coordinate which match the mask, sorted by increasing distance from centre.
        /// If mask is zero all tiles will pass the test. By default returns
        /// information on all tiles.
        void get_sorted_circle_contents( const Hex_coord,
                                         const std::uint16_t r,
                                         std::vector<Hex_grid::Hex_tile>& tiles,
                                         HexContentFlags flags = 0u ) const;

        std::uint16_t get_step_distance_between( const Hex_coord A, const Hex_coord B ) const;

        /// Clears and fills the vector v the coordinates of any tiles which contain a bot,
        /// a Sun or an Asteroid, and which is no further from A than @a maxSteps. The A tile
        /// is always excluded from the returned results.
        ///
        /// @pre A and B must be on the board.
        void contents_under_line( const Hex_coord A,
                                  const Hex_coord B,
                                  const std::uint16_t maxSteps,
                                  std::vector<Hex_coord>& v ) const;

        Rectf surface_rect() const noexcept;

        const Hex_grid&   grid() const noexcept;

        /// Returns the HexContentFlags of the hex board tile intersected by the
        /// half-line. Returns invalid_tile if there is no tile or an invalid tile at
        /// the intersection location. Places the board at Y=0 in the x-z plane.
        Hex_grid::Hex_tile get_line_intersection( const Half_line_3d& ) const;

        bool operator == (const Game_hex_grid&) const noexcept;
        bool operator != (const Game_hex_grid&) const noexcept;

    private:

        HexContentFlags sun_flag_contents( Hex_coord xy ) const;

    private:

        Hex_grid                         grid_;
    };
}

#include "solosnake/blue/blue_game_hex_grid.inl"
#endif
