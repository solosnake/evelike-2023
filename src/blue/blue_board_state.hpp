#ifndef blue_board_state_hpp
#define blue_board_state_hpp

#include <cstdint>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <vector>
#include <utility>
#include "solosnake/blue/blue_asteroid.hpp"
#include "solosnake/blue/blue_board_state_grid_view.hpp"
#include "solosnake/blue/blue_game_hex_grid.hpp"
#include "solosnake/blue/blue_good_rand.hpp"
#include "solosnake/blue/blue_hit_by_line.hpp"
#include "solosnake/blue/blue_machine.hpp"
#include "solosnake/blue/blue_machine_event.hpp"
#include "solosnake/blue/blue_secondary_event.hpp"
#include "solosnake/blue/blue_sun.hpp"
#include "solosnake/blue/blue_team.hpp"
#include "solosnake/blue/blue_trade_request.hpp"

namespace blue
{
    class Hex_pathfinder;

    /// The changes that take place in one frame.
    /// The contents of this are only valid
    struct Frame_events
    {
        std::vector<Machine_event>      machine_events;
        std::vector<Secondary_event>     secondary_events;
    };

    /// Owns the board, and also the details of the fleet and end game
    /// condition, but unaware of graphical / user level concepts like
    /// selections etc. This class is the game kernel.
    ///
    /// This class should have the responsibility for logging the game
    /// state changes etc.
    class Board_state final : public Hit_by_line
    {
    public:

        Board_state(
            unsigned int random_seed,
            Game_hex_grid&&,
            std::vector<Asteroid>&&,
            std::vector<Sun>&& );

        ~Board_state() final;

        /// Adds the given Machine to the board at the stated location. If the
        /// location is invalid or already occupied, this throws. If not, the
        /// Machine is added to the board (which may generate the machines own
        /// events) and then the Board_state will also send its own
        /// MachineAdded Machine_event.
        void add_machine_to_board(
            Hex_coord,
            HexFacingName,
            std::unique_ptr<Machine> );

        /// Advance the board state by one frame and write the events that
        /// occurred this frame to the Frame_events. The contents of the
        /// Frame_events are only guaranteed to be valid until the next call
        /// to 'advance_one_frame'.
        void advance_one_frame(Frame_events&);

        /// Returns the number of asteroids currently on the board.
        size_t asteroids_count() const;

        /// Returns the number of suns currently on the board.
        size_t suns_count() const;

        /// Returns the number of machines currently on the board.
        size_t machines_count() const;

        /// Returns the indexed Asteroid.
        const Asteroid& asteroid_at( size_t n ) const;

        /// Returns the indexed Sun.
        const Sun& sun_at( size_t n ) const;

        /// Returns the indexed Machine.
        const Machine& machine_at( size_t n ) const;

        /// Returns the Machine object with the coordinates @a xy, if any,
        /// else returns nullptr.
        const Machine* machine_at_coord( Hex_coord xy ) const;

        /// Returns the Asteroid object with the coordinates @a xy, if any,
        /// else returns nullptr.
        const Asteroid* asteroid_at_coord( Hex_coord xy ) const;

        /// Returns the width of the grid.
        unsigned int grid_width() const;

        /// Returns the height of the grid.
        unsigned int grid_height() const;

        const std::vector<team>& teams() const;

        /// Equality operator for Board_state. Returns true only if two
        /// Board_state are logically equivalent.
        bool operator == (const Board_state& ) const noexcept;

    private:

        Board_state(
            Good_rand&&,
            Game_hex_grid&&,
            std::vector<Asteroid>&&,
            std::vector<Sun>&& );

        const Game_hex_grid& hex_board() const;

        const Hex_grid& grid() const;

    private:

        struct Delayed_machine_event
        {
            unsigned int  timestamp_;
            Machine_event event_;

            Delayed_machine_event( unsigned int ts, const Machine_event& e )
                : timestamp_( ts ), event_( e )
            {
            }

            bool operator == ( const Delayed_machine_event& rhs ) const noexcept
            {
                return timestamp_ == rhs.timestamp_ && event_ == rhs.event_;
            }

            bool operator <  ( const Delayed_machine_event& rhs ) const noexcept
            {
                return timestamp_ > rhs.timestamp_;
            }
        };

        using Machine_coord = std::pair<Hex_coord, Machine*>;

    private:

        Board_state( const Board_state& ) = delete;

        Board_state& operator=( const Board_state& ) = delete;

        Board_state( Board_state&& );

        Board_state& operator=( Board_state&& );

        /// This creates a Machine exploded Machine_event - it is NOT
        /// the handler for it!
        void explode_machine( Machine& );

        void weapon_fired( const Machine_event&, const Line_hit );

        void cap_drain_fired( const Machine_event&, const Line_hit );

        void cap_xfer_fired( const Machine_event&, const Line_hit );

        void armour_repairer_fired( const Machine_event&, const Line_hit );

        void miner_fired( const Machine_event&, const Line_hit );

        void handle_trade_request( const Trade_request& );

        void handle_trade_transaction( const Trade_request& );

        void handle_trade_query( const Trade_request& );

        /// Here is where events raised by machines cause things to happen in
        /// the board. Events are processed after all machines have been
        /// advanced in the frame. All events are either completely resolved, or
        /// become delayed events. No new events must be generated by processing
        /// events.
        void handle_event( const Machine_event& );

        void handle_fired( const Machine_event& );

        void handle_broadcast( const Machine_event& );

        void handle_transmission( const Machine_event& );

        void handle_initiated_self_destruct( const Machine_event& );

        void handle_completed_build( const Machine_event& );

        void handle_cargo_transfer( const Machine_event& );

        void handle_machine_cash_transfer( const Machine_event& );

        void handle_machine_received_damage( const Machine_event& );

        void handle_machine_received_repairs( const Machine_event& );

        void handle_machine_cap_altered( const Machine_event& );

        /// Returns the Machine object with the coordinates @a xy,
        /// if any, else returns nullptr.
        Machine* machine_at( Hex_coord xy ) const;

        /// Returns the Asteroid object with the coordinates @a xy, if any. The
        /// Asteroid may be out-of-game due to it having zero volume, in which
        /// case it does not exist for the purposes of the board elements, and
        /// it should no longer appear in the grid as contents.
        /// This can return nullptr.
        Asteroid* asteroid_at( Hex_coord xy );

        /// Returns the Sun object with the coordinates @a xy, if any,
        /// else returns nullptr.
        Sun* sun_at( Hex_coord xy );

        /// Requires that there is a Machine at the given coordinate.
        float machine_radius_at( Hex_coord ) const;

        /// Requires that there is an Asteroid at the given coordinate.
        float asteroid_radius_at( Hex_coord ) const;

        void send_instructions_to_board(
            Hex_coord,
            const std::shared_ptr<const Instructions>& );

        /// Returns a positive hit if the straight line from @a from to
        /// @a target intersects with the radius of any bot or Asteroid on the
        /// board, up to and including @a max_range. The starting Hex_coord
        /// @a from is always excluded from the calculations and will never
        /// be returned as a hit.
        Line_hit find_first_hit_by_line(
            Hex_coord from,
            Hex_coord target,
            std::uint16_t max_range ) const final;

        /// Checks that the grid and the machines states are consistent with
        /// each other.
        bool grid_and_machines_are_consistent() const;

    private:

        /// Access to members to serialize to/from JSON.
        friend class to_from_json;

    private:

        std::uint64_t                               frame_number_;
        std::vector<Trade_request>                  trade_requests_;
        std::vector<Machine_event>                  machine_events_;
        std::vector<Secondary_event>                secondary_events_;
        std::priority_queue<Delayed_machine_event>  delayed_events_;
        mutable std::vector<Hex_coord>              collisions_;
        std::vector<Asteroid>                       asteroids_;
        std::vector<Sun>                            suns_;
        std::vector<Hex_grid::Hex_tile>             tile_buffer_; ///< Used as temp by some calls.
        Board_state_grid_view                       grid_;
        std::vector<team>                           teams_;
        Good_rand                                   randoms_;
        std::unique_ptr<Hex_pathfinder>             pathfinder_;
    };
}

#include "solosnake/blue/blue_board_state.inl"
#endif
