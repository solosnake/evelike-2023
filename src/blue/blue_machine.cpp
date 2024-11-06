#include <climits>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "solosnake/blue/blue_blueprint.hpp"
#include "solosnake/blue/blue_explosion_damages.hpp"
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_hexcontentflags.hpp"
#include "solosnake/blue/blue_hex_pathfinder.hpp"
#include "solosnake/blue/blue_hit_by_line.hpp"
#include "solosnake/blue/blue_line_hit.hpp"
#include "solosnake/blue/blue_machine.hpp"
#include "solosnake/blue/blue_machine_event.hpp"
#include "solosnake/blue/blue_machine_events.hpp"
#include "solosnake/blue/blue_movement_grid_view.hpp"
#include "solosnake/blue/blue_refine.hpp"
#include "solosnake/blue/blue_quote.hpp"
#include "solosnake/blue/blue_throw.hpp"
#include "solosnake/blue/blue_unreachable.hpp"

#define HEX_CONTENT_TO_OCCUPANT( N )            (((N) & HEX_CONTENT_FLAGS) >> 4)
#define NAVIGATION_BACKGROUND_FRAMES_COUNT      (2)
#define ROUTE_PLOTTING_BACKGROUND_FRAMES_COUNT  (NAVIGATION_BACKGROUND_FRAMES_COUNT)

// This is not intended to persist and should be deduced from the machine's build.
#define BLUE_MACHINE_DEFAULT_RADIUS (1.0f)
#ifdef BLUE_MACHINE_DEFAULT_RADIUS
#   ifdef _MSC_VER // Visual Studio supports pragma messages
#     ifndef SOLOSNAKE_STRINGIFY
#       define SOLOSNAKE_STRINGIFY(x) #x
#       define SOLOSNAKE_TOSTRING(x) SOLOSNAKE_STRINGIFY(x)
#     endif
#     pragma message(__FILE__ "(" SOLOSNAKE_TOSTRING( __LINE__) "): Warning: Using incorrect machine radius.")
#   else
#     pragma message("Warning: Using incorrect machine radius")
#   endif
#endif

namespace blue
{
    namespace
    {
        inline bool direction_means_clockwise( const unsigned int i )
        {
            // Clockwise is odd numbers.
            return ( i & 1 ) == BLUE_CLOCKWISE;
        }

        inline bool direction_means_counter_clockwise( const unsigned int i )
        {
            // Counter-clockwise is even numbers.
            return ( i & 1 ) != BLUE_CLOCKWISE;
        }

        //--------------------------------------------------------------------------------------
        // MATH FUNCTIONS
        //--------------------------------------------------------------------------------------

        void add( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = op1 + op2;
        }

        void sub( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = op1 - op2;
        }

        void mul( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = op1 * op2;
        }

        void div( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = op1 / op2;
        }

        void Xor( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = op1 ^ op2;
        }

        void Or( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = op1 | op2;
        }

        void And( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = op1 & op2;
        }

        void Eq( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = op1 == op2 ? 1 : 0;
        }

        void NEq( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = op1 != op2 ? 1 : 0;
        }

        void bool_and( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = ( op1 && op2 ) ? 1 : 0;
        }

        void bool_or( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = ( op1 || op2 ) ? 1 : 0;
        }

        void bool_lt( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = ( op1 < op2 ) ? 1 : 0;
        }

        void right_shift( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = op1 >> op2;
        }

        void left_shift( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = op1 << op2;
        }

        void bool_gt( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = ( op1 > op2 ) ? 1 : 0;
        }

        void bool_lte( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = ( op1 <= op2 ) ? 1 : 0;
        }

        void bool_gte( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = ( op1 >= op2 ) ? 1 : 0;
        }

        void modulo( int16_t& result, int16_t op1, int16_t op2 )
        {
            result = ( op1 % op2 );
        }

        Resistances make_resistances( const Hardware& hw )
        {
            const float k = hw.kinetic_resistance()   * 0.01f; // Convert
            const float t = hw.thermal_resistance()   * 0.01f; // to
            const float e = hw.em_resistance()        * 0.01f; // %
            const float r = hw.radiation_resistance() * 0.01f; //
            return Resistances( k, t, e, r );
        }

        /// The cpu speed metric is a measure of how many instructions
        /// a machine executes per cycle. A machine with a cpu speed metric
        /// of 1.0 matches the "benchmark" machine. A metric of 2.0 means
        /// this machine is twice as fast at executing instructions as the
        /// benchmark machine.
        float cpu_speed_metric( const Hardware& hw )
        {
            assert( hw.cycles_per_instruction() > 0u );

            const float cycles_per_tick        = static_cast<float>( hw.processor_speed_cycles_per_tick() );
            const float instruction_cycle_cost = static_cast<float>( hw.cycles_per_instruction() );
            const float instructions_per_cycle = cycles_per_tick / instruction_cycle_cost;

            return instructions_per_cycle / BLUE_CPU_BASELINE_INSTRUCTIONS_PER_CYCLE;
        }

        /// Better hardware has shorter pulse periods
        unsigned int calculate_pulse_period( unsigned int defaultPeriod, const Hardware& hw )
        {
            float s = cpu_speed_metric( hw );
            return s > 0.0f ? static_cast<unsigned int>( defaultPeriod / s ) : defaultPeriod;
        }

        enum ArgType
        {
            ValueArg,
            LocationArg,
            DirectionArg,
            DataArg
        };

        /// Arguments to machine functions originate within the instruction
        /// but can be indirections, referring to other instructions to be
        /// interpreted as values or locations. It is an error for an
        /// instruction to use another instruction as an argument source
        /// if that instruction is neither a value or location. This class
        /// checks that non value instructions are never used as arguments.
        class Arg_fetcher
        {
        public:

            Arg_fetcher();

            enum BadFetch
            {
                GoodFetch,
                InvalidValue    = Machine::ExpectedValueNotFoundAtLine,
                InvalidLocation = Machine::ExpectedLocationNotFoundAtLine
            };

            bool fetch( ArgType, const Instruction&, const Instructions& );

            bool fetch( ArgType, ArgType, const Instruction&, const Instructions& );

            bool fetch( ArgType, ArgType, ArgType, const Instruction&, const Instructions& );

            /// Returns arg i as a VALUE
            uint16_t  value( unsigned int i ) const;

            /// Returns arg i as a LOCATION
            Hex_coord  location( unsigned int i ) const;

            /// Returns arg i as a direction (CW/CCW).
            uint16_t  direction( unsigned int i ) const;

            /// Returns arg i as a face (0-5).
            uint16_t  facing( unsigned int i ) const;

            bool      bad_fetch() const                   { return GoodFetch != fetch_result_; }

            uint16_t  fetch_result() const                { return fetch_result_; }

        private:

            bool fetch( const ArgType*, unsigned int n, const Instruction&, const Instructions& );

            uint16_t    args_[3];
            uint8_t     fetch_result_;      // 0 = no error, 1 = bad value, 2 = bad location.
            uint8_t     written_args_flag_;
        };

        uint16_t  Arg_fetcher::value( unsigned int i ) const
        {
            assert( ( written_args_flag_ & ( 1 << ( i + 1 ) ) ) > 0 );
            return args_[i];
        }

        Hex_coord  Arg_fetcher::location( unsigned int i ) const
        {
            assert( ( written_args_flag_ & ( 1 << ( i + 1 ) ) ) > 0 );
            return Hex_coord::from_uint16( args_[i] );
        }

        uint16_t  Arg_fetcher::direction( unsigned int i ) const
        {
            assert( ( written_args_flag_ & ( 1 << ( i + 1 ) ) ) > 0 );
            return args_[i] % 2;
        }

        uint16_t  Arg_fetcher::facing( unsigned int i ) const
        {
            assert( ( written_args_flag_ & ( 1 << ( i + 1 ) ) ) > 0 );
            return args_[i] % 6;
        }

        Arg_fetcher::Arg_fetcher() : fetch_result_( GoodFetch ), written_args_flag_( 0 )
        {
            args_[0] = args_[1] = args_[2] = 0;
        }

        inline bool Arg_fetcher::fetch( ArgType a, const Instruction& i, const Instructions& code )
        {
            return fetch( &a, 1u, i, code );
        }

        bool Arg_fetcher::fetch( ArgType a, ArgType b, const Instruction& i, const Instructions& code )
        {
            const ArgType args[] = { a, b };
            return fetch( args, 2u, i, code );
        }

        bool Arg_fetcher::fetch( ArgType a, ArgType b, ArgType c, const Instruction& i, const Instructions& code )
        {
            const ArgType args[] = { a, b, c };
            return fetch( args, 3u, i, code );
        }

        bool Arg_fetcher::fetch( const ArgType* types, unsigned int n, const Instruction& c, const Instructions& code )
        {
            written_args_flag_ = 0;
            fetch_result_      = GoodFetch;

            for( unsigned int i = 0; i < n; ++i )
            {
                if( c.is_indirection( i ) )
                {
                    const Instruction deref = code[ c.arg( i ) ];

                    // Only the arg0 portion of a value instruction is valid.
                    args_[i] = deref.arg( 0 );

                    // Record that we fetched arg i.
                    written_args_flag_ |= ( 1 << ( i + 1 ) );

                    if( ( types[i] == ValueArg || types[i] == DirectionArg ) && deref.cmd() != BLUE_ASM_NOP )
                    {
                        fetch_result_ = InvalidValue;
                    }
                    else if( types[i] == LocationArg && deref.cmd() != BLUE_ASM_LOCATION )
                    {
                        fetch_result_ = InvalidLocation;
                    }
                    else if( types[i] == DataArg && ( deref.cmd() != BLUE_ASM_LOCATION && deref.cmd() != BLUE_ASM_NOP ) )
                    {
                        fetch_result_ = InvalidValue;
                    }
                }
                else
                {
                    args_[i]   = c.arg( i );

                    // Record that we fetched arg i.
                    written_args_flag_ |= ( 1 << ( i + 1 ) );
                }
            }

            return fetch_result_ == GoodFetch;
        }
    }

    //-------------------------------------------------------------------------

    /// Private constructor for machine. Machines are constructed from
    /// blueprints through the 'create_machine' call. This ensures that
    /// the blueprint for this machine appears always as blueprint #0
    /// in the array of known blueprints and also constrains all machines
    /// to have blueprints; the blueprints themselves impose related
    /// constraints on the composition of machines.
    Machine::Machine(
        const Machine_name& name,
        const uint8_t team_hue_shift,
        Hardware&& hw,
        Hardpoint_collective&& hards,
        Instructions&& code,
        const unsigned int defaultPulsePeriod,
        std::vector<std::shared_ptr<Blueprint>>&& known_blueprints )
        : machines_name_(name)
        , classification_()
        , bounding_radius_( BLUE_MACHINE_DEFAULT_RADIUS )
        , known_blueprints_( move( known_blueprints ) )
        , grid_()
        , detect_hit_( nullptr )
        , machine_events_( nullptr )
        , trade_requests_( nullptr )
        , rand_( nullptr )
        , pathfinder_( nullptr )
        , path_search_handle_( nullptr )
        , watched_refcount_( 0u )
        , next_code_store_id_( 0u )
        , sent_instructions_()
        , message_buffer_()
        , current_message_()
        , navigation_instructions_()
        , navigation_instruction_pointer_( 0u )
        , navigation_expected_next_facing_( FacingTile0 )
        , navigation_expected_next_xy_()
        , sense_results_()
        , hardpoints_( std::move(hards) )
        , cargo_hold_()
        , hardware_( std::move(hw) )
        , instructions_( std::move( code ) )
        , pulse_variable_( 0u )
        , pulse_period_( calculate_pulse_period( defaultPulsePeriod, hw ) )
        , instruction_pointer_( 1u )
        , cycles_accumulator_( 0u )
        , propulsion_()
        , combat_system_()
        , blockingCondition_( NothingBlocking )
        , capacitance_( 0u )
        , external_cap_delta_( 0 )
        , error_code_( 0u )
        , credits_( 0u )
        , navigating_( false )
        , instruction_pointer_updated_( false )
        , self_destruct_triggered_( false )
        , team_hue_shift_( team_hue_shift )
    {
        // Calculate these from moved-in values.
        classification_ = Classification( *known_blueprints_.at(0u) );
        cargo_hold_     = Cargohold( hardware_.cargohold_volume() );
        propulsion_     = Propulsion( hardware_.is_mobile() ? Propulsion( hardware_ ) : Propulsion() );
        combat_system_  = Combat_system( hardware_.hull_hitpoints(), hardware_.armour_hitpoints(), make_resistances( hardware_ ) );

        assert( ! known_blueprints_.empty() );
        std::memset( blocking_variables_, 0, sizeof( blocking_variables_ ) );
        std::memset( readouts_, 0, sizeof( readouts_ ) );
        hardware_.validate();
        sense_results_.reserve( Hex_grid::count_of_hexagons_inside_ring( max_sensing_radius() ) );
        set_cpu_speed_readout();
        readouts_[ ReadoutOne ] = 1.0f;
        message_buffer_.reserve( hw.max_stored_messages() );
        // Note - neither plotted_route_ or navigation_instructions_ are
        // reserved - many bots may not use these as they are conditional on
        // the commands being called.
    }

    Machine::~Machine()
    {
        assert( watched_refcount_ == 0u );
        if( path_search_handle_ && pathfinder_ )
        {
            pathfinder_->cancel( path_search_handle_ );
            pathfinder_ = nullptr;
        }
    }


    std::unique_ptr<Machine> Machine::create_machine( Machine_template&& t )
    {
        Machine_template mt( std::move(t) );

        if( nullptr == mt.machines_blueprint.get() )
        {
            ss_throw("machine blueprint pointer is empty.");
        }

        // The machines own BP is required to be first in the machine's list.
        if( mt.known_blueprints.empty() )
        {
            mt.known_blueprints.push_back( mt.machines_blueprint );
        }
        else
        {
            if(*mt.known_blueprints.at(0) != *mt.machines_blueprint)
            {
                // Known bp must be first.
                mt.known_blueprints.insert(mt.known_blueprints.begin(), mt.machines_blueprint);
            }
        }

        auto hw  = mt.machines_blueprint->hardware_of_blueprint();
        auto hpc = Hardpoint_collective( hw, *mt.machines_blueprint );

        return std::make_unique<Machine>( mt.machines_name,
                                          mt.hue_shift,
                                          std::move(hw),
                                          std::move(hpc),
                                          std::move(mt.boot_code),
                                          mt.pulse_period,
                                          std::move(mt.known_blueprints) );
    }

/*
    /// Create a machine from its blueprint. Ensures that this blueprint always
    /// appears as blueprint #0 in the machine's inner blueprint list (if any).
    std::unique_ptr<machine> machine::create_machine( const std::string_view& machinesName,
                                                      const uint8_t hue_shift,
                                                      const std::shared_ptr<blueprint>& machinesBP,
                                                      const std::vector<std::shared_ptr<blueprint>>& knownBPs,
                                                      const unsigned int defaultPulsePeriod,
                                                      instructions&& bootCode )
    {
        assert( machinesBP.get() != nullptr );

        std::vector<std::shared_ptr<blueprint>> bps;
        bps.reserve( knownBPs.size() + 1u );
        bps.push_back( machinesBP );
        bps.insert( bps.begin(), knownBPs.cbegin(), knownBPs.cend() );

        const hardware hw = machinesBP->hardware_of_blueprint();

        return std::make_unique<machine>( machinesName,
                                          hue_shift,
                                          hw,
                                          Hardpoint_collective( hw, *machinesBP ),
                                          move( bootCode ),
                                          defaultPulsePeriod,
                                          move( bps ) );
    }
*/

    /// Called when the board process the machine_completed_build event, which is created
    /// by the complete_build machine function. This always adds a boot code instruction set
    /// at the given codeIndex.
    std::unique_ptr<Machine> Machine::build_machine( const uint16_t blueprintIndex,
                                                     const uint16_t codeIndex )
    {
        assert( blueprintIndex < known_blueprints_.size() );

        // TODO - how to handle childs names?
        ss_wrn("Bad machine childs name");
        Machine_name childsName( "XX" );

        auto childsBootCode = *pop_code( codeIndex );
        auto childsBP       = known_blueprints_[blueprintIndex];

        // Does an offspring know all of the parents BPOs?
        std::vector<std::shared_ptr<Blueprint>> childsKnownBPs;
        childsKnownBPs.reserve( known_blueprints_.size() );

        // Child's BP must be at zero.
        childsKnownBPs.push_back( childsBP );

        // Add other BPs known to parent.
        for( size_t i = 0u; i < known_blueprints_.size(); ++i )
        {
            if( i != blueprintIndex )
            {
                childsKnownBPs.push_back( known_blueprints_[i] );
            }
        }

        Hardware hw = childsBP->hardware_of_blueprint();
        Hardpoint_collective hpc( hw, *childsBP );

        return std::make_unique<Machine>( childsName,
                                          team_hue_shift(),
                                          std::move(hw),
                                          std::move(hpc),
                                          std::move( childsBootCode ),
                                          pulse_period_,
                                          move( childsKnownBPs ) );
    }

    void Machine::setup_externals( std::vector<Machine_event>& eventsDest,
                                   std::vector<Trade_request>& trade_requests,
                                   const Hit_by_line& detect_hit,
                                   Hex_pathfinder& pathfinder,
                                   Randoms& shared_randoms )
    {
        // Initialise aliased pointers.
        assert( detect_hit_          == nullptr );
        assert( machine_events_      == nullptr );
        assert( trade_requests_      == nullptr );
        assert( rand_                == nullptr );
        assert( path_search_handle_  == nullptr );
        assert( pathfinder_          == nullptr );

        detect_hit_     = &detect_hit;
        machine_events_ = &eventsDest;
        trade_requests_ = &trade_requests;
        rand_           = &shared_randoms;
        pathfinder_     = &pathfinder;
    }

    void Machine::add_to_board( const Hex_coord pos, const HexFacingName facing, Machine_grid_view grid )
    {
        if( grid_.has_grid() )
        {
            ss_throw( "Machine is already on board." );
        }

        if( false == grid.is_on_board( pos ) )
        {
            ss_throw( "New machine location is not on board." );
        }

        if( ! grid.is_tile_empty_and_unreserved( pos ) )
        {
            ss_throw( "Machine location is not empty and unreserved." );
        }

        grid_ = grid;

        propulsion_.reset( pos, facing );

        hardpoints_.change_location( pos );
        hardpoints_.change_orientation( fixedAngle_from_facing( facing ) );

        // Do a sense of zero tiles, so we fill in a default sense result.
        sense_results_.sense_surroundings( grid_, board_xy(), 0u );
    }

    void Machine::removed_from_board()
    {
        if( path_search_handle_ )
        {
            assert( pathfinder_ );
            pathfinder_->cancel( path_search_handle_ );
        }

        grid_           = Machine_grid_view();
        detect_hit_      = nullptr;
        machine_events_ = nullptr;
        trade_requests_ = nullptr;
        rand_           = nullptr;
        pathfinder_     = nullptr;
    }

    /// Returns the type and magnitude of the explosion if this machine blows
    /// up *NOW*
    Machine_exploded Machine::get_explosion() const noexcept
    {
        return get_machines_explosive_power( *this );
    }

    void Machine::start_advancing_n_tiles(
        bool wantMoveCountStored,
        uint16_t storageLine,
        uint16_t tiles )
    {
        assert( ! is_moving() );
        assert( is_not_blocked() );
        blockingCondition_     = WaitingForNTileMoves;
        blocking_variables_[0] = tiles; // This will be number of turns required.
        blocking_variables_[1] = storageLine;
        blocking_variables_[2] = wantMoveCountStored ? 1u : 0u;
        propulsion_.start_n_tile_move( grid_, tiles );
    }

    void Machine::n_tile_moves_completed()
    {
        assert( ! is_moving() );

        // blocking_variables_[2] contains 1 if move count should be stored.
        if( blocking_variables_[2] )
        {
            // User wants the number of tiles moved stored somewhere:
            store( blocking_variables_[1], propulsion_.number_of_tiles_advanced() );
        }
    }

    /// All the sensing costs have been paid (cap and time delay). Now
    /// do the sensing and return to user.
    void Machine::n_tiles_sensing_completed()
    {
        sense_results_.sense_surroundings( grid_, board_xy(), blocking_variables_[0] );

        // Check conditions:
        assert( ! sense_results_.is_empty() );
        assert( sense_results_.result_location( 0 ) == board_xy() );
        assert( sense_results_.get_sense_origin() == board_xy() );
    }

    void Machine::start_n_face_turns( uint16_t direction, uint16_t n_faces )
    {
        assert( ! is_moving() );
        assert( is_not_blocked() );

        // Clamp to max possible face turns.
        if( n_faces > BLUE_MAX_FACE_TURNS_LIMIT )
        {
            n_faces = BLUE_MAX_FACE_TURNS_LIMIT + ( n_faces % BLUE_MAX_FACE_TURNS_LIMIT );
        }

        propulsion_.start_n_face_turns( direction_means_counter_clockwise( direction ), n_faces );
        blockingCondition_     = WaitingForNFaceTurns;
        blocking_variables_[0] = n_faces;     // This will be number of turns required.
    }

    /// Checks to see if the blocking status changed.
    /// Returns true if the machine is no longer blocked.
    /// This is checked once per tick, and changes the blocking variables.
    void Machine::tick_blocked_status()
    {
        if( blockingCondition_ != NothingBlocking )
        {
            bool is_blocked = true;

            // Check to see if blocking should continue...
            switch( blockingCondition_ )
            {
                case WaitingForever:
                    break;

                case WaitingForNFaceTurns:
                    is_blocked = propulsion_.is_turning();
                    break;

                case WaitingForNTileMoves:
                    is_blocked = propulsion_.is_advancing();
                    break;

                case WaitingForSensingCompleted:
                    is_blocked = ( 0 != blocking_variables_[1] );
                    if( is_blocked )
                    {
                        assert( blocking_variables_[1] > 0u );
                        // Tick down the time it takes to complete sensing:
                        --blocking_variables_[1];
                    }
                    break;

                case WaitingForSynch:
                case WaitingForAStarRoute:
                case WaitingForRoutePlot:
                case WaitingForTrade:
                    assert( blocking_variables_[0] > 0u );
                    --blocking_variables_[0];
                    is_blocked = ( 0u != blocking_variables_[0] );
                    break;

                case WaitingForTransmitCapCostToBePaid:
                    assert( blocking_variables_[0] > 0u );
                    blocking_variables_[0] = cap_try_pay_cap( blocking_variables_[0] );
                    is_blocked = ( 0u != blocking_variables_[0] );
                    break;

                case WaitingForBroadcastCapCostToBePaid:
                    assert( blocking_variables_[0] > 0u );
                    blocking_variables_[0] = cap_try_pay_cap( blocking_variables_[0] );
                    is_blocked = ( 0u != blocking_variables_[0] );
                    break;

                case WaitingForBuildToComplete:

                    if( blocking_variables_[0] > 0u )
                    {
                        -- blocking_variables_[0];
                    }

                    if( blocking_variables_[2] > 0u )
                    {
                        blocking_variables_[2] = cap_try_pay_cap( blocking_variables_[2] );
                    }

                    // Block until all time has passed and all cap is paid.
                    is_blocked = ( blocking_variables_[0] > 0u || blocking_variables_[2] > 0u );
                    break;

                case WaitingForRefiningCycleToEnd:
                    // 0 = cap, 1 = time, 2 = units remaining, 3 = type of ore
                    if( blocking_variables_[0] > 0u )
                    {
                        blocking_variables_[0] = cap_try_pay_cap( blocking_variables_[0] );
                    }

                    if( blocking_variables_[1] > 0u )
                    {
                        -- blocking_variables_[1];
                    }

                    // Block until all time has passed and all cap is paid.
                    is_blocked = ( blocking_variables_[0] > 0u || blocking_variables_[1] > 0u );
                    break;

                    break;

                default:
                    ss_unreachable;
            }

            // If it became un-blocked...
            if( ! is_blocked )
            {
                // Save blocking condition for switch loop. This allows us
                // to continue or change blocking.
                auto oldBlockingReason = blockingCondition_;

                // Blocking ended, reset blocking condition.
                blockingCondition_ = NothingBlocking;

                switch( oldBlockingReason )
                {
                    case WaitingForNTileMoves:
                        n_tile_moves_completed();
                        break;

                    case WaitingForSensingCompleted:
                        n_tiles_sensing_completed();
                        break;

                    case WaitingForever:
                    case NothingBlocking:
                    case WaitingForNFaceTurns:
                    case WaitingForSynch:
                        break;

                    case WaitingForTransmitCapCostToBePaid:
                        // Cap cost for transmission was paid.
                        do_transmission(
                            Hex_coord::from_uint16( blocking_variables_[1] ),
                            blocking_variables_[2],
                            blocking_variables_[3] );
                        // Block until next frame so we receive broadcast.
                        synch( 1 );
                        break;

                    case WaitingForBroadcastCapCostToBePaid:
                        // Cap cost for broadcast was paid.
                        do_broadcast(
                            blocking_variables_[1],
                            blocking_variables_[2],
                            blocking_variables_[3] );
                        // Block until next frame so we receive broadcast.
                        synch( 1 );
                        break;

                    case WaitingForBuildToComplete:
                        complete_build(
                            blocking_variables_[3],
                            Hex_coord::from_uint16( blocking_variables_[4] ),
                            blocking_variables_[5],
                            blocking_variables_[6] );
                        break;

                    case WaitingForAStarRoute:
                        complete_search_and_start_navigating();
                        break;

                    case WaitingForRoutePlot:
                        complete_route_plotting();
                        break;

                    case WaitingForRefiningCycleToEnd:
                        end_a_refining_cycle();
                        break;

                    case WaitingForTrade:
                        break;

                    default:
                        ss_unreachable;
                }

            }
        }
    }

    float Machine::build_progression() const noexcept
    {
        assert( blockingCondition_ != WaitingForBuildToComplete || blocking_variables_[1] > 0.0f );
        return blockingCondition_ == WaitingForBuildToComplete
            ? ( static_cast<float>( blocking_variables_[0] ) / blocking_variables_[1] )
            : 0.0f;
    }

    void Machine::update_readouts()
    {
        pulse_variable_ = ( 1u + pulse_variable_ ) % pulse_period_;
        float pulser0 = pulse_variable_ / static_cast<float>( pulse_period_ );
        float pulser1 = 0.5f + pulser0;
        readouts_[ Pulse0 ]                 = 0.5f + 0.5f * ( 0.5f + 0.5f * std::sin( BLUE_TWO_PI * pulser0 ) );
        readouts_[ Pulse1 ]                 = 0.5f + 0.5f * ( 0.5f + 0.5f * std::sin( BLUE_TWO_PI * pulser1 ) );
        readouts_[ ReadoutAdvanceSpeedLvl ] = propulsion_.speed_level();
        readouts_[ ReadoutTurnSpeedLvl ]    = propulsion_.turn_speed_level();
        readouts_[ ReadoutCapacitorLvl ]    = capacitance_ ? static_cast<float>( capacitance_ ) / hardware_.cap_capacity() : 0.0f;
        readouts_[ ReadoutSensingLvl ]      = blockingCondition_ == WaitingForSensingCompleted         ? 1.0f : 0.0f;
        readouts_[ ReadoutBroadcastingLvl ] = blockingCondition_ == WaitingForTransmitCapCostToBePaid  ? 1.0f : 0.0f;
        readouts_[ ReadoutTransmittingLvl ] = blockingCondition_ == WaitingForBroadcastCapCostToBePaid ? 1.0f : 0.0f;
        readouts_[ ReadoutBuildingLvl ]     = build_progression();
    }

    void Machine::resolve_movement( Movement_grid_view g )
    {
        // Copy pre-conditions:
        const Position_info positionBefore = get_position_info();

        // Motion systems do not use cap.
        tick_motion_system( g );

        // See what changed and broadcast changes.
        const Position_info positionNow = get_position_info();
        if( positionNow != positionBefore )
        {
            if( positionNow.angle != positionBefore.angle )
            {
                hardpoints_.change_orientation( positionNow.angle );
            }

            machine_events_->push_back( Machine_event::machine_positional_update( *this ) );

            if( positionNow.gridXY != positionBefore.gridXY )
            {
                hardpoints_.change_location( positionNow.gridXY );
                machine_events_->push_back( Machine_event::machine_change_tile( *this, positionBefore.gridXY, positionNow.gridXY ) );
            }
        }
    }

    /// Ticks the machine forward one frame. Performs one frames worth of
    /// gameplay.
    void Machine::advance_one_frame()
    {
        // Check that all events were processed and we are not slowly filling
        // the broadcast/transmission buffer up (a slow memory leak).
        assert( sent_instructions_.empty() );

        // Make changes.
        tick_capacitor_system();
        tick_hardpoint_systems();
        tick_blocked_status();
        tick_instructions();

        // Later this could be moved into write-on-change
        // instead of always write.
        update_readouts();
    }

    /// Machine receives an external broadcast. This may have originated from
    /// another machine, a player, or maybe even itself.
    void Machine::receive_broadcast( const std::shared_ptr<const Instructions>& code )
    {
        ss_dbg( machines_name_.c_str(), " received broadcast" );

        if( message_buffer_.size() < max_stored_messages() )
        {
            message_buffer_.push_back( code );
        }
        else
        {
            if( max_stored_messages() == 0u )
            {
                ss_dbg( machines_name_.c_str(), " has zero sized msg buffer" );
            }
            else
            {
                ss_dbg( machines_name_.c_str(), " msg buffer full" );
            }
        }
    }

    /// A machine can execute several instructions per tick, or may
    /// have to wait several ticks to execute an instruction.
    /// A machine will try to execute as many instructions as it can
    /// but the actual amount executed will be limited by the cap.
    /// If a machine runs out of cap it will continue to "spin" and
    /// waste cycles.
    void Machine::tick_instructions()
    {
        assert( hardware_.cycles_per_instruction() > 0u );
        assert( cycles_accumulator_ + hardware_.processor_speed_cycles_per_tick() < USHRT_MAX );

        if( is_not_blocked() )
        {
            cycles_accumulator_ += hardware_.processor_speed_cycles_per_tick();
            while( cycles_accumulator_ > hardware_.cycles_per_instruction() )
            {
                cycles_accumulator_ -= hardware_.cycles_per_instruction();

                if( is_not_blocked() )
                {
                    // Inside this call the machine will subtract cap and
                    // 'spin', using cycles even if the cap is out.
                    execute_current_instruction();
                }
            }
        }
    }

    /// The capacitor does not refill linearly. It fills faster when empty
    /// and its rate of recharge slows as it fills up. Here is where and when
    /// any external changes to the capacitor are applied.
    void Machine::tick_capacitor_system()
    {
        assert( hardware_.cap_capacity() > 0 );
        assert( capacitance_ <= hardware_.cap_capacity() );

        // Use 2x - x*x as formula, where 0 <= x <= 1
        float x = static_cast<float>( capacitance_ ) / hardware_.cap_capacity();
        assert( 0.0f <= x && x <= 1.0f );

        uint16_t generated_cap = cap_refill_per_tick_from_engines();

        // Add capacitance received via solar panels
        uint16_t cap_from_suns = cap_from_solar_panels_at_current_tile();

        capacitance_ += static_cast<uint16_t>( generated_cap * ( 1.0f - ( x * 2.0f - x ) ) );
        assert( capacitance_ <= hardware_.cap_capacity() );

        // Apply external changes, solar power and reset external delta.
        int c = std::min<int>( std::numeric_limits<uint16_t>::max(), static_cast<int>( capacitance_ ) + cap_from_suns + external_cap_delta_ );
        capacitance_ = ( c > 0 ) ? static_cast<uint16_t>( c ) : 0u;
        external_cap_delta_ = 0;

        // Clamp to upper hardware limits.
        capacitance_ = capacitance_ > hardware_.cap_capacity() ? hardware_.cap_capacity() : capacitance_;
    }

    /// Applies the per-instruction cap cost. Expects the machine to be in the
    /// unblocked state.
    void Machine::execute_current_instruction()
    {
        assert( is_not_blocked() );

        if( cap_try_pay_single_instruction_cost() )
        {
            if( ! navigating_ )
            {
                execute_code_instruction();
            }
            else
            {
                execute_navigation_instruction();
            }
        }
    }

    /// Navigation instructions are stored in vector and not an instructions
    /// class - we do this to allow us to refill the array and re-use the memory.
    /// This means that these instructions use a zero based instruction pointer.
    /// Also, the navigation instruction buffer is not a loop.
    void Machine::execute_navigation_instruction()
    {
        assert( navigation_instruction_pointer_ <= navigation_instructions_.size() );
        assert( navigating_ );
        assert( ! navigation_instructions_.empty() );

        if( navigation_instruction_pointer_ > 0u )
        {
            // At least one instruction was executed. We require each
            // instruction to succeed to continue navigating.
            if( navigation_expected_next_facing_ != facing_direction() ||
                    navigation_expected_next_xy_ != board_xy() )
            {
                // Something blocked the navigation.
                end_navigation();
            }
            else if( navigation_instruction_pointer_ == navigation_instructions_.size() )
            {
                // Navigation successfully completed.
                end_navigation();
            }
        }

        if( navigating_ )
        {
            Instruction i = navigation_instructions_[navigation_instruction_pointer_];
            ++navigation_instruction_pointer_;

            // These navigation instructions are machine assembled, we
            // know they contain no indirections.
            switch( i.cmd() )
            {
                case BLUE_ASM_TURN:
                {
                    assert( ! i.is_indirection( 0 ) );
                    assert( ! i.is_indirection( 1 ) );

                    const uint16_t turns_count    = i.arg( 0u );
                    const uint16_t turn_direction = i.arg( 1u );

                    bool turn_ccw = direction_means_counter_clockwise( turn_direction );
                    navigation_expected_next_facing_ = propulsion_.facing_after_n_turns( turn_ccw, turns_count );
                    navigation_expected_next_xy_     = board_xy();

                    command_turn( turn_direction, turns_count );
                }
                break;

                case BLUE_ASM_ADVANCE:
                {
                    assert( ! i.is_indirection( 0u ) );
                    navigation_expected_next_facing_ = facing_direction();
                    navigation_expected_next_xy_     = Hex_grid::calculate_neighbour_coord( board_xy(), navigation_expected_next_facing_, i.arg( 0u ) );
                    command_advance( i.arg( 0u ), 0u, false );
                }
                break;

                default:
                    ss_unreachable;
            }
        }
    }

    void Machine::execute_code_instruction()
    {
        assert( instruction_pointer_ > 0u );
        assert( instruction_pointer_ <= instructions_.size() );
        assert( false == instruction_pointer_updated_ );

        const size_t instruction_index = instruction_pointer_;
        Instruction i = instructions_[instruction_pointer_];

        Arg_fetcher args;

        switch( i.cmd() )
        {
            case BLUE_ASM_NOP:
                // This instruction is in fact only data. See also BLUE_ASM_LOCATION
                break;

            case BLUE_ASM_JMP:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_set_instruction_pointer( args.value( 0u ) );
                }
                break;

            case BLUE_ASM_STORE_JMP:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), static_cast<uint16_t>( instruction_pointer_ ) );
                    command_set_instruction_pointer( args.value( 1u ) );
                }
                break;

            case BLUE_ASM_COPY:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    if( i.is_indirection( 0u ) )
                    {
                        // Move, e.g. [TO] = [FROM]
                        command_copy( args.value( 0u ), args.value( 1u ) );
                    }
                    else
                    {
                        // Assignment, e.g. [TO] = "FROM"
                        command_store_value( args.value( 1u ), args.value( 0u ) );
                    }
                }
                break;

            case BLUE_ASM_PRINT_TXT:
                machine_events_->push_back( Machine_event::machine_text( *this, i.args_as_chars() ) );
                break;

            case BLUE_ASM_PRINT_VAL:
                if( i.is_indirection( 0 ) )
                {
                    // Show full instruction
                    machine_events_->push_back( Machine_event::machine_print_instruction( *this, instructions_[i.arg( 0u )] ) );
                }
                else
                {
                    // Show value
                    machine_events_->push_back( Machine_event::machine_text( *this, i.arg( 0u ) ) );
                }
                break;

            case BLUE_ASM_ADD:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_math( i, add );
                }
                break;

            case BLUE_ASM_SUB:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_math( i, sub );
                }
                break;

            case BLUE_ASM_MUL:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_math( i, mul );
                }
                break;

            case BLUE_ASM_DIV:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_math( i, div );
                }
                break;

            case BLUE_ASM_RAND_RANGE:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), get_rand( args.value( 1u ), args.value( 2u ) ) );
                }
                break;

            case BLUE_ASM_RAND:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), get_rand( 0u, USHRT_MAX - 1 ) );
                }
                break;

            case BLUE_ASM_CAP:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), capacitance_ );
                }
                break;

            case BLUE_ASM_MAX_CAP:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), hardware_.cap_capacity() );
                }
                break;

            case BLUE_ASM_SYNCH:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    synch( args.value( 0u ) );
                }
                break;

            case BLUE_ASM_SENSE_MAX:
                command_sense( max_sensing_radius() );
                break;

            case BLUE_ASM_SENSE:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_sense( args.value( 0u ) );
                }
                break;

            case BLUE_ASM_TURN:
                // arg0 is turn count, arg1 is direction.
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_turn( args.direction( 1u ), args.value( 0u ) );
                }
                break;

            case BLUE_ASM_ADVANCE:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_advance( args.value( 0u ), 0u, false );
                }
                break;

            case BLUE_ASM_ADVANCE_WITH_STORE:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_advance( args.value( 0u ), args.value( 1u ), true );
                }
                break;

            case BLUE_ASM_SENSE_RESULT_COUNT:
                assert( ! sense_results_.is_empty() );
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), sense_results_.result_count() );
                }
                break;

            case BLUE_ASM_SENSE_RESULT_LOCATION:
                assert( ! sense_results_.is_empty() );
                // arg0 is where to store the location, arg1 is which index to fetch.
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    uint16_t index = args.value( 1u ) % sense_results_.result_count();
                    command_store_location( args.value( 0u ), sense_results_.result_location( index ) );
                }
                break;

            case BLUE_ASM_SENSE_RESULT_TYPE:
                assert( ! sense_results_.is_empty() );
                // arg0 is where to store the location, arg1 is which index to fetch.
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    uint16_t index = args.value( 1u ) % sense_results_.result_count();
                    command_store_value( args.value( 0u ), sense_results_.result_type( index ) );
                }
                break;

            case BLUE_ASM_MAX_SENSE_RANGE:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), max_sensing_radius() );
                }
                break;

            case BLUE_ASM_CW_TURN_COUNT_TO:
                // arg1 is line location is stored at
                assert( i.is_indirection( 1u ) );
                if( args.fetch( ValueArg, LocationArg, i, instructions_ ) )
                {
                    command_get_clockwise_turn_count( args.value( 0u ), args.location( 1u ) );
                }
                break;

            case BLUE_ASM_DISTANCE_TO:
                // arg1 is line location is stored at
                assert( i.is_indirection( 1u ) );
                if( args.fetch( ValueArg, LocationArg, i, instructions_ ) )
                {
                    command_get_distance_to( args.value( 0u ), args.location( 1u ) );
                }
                break;

            case BLUE_ASM_DISTANCE_BETWEEN:
                assert( i.is_indirection( 1u ) );
                assert( i.is_indirection( 2u ) );
                if( args.fetch( ValueArg, LocationArg, LocationArg, i, instructions_ ) )
                {
                    // arg0 is where to store the location.
                    // arg1 and arg2 are lines locations are stored at
                    command_get_distance_between( args.value( 0u ), args.location( 1u ), args.location( 2u ) );
                }
                break;

            case BLUE_ASM_GET_LOCATION:
                assert( ! is_moving() );
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_location( args.value( 0u ), board_xy() );
                }
                break;

            case BLUE_ASM_LINENUMBER:
                // Line number corresponds to the instruction pointer.
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), static_cast<uint16_t>( instruction_pointer_ ) );
                }
                break;

            case BLUE_ASM_GET_LINENUMBER_PLUS:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    const auto currentLine = static_cast<uint16_t>( instruction_pointer_ );
                    command_store_value( args.value( 0u ), args.value( 1u ) + currentLine );
                }
                break;

            case BLUE_ASM_LINE_COUNT:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), instructions_.size() );
                }
                break;

            case BLUE_ASM_MAX_TRANSMIT_RANGE:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), hardware_.max_transmit_range() );
                }
                break;

            case BLUE_ASM_TRANSMIT:
                if( args.fetch( LocationArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    // 0 is line destination location is stored at
                    // 1 is what code to begin transmitting.
                    // 2 is how much code to transmit.
                    command_transmit( args.location( 0u ), args.value( 1u ), args.value( 2u ) );
                }
                break;

            case BLUE_ASM_MAX_BROADCAST_RANGE:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), hardware_.max_broadcast_range() );
                }
                break;

            case BLUE_ASM_BROADCAST:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    // 0 is how far to broadcast
                    // 1 is what code to begin transmitting.
                    // 2 is how much code to transmit.
                    command_broadcast( args.value( 0u ), args.value( 1u ), args.value( 2u ) );
                }
                break;

            case BLUE_ASM_GET_BLUEPRINT_COUNT:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), static_cast<uint16_t>( known_blueprints_.size() ) );
                }
                break;

            case BLUE_ASM_XOR:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_math( i, Xor );
                }
                break;

            case BLUE_ASM_OR:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_math( i, Or );
                }
                break;

            case BLUE_ASM_AND:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_math( i, And );
                }
                break;

            case BLUE_ASM_EQ:
                if( args.fetch( ValueArg, DataArg, DataArg, i, instructions_ ) )
                {
                    command_math( i, Eq );
                }
                break;

            case BLUE_ASM_NEQ:
                if( args.fetch( ValueArg, DataArg, DataArg, i, instructions_ ) )
                {
                    command_math( i,  NEq );
                }
                break;

            case BLUE_ASM_BOOL_AND:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_math( i, bool_and );
                }
                break;

            case BLUE_ASM_BOOL_OR:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_math( i, bool_or );
                }
                break;

            case BLUE_ASM_BOOL_LT:
                if( args.fetch( ValueArg, DataArg, DataArg, i, instructions_ ) )
                {
                    command_math( i, bool_lt );
                }
                break;

            case BLUE_ASM_RIGHT_SHIFT:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_math( i, right_shift );
                }
                break;

            case BLUE_ASM_LEFT_SHIFT:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_math( i, left_shift );
                }
                break;

            case BLUE_ASM_BOOL_GT:
                if( args.fetch( ValueArg, DataArg, DataArg, i, instructions_ ) )
                {
                    command_math( i, bool_gt );
                }
                break;

            case BLUE_ASM_BOOL_LTE:
                if( args.fetch( ValueArg, DataArg, DataArg, i, instructions_ ) )
                {
                    command_math( i, bool_lte );
                }
                break;

            case BLUE_ASM_BOOL_GTE:
                if( args.fetch( ValueArg, DataArg, DataArg, i, instructions_ ) )
                {
                    command_math( i, bool_gte );
                }
                break;

            case BLUE_ASM_MODULO:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_math( i, modulo );
                }
                break;

            case BLUE_ASM_JMP_EQ:
                if( args.fetch( DataArg, DataArg, ValueArg, i, instructions_ ) )
                {
                    if( args.value( 0u ) == args.value( 1u ) )
                    {
                        command_set_instruction_pointer( args.value( 2u ) );
                    }
                }
                break;

            case BLUE_ASM_JMP_NEQ:
                if( args.fetch( DataArg, DataArg, ValueArg, i, instructions_ ) )
                {
                    if( args.value( 0u ) != args.value( 1u ) )
                    {
                        command_set_instruction_pointer( args.value( 2u ) );
                    }
                }
                break;

            case BLUE_ASM_JMP_LT:
                if( args.fetch( DataArg, DataArg, ValueArg, i, instructions_ ) )
                {
                    if( args.value( 0u ) < args.value( 1u ) )
                    {
                        command_set_instruction_pointer( args.value( 2u ) );
                    }
                }
                break;

            case BLUE_ASM_JMP_GT:
                if( args.fetch( DataArg, DataArg, ValueArg, i, instructions_ ) )
                {
                    if( args.value( 0u ) > args.value( 1u ) )
                    {
                        command_set_instruction_pointer( args.value( 2u ) );
                    }
                }
                break;

            case BLUE_ASM_JMP_LTE:
                if( args.fetch( DataArg, DataArg, ValueArg, i, instructions_ ) )
                {
                    if( args.value( 0u ) <= args.value( 1u ) )
                    {
                        command_set_instruction_pointer( args.value( 2u ) );
                    }
                }
                break;

            case BLUE_ASM_JMP_GTE:
                if( args.fetch( DataArg, DataArg, ValueArg, i, instructions_ ) )
                {
                    if( args.value( 0u ) >= args.value( 1u ) )
                    {
                        command_set_instruction_pointer( args.value( 2u ) );
                    }
                }
                break;

            case BLUE_ASM_ABS:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), static_cast<uint16_t>( abs( static_cast<int16_t>( args.value( 1u ) ) ) ) );
                }
                break;

            case BLUE_ASM_SWAP_CODE:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_swap_code( args.value( 0u ), args.value( 1u ), args.value( 2u ) );
                }
                break;

            case BLUE_ASM_COPY_CODE:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_copy_code( args.value( 0u ), args.value( 1u ), args.value( 2u ) );
                }
                break;

            case BLUE_ASM_COMPARE_CODE_LINE:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    // Result is stored at 0, compare 1 and 2.
                    command_compare_code( args.value( 0u ), args.value( 1u ), args.value( 2u ) );
                }
                break;

            case BLUE_ASM_ASSERT:
                assert( ! i.is_indirection( 2u ) );
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_assert( args.value( 0u ), args.value( 1u ), args.value( 2u ) );
                }
                break;

            case BLUE_ASM_MSG_CLEAR_MSGS:
                command_clear_msgs();
                break;

            case BLUE_ASM_COPY_MSGBUF_CODE:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    // Src, dest, count.
                    command_copy_msgbuf_code( args.value( 0u ), args.value( 1u ), args.value( 2u ) );
                }
                break;

            case BLUE_ASM_MOVE_MSG_MSGBUF:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_move_msg_to_msgbuf( args.value( 0u ) );
                }
                break;

            case BLUE_ASM_MSGBUF_LINE_COUNT:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), current_message_ ? current_message_->size() : 0u );
                }
                break;

            case BLUE_ASM_GET_MSGS_COUNT:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), static_cast<uint16_t>( message_buffer_.size() ) );
                }
                break;

            case BLUE_ASM_GET_MAX_MSGS:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), hardware_.max_stored_messages() );
                }
                break;

            case BLUE_ASM_CAN_BUILD:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    // arg0 is where to store the result.
                    // arg1 is what blueprint to use.
                    command_can_build( args.value( 1u ), args.value( 0u ) );
                }
                break;

            case BLUE_ASM_BUILD:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    // What blueprint, where to start copying code from, how much code to copy.
                    command_build( args.value( 0u ), args.value( 1u ), args.value( 2u ) );
                }
                break;

            case BLUE_ASM_CAN_AIM_AT:
                assert( i.is_indirection( 1u ) );
                if( args.fetch( ValueArg, LocationArg, i, instructions_ ) )
                {
                    command_can_aim_at( args.value( 0u ), args.location( 1 ) );
                }
                break;

            case BLUE_ASM_AIM_AT:
                assert( i.is_indirection( 0u ) );
                if( args.fetch( LocationArg, i, instructions_ ) )
                {
                    command_aim_at( args.location( 0u ) );
                }
                break;

            case BLUE_ASM_CAN_FIRE_AT:
                assert( i.is_indirection( 1u ) );
                if( args.fetch( ValueArg, LocationArg, i, instructions_ ) )
                {
                    command_can_fire_at( args.value( 0u ), args.location( 1u ) );
                }
                break;

            case BLUE_ASM_FIRE_AT:
                assert( i.is_indirection( 0u ) );
                if( args.fetch( LocationArg, i, instructions_ ) )
                {
                    command_fire_at( args.location( 0u ) );
                }
                break;

            case BLUE_ASM_HAS_LINE_OF_FIRE_TO:
                assert( i.is_indirection( 1u ) );
                if( args.fetch( ValueArg, LocationArg, i, instructions_ ) )
                {
                    command_has_line_of_fire_to( args.value( 0u ), args.location( 1u ) );
                }
                break;

            case BLUE_ASM_SELF_DESTRUCT:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_self_destruct( args.value( 0u ) );
                }
                break;

            case BLUE_ASM_IS_LOCATION:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    // Store 'true' if arg1 instruction is a location type.
                    command_store_value( args.value( 0u ), static_cast<uint16_t>( instructions_[ args.value( 1u ) ].cmd() == BLUE_ASM_LOCATION ) );
                }
                break;

            case BLUE_ASM_IS_CARGO_LOCKED:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), cargo_hold_.is_locked() ? 1u : 0u );
                }
                break;

            case BLUE_ASM_LOCK_CARGO:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    cargo_hold_.set_locked( args.value( 0u ) > 0u );
                }
                break;

            case BLUE_ASM_CARGOHOLD_FREE_SPACE:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), cargo_hold_.free_volume() );
                }
                break;

            case BLUE_ASM_CARGOHOLD_SPACE_USED:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), cargo_hold_.used_volume() );
                }
                break;

            case BLUE_ASM_CARGOHOLD_VOLUME:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), cargo_hold_.total_volume() );
                }
                break;

            case BLUE_ASM_CARGOHOLD_UNITS_OF:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    if( args.value( 1u ) >= BLUE_TRADABLE_TYPES_COUNT )
                    {
                        command_store_value( args.value( 0u ), 0u );
                    }
                    else
                    {
                        command_store_value( args.value( 0u ), cargo_hold_[args.value( 1u )] );
                    }
                }
                break;

            case BLUE_ASM_CARGOHOLD_VOL_OF:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    if( args.value( 1u ) >= BLUE_TRADABLE_TYPES_COUNT )
                    {
                        command_store_value( args.value( 0u ), 0u );
                    }
                    else
                    {
                        const auto type_in_cargo  = static_cast<TradableTypes>( args.value( 1u ) );
                        const auto units_in_cargo = cargo_hold_[ args.value( 1u ) ];
                        const auto units_volume   = static_cast<uint16_t>( volume_of_n_units_of( type_in_cargo, units_in_cargo ) );
                        command_store_value( args.value( 0u ), units_volume );
                    }
                }

                break;

            case BLUE_ASM_GIVE_CARGO_TO:
                if( args.fetch( LocationArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_give_cargo_to( args.location( 0u ), args.value( 1u ), args.value( 2u ) );
                }
                break;

            case BLUE_ASM_TAKE_CARGO_FROM:
                if( args.fetch( LocationArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_take_cargo_from( args.location( 0 ), args.value( 1 ), args.value( 2 ) );
                }
                break;

            case BLUE_ASM_NAVIGATE_TO:
                assert( i.is_indirection( 0u ) );
                if( args.fetch( LocationArg, i, instructions_ ) )
                {
                    command_navigate_to( args.location( 0u ) );
                }
                break;

            case BLUE_ASM_NAVIGATE_N_TOWARDS:
                assert( i.is_indirection( 1u ) );
                if( args.fetch( ValueArg, LocationArg, i, instructions_ ) )
                {
                    command_navigate_n_towards( args.value( 0u ), args.location( 1u ) );
                }
                break;

            case BLUE_ASM_SET_ASTAR_WEIGHT:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_set_astar_weight( args.value( 0u ), args.value( 1u ) );
                }
                break;

            case BLUE_ASM_GET_ASTAR_WEIGHT:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), astar_weight( args.value( 1u ) ) );
                }
                break;

            case BLUE_ASM_REFINE:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_refine( args.value( 0u ), args.value( 1u ) );
                }
                break;

            case BLUE_ASM_GET_REFINE_PERIOD:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), refine_period( args.value( 1u ) ) );
                }
                break;

            case BLUE_ASM_PLOT_ROUTE_BETWEEN:
                assert( i.is_indirection( 0u ) );
                assert( i.is_indirection( 1u ) );
                if( args.fetch( LocationArg, LocationArg, i, instructions_ ) )
                {
                    command_plot_route_between( args.location( 1u ), args.location( 1u ) );
                }
                break;

            case BLUE_ASM_GET_ROUTE_LENGTH:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), route_length() );
                }
                break;

            case BLUE_ASM_COPY_ROUTE:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_copy_route( args.value( 0u ), args.value( 1u ), args.value( 2u ) );
                }
                break;

            case BLUE_ASM_SET_ERROR_CODE:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    set_error_code( args.value( 0u ) );
                }
                break;

            case BLUE_ASM_GET_ERROR_CODE:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), error_code_ );
                }
                break;

            case BLUE_ASM_GET_PRICE:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_get_price_of( args.value( 0u ), static_cast<uint8_t>( args.value( 1u ) ) );
                }
                break;

            case BLUE_ASM_GET_AVAILABILITY:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_get_availability_of( args.value( 0u ), static_cast<uint8_t>( args.value( 1u ) ) );
                }
                break;

            case BLUE_ASM_TRY_BUY:
                if( args.fetch( ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_try_buy( static_cast<uint8_t>( args.value( 0u ) ), args.value( 1u ) );
                }
                break;

            case BLUE_ASM_TRY_BUY_WITH_STORE:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_try_buy_with_store( args.value( 0u ), static_cast<uint8_t>( args.value( 1u ) ), args.value( 2u ) );
                }
                break;

            case BLUE_ASM_FOR_SALE:
                if( args.fetch( ValueArg, ValueArg, ValueArg, i, instructions_ ) )
                {
                    command_for_sale( static_cast<uint8_t>( args.value( 0u ) ), args.value( 1u ), args.value( 2u ) );
                }
                break;

            case BLUE_ASM_CANCEL_SALES:
                command_cancel_sales();
                break;

            case BLUE_ASM_GET_CREDITS_BALANCE:
                if( args.fetch( ValueArg, i, instructions_ ) )
                {
                    command_store_value( args.value( 0u ), credits_ );
                }
                break;

            case BLUE_ASM_TRANSFER_CREDITS_TO:
                if( args.fetch( LocationArg, ValueArg, i, instructions_ ) )
                {
                    command_transfer_credits_to( args.location( 0u ), args.value( 1u ) );
                }
                break;

            case BLUE_ASM_LOCATION:
                // This 'instruction' is in fact a data block containing
                // a location. A no-op instruction. See also BLUE_ASM_NOP.
                break;

            default:
                // An instruction we did not understand. This could be due to
                // malformed ASM. Bad instructions are not program errors as they
                // cause no harm, but they are errors of interest to the user,
                // so we report them as such.
                ss_err( "Unknown instruction ", i.cmd() );
                signal_error( UnknownInstruction );
                break;
        }

        if( args.bad_fetch() )
        {
            ss_wrn( "Bad fetch at line ", instruction_index );
            signal_error( static_cast<Machine::ErrorType>( args.fetch_result() ) );
        }

        if( ! instruction_pointer_updated_ )
        {
            ++instruction_pointer_;
        }
        else
        {
            instruction_pointer_updated_ = false;
        }

        // Instructions automatically loop.
        if( instruction_pointer_ > instructions_.size() )
        {
            instruction_pointer_ = 1u;
        }

        assert( instruction_pointer_ > 0u );
        assert( instruction_pointer_ <= instructions_.size() );
    }

    void Machine::synch( const uint16_t n )
    {
        if( n > 0u )
        {
            assert( ! is_blocked() );
            blockingCondition_    = WaitingForSynch;
            blocking_variables_[0] = n;
        }
    }

    void Machine::signal_error( ErrorType e )
    {
        machine_events_->push_back( Machine_event::machine_error( *this, static_cast<uint16_t>( instruction_pointer_ ), static_cast<uint8_t>( e ) ) );
        set_error_code( e );
    }

    uint16_t Machine::get_rand( uint16_t A, uint16_t B )
    {
        assert( rand_ );

        uint16_t a = A < B ? A : B;
        uint16_t b = A < B ? B : A;

        assert( b < USHRT_MAX );

        uint16_t r = ( nullptr == rand_ ) ? uint16_t(0u) : a + static_cast<uint16_t>( rand_->urand() % ( uint16_t(1u) + b - a ) );

        assert( r >= a );
        assert( r <= b );

        return r;
    }

    uint16_t Machine::cap_try_pay_cap( uint16_t cost )
    {
        if( capacitance_ >= cost )
        {
            // It was paid.
            capacitance_ -= cost;
            cost = 0;
        }
        else
        {
            cost -= capacitance_;
            capacitance_ = 0u;
        }

        return cost;
    }

    uint16_t Machine::cap_try_extract_sensing( uint16_t senseRadius )
    {
        unsigned int costPerTile = cap_sense_cost_per_tile();
        unsigned int n_tiles     = Hex_grid::count_of_hexagons_inside_ring( senseRadius ) - 1u;
        unsigned int capCost     = n_tiles * costPerTile;

        if( capCost > capacitance_ )
        {
            // Not enough cap. See how far we could sense...
            while( senseRadius > 0u && capCost > capacitance_ )
            {
                --senseRadius;
                n_tiles  = Hex_grid::count_of_hexagons_inside_ring( senseRadius ) - 1u;
                capCost  = n_tiles * costPerTile;
            }
        }

        // Apply cap cost (which may be zero). capCost is guaranteed to be less
        // than capacitance at this point.
        capacitance_ -= static_cast<uint16_t>( capCost );

        return senseRadius;
    }

    void Machine::command_math( const Instruction& i, const math_function_t op )
    {
        // Read all variables before performing op, in case the op has some
        // side effect which changes the read values.
        const uint16_t value_line = i.is_indirection( 0u ) ? instructions_[ i.arg( 0u ) ].arg( 0u ) : i.arg( 0u );

        const bool lhsOpIsValue  = i.is_indirection( 1u ) ? ( instructions_[ i.arg( 1u ) ].cmd() == BLUE_ASM_NOP ) : true;
        const bool rhsOpIsValue  = i.is_indirection( 2u ) ? ( instructions_[ i.arg( 2u ) ].cmd() == BLUE_ASM_NOP ) : true;

        const uint16_t n0 = i.is_indirection( 1u ) ? instructions_[ i.arg( 1u ) ].arg( 0u ) : i.arg( 1u );
        const uint16_t n1 = i.is_indirection( 2u ) ? instructions_[ i.arg( 2u ) ].arg( 0u ) : i.arg( 2u );

        // Either both are values, or both are not (and so are locations).
        if( lhsOpIsValue == rhsOpIsValue )
        {
            int16_t result;
            ( *op )( result, n0, n1 );
            store( value_line, result );
        }
        else
        {
            signal_error( ExpectedValueNotFoundAtLine );
        }
    }

    /// When commanded to sense, the machine blocks for an amount of time which
    /// depends on its sensing speed and the requested distance to sense.
    /// Sensing zero returns immediately as this is just the machines own location.
    /// A machine may sense less than requested if there is insufficient cap
    /// to meet the request. In this case the machine will sense as far as is
    /// possible with the available cap. The cap cost comes before the sensing.
    /// The sensing results are the results of the locale sensed at the end of
    /// the period of time, and so are up-to-date when control is returned to the
    /// machine.
    void Machine::command_sense( const uint16_t senseRadius )
    {
        // SHOULD THIS INSTEAD SLEEP UNTIL IT CAN HONOR THE SENSE REQUEST, IF AT ALL POSSIBLE?
        // USER MAY BE RELYING ON SENSE RADIUS?

        // Clamp requested sense radius to max possible radius.
        uint16_t radius = senseRadius > max_sensing_radius() ? max_sensing_radius() : senseRadius;

        // See how far we can sense and extract sensing cost to cap:
        radius = cap_try_extract_sensing( radius );

        assert( blockingCondition_ == NothingBlocking );
        blockingCondition_ = WaitingForSensingCompleted;

        // How far we need sensed.
        blocking_variables_[0] = radius;

        // How many ticks we need to wait until sensing is completed:
        const unsigned int sensedAreaTileCount = Hex_grid::count_of_hexagons_inside_ring( radius );

        const unsigned int tilesPer10Ticks = ( hardware_.tiles_sensed_per_10_ticks() > 1u )
                                             ? hardware_.tiles_sensed_per_10_ticks()
                                             : 1u;

        const unsigned int ticksNeededToSenseTiles = sensedAreaTileCount * 10 / tilesPer10Ticks;

        blocking_variables_[1] = static_cast<uint16_t>( std::min<unsigned int>( USHRT_MAX, ticksNeededToSenseTiles ) );

        if( radius > 0u )
        {
            machine_events_->push_back( Machine_event::machine_sensed( *this, radius, blocking_variables_[1] ) );
        }
    }

    void Machine::command_copy_code( const uint16_t from, const uint16_t to, const uint16_t n )
    {
        std::vector<Instruction> code;
        code.reserve( n );

        for( unsigned int i = 0u; i < n; ++i )
        {
            code.push_back( instructions_[ from + i ] );
        }

        for( unsigned int i = 0u; i < n; ++i )
        {
            instructions_[ to + i ] = code[i];
        }
    }

    void Machine::command_swap_code( const uint16_t from, const uint16_t to, const uint16_t n )
    {
        std::vector<Instruction> fromRange;
        std::vector<Instruction> toRange;
        fromRange.reserve( n );
        toRange.reserve( n );

        for( unsigned int i = 0u; i < n; ++i )
        {
            fromRange.push_back( instructions_[ from + i ] );
            toRange.push_back( instructions_[ to + i ] );
        }

        assert( fromRange.size() == n );
        assert( toRange.size()   == n );

        for( unsigned int i = 0u; i < n; ++i )
        {
            instructions_[ from + i ] = toRange[i];
            instructions_[ to   + i ] = fromRange[i];
        }
    }

    void Machine::command_compare_code( const uint16_t storeAt, const uint16_t lineA, const uint16_t lineB )
    {
        store( storeAt, instructions_[lineA] == instructions_[lineB] ? uint16_t( 1u ) : uint16_t( 0u ) );
    }

    void Machine::command_get_clockwise_turn_count( const uint16_t storeAt, const Hex_coord xy )
    {
        const uint16_t n = static_cast<uint16_t>( Hex_grid::get_clockwise_turn_count( board_xy(), facing_direction(), xy ) );
        store( storeAt, n );
    }

    void Machine::command_can_aim_at( const uint16_t storeAt, const Hex_coord target_xy )
    {
        store( storeAt, static_cast<uint16_t>( hardpoints_.can_aim_at( target_xy ) ) );
    }

    /// 'aim_at' is different to the other commands in that it is non blocking.
    /// This is because it addresses several sub components at the same time,
    /// and some of these may not be able to satisfy the request, and some might
    /// but at different rates etc. So to be a blocking call it is not clear
    /// what to block until - the last Hardpoint has completed aiming? The
    /// first?
    void Machine::command_aim_at( Hex_coord target_xy )
    {
        // Aiming at self will not succeed, it is prevented by hardpoints_.
        hardpoints_.aim_at( target_xy );
    }

    void Machine::command_has_line_of_fire_to( const uint16_t storeAt, const Hex_coord target_xy )
    {
        const Hex_coord machine_xy = board_xy();
        const uint16_t distance_to_xy = Hex_grid::get_step_distance_between( machine_xy, target_xy );

        if( distance_to_xy > 1u )
        {
            Line_hit hit = detect_hit_->find_first_hit_by_line( machine_xy, target_xy, distance_to_xy );

            // We must hit nothing for there to be a clear line of sight.
            store( storeAt, static_cast<uint16_t>( !hit.hit ) );
        }
        else
        {
            // When aiming at self distance is zero and so this returns false (no aim at self).
            store( storeAt, distance_to_xy );
        }
    }

    void Machine::command_can_fire_at( const uint16_t storeAt, const Hex_coord target_xy )
    {
        // Store how many hardpoints can fire on xy.
        store( storeAt, static_cast<uint16_t>( hardpoints_.can_fire_at( capacitance_, target_xy ) ) );
    }

    void Machine::command_fire_at( const Hex_coord target_xy )
    {
        const std::vector<Hardpoint_shot>& shots_fired = hardpoints_.fire_at( *rand_, capacitance_, target_xy );

        if( ! shots_fired.empty() )
        {
            // Send one Machine_event per Hardpoint Fired.
            for( size_t i = 0u; i < shots_fired.size(); ++i )
            {
                machine_events_->push_back( Machine_event::machine_fired( *this, shots_fired[i], target_xy ) );
            }

            // Synch with next frame so that firing appears to happen before next
            // command is executed.
            synch( 1u );
        }
    }

    /// Clears the message buffer. The current message is unchanged.
    void Machine::command_clear_msgs()
    {
        message_buffer_.clear();
    }

    /// Moves message @n to be the current message and removes the indexed
    /// message from the message buffer.
    void Machine::command_move_msg_to_msgbuf( const uint16_t n )
    {
        if( n < message_buffer_.size() )
        {
            current_message_ = message_buffer_[n];
            message_buffer_.erase( message_buffer_.begin() + n );
        }
    }

    /// Copies message from the current message buffer to the destination.
    /// If destination is shorter than incoming, then the incoming is truncated
    /// to fit destination.
    /// If destination is out of range for the machine, no copy takes place.
    /// If there is no current message, this has no effect.
    void Machine::command_copy_msgbuf_code( const uint16_t src, const uint16_t dest, const uint16_t count )
    {
        if( dest >= instructions_.size() )
        {
            return; // Do nothing.
        }

        if( current_message_ )
        {
            std::vector<Instruction> code;
            code.reserve( count );

            const Instructions& src_code = *current_message_;

            const size_t start = src;
            const size_t end   = ( ( src + count ) > current_message_->size() ) ? current_message_->size() : ( src + count );

            if( end > start )
            {
                for( size_t i = start; i < end; ++i )
                {
                    // +1 because instructions use 1 based indexing.
                    code.push_back( src_code[i + 1] );
                }
            }

            code.resize( count, Instruction::nop() );

            place_instructions( code, dest );
        }
        else
        {
            // No current message. May have had no broadcasts
            // or has no message buffer.
        }
    }

    void Machine::place_instructions( const std::vector<Instruction>& code, const size_t line )
    {
        if( line < instructions_.size() )
        {
            size_t endline = line + code.size();

            if( endline > instructions_.size() )
            {
                endline = instructions_.size();
            }

            const size_t copy_count = endline - line;

            for( size_t i = 0u; i < copy_count; ++i )
            {
                instructions_[i + line] = code[i];
            }
        }
    }

    bool Machine::is_valid_instruction_range( const uint16_t codeBegin, const uint16_t codeLength ) const
    {
        return
            ( codeBegin > 0u ) &&
            ( codeLength > 0u ) &&
            ( ( codeBegin - 1u ) < instructions_.size() ) &&
            ( ( codeBegin + codeLength - 1u ) <= instructions_.size() );
    }

    std::shared_ptr<const Instructions> Machine::fetch_instructions( uint16_t codeBegin, uint16_t codeLength ) const
    {
        assert( is_valid_instruction_range( codeBegin, codeLength ) );
        return std::make_shared<const Instructions>( instructions_.subset( codeBegin, codeLength ) );
    }

    uint16_t Machine::push_code( const std::shared_ptr<const Instructions>& code )
    {
        assert( sent_instructions_.find( next_code_store_id_ ) == sent_instructions_.end() );

        // We guarantee that next_code_store_id_ is always available to use.
        const uint16_t id = next_code_store_id_;
        sent_instructions_[id] = code;

        if( sent_instructions_.size() == std::numeric_limits<uint16_t>::max() )
        {
            ss_throw( "Too many stored instructions." );
        }

        // Now find the next free id.
        do
        {
            ++next_code_store_id_;   // This will wrap around to zero after USHRT_MAX.
        }
        while( sent_instructions_.find( next_code_store_id_ ) != sent_instructions_.end() );

        return id;
    }

    std::shared_ptr<const Instructions> Machine::pop_sent( const uint16_t id )
    {
        return pop_code( id );
    }

    std::shared_ptr<const Instructions> Machine::pop_code( const uint16_t id )
    {
        auto it = sent_instructions_.find( id );

        if( it == sent_instructions_.cend() )
        {
            ss_throw( "machine::pop_code received invalid code id." );
        }

        std::shared_ptr<const Instructions> code = it->second;
        sent_instructions_.erase( it );
        return code;
    }

    uint16_t Machine::calculate_broadcast_cost( const uint16_t range ) const
    {
        // Never return 0 as the blocking algorithm requires at least 1.
        return range * cap_broadcast_cost_per_tile() + 1u;
    }

    uint16_t Machine::calculate_transmission_cost_to( const Hex_coord xy ) const
    {
        // Never return 0 as the blocking algorithm requires at least 1.
        return Hex_grid::get_step_distance_between( xy, board_xy() ) * cap_transmit_cost_per_tile() + 1u;
    }

    void Machine::do_broadcast( const uint16_t range, const uint16_t codeBegin, const uint16_t codeLength )
    {
        if( is_valid_instruction_range( codeBegin, codeLength ) )
        {
            auto id = push_broadcast( fetch_instructions( codeBegin, codeLength ) );
            machine_events_->push_back( Machine_event::machine_broadcasted( *this, id, range ) );
        }
    }

    void Machine::do_transmission( const Hex_coord dstXY, const uint16_t codeBegin, const uint16_t codeLength )
    {
        if( is_valid_instruction_range( codeBegin, codeLength ) )
        {
            auto id = push_transmission( fetch_instructions( codeBegin, codeLength ) );
            machine_events_->push_back( Machine_event::machine_transmitted( *this, id, dstXY ) );
        }
    }

    void Machine::command_broadcast( uint16_t range, const uint16_t codeBegin, const uint16_t codeLength )
    {
        // Broadcasting (and transmission) always block - this is because
        // we want the machine to execute its next instruction as if the
        // broadcast had occurred.
        assert( ! is_blocked() );

        // Cap at max range
        range = std::min( range, hardware_.max_broadcast_range() );

        blockingCondition_ = WaitingForBroadcastCapCostToBePaid;
        blocking_variables_[0] = calculate_broadcast_cost( range );
        blocking_variables_[1] = range;
        blocking_variables_[2] = codeBegin;
        blocking_variables_[3] = codeLength;

        assert( is_blocked() );
        assert( blocking_variables_[0] > 0u );
    }

    void Machine::command_transmit( const Hex_coord destinationXY, const uint16_t codeBegin, const uint16_t codeLength )
    {
        // Transmission (and broadcasting) always block - this is because
        // we want the machine to execute its next instruction as if the
        // transmission had occurred.
        assert( ! is_blocked() );

        // No transmission if out of range.
        if( range_to( destinationXY ) <= hardware_.max_transmit_range() )
        {
            blockingCondition_ = WaitingForTransmitCapCostToBePaid;
            blocking_variables_[0] = calculate_transmission_cost_to( destinationXY );
            blocking_variables_[1] = Hex_coord::to_uint16( destinationXY );
            blocking_variables_[2] = codeBegin;
            blocking_variables_[3] = codeLength;
            assert( is_blocked() );
        }
    }

    bool Machine::reserve_build_site( Hex_coord& site )
    {
        if( grid_.try_get_neighbour_tile( board_xy(), facing_direction(), site ) )
        {
            if( grid_.is_tile_empty_and_unreserved( site ) )
            {
                grid_.reserve_empty_tile( site );
                return true;
            }
        }

        return false;
    }

    bool Machine::can_build_blueprint( const uint16_t n ) const
    {
        return ( n < known_blueprints_.size() )
            && cargo_hold_.cargo_contents().can_afford( known_blueprints_[n]->build_cost() );
    }

    uint16_t Machine::calculate_build_time( const Blueprint& bpo ) const
    {
        assert( bpo.build_complexity() >= 0.0f );

        const auto bpoTime = hardware_.ticks_to_build_a_component() * bpo.component_count() * ( 1.0f + bpo.build_complexity() );

        // Clamp to the upper limit of the unsigned 16 value.
        const uint16_t time_needed = std::min<uint16_t>( USHRT_MAX, static_cast<uint16_t>( bpoTime ) );

        return time_needed;
    }

    uint16_t Machine::calculate_build_cap_cost( const Blueprint& bpo ) const
    {
        const size_t bpoCapNeeded = hardware_.capcost_to_build_10_components() * bpo.component_count();
        const uint16_t capNeeded = static_cast<uint16_t>( std::min<size_t>( USHRT_MAX, bpoCapNeeded ) );
        return capNeeded;
    }

    void Machine::command_build( const uint16_t blueprintIndex, const uint16_t codeBegin, const uint16_t codeLength )
    {
        if( can_build_blueprint( blueprintIndex ) )
        {
            Hex_coord site;
            if( reserve_build_site( site ) )
            {
                const Blueprint& bpo = *known_blueprints_[blueprintIndex];

                // Extract build cost.
                destroy_cargo( bpo.build_cost() );

                blockingCondition_     = WaitingForBuildToComplete;
                blocking_variables_[0] = calculate_build_time( bpo );
                blocking_variables_[1] = blocking_variables_[0];
                blocking_variables_[2] = calculate_build_cap_cost( bpo );
                blocking_variables_[3] = blueprintIndex;
                blocking_variables_[4] = Hex_coord::to_uint16( site );
                blocking_variables_[5] = codeBegin;
                blocking_variables_[6] = codeLength;

                start_build( blocking_variables_[5], site );
            }
        }
    }

    void Machine::command_can_build( const uint16_t blueprintIndex, const uint16_t storeAt )
    {
        command_store_value( storeAt, can_build_blueprint( blueprintIndex ) ? 1 : 0 );
    }

    void Machine::start_build( const uint16_t blueprintIndex, const Hex_coord site )
    {
        machine_events_->push_back( Machine_event::machine_started_build( *this, site, blueprintIndex ) );
    }

    void Machine::complete_build( const uint16_t blueprintIndex,
                                  const Hex_coord site,
                                  const uint16_t codeToCopyStart,
                                  const uint16_t codeToCopyCount )
    {
        auto code   = fetch_instructions( codeToCopyStart, codeToCopyCount );
        auto codeId = push_code( code );
        machine_events_->push_back( Machine_event::machine_completed_build( *this, site, blueprintIndex, codeId ) );
    }

    void Machine::destroy_cargo( const Amount& a )
    {
        cargo_hold_.destroy_cargo( a );
    }

    /// Try to transfer cargo from this machine to receiving machine.
    void Machine::try_transfer_cargo_to( Machine& receiver, TradableTypes t, const uint16_t units )
    {
        if( units > 0u && false == cargo_hold_.is_locked() && false == receiver.cargo_hold_.is_locked() )
        {
            Amount a( t, units );

            if( cargo_hold_.contains_at_least( a ) && receiver.cargo_hold_.has_capacity_for( a ) )
            {
                cargo_hold_.transfer_cargo_to( a, receiver.cargo_hold_ );
            }
        }
    }

    void Machine::command_take_cargo_from( const Hex_coord xy, const uint16_t cargoType, const uint16_t units )
    {
        if( units > 0u && cargoType < BLUE_TRADABLE_TYPES_COUNT && is_hex_coord_of_machine_alongside( xy ) )
        {
            machine_events_->push_back( Machine_event::machine_cargo_xfer( *this, xy, board_xy(), cargoType, units ) );
            synch( 1u );
        }
    }

    void Machine::command_give_cargo_to( const Hex_coord xy, const uint16_t cargoType, const uint16_t units )
    {
        if( units > 0u && cargoType < BLUE_TRADABLE_TYPES_COUNT && is_hex_coord_of_machine_alongside( xy ) )
        {
            if( cargo_hold_[cargoType] >= units )
            {
                machine_events_->push_back( Machine_event::machine_cargo_xfer( *this, board_xy(), xy, cargoType, units ) );
                synch( 1u );
            }
        }
    }

    uint16_t Machine::refine_period( const uint16_t ore ) const
    {
        const OreTypes types[] = { PanguiteOre, KamaciteOre, AtaxiteOre, ChondriteOre };
        const float period = ceil( blue::refine_period_factor( types[ore % 4u] ) * hardware_.refine_period() );
        return std::max<uint16_t>( 1u, static_cast<uint16_t>( std::min<float>( period, USHRT_MAX ) ) );
    }

    void Machine::command_refine( const uint16_t unitsToRefine, const uint16_t oreType )
    {
        assert( ! is_blocked() );

        static_assert( BLUE_REFINABLES_BEGIN == 0,
                       "oreType range check here depends on BLUE_REFINABLES_BEGIN being zero" );

        if( oreType < BLUE_REFINABLES_END )
        {
            start_a_refining_cycle( static_cast<OreTypes>( oreType ), unitsToRefine );
        }
    }

    /// A refining cycle begins by debiting the Cargohold and continues until
    /// the cap cost is paid and/or the refine frames have passed, which is when
    /// the cargo is credited with the refined unit and the cycled begins again.
    void Machine::start_a_refining_cycle( const OreTypes ore, const uint16_t unitsToRefine )
    {
        assert( ! is_blocked() );

        if( unitsToRefine > 0u )
        {
            const Amount oneOreUnit( ore, 1 );

            if( cargo_hold_.contains_at_least( oneOreUnit ) )
            {
                destroy_cargo( oneOreUnit );
                blockingCondition_     = WaitingForRefiningCycleToEnd;
                blocking_variables_[0] = hardware_.cap_refine_cost();
                blocking_variables_[1] = refine_period( ore );
                blocking_variables_[2] = unitsToRefine - 1u;
                blocking_variables_[3] = ore;
            }
        }
    }

    void Machine::end_a_refining_cycle()
    {
        static_assert( BLUE_REFINABLES_BEGIN == 0, "Some code assumes BLUE_REFINABLES_BEGIN is zero (in checks)" );
        assert( ! is_blocked() );
        assert( blocking_variables_[3] < BLUE_REFINABLES_END );

        const OreTypes ore = static_cast<OreTypes>( blocking_variables_[3] );
        const Amount refined = refine( ore, 1u );

        if( cargo_hold_.try_add_cargo( refined ) )
        {
            const uint16_t unitsToRefine = blocking_variables_[2];
            start_a_refining_cycle( ore, unitsToRefine );
        }
    }

    void Machine::command_self_destruct( const uint16_t n )
    {
        if( ! self_destruct_triggered_ )
        {
            // Self destruct is one way...
            self_destruct_triggered_ = true;

            if( 0u == n )
            {
                // Immediate self destruct must stop everything.
                blockingCondition_ = WaitingForever;
            }

            machine_events_->push_back( Machine_event::machine_initiated_self_destruct( *this, n ) );
        }
    }

    void Machine::command_assert( const uint16_t op1,
                                  const uint16_t op2,
                                  const uint16_t operation )
    {
        if( ! Instruction::check_assert( op1, op2, operation ) )
        {
            machine_events_->push_back(
                Machine_event::machine_asserted( *this,
                                                 static_cast<uint16_t>( instruction_pointer_ ),
                                                 op1,
                                                 op2,
                                                 operation ) );
        }
    }

    bool Machine::can_move_to( const Hex_coord xy ) const
    {
        return is_mobile() && ( board_xy() != xy ) && grid_.is_on_board( xy );
    }

    /// Here the machine moves into a navigating state if it receives a
    /// non-empty set of instructions back from the pathfinder.
    void Machine::switch_to_navigation_instructions()
    {
        navigation_instruction_pointer_ = 0u;
        navigating_ = ! navigation_instructions_.empty();
        assert( ! path_search_handle_ );
    }

    /// Limited navigation means we will stop after moving 'move_limit' tiles, or
    /// when the destination is reached, or when blocked, whichever happens first.
    /// Unlimited means we stop when blocked or destination is reached.
    /// @param move_limit How many steps to stop after while moving towards @a xy.
    void Machine::begin_navigation( const Hex_coord xy, const uint16_t move_limit )
    {
        assert( ! is_blocked() );

        if( can_move_to( xy ) )
        {
            if( 1u == Hex_grid::get_step_distance_between( board_xy(), xy ) )
            {
                // Special case of moving to a tile alongside.
                Hex_coord steps[] = { board_xy(), xy };
                std::vector<Hex_coord> path( steps, steps + 2 );
                make_path_instructions( path, facing_direction(), navigation_instructions_ );
                switch_to_navigation_instructions();
            }
            else
            {
                start_navigation_search( board_xy(), xy, move_limit );

                // We amortize the cost of navigation over several frames.
                blockingCondition_     = WaitingForAStarRoute;
                blocking_variables_[0] = NAVIGATION_BACKGROUND_FRAMES_COUNT;
            }
        }
    }

    /// Called from end of blocked condition.
    void Machine::complete_search_and_start_navigating()
    {
        assert( ! is_blocked() );
        finish_navigation_search();
        switch_to_navigation_instructions();
    }

    void Machine::command_navigate_to( const Hex_coord xy )
    {
        if( grid_.is_on_board( xy ) )
        {
            begin_navigation( xy, 0u );
        }
    }

    // 'n' = move_limit
    void Machine::command_navigate_n_towards( const uint16_t move_limit, const Hex_coord xy )
    {
        if( move_limit > 0u && grid_.is_on_board( xy ) )
        {
            begin_navigation( xy, move_limit );
        }
    }

    void Machine::command_set_astar_weight( const uint16_t tileType, const uint16_t weight )
    {
        constexpr Hex_path_costs::TileCostVariable costs[3] =
        {
            Hex_path_costs::CostOfUnoccupiedTile,
            Hex_path_costs::CostOfTileWithBot,
            Hex_path_costs::CostOfTileWithAsteroid,
        };

        const uint8_t w = static_cast<uint8_t>( std::min<uint16_t>( weight, Hex_path_costs::MaxPathCost ) );

        path_costs_.set_path_cost( costs[ tileType % 3u ], w );
    }

    uint16_t Machine::astar_weight( const uint16_t tileTypeIndex ) const
    {
        assert( 0 );
        constexpr HexContentFlags costs[3] =
        {
            HexIsUnoccupied,
            HexHasBot,
            HexHasAsteroid
        };

        return static_cast<uint16_t>( path_costs_.get_path_cost( costs[ tileTypeIndex % 3u ] ) );
    }

    /// Starts asynchronous A* hexpath calculation associated with
    /// path_search_handle_.
    /// @param move_limit - No matter what path is found, navigate no more than
    /// this number of tiles towards the destination. Zero means no limit.
    void Machine::start_navigation_search( const Hex_coord start,
                                           const Hex_coord dest,
                                           const uint16_t move_limit )
    {
        assert( pathfinder_ );
        assert( ! path_search_handle_ );

        // Note: if max sensing is zero no path will ever be detected.
        uint16_t search_range = max_sensing_radius();
        path_search_handle_ = pathfinder_->search_for_path_between( start,
                                                                    dest,
                                                                    grid_,
                                                                    path_costs_,
                                                                    facing_direction(),
                                                                    board_xy(),
                                                                    search_range, move_limit );

        assert( path_search_handle_ );
    }

    /// Uses handle to complete the search and copy instructions.
    void Machine::finish_navigation_search()
    {
        assert( path_search_handle_ );
        navigation_instructions_ = pathfinder_->get_path_instructions( path_search_handle_ );
        assert( ! path_search_handle_ );
    }

    void Machine::end_navigation()
    {
        assert( navigating_ );
        assert( ! is_blocked() );
        navigating_ = false;
    }

    void Machine::command_plot_route_between( const Hex_coord A, const Hex_coord B )
    {
        assert( ! is_blocked() );
        assert( ! path_search_handle_ );

        plotted_route_.clear();

        if( grid_.is_on_board( A ) && grid_.is_on_board( B ) )
        {
            if( A != B )
            {
                if( is_in_sensor_range( A ) && is_in_sensor_range( B ) )
                {
                    if( 1 == Hex_grid::get_step_distance_between( A, B ) )
                    {
                        // Special case, side-by-side locations.
                        plotted_route_.push_back( A );
                        plotted_route_.push_back( B );
                    }
                    else
                    {
                        start_navigation_search( A, B, 0u );

                        // We amortize the cost of route plotting over several frames.
                        blockingCondition_     = WaitingForRoutePlot;
                        blocking_variables_[0] = ROUTE_PLOTTING_BACKGROUND_FRAMES_COUNT;
                    }
                }
            }
        }
    }

    void Machine::complete_route_plotting()
    {
        assert( path_search_handle_ );
        plotted_route_ = pathfinder_->get_path( path_search_handle_ );
        assert( ! path_search_handle_ );
    }

    void Machine::command_copy_route( const uint16_t from, const uint16_t to, const uint16_t n )
    {
        if( ! plotted_route_.empty() )
        {
            for( size_t i = 0u; i < n; ++i )
            {
                Hex_coord xy = plotted_route_[( from + i ) % plotted_route_.size() ];
                instructions_[ to + i ] = Instruction::location( xy.x, xy.y );
            }
        }
    }

    void Machine::set_cpu_speed_readout()
    {
        readouts_[ ReadoutCPUBenchmark ] = cpu_speed_metric( hardware_ );
    }

    float Machine::capacitance_proportion_from_nuclear_power() const noexcept
    {
        const float nuke_cap = cap_refill_per_tick_from_engines();

        // Max theoretical contribution of cap from current hardware.
        const float sun_cap = cap_from_panels( hardware_.cap_solar_panels_count(), max_sun_strength() );

        float nuke_proportion = 1.0f;

        if( nuke_cap <= 0.0f )
        {
            // No internal cap generation at all. All cap is externally
            // generated (solar or transferred).
            nuke_proportion = 0.0f;
        }
        else
        {
            if( sun_cap > 0.0f )
            {
                // Machine has solar generation hardware and nuclear hardware.
                nuke_proportion = nuke_cap / ( sun_cap + nuke_cap );
            }
            else
            {
                // Machine has only nuclear engines (1.0f).
            }
        }

        return nuke_proportion;
    }

    void Machine::trade_synch( const uint8_t tradable,
                               const uint8_t tradeType,
                               const uint16_t n,
                               const bool store,
                               const uint16_t storeAt )
    {
        assert( ! is_blocked() );

        Trade_request t;
        t.customer_     = this;
        t.request_type_ = tradeType;
        t.commodity_    = tradable;
        t.quantity_     = n;

        trade_requests_->push_back( t );

        blockingCondition_     = WaitingForTrade;
        blocking_variables_[0] = 1u; // Block until next frame, when trades must be resolved by Board_state.
        blocking_variables_[1] = store;
        blocking_variables_[2] = storeAt;
    }

    uint16_t Machine::sell_goods_to( Machine& buyer, TradableTypes commodity, uint16_t n )
    {
        assert( buyer.blockingCondition_ == WaitingForTrade );
        assert( &buyer != this );
        assert( buyer.distance_between( buyer.board_xy(), board_xy() ) == 1 );

        const uint16_t sellLimit    = items_on_sale_[ commodity ].quantity;
        const uint16_t pricePerUnit = items_on_sale_[ commodity ].price;
        const uint16_t unitsInCargo = cargo_hold_.count_of( commodity );
        const uint16_t wantToSell   = unitsInCargo  > sellLimit ? sellLimit : unitsInCargo - sellLimit;

        // We will continuously modify this to the lowest value
        // that meets all the constraints of the buyer and seller.
        uint16_t agreeToSell  = wantToSell > n ? n : wantToSell;

        if( pricePerUnit > 0 )
        {
            // We cannot sell more units than we can store payment for.
            const uint16_t sellersBalanceSpace = ( BLUE_MAX_CREDITS_PER_MACHINE - credits_ ) / pricePerUnit;
            agreeToSell = std::min( agreeToSell, sellersBalanceSpace );

            // We cannot exchange more than the buyer can afford .
            const uint16_t buyerCanAfford = buyer.credits_ / pricePerUnit;
            agreeToSell = std::min( agreeToSell, buyerCanAfford );
        }

        // We cannot sell more than the buyer can carry.
        const uint16_t volumePerUnit = static_cast<uint16_t>( blue::volume_of_n_units_of( commodity, 1u ) );
        const uint16_t maxUnitsBuyerCanStore = buyer.cargo_hold_.free_volume() / volumePerUnit;
        agreeToSell = std::min( agreeToSell, maxUnitsBuyerCanStore );

        const uint16_t totalPrice = agreeToSell * pricePerUnit;
        const Amount a( commodity, agreeToSell );

        assert( volumePerUnit > 0 );
        assert( cargo_hold_.contains_at_least( a ) );
        assert( buyer.cargo_hold_.has_capacity_for( a ) );
        assert( items_on_sale_[ commodity ].quantity >= agreeToSell );
        assert( ( static_cast<unsigned int>( agreeToSell )* pricePerUnit ) <= BLUE_MAX_CREDITS_PER_MACHINE );

        if( agreeToSell > 0u )
        {
            assert( buyer.credits_ >= totalPrice );
            assert( ( credits_ + totalPrice ) <= max_credits() );

            // Do the trade.
            cargo_hold_.transfer_cargo_to( a, buyer.cargo_hold_ );
            buyer.credits_ -= totalPrice;
            credits_       += totalPrice;
            items_on_sale_[ commodity ].quantity -= agreeToSell;
        }

        return agreeToSell;
    }

    /// An externally generated reply to a trade request made via the shared
    /// vector of trade requests. At this point the trade has already been
    /// completed. The machine's own request is passed back as
    /// well as a result which is either the amount of units of the requested
    /// tradable type that have been sold to this machine, the number of units
    /// that are (were) available during the frame, or the lowest price of a
    /// unit of the requested tradable type. It is safe for a machine to
    /// generate events here.
    void Machine::trade_reply( const uint16_t result )
    {
        assert( blockingCondition_ == WaitingForTrade );

        if( blocking_variables_[1] )
        {
            // User wanted results stored.
            command_store_value( blocking_variables_[2], result );
        }
    }

    Quote Machine::get_trade_quote( const Trade_request& r )
    {
        assert( r.customer_ != this );

        const uint16_t amountInCargo = cargo_hold_.count_of( r.commodity_ );
        const uint16_t sellLimit     = items_on_sale_[ r.commodity_ ].quantity;

        const uint16_t amountOnSale  = std::min( amountInCargo, sellLimit );

        if( amountOnSale > 0u )
        {
            const For_sale available_goods( items_on_sale_[ r.commodity_ ].price, amountOnSale );

            // We are selling this item type and we have some for sale.
            if( r.request_type_ == Buying )
            {
                if( r.price_limit_ >= available_goods.price )
                {
                    // Buyer can afford our goods.
                    return Quote( *this, available_goods );
                }
            }
            else
            {
                return Quote( *this, available_goods );
            }
        }

        // Return empty Quote indicating we are not selling the desired goods.
        return Quote();
    }

    /// Will transfer @a amount if credit balance has sufficient funds. If
    /// the balance does not have enough to do the whole transaction, nothing
    /// will be transferred.
    void Machine::command_transfer_credits_to( const Hex_coord xy, const uint16_t amount )
    {
        if( amount <= credits_ )
        {
            // Do not debit at the point because there may be nothing at xy.
            machine_events_->push_back( Machine_event::machine_transfers_credits( *this, xy, amount ) );
        }

        // Sync because the transfer is handled by the Board_state and we need
        // to not move etc while a transfer is pending.
        synch( 1 );
    }

    /// Called from Board_state event handler. Returns true only if money
    /// changed hands.
    bool Machine::try_transfer_credits_to( Machine& receiver, const uint16_t amount )
    {
        bool transferred = false;

        if( 1 == Hex_grid::get_step_distance_between( board_xy(), receiver.board_xy() ) )
        {
            if( credits_ >= amount && receiver.can_add_credits( amount ) )
            {
                credits_          -= amount;
                receiver.credits_ += amount;
                transferred = true;
            }
        }

        return transferred;
    }

    /// Used to assign credits to machine by external system. Assignment
    /// completely succeeds, or fails.
    void Machine::give_credits( const uint16_t amount )
    {
        if( can_add_credits( amount ) )
        {
            credits_ += amount;
        }
    }

    void Machine::command_cancel_sales()
    {
        for( size_t i = 0; i < BLUE_TRADABLE_TYPES_COUNT; ++i )
        {
            items_on_sale_[i].quantity = 0u;
        }
    }

    /// A machine can offer to sell items it does not have in the cargo.
    /// Placing on sale informs the machine's trade system that it is willing
    /// to sell n units of a cargo type, if the opportunity arises and they
    /// are in the cargo hold. When n items have been sold the sale will be
    /// disabled until the next sale request, or if the sale order is updated.
    void Machine::command_for_sale( const uint8_t tradable, const uint16_t n, const uint16_t price )
    {
        items_on_sale_[tradable % BLUE_TRADABLE_TYPES_COUNT].quantity = n;
        items_on_sale_[tradable % BLUE_TRADABLE_TYPES_COUNT].price    = price;
    }

    void Machine::command_try_buy( const uint8_t tradable, const uint16_t n )
    {
        trade_synch( tradable, static_cast<uint8_t>( Buying ), n, false, 0u );
    }

    void Machine::command_try_buy_with_store( const uint16_t storeAt, const uint8_t tradable, const uint16_t n )
    {
        trade_synch( tradable, static_cast<uint8_t>( Buying ), n, true, storeAt );
    }

    void Machine::command_get_price_of( const uint16_t storeAt, const uint8_t tradable )
    {
        trade_synch( tradable, static_cast<uint8_t>( QueryPrice ), 0u, true, storeAt );
    }

    void Machine::command_get_availability_of( const uint16_t storeAt, const uint8_t tradable )
    {
        trade_synch( tradable, static_cast<uint8_t>( QueryAvailability ), 0u, true, storeAt );
    }
}
