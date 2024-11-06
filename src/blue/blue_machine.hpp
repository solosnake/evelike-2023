#ifndef blue_machine_hpp
#define blue_machine_hpp

#include <cassert>
#include <cstdint>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "solosnake/blue/blue_bgr.hpp"
#include "solosnake/blue/blue_blueprint.hpp"
#include "solosnake/blue/blue_chassis.hpp"
#include "solosnake/blue/blue_cargohold.hpp"
#include "solosnake/blue/blue_clockwise.hpp"
#include "solosnake/blue/blue_classification.hpp"
#include "solosnake/blue/blue_combat_system.hpp"
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_game_hex_grid.hpp"
#include "solosnake/blue/blue_game_logic_constants.hpp"
#include "solosnake/blue/blue_hardware.hpp"
#include "solosnake/blue/blue_hardpoint_collective.hpp"
#include "solosnake/blue/blue_hex_pathfinder.hpp"
#include "solosnake/blue/blue_hex_path_costs.hpp"
#include "solosnake/blue/blue_instructions.hpp"
#include "solosnake/blue/blue_machine_name.hpp"
#include "solosnake/blue/blue_machine_event.hpp"
#include "solosnake/blue/blue_machine_events.hpp"
#include "solosnake/blue/blue_machine_grid_view.hpp"
#include "solosnake/blue/blue_machine_name.hpp"
#include "solosnake/blue/blue_machine_readout.hpp"
#include "solosnake/blue/blue_machine_template.hpp"
#include "solosnake/blue/blue_movement_grid_view.hpp"
#include "solosnake/blue/blue_for_sale.hpp"
#include "solosnake/blue/blue_position_info.hpp"
#include "solosnake/blue/blue_propulsion.hpp"
#include "solosnake/blue/blue_quote.hpp"
#include "solosnake/blue/blue_sense_results.hpp"
#include "solosnake/blue/blue_trade_request.hpp"

namespace blue
{
    class Hit_by_line;
    class Randoms;


    class Machine
    {
    public:

        /// Construct a machine from a template.
        static std::unique_ptr<Machine> create_machine( Machine_template&& );

        /// TODO: REMOVE
        /// Machines should be created with the create_machine call and not directly.
        Machine(
            const Machine_name&,
            uint8_t team_hue_shift,
            Hardware&&,
            Hardpoint_collective&&,
            Instructions&&,
            unsigned int pulse_period,
            std::vector<std::shared_ptr<Blueprint>>&& );

        ~Machine();

        /// Sets up the pointer connections between the machine and Board_state.
        void setup_externals(
            std::vector<Machine_event>&,
            std::vector<Trade_request>&,
            const Hit_by_line&,
            Hex_pathfinder& pathfinder,
            Randoms& shared_randoms );

        /// Facing specifies which neighbour the machine is pointing towards.
        void add_to_board( const Hex_coord, const HexFacingName facing, Machine_grid_view );

        void removed_from_board();

        std::unique_ptr<Machine> build_machine( const std::uint16_t blueprintIndex,
                                                const std::uint16_t codeIndex );

        bool resolve_combat( Randoms& );

        void resolve_movement( Movement_grid_view );

        void advance_one_frame();

        void receive_broadcast( const std::shared_ptr<const Instructions>& );

        std::shared_ptr<const Instructions> pop_broadcast( const std::uint16_t id );

        std::shared_ptr<const Instructions> pop_transmission( const std::uint16_t id );

        void inc_watched_refcount() const;

        void dec_watched_refcount() const;

        bool is_not_being_watched() const noexcept;

        bool is_being_watched() const noexcept;

        bool try_add_cargo( Amount& );

        void try_transfer_cargo_to( Machine&, TradableTypes, const std::uint16_t units );

        bool try_transfer_credits_to( Machine&, const std::uint16_t amount );

        void give_credits( const std::uint16_t amount );

        /// Returns the maximum number of credits a machine can carry.
        std::uint16_t max_credits() const noexcept;

        bool can_add_credits( const std::uint16_t amount ) const noexcept;

        std::uint16_t sell_goods_to( Machine&, TradableTypes, std::uint16_t units );

        void trade_reply( const std::uint16_t result );

        Quote get_trade_quote( const Trade_request& );

        const Cargohold& cargo() const noexcept;

        std::string_view name() const noexcept;

        std::string_view class_name() const noexcept;

        Classification machine_class() const noexcept;

        std::uint8_t team_hue_shift() const noexcept;

        const Instructions& code() const noexcept;

        void kill();

        void apply_damage( const Damages& d );

        float get_indexed_hardpoint_falloff_at_range( size_t i, std::uint16_t range ) const;

        void apply_armour_repairs( const int r );

        void apply_cap_alteration( const int d );

        bool is_alive() const noexcept;

        bool is_dead() const noexcept;

        bool is_accelerating() const noexcept;

        bool is_turning_cw() const noexcept;

        bool is_turning_ccw() const noexcept;

        bool is_mobile() const noexcept;

        int max_speed() const noexcept;

        int max_turn_speed() const noexcept;

        std::uint16_t max_sensing_radius() const noexcept;

        Radians angle_as_radians() const noexcept;

        Position_info get_position_info() const noexcept;

        Fixed_angle angle() const noexcept;

        Hex_coord board_xy() const noexcept;

        /// Bounding radius in world units.
        float bounding_radius() const noexcept;

        std::uint16_t capacitance_remaining() const noexcept;

        /// A constant estimate of how much of the capacitance in storage
        /// at any time was generated from local nuclear energy. Returns a
        /// value in the range 0 to 1. This estimate does not take into account
        /// the suns strength at the current tile but rather gives an estimate
        /// of solar vs nuclear. A machine with no nuclear capabilities will
        /// return 0.0 always.
        float capacitance_proportion_from_nuclear_power() const noexcept;

        std::uint16_t components_count() const noexcept;

        std::shared_ptr<Blueprint> machine_blueprint() const noexcept;

        const std::vector<Turning_hardpoint>& turning_hardpoints() const noexcept;

        void offset_after_n_frames( const float frames, float* const unaliased xy ) const;

        Machine_exploded get_explosion() const noexcept;

        float readout( const Machine_readout ) const noexcept;

        HexFacingName facing_direction() const;

        enum ErrorType : std::uint16_t
        {
            ProgramRaisedError              = 1,
            ExpectedLocationNotFoundAtLine  = 2,
            ExpectedValueNotFoundAtLine     = 3,
            AssertionFailed                 = 4,
            UnknownInstruction              = 5
        };

        //--------------------------------------------------------------------------------------
        // STATE QUERY
        //--------------------------------------------------------------------------------------

        bool is_blocked() const noexcept;

        bool is_not_blocked() const noexcept;

        bool is_moving() const noexcept;

        bool is_turning() const noexcept;

        bool is_advancing() const noexcept;

        /// Returns 0.0 - 1.0 if building is in progress. Returns 0.0 otherwise.
        float build_progression() const noexcept;

    private:

        //--------------------------------------------------------------------------------------
        // Generic
        //--------------------------------------------------------------------------------------

        /// Sets up the CPU speed readout value, a measure of how fast this
        /// machines CPU is against other CPUs.
        void set_cpu_speed_readout();

        /// Sets all the varying readout values based on current internal
        /// state of machine.
        void update_readouts();

        /// Reports the given error type. Assumes the error has just occurred,
        /// and uses the current state of the machine when reporting.
        void signal_error( ErrorType );

        /// Callable from the machine code.
        void set_error_code( std::uint16_t );

        /// Executes the current Instruction (either code or navigation), if
        /// there is enough cap.
        void execute_current_instruction();

        void execute_code_instruction();

        void execute_navigation_instruction();

        /// Returns the next random number between A and B, inclusive.
        std::uint16_t get_rand( std::uint16_t A, std::uint16_t B );

        //--------------------------------------------------------------------------------------
        // STATE UPDATERS
        //--------------------------------------------------------------------------------------

        void tick_motion_system( Movement_grid_view& );

        void tick_hardpoint_systems();

        void tick_capacitor_system();

        void tick_instructions();

        void tick_blocked_status();

        //--------------------------------------------------------------------------------------
        //        COMMANDS
        //--------------------------------------------------------------------------------------

        /// Defines the math function pointer.
        typedef void ( *math_function_t )( std::int16_t& result, std::int16_t op1, std::int16_t op2 );

        // These are the only things the code can DO to the machine:
        void command_set_instruction_pointer( const size_t reg0 );

        /// Stores value in 'line'.
        void command_store_value(
            const size_t line,
            const std::uint16_t value );

        /// Copies @a n lines of code beginning at @a from to @a to.
        void command_copy_code(
            const std::uint16_t from,
            const std::uint16_t to,
            const std::uint16_t n );

        /// Swaps @a n lines of code beginning at @a from to @a to.
        void command_swap_code(
            const std::uint16_t from,
            std::uint16_t to,
            const std::uint16_t n );

        /// Compares lines of code A and B and stores true if they are identical.
        void command_compare_code(
            const std::uint16_t storeAt,
            const std::uint16_t lineA,
            const std::uint16_t lineB );

        /// Stores value in 'line'.
        void command_store_location(
            const size_t line,
            const Hex_coord xy );

        void command_store_is_turning( const size_t reg0 );

        /// Copies a command line Instruction from one location to another.
        void command_copy(
            const size_t from,
            const size_t to );

        /// Causes machine to stop executing its instructions this frame and
        /// continue at next Instruction nth next frame. n = 0 has no effect.
        void synch( const std::uint16_t n );

        /// This command is ignored if the ship is already turning, or is
        /// advancing.
        void command_turn(
            const std::uint16_t direction,
            const std::uint16_t n_faces );

        /// This command is ignored if the ship is already advancing, or is
        /// turning.
        void command_advance(
            const std::uint16_t tiles,
            const std::uint16_t storageLine,
            const bool storeResult );

        void command_get_clockwise_turn_count(
            const std::uint16_t storeAt,
            const Hex_coord );

        void command_get_distance_to(
            const std::uint16_t storeAt,
            const Hex_coord );

        void command_get_distance_between(
            const std::uint16_t storeAt,
            const Hex_coord,
            const Hex_coord );

        /// Stores true/false (1/0) if this coordinate is reachable by hardpoints.
        void command_can_aim_at(
            const std::uint16_t storeAt,
            const Hex_coord );

        void command_aim_at( const Hex_coord );

        /// Stores 1/0 if there is a unobstructed line of fire to the coordinate.
        void command_has_line_of_fire_to(
            const std::uint16_t storeAt,
            const Hex_coord );

        /// Stores how many hardpoints can fire on this coordinate.
        void command_can_fire_at(
            const std::uint16_t storeAt,
            const Hex_coord );

        void command_fire_at( const Hex_coord );

        void command_sense( const std::uint16_t senseRadius );

        void command_assert(
            const std::uint16_t op1,
            const std::uint16_t op2,
            const std::uint16_t operation );

        void command_clear_msgs();

        void command_copy_msgbuf_code(
            const std::uint16_t from,
            const std::uint16_t to,
            const std::uint16_t count );

        void command_move_msg_to_msgbuf( const std::uint16_t messageIndex );

        /// Stores true/false if the machine can build (has the required materials in the hold)
        /// the indexed Blueprint.
        void command_can_build(
            const std::uint16_t blueprintIndex,
            const std::uint16_t storeAt );

        /// If the machine has the required materials in the hold to build the indexed Blueprint,
        /// and the build-site of this machine can be reserved, then the materials will be taken
        /// and construction will be started on a new machine at that build site,
        void command_build(
            const std::uint16_t blueprintIndex,
            const std::uint16_t codeToCopyStart,
            const std::uint16_t codeToCopyCount );

        /// No broadcast if range > broadcast range.
        /// Broadcast takes place when all cap cost has been paid. Machine is
        /// in blocked state until then. If cap cost can be paid then broadcast
        /// is immediate.
        void command_broadcast(
            std::uint16_t range,
            const std::uint16_t codeToBroadcastStart,
            const std::uint16_t codeToBroadcastCount );

        /// No transmission if destinationXY is outside transmit range. Transmit
        /// takes place when all cap cost has been paid. Machine is in blocked
        /// state until then. If cap cost can be paid then transmission is
        /// immediate.
        void command_transmit(
            const Hex_coord destinationXY,
            const std::uint16_t codeToBroadcastStart,
            const std::uint16_t codeToBroadcastCount );

        /// If the command at value_location is not data an error occurs and
        /// this command is ignored.
        void command_math( const Instruction& i, const math_function_t op );

        /// Initiates self destruct in n frames.
        void command_self_destruct( const std::uint16_t n );

        //--------------------------------------------------------------------------------------
        // RAW INSTRUCTIONS
        //--------------------------------------------------------------------------------------

        void store(
            const size_t line,
            const std::int16_t what );

        void store(
            const size_t line,
            const std::uint16_t what );

        void store(
            const size_t line,
            const Hex_coord xy );

        void place_instructions(
            const std::vector<Instruction>&,
            const size_t line );

        //--------------------------------------------------------------------------------------
        // STATE CHANGERS
        //--------------------------------------------------------------------------------------

        /// See BLUE_CLOCKWISE and BLUE_C_CLOCKWISE for correct directions.
        void start_n_face_turns(
            const std::uint16_t direction,
            const std::uint16_t n_faces );

        void start_advancing_n_tiles(
            const bool,
            const std::uint16_t,
            const std::uint16_t );

        void n_tile_moves_completed();

        void n_tiles_sensing_completed();

        /// Returns true if @param xy is within range of at least one Hardpoint.
        bool is_in_range_to_fire_on( const Hex_coord xy );

        //--------------------------------------------------------------------------------------
        // RECEIVED MESSAGES
        //--------------------------------------------------------------------------------------

        std::uint16_t max_stored_messages() const noexcept;

        //--------------------------------------------------------------------------------------
        // NAVIGATION
        //--------------------------------------------------------------------------------------

        /// Returns true if xy is a coordinate that is not the present
        /// location, is on board and this machine could try to move to it.
        bool can_move_to( const Hex_coord xy ) const;

        void command_navigate_to( const Hex_coord );

        /// @param move_limit How many steps to stop after while moving towards @a xy.
        void command_navigate_n_towards(
            const std::uint16_t move_limit,
            const Hex_coord xy );

        /// Weighting variables are unoccupied (0), bot (1), Asteroid (2); and
        /// wrap around.
        void command_set_astar_weight(
            const std::uint16_t tileType,
            const std::uint16_t weight );

        /// The route plotted will contain both A and B, and starts from A.
        /// No route will be plotted if: A or B are not on board,  A and B are
        /// same location, or A or B are outside of sensor range.
        void command_plot_route_between(
            const Hex_coord A,
            const Hex_coord B );

        void complete_route_plotting();

        void complete_search_and_start_navigating();

        void switch_to_navigation_instructions();

        void start_navigation_search(
            const Hex_coord start,
            const Hex_coord dest,
            const std::uint16_t move_limit );

        void finish_navigation_search();

        void begin_navigation(
            const Hex_coord xy,
            const std::uint16_t move_limit );

        void end_navigation();

        std::uint16_t route_length() const;

        /// Weighting variables are unoccupied (0), bot (1), Asteroid (2)
        /// and wrap around.
        std::uint16_t astar_weight( const std::uint16_t tile_type_index ) const;

        /// Copies @a n lines of route (coordinate location instructions)
        /// beginning at @a from to @a to. Operates similar to copy code,
        /// the route array is treated with modulo arithmetic and if from + n
        /// goes past the end of the plotted route, it will wrap around.
        void command_copy_route(
            const std::uint16_t from,
            const std::uint16_t to,
            const std::uint16_t n );

        //--------------------------------------------------------------------------------------
        // CAPACITOR
        //--------------------------------------------------------------------------------------

        bool cap_try_pay_single_instruction_cost() noexcept;

        bool cap_try_pay_firing_cost() noexcept;

        std::uint16_t cap_instruction_cost() const noexcept;

        std::uint16_t cap_firing_cost() const noexcept;

        std::uint16_t cap_sense_cost_per_tile() const noexcept;

        std::uint16_t cap_broadcast_cost_per_tile() const noexcept;

        std::uint16_t cap_transmit_cost_per_tile() const noexcept;

        /// This is the contribution from the engines, not the solar.
        std::uint16_t cap_refill_per_tick_from_engines() const noexcept;

        /// Extracts cost from cap, and returns true if all of cost was paid by
        /// cap (which indicates that an action can proceed).
        std::uint16_t cap_try_pay_cap( std::uint16_t cost );

        /// Tries to apply the cost of sensing the requested distance, but
        /// if there is insufficient cap, scales back sense radius to the
        /// available cap and returns the sensing distance paid for, which may
        /// be zero.
        std::uint16_t cap_try_extract_sensing( const std::uint16_t senseRadius );

        /// How much cap is generated by the machine's solar panels at its
        /// current location.
        std::uint16_t cap_from_solar_panels_at_current_tile() const noexcept;

        //--------------------------------------------------------------------------------------
        // BUILDING & CARGO
        //--------------------------------------------------------------------------------------

        /// Reserves the facing tile if available, and returns true if it
        /// can be built on. Fills in the coordinate of the build site if it
        /// is available.
        bool reserve_build_site( Hex_coord& );

        bool can_build_blueprint( const std::uint16_t blueprintIndex ) const;

        void start_build(
            const std::uint16_t blueprintIndex,
            const Hex_coord site );

        void complete_build(
            const std::uint16_t blueprintIndex,
            const Hex_coord site,
            const std::uint16_t codeToCopyStart,
            const std::uint16_t codeToCopyCount );

        void command_take_cargo_from(
            const Hex_coord,
            const std::uint16_t cargoType,
            const std::uint16_t units );

        void command_give_cargo_to(
            const Hex_coord,
            const std::uint16_t cargoType,
            const std::uint16_t units );

        std::uint16_t calculate_build_time( const Blueprint& ) const;

        std::uint16_t calculate_build_cap_cost( const Blueprint& ) const;

        void destroy_cargo( const Amount& );

        //--------------------------------------------------------------------------------------
        // SELLING AND CREDITS
        //--------------------------------------------------------------------------------------

        void command_cancel_sales();

        void command_for_sale(
            const std::uint8_t tradable,
            const std::uint16_t n,
            const std::uint16_t price );

        void command_try_buy(
            const std::uint8_t tradable,
            const std::uint16_t n );

        void command_try_buy_with_store(
            const std::uint16_t storeAt,
            const std::uint8_t tradable,
            const std::uint16_t n );

        void command_get_price_of(
            const std::uint16_t storeAt,
            const std::uint8_t tradable );

        void command_get_availability_of(
            const std::uint16_t storeAt,
            const std::uint8_t tradable );

        void command_transfer_credits_to(
            const Hex_coord dest,
            const std::uint16_t n );

        void trade_synch(
            const std::uint8_t tradable,
            const std::uint8_t tradeType,
            const std::uint16_t n,
            bool store,
            const std::uint16_t storeAt );

        //--------------------------------------------------------------------------------------
        // REFINING
        //--------------------------------------------------------------------------------------

        /// "Refine N units of X". If oreType is not a valid ore, no refining occurs.
        void command_refine(
            const std::uint16_t unitsToRefine,
            const std::uint16_t oreType );

        /// How many frames it takes to refine a single unit of @a ore. Max
        /// is 65535 frames, min is 1 frame.
        std::uint16_t refine_period(
            const std::uint16_t oreType ) const;

        void start_a_refining_cycle(
            const OreTypes oreType,
            const uint16_t unitsToRefine );

        void end_a_refining_cycle();

        //--------------------------------------------------------------------------------------
        // TRANSMITTING AND BROADCASTING
        //--------------------------------------------------------------------------------------

        bool is_valid_instruction_range(
            const std::uint16_t codeBegin,
            const std::uint16_t codeLength ) const;

        void do_transmission(
            const Hex_coord xy,
            const std::uint16_t codeBegin,
            const std::uint16_t codeLength );

        void do_broadcast(
            const std::uint16_t range,
            const std::uint16_t codeBegin,
            const std::uint16_t codeLength );

        std::uint16_t calculate_transmission_cost_to( const Hex_coord xy ) const;

        std::uint16_t calculate_broadcast_cost( const std::uint16_t range ) const;

        std::uint16_t push_broadcast( const std::shared_ptr<const Instructions>& );

        std::uint16_t push_transmission( const std::shared_ptr<const Instructions>& );

        std::shared_ptr<const Instructions> pop_sent( std::uint16_t id );

        /// Adds @a code to the stack and returns its unique I.D. This I.D. is used to pop it
        /// from the stack.
        std::uint16_t push_code( const std::shared_ptr<const Instructions>& );

        std::shared_ptr<const Instructions> pop_code( std::uint16_t id );

        /// Returns a subset of the machine's instructions starting at @a
        /// codeBegin and up to @a codeLength instructions long.
        /// If start and (start + n - 1) must represent a valid range of
        /// instructions. instructions use 1 based indexing.
        std::shared_ptr<const Instructions> fetch_instructions(
            const std::uint16_t codeBegin,
            const std::uint16_t codeLength ) const;

        //--------------------------------------------------------------------------------------
        // Helpers
        //--------------------------------------------------------------------------------------

        /// Returns true if xy is not beyond sensor range of current location.
        bool is_in_sensor_range( const Hex_coord xy ) const;

        /// Returns true if xy is a neighbouring coordinate and it contains a machine.
        bool is_hex_coord_of_machine_alongside( const Hex_coord xy ) const;

        /// Returns the number of jumps to move from present position to @a xy
        /// without taking into account board contents etc. Neighbouring hexes
        /// return 1, etc.
        /// Returns zero if xy is not within the board bounds.
        size_t range_to( const Hex_coord xy ) const;

        /// Returns the number of jumps to move from @a A to @a B
        /// without taking into account board contents etc. Neighbouring hexes
        /// return 1, etc.
        /// Returns zero if @a A or @a B is not within the board bounds.
        size_t distance_between(
            const Hex_coord A,
            const Hex_coord B ) const;

        /// Calculate the facing the user wishes to end up at when starting from
        /// current_facing, moving in rotation 'direction' (CW or CCW) and
        /// moving through n faces.
        static size_t calculate_facing(
            size_t current_facing,
            const size_t direction,
            const size_t n_faces ) noexcept;

    private:

        /// Blocking is assumed to be tick based, e.g. whatever we are blocking
        /// on requires N ticks, not N instructions to pass.
        enum BlockingCondition
        {
            NothingBlocking,
            WaitingForever,
            WaitingForNFaceTurns,
            WaitingForNTileMoves,
            WaitingForSensingCompleted,
            WaitingForSynch,
            WaitingForTransmitCapCostToBePaid,
            WaitingForBroadcastCapCostToBePaid,
            WaitingForBuildToComplete,
            WaitingForAStarRoute,
            WaitingForRoutePlot,
            WaitingForRefiningCycleToEnd,
            WaitingForTrade
        };

        Machine_name                                                    machines_name_;
        Classification                                                  classification_;
        float                                                           bounding_radius_;
        std::vector<std::shared_ptr<Blueprint>>                         known_blueprints_;
        Machine_grid_view                                               grid_;
        const Hit_by_line*                                              detect_hit_;
        std::vector<Machine_event>*                                     machine_events_;
        std::vector<Trade_request>*                                     trade_requests_;
        Randoms*                                                        rand_;
        Hex_pathfinder*                                                 pathfinder_;
        Hex_path_search*                                                path_search_handle_;
        mutable std::uint32_t                                           watched_refcount_;  // How many other parties have a pointer to this machine.
        std::uint16_t                                                   next_code_store_id_;
        std::map<std::uint16_t, std::shared_ptr<const Instructions>>    sent_instructions_; // Broadcast or Transmitted.
        std::vector<std::shared_ptr<const Instructions>>                message_buffer_;
        std::shared_ptr<const Instructions>                             current_message_;
        std::vector<Hex_coord>                                          plotted_route_;
        std::vector<Instruction>                                        navigation_instructions_;
        size_t                                                          navigation_instruction_pointer_;
        HexFacingName                                                   navigation_expected_next_facing_;
        Hex_coord                                                       navigation_expected_next_xy_;
        Sense_results                                                   sense_results_;
        Hardpoint_collective                                            hardpoints_;
        Cargohold                                                       cargo_hold_;
        Hardware                                                        hardware_;
        Instructions                                                    instructions_;
        unsigned int                                                    pulse_variable_;
        unsigned int                                                    pulse_period_;
        float                                                           readouts_[BLUE_MACHINE_READOUTS_COUNT];
        size_t                                                          instruction_pointer_;
        size_t                                                          cycles_accumulator_;
        Propulsion                                                      propulsion_;
        Combat_system                                                   combat_system_;
        Hex_path_costs                                                  path_costs_;
        BlockingCondition                                               blockingCondition_;
        std::uint16_t                                                   blocking_variables_[7];
        std::uint16_t                                                   capacitance_;
        std::int32_t                                                    external_cap_delta_;
        std::uint16_t                                                   error_code_;
        std::uint16_t                                                   credits_;
        For_sale                                                        items_on_sale_[BLUE_TRADABLE_TYPES_COUNT];
        bool                                                            navigating_;
        bool                                                            instruction_pointer_updated_;
        bool                                                            self_destruct_triggered_;
        std::uint8_t                                                    team_hue_shift_;
    };
}

#include "solosnake/blue/blue_machine.inl"
#endif
