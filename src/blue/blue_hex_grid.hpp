#ifndef blue_hex_grid_hpp
#define blue_hex_grid_hpp

#include <cstdint>
#include <vector>
#include "solosnake/blue/blue_unaliased.hpp"
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_hexcontentflags.hpp"
#include "solosnake/blue/blue_hexfacingname.hpp"

#define SS_SQRT_3 (1.7320508075688772935274463415059f)

namespace blue
{
    class Half_line_3d;
    class Rectf;

    ///                                                                      //
    /// See http://www.redblobgames.com/grids/hexagons/                      //
    ///                                                                      //
    /// W = width, S = size, H = height (2 units).                           //
    ///                                                                      //
    /// In the vertical orientation, the width of a hexagon                  //
    /// is width = size * 2.                                                 //
    ///                                                                      //
    /// The horizontal distance between adjacent hexes is                    //
    /// horiz = 3/4 * width.                                                 //
    ///                                                                      //
    /// The height of a hexagon is height = sqrt(3)/2 * width.               //
    ///                                                                      //
    /// The vertical distance between adjacent hexes is                      //
    /// vert = height.                                                       //
    ///                                                                      //
    ///    _ _ _ _ _                                                         //
    ///   /         \       |                                                //
    ///  /           \      |                                                //
    /// /       ______\     | H = 2    _______________                       //
    /// \         S   /     |             W = 2 x S                          //
    ///  \           /      |                                                //
    ///   \_ _ _ _ _/       |                                                //
    ///                                                                      //
    /// We set the height of an unscaled hexagon to be 2.0 so we             //
    /// can inscribe a unit circle inside the hexagon.                       //
    ///                                                                      //
    /// If the height is 2.0, the width is 4 / sqrt(3) ~= 2.31               //
    ///                                                                      //
    /// The Hex_grid will use the "odd-q" layout for its public              //
    /// coordinate system. These coordinates can range from 0 to             //
    /// (MaxPermittedGridWidth-1) in X and from 0 to                         //
    /// (MaxPermittedGridHeight-1) in Y.                                     //
    ///                                                                      //
    /// Public ("Odd-d") Coordinates (x,y):                                  //
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
    ///                                                                      //
    /// "odd-q" is related to cube coordinates x,y,z by the following        //
    /// conversion:                                                          //
    ///                                                                      //
    /// coord.q = cubic.x                                                    //
    /// coord.r = cubic.z + (cubic.x - (cubic.x & 1)) / 2                    //
    ///                                                                      //
    /// cubic.x = coord.q                                                    //
    /// cubic.z = coord.r - (coord.q - (coord.q & 1) / 2                     //
    /// cubic.y = -cubic.x - cubic.z                                         //
    ///                                                                      //
    /// axial coordinates are related to cubic coordinates by the            //
    /// constraint that                                                      //
    /// cubic.x + cubic.y + cubic.z = 0 so we only need to store 2           //
    /// coordinates.                                                         //
    ///                                                                      //
    /// Axial coordinates (x,z)                                              //
    ///    _ _ _ _ _               _ _ _ _ _                                 //
    ///   /         \             /         \                                //
    ///  /           \           /           \                               //
    /// /     0,0     \_ _ _ _ _/     2,-1    \_ _ _ _                       //
    /// \             /         \             /                              //
    ///  \           /           \           /                               //
    ///   \_ _ _ _ _/     1,0     \_ _ _ _ _/     3,-1                       //
    ///   /         \             /         \                                //
    ///  /           \           /           \                               //
    /// /     0,1     \_ _ _ _ _/     2,0     \_ _ _ _                       //
    /// \             /         \             /                              //
    ///  \           /           \           /                               //
    ///   \_ _ _ _ _/     1,1     \_ _ _ _ _/     3,0                        //
    ///   /         \             /         \                                //
    ///  /           \           /           \                               //
    /// /     0,2     \_ _ _ _ _/     2,1     \_ _ _ _                       //
    ///                                                                      //
    /// The grid stores one HexContentFlags per hexagonal tile,              //
    /// and allows queries for returning these HexContentFlags.              //
    /// The user should use the flags to store board data.                   //
    ///                                                                      //
    /// The Hex_grid supports the notion of an invalid tile flag.            //
    /// This is the value returned for queries which lie outside             //
    /// of the board.                                                        //
    ///                                                                      //
    /// The neighbours are named as follows, with coords for                 //
    /// illustration of axis directions:                                     //
    ///                                                                      //
    ///                _ _ _ _ _                                             //
    ///               /         \ NOTE: Axial Coords (not the public coords) //
    ///              /           \                                           //
    ///    _ _ _ _ _/    Tile0    \_ _ _ _ _                                 //
    ///   /         \     1,0     /         \                                //
    ///  /           \           /           \                               //
    /// /    Tile5    \_ _ _ _ _/    Tile1    \                              //
    /// \     0,1     /         \     2,1     /                              //
    ///  \           /           \           /                               //
    ///   \_ _ _ _ _/     1,1     \_ _ _ _ _/                                //
    ///   /         \             /         \                                //
    ///  /           \           /           \                               //
    /// /    Tile4    \_ _ _ _ _/    Tile2    \                              //
    /// \     0,2     /         \     2,2     /                              //
    ///  \           /           \           /                               //
    ///   \_ _ _ _ _/    Tile3    \_ _ _ _ _/                                //
    ///             \     1,2     /                                          //
    ///              \           /                                           //
    ///               \_ _ _ _ _/                                            //
    ///                                                                      //
    ///                                                                      //
    /// These are how the coordinates change as you move from                //
    /// tile to tile:                                                        //
    ///                _ _ _ _ _                                             //
    ///               /         \ NOTE: Axial Coords (not the public coords) //
    ///              /           \                                           //
    ///    _ _ _ _ _/    Tile0    \_ _ _ _ _                                 //
    ///   /         \    +0,-1    /         \                                //
    ///  /           \           /           \                               //
    /// /    Tile5    \_ _ _ _ _/    Tile1    \                              //
    /// \    -1,+0    /         \    +1,-1    /                              //
    ///  \           /           \           /                               //
    ///   \_ _ _ _ _/     1,1     \_ _ _ _ _/                                //
    ///   /         \             /         \                                //
    ///  /           \           /           \                               //
    /// /    Tile4    \_ _ _ _ _/    Tile2    \                              //
    /// \    -1,+1    /         \    +1,+0    /                              //
    ///  \           /           \           /                               //
    ///   \_ _ _ _ _/    Tile3    \_ _ _ _ _/                                //
    ///             \    +0,+1    /                                          //
    ///              \           /                                           //
    ///               \_ _ _ _ _/                                            //
    ///                                                                      //
    ///                                                                      //
    /// Note: There is no convenient way to store the coordinate changes     //
    /// as you move from tile to tile in the public coordinate system:       //
    /// hence the use of the Axial system internally.                        //
    ///
    class Hex_grid
    {
    public:

        /// These limits interact with A* decision weighting and many other
        /// design decisions made in the code - do not change them without
        /// carefully examining the code.
        enum HexMaxDimensions
        {
            MaxPermittedGridWidth  = 100,
            MaxPermittedGridHeight = 100
        };

        struct Hex_tile
        {
            Hex_coord        tile_coordinate;
            HexContentFlags  tile_contents;
        };

        struct Neighbouring_hex_tile : public Hex_tile
        {
            /// Returns the facing of this neighbour as viewed from the tile to
            /// which this neighbours.
            HexFacingName facing() const { return static_cast<HexFacingName>( relative_facing ); }

            std::int8_t relative_facing; ///< HexFacingName as uint8_t.
        };

        class Neighbours
        {
            friend class Hex_grid;

        public:
            const Neighbouring_hex_tile* begin() const noexcept                { return neighbours_; }
            const Neighbouring_hex_tile* end() const noexcept                  { return neighbours_ + size_; }
            size_t size() const noexcept                                       { return size_; }
            const Neighbouring_hex_tile& operator[]( size_t i ) const noexcept { return neighbours_[i]; }
            const Hex_tile& centre() const noexcept                            { return centre_; }

        private:
            Neighbouring_hex_tile neighbours_[6];
            Hex_tile              centre_;
            std::uint8_t          size_;
        };

        /// Constructs an empty 0 x 0 grid with invalid tile value zero.
        Hex_grid() = default;

        /// Constructs a grid containing @a width x @a height tiles.
        /// width and height must be BETWEEN 0 and max value. We restrict the
        /// board size to these dimensions for practical reasons of rendering it.
        /// A 100 x 100 board has 10,000 tiles, each needing 6 vertices, or
        /// 60,000 vertices. The max indexable limit is 65535.
        ///
        /// @param invalidTile The invalidTile flag is the flag that indicates
        ///                    this tile is not part of the board.
        /// @param defaultCont The initial width x height tiles will be set to
        ///                    this value.
        Hex_grid(
            unsigned int width,
            unsigned int height,
            HexContentFlags defaultContent,
            HexContentFlags invalidTile );

        /// Returns the value (a set of flags) for board coord @a xy.
        /// Throws if @a xy is not a valid Hex_coord for this grid.
        HexContentFlags contents_at( const Hex_coord xy ) const;

        /// Returns the value (a set of flags) for board coord @a xy.
        /// Assumes that @a xy is a valid Hex_coord for this grid.
        /// @pre @a xy is a valid Hex_coord for this grid.
        HexContentFlags contents( const Hex_coord xy ) const noexcept;

        /// contents.at(Hex_coord) = flags
        /// Throws if the coordinate is not on the board.
        void set_contents( const Hex_coord, const HexContentFlags flags );

        /// contents.at(Hex_coord) = flags
        /// Checks that the Hex_coord is within the grid.
        void set_contents_at( const Hex_coord, const HexContentFlags flags );

        /// Returns the invalid tile flags value - this is the value for tiles
        /// outside of the board bounds.
        HexContentFlags invalid_tile_value() const noexcept;

        /// Returns the number of non-invalid tiles on the board. An invalid
        /// tile is one which has one or more flags which match the invalid
        /// tile value.
        unsigned int valid_tiles_count() const noexcept;

        /// Returns the max possible number of tiles (same as width x height).
        unsigned int max_grid_size() const noexcept;

        /// Returns the number of islands (sets of tiles connected to one
        /// another and also being unconnected to some other sets of tiles).
        unsigned int number_of_islands() const noexcept;

        /// Returns true if @a xy is inside the board array dimensions.
        /// Validity of the contents of @a xy is not considered, only location.
        bool is_in_board_bounds( const Hex_coord xy ) const noexcept;

        /// Returns true if @a xy is a valid tile of the board.
        bool is_on_board( const Hex_coord xy ) const noexcept;

        /// Returns details of this tile and its (up to) 6 neighbouring tiles
        /// which pass the mask test. The coord must lie on the grid - if it is
        /// not within the grid bounds the Neighbours are always returned empty.
        /// If mask is zero all tiles will pass the test.
        Neighbours get_neighbours(
            const Hex_coord,
            HexContentFlags mask ) const noexcept;

        /// Returns the contents of the neighbour. This might return
        /// invalidTile if the tile is out of bounds.
        HexContentFlags get_neighbour_contents(
            const Hex_coord,
            const HexFacingName ) const noexcept;

        /// If the tile is on the board, returns true and fills in the
        /// Hex_coord @a n, else returns false.
        bool try_get_neighbour_tile(
            const Hex_coord,
            const HexFacingName,
            Hex_coord& n ) const noexcept;

        /// Fills the array with details of any content tiles, including invalid
        /// tiles, within the circle of radius r at the given coord which pass
        /// the mask test.
        /// If mask is zero all tiles will pass the test.
        /// Results are sorted by distance from the centre, closest first.
        /// The order the results appear around the centre begins at the
        /// lower left tile and moves counter clockwise around the circle.
        /// A radius of zero returns the original non-invalid coord specified.
        void get_sorted_circle_contents(
            const Hex_coord,
            const std::uint16_t r,
            std::vector<Hex_tile>&,
            HexContentFlags mask ) const;

        /// Assuming the board is in the Y = boardY plane, if the line intersects
        /// a location within the board bounds, returns the HexContentFlags of
        /// that location (this might be the invalid tile). If the intersection
        /// lies outside of the board bounds, then the Hex_tile returned is the
        /// invalid coord and the contents are the invalid HexContentFlags. If
        /// the intersection lies within the board bounds, then the contents
        /// may be invalid, but the coordinate will be valid.
        Hex_tile get_line_intersection(
            const Half_line_3d& line,
            const float boardY ) const;

        /// Clears and fills the vector v the coordinates of any tile lying
        /// beneath the straight line from A to B, and whose contents pass the
        /// mask test, and which is no further from A than @a maxSteps.
        /// The A tile is always excluded from the returned results.
        /// @note Has no effect if A and B are same point: no contents are
        /// gathered as no "line" is described. This function cannot and should
        /// not be used to get the contents under a single hex.
        /// @pre A and B must be on the board.
        void contents_under_line(
            const Hex_coord A,
            const Hex_coord B,
            const std::uint16_t maxSteps,
            const HexContentFlags mask,
            std::vector<Hex_coord>& v ) const;

        /// Width refers to the X coord.
        int grid_width() const noexcept;

        /// Height refers to Y coord.
        int grid_height() const noexcept;

        /// How many (valid and invalid) tiles within the board bounds.
        size_t grid_size() const noexcept;

        /// If we create a 3d box with the Hex_grid as its base, and the height
        /// of the box above the base being the same as the shortest side of the
        /// base, this call returns the length of the diagonal stretching from the two
        /// corners furthest apart. This call is intended to be used for calculating
        /// max view distances for cameras etc.
        float get_short_side_3d_box_diagonal() const;

        /// Returns the board's bounding rectangle.
        Rectf surface_rect() const;

        /// Calculates the float x,y coordinate for the coord @a c.
        static void calculate_xy(
            const Hex_coord c,
            float* unaliased fxy );

        /// Returns the radius of the circle that fits inside the hexagonal
        /// tile and touches the sides, the inscribed
        static constexpr float tile_inner_radius() noexcept;

        /// Calculates the unbounded grid coordinate for world x and world y.
        /// x and y are coordinates on the scaled world board. The returned
        /// coordinate might lie outside the bounds of the grid (and be
        /// invalid). The returned coordinate should be checked to see if it
        /// is a valid coord for a given board.
        static Hex_coord calculate_coord(
            const float x,
            const float y );

        /// Returns the coordinates of the neighbour of @a c in the
        /// direction @a f which is @a n steps away.
        static Hex_coord calculate_neighbour_coord(
            const Hex_coord c,
            const HexFacingName f,
            const int n );

        /// Returns the coordinates of the neighbour of @a c in the
        /// direction @a f.
        static Hex_coord facing_neighbour_coord(
            const Hex_coord c,
            const HexFacingName facing );

        /// Returns the facing direction of the neighbour of c. Requires that
        /// neighbour is a next tile neighbour of c.
        static HexFacingName neighbors_facing(
            const Hex_coord c,
            const Hex_coord neighbour );

        /// Sets 18 floats, as an array of xyz[6], the vertex coordinates of the
        /// hexagon at (x,y), where the hexagon tiles are scaled up or down by
        /// scaleHexagon (which allows for the hexagon to be shrunk or enlarged
        /// while maintaining spacing). The scaling does not change the distance
        /// between hexagons, nor their location, only the returned vertex
        /// locations.
        /// The board is considered to be at Y=0 the x-z plane.
        static void get_hexagon_vertices(
            const Hex_coord xy,
            const float scaleHexagon,
            float* unaliased fx3x6 );

        /// Returns 0-5, the number of turns required to turn from the current
        /// facing to face the given coordinate (as best as possible, e.g. bring
        /// it into the facing quadrant).
        /// From the return value it is possible to calculate which turn
        /// direction is shorter to turn in.
        static unsigned int get_clockwise_turn_count(
            const Hex_coord xyFrom,
            const HexFacingName facing,
            const Hex_coord xyTo );

        /// Returns the number of tiles encompassed by a hexagonally
        /// tiled circle. Radius 0 gives 1, 1 gives 7, 2 gives 19 etc
        /// The minimum returned values is 1.
        static unsigned int count_of_hexagons_inside_ring(
            const unsigned int radius );

        /// Returns the number of steps to get from Hex_coord A to Hex_coord B.
        /// If A == B this returns 0. If A and B are Neighbours, 1 etc.
        /// This is not a path finding method, and does not take into
        /// account the board contents. It is a 'as the crow flies' board
        /// distance measurement, the number of moves needed to get from A to
        /// B.
        static std::uint16_t get_step_distance_between(
            const Hex_coord A,
            const Hex_coord B );

        bool operator == ( const Hex_grid& rhs ) const noexcept;
        bool operator != ( const Hex_grid& rhs ) const noexcept;

    private:

        /// Permit a friend class to access our members.
        friend class to_from_json;

        /// Ctor used by serialisation.
        Hex_grid(
            std::vector<HexContentFlags>&& grid,
            unsigned int width,
            unsigned int height,
            HexContentFlags invalidTile );

        /// Counts the number of valid tiles in the grid array, using the
        /// invalid flag.
        unsigned int count_valid_tiles() const;

        /// Convert axial coordinates to an offset into the array by converting
        /// to odd-q coordinates first.
        inline int axial_index( const int x, const int z ) const noexcept;

    private:

        std::vector<HexContentFlags> grid_{};
        std::int8_t                  width_{0U};
        std::int8_t                  height_{0U};
        HexContentFlags              invalid_tile_value_{0};
    };
}

#include "solosnake/blue/blue_hex_grid.inl"
#endif
