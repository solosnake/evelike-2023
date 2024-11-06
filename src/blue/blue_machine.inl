#include "solosnake/blue/blue_machine.hpp"

namespace blue
{
    //-------------------------------------------------------------------------
    // Public member functions.
    //-------------------------------------------------------------------------

    inline const Cargohold& Machine::cargo() const noexcept
    {
        return cargo_hold_;
    }

    inline std::string_view Machine::name() const noexcept
    {
        return machines_name_;
    }

    inline std::string_view Machine::class_name() const noexcept
    {
        return std::string_view(classification_.c_str());
    }

    inline Classification Machine::machine_class() const noexcept
    {
        return classification_;
    }

    inline std::uint8_t Machine::team_hue_shift() const noexcept
    {
        return team_hue_shift_;
    }

    inline const Instructions& Machine::code() const noexcept
    {
        return instructions_;
    }

    inline void Machine::kill()
    {
        combat_system_.kill();
    }

    inline void Machine::apply_damage( const Damages& d )
    {
        combat_system_.apply_damage( d );
    }

    inline float Machine::get_indexed_hardpoint_falloff_at_range( size_t i, std::uint16_t range ) const
    {
        return hardpoints_.get_indexed_hardpoint_falloff_at_range( i, range );
    }

    inline void Machine::apply_armour_repairs( const int r )
    {
        combat_system_.repair_armour( r );
    }

    inline void Machine::apply_cap_alteration( const int d )
    {
        assert( d > -65536 && d < 65536 );
        external_cap_delta_ += d;
    }

    inline bool Machine::is_alive() const noexcept
    {
        return combat_system_.is_alive();
    }

    inline bool Machine::is_dead() const noexcept
    {
        return false == combat_system_.is_alive();
    }

    inline bool Machine::is_accelerating() const noexcept
    {
        return propulsion_.is_accelerating();
    }

    inline bool Machine::is_turning_cw() const noexcept
    {
        return propulsion_.is_turning_cw();
    }

    inline bool Machine::is_turning_ccw() const noexcept
    {
        return propulsion_.is_turning_ccw();
    }

    inline bool Machine::is_mobile() const noexcept
    {
        return propulsion_.max_speed() > 0 || propulsion_.max_turn_speed() > 0;
    }

    inline int Machine::max_speed() const noexcept
    {
        return propulsion_.max_speed();
    }

    inline int Machine::max_turn_speed() const noexcept
    {
        return propulsion_.max_turn_speed();
    }

    inline std::uint16_t Machine::max_sensing_radius() const noexcept
    {
        return hardware_.max_sensing_radius();
    }

    inline Radians Machine::angle_as_radians() const noexcept
    {
        return propulsion_.angle_as_radians();
    }

    inline Position_info Machine::get_position_info() const noexcept
    {
        return propulsion_.get_position_info();
    }

    inline Fixed_angle Machine::angle() const noexcept
    {
        return propulsion_.angle();
    }

    inline Hex_coord Machine::board_xy() const noexcept
    {
        return propulsion_.gridXY();
    }

    /// Bounding radius in world units.
    inline float Machine::bounding_radius() const noexcept
    {
        return bounding_radius_;
    }

    inline std::uint16_t Machine::capacitance_remaining() const noexcept
    {
        return capacitance_;
    }

    inline std::uint16_t Machine::components_count() const noexcept
    {
        return classification_.total_components_count();
    }

    inline std::shared_ptr<Blueprint> Machine::machine_blueprint() const noexcept
    {
        return known_blueprints_[0];
    }

    inline const std::vector<Turning_hardpoint>& Machine::turning_hardpoints() const noexcept
    {
        return hardpoints_.turning_hardpoints();
    }

    inline void Machine::offset_after_n_frames( const float frames, float* const unaliased xy ) const
    {
        return propulsion_.offset_after_n_frames( frames, xy );
    }

    //-------------------------------------------------------------------------

    inline bool Machine::resolve_combat( Randoms& r )
    {
        return combat_system_.resolve( r );
    }

    inline float Machine::readout( const Machine_readout r ) const noexcept
    {
        return readouts_[r];
    }

    inline HexFacingName Machine::facing_direction() const
    {
        return propulsion_.facing();
    }

    inline void Machine::inc_watched_refcount() const
    {
        ++watched_refcount_;
    }

    inline void Machine::dec_watched_refcount() const
    {
        assert( watched_refcount_ > 0 );
        --watched_refcount_;
    }

    inline bool Machine::is_not_being_watched() const noexcept
    {
        return watched_refcount_ == 0;
    }

    inline bool Machine::is_being_watched() const noexcept
    {
        return watched_refcount_ != 0;
    }

    /// A machine becomes blocked while it is waiting for an action to
    /// complete...
    inline bool Machine::is_not_blocked() const noexcept
    {
        return blockingCondition_ == NothingBlocking;
    }

    /// A machine becomes blocked while it is waiting for an action to
    /// complete...
    inline bool Machine::is_blocked() const noexcept
    {
        return blockingCondition_ != NothingBlocking;
    }

    /// A ship can either turn or advance, it cannot do both at the same time.
    inline void Machine::tick_motion_system( Movement_grid_view& g )
    {
        propulsion_.tick( g );
    }

    inline void Machine::tick_hardpoint_systems()
    {
        hardpoints_.tick();
    }

    // These are the only things the code can DO to the machine:
    inline void Machine::command_set_instruction_pointer( const size_t reg0 )
    {
        instruction_pointer_ = reg0;
        instruction_pointer_updated_ = true;
    }

    inline void Machine::set_error_code( std::uint16_t e )
    {
        error_code_ = e;
    }

    inline void Machine::command_store_value( const size_t line, const std::uint16_t value )
    {
        store( line, value );
    }

    inline void Machine::command_store_location( const size_t line, const Hex_coord xy )
    {
        store( line, xy );
    }

    inline void Machine::command_store_is_turning( const size_t reg0 )
    {
        store( reg0, is_turning() ? static_cast<std::uint16_t>( 1u ) : static_cast<std::uint16_t>( 0u ) );
    }

    inline void Machine::command_copy( const size_t from, const size_t to )
    {
        instructions_[to] = instructions_[from];
    }

    inline void Machine::store( const size_t line, const std::uint16_t what )
    {
        instructions_[line] = Instruction::value( what );
    }

    inline void Machine::store( const size_t line, const std::int16_t what )
    {
        assert( static_cast<std::int16_t>( static_cast<std::uint16_t>( what ) ) == what );
        instructions_[line] = Instruction::value( static_cast<std::uint16_t>( what ) );
    }

    inline void Machine::store( const size_t line, const Hex_coord xy )
    {
        instructions_[line] = Instruction::location( xy.x, xy.y );
    }

    inline bool Machine::is_moving() const noexcept
    {
        return propulsion_.is_moving();
    }

    inline bool Machine::is_turning() const noexcept
    {
        return propulsion_.is_turning();
    }

    inline bool Machine::is_advancing() const noexcept
    {
        return propulsion_.is_advancing();
    }

    //--------------------------------------------------------------------------------------
    // DISTANCE AND RANGE
    //--------------------------------------------------------------------------------------

    inline bool Machine::is_in_sensor_range( const Hex_coord xy ) const
    {
        return grid_.get_step_distance_between( board_xy(), xy ) <= max_sensing_radius();
    }

    inline  bool Machine::is_hex_coord_of_machine_alongside( const Hex_coord xy ) const
    {
        return ( 1 == Hex_grid::get_step_distance_between( xy, board_xy() ) ) &&
               is_bot_on_tile( grid_.contents( xy ) );
    }

    inline bool Machine::is_in_range_to_fire_on( const Hex_coord xy )
    {
        const auto dist = range_to( xy );
        return dist <= hardpoints_.max_range() && dist >= hardpoints_.min_range();
    }

    inline size_t Machine::range_to( const Hex_coord xy ) const
    {
        return grid_.get_step_distance_between( board_xy(), xy );
    }

    inline size_t Machine::distance_between( const Hex_coord A, const Hex_coord B ) const
    {
        return grid_.get_step_distance_between( A, B );
    }

    inline void Machine::command_get_distance_to( const std::uint16_t storeAt, const Hex_coord xy )
    {
        assert( range_to( xy ) < std::numeric_limits<std::uint16_t>::max() );
        store( storeAt, static_cast<std::uint16_t>( range_to( xy ) ) );
    }

    inline void Machine::command_get_distance_between( const std::uint16_t storeAt,
                                                          const Hex_coord A,
                                                          const Hex_coord B )
    {
        assert( distance_between( A, B ) < std::numeric_limits<std::uint16_t>::max() );
        store( storeAt, static_cast<std::uint16_t>( distance_between( A, B ) ) );
    }

    //--------------------------------------------------------------------------------------

    inline bool Machine::try_add_cargo( Amount& a )
    {
        return cargo_hold_.try_add_cargo( a );
    }

    inline void Machine::command_turn( const std::uint16_t direction, const std::uint16_t n_faces )
    {
        if( is_mobile() && ( false == is_moving() ) )
        {
            if( n_faces > 0 )
            {
                start_n_face_turns( direction, n_faces );
            }
            else
            {
                // Basically a NOP. Turn through zero faces.
            }
        }
    }

    inline void Machine::command_advance( const std::uint16_t tiles,
                                          const std::uint16_t storageLine,
                                          const bool storeResult )
    {
        if( is_mobile() && ( false == is_moving() ) )
        {
            if( tiles > 0 )
            {
                start_advancing_n_tiles( storeResult, storageLine, tiles );
            }
            else
            {
                // NOP - move zero tiles.
            }
        }
    }

    //--------------------------------------------------------------------------------------
    // NAVIGATION
    //--------------------------------------------------------------------------------------

    inline std::uint16_t Machine::route_length() const
    {
        return static_cast<std::uint16_t>( plotted_route_.size() );
    }

    //--------------------------------------------------------------------------------------
    // CAPACITOR
    //--------------------------------------------------------------------------------------

    /// Returns the amount of cap @a nPanels generates on a tile with Sun
    /// strength @a sunStrength.
    inline uint16_t cap_from_panels( uint16_t nPanels, uint16_t sunStrength ) noexcept
    {
        unsigned int cap = nPanels * sunStrength;

        // Each solar panel captures a tile's worth of Sun.
        float c = std::min<float>( BLUE_SUN_STRENGTH_TO_CAP_FACTOR * cap, 0xFFFF );

        return static_cast<uint16_t>( c );
    }

    inline uint16_t Machine::cap_from_solar_panels_at_current_tile() const noexcept
    {
        return cap_from_panels(
                   hardware_.cap_solar_panels_count(),
                   grid_.tile_sun_strength( board_xy() ) );
    }

    inline std::uint16_t Machine::cap_instruction_cost() const noexcept
    {
        return hardware_.cap_instruction_cost();
    }

    inline std::uint16_t Machine::cap_firing_cost() const noexcept
    {
        return hardware_.cap_firing_cost();
    }

    inline std::uint16_t Machine::cap_sense_cost_per_tile() const noexcept
    {
        return hardware_.cap_sensing_cost_per_tile();
    }

    inline std::uint16_t Machine::cap_broadcast_cost_per_tile() const noexcept
    {
        return hardware_.cap_broadcast_cost_per_tile();
    }

    inline std::uint16_t Machine::cap_transmit_cost_per_tile() const noexcept
    {
        return hardware_.cap_transmit_cost_per_tile();
    }

    inline std::uint16_t Machine::cap_refill_per_tick_from_engines() const noexcept
    {
        return hardware_.cap_refill_per_tick();
    }

    inline bool Machine::cap_try_pay_single_instruction_cost() noexcept
    {
        std::uint16_t cost = cap_instruction_cost();

        if( capacitance_ >= cost )
        {
            capacitance_ -= cost;
            return true;
        }

        return false;
    }

    inline bool Machine::cap_try_pay_firing_cost() noexcept
    {
        std::uint16_t cost = cap_firing_cost();

        if( capacitance_ >= cost )
        {
            capacitance_ -= cost;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    // RECEIVED MESSAGES
    //--------------------------------------------------------------------------------------

    inline std::uint16_t Machine::max_stored_messages() const noexcept
    {
        return hardware_.max_stored_messages();
    }

    //--------------------------------------------------------------------------------------
    // SENT MESSAGES
    //--------------------------------------------------------------------------------------

    inline std::shared_ptr<const Instructions> Machine::pop_broadcast( const std::uint16_t id )
    {
        return pop_sent( id );
    }

    inline std::shared_ptr<const Instructions> Machine::pop_transmission( const std::uint16_t id )
    {
        return pop_sent( id );
    }

    inline std::uint16_t Machine::push_broadcast( const std::shared_ptr<const Instructions>& code )
    {
        return push_code( code );
    }

    inline std::uint16_t Machine::push_transmission( const std::shared_ptr<const Instructions>& code )
    {
        return push_code( code );
    }

    inline bool Machine::can_add_credits( const std::uint16_t amount ) const noexcept
    {
        return credits_ <= (BLUE_MAX_CREDITS_PER_MACHINE - amount);
    }

    inline std::uint16_t Machine::max_credits() const noexcept
    {
        return BLUE_MAX_CREDITS_PER_MACHINE;
    }

    inline size_t Machine::calculate_facing( size_t current_facing,
                                             const size_t direction,
                                             const size_t n_faces ) noexcept
    {
        assert( direction == BLUE_C_CLOCKWISE || direction == BLUE_CLOCKWISE );
        current_facing += ( ( direction == BLUE_CLOCKWISE ) ? n_faces : ( 5 - n_faces ) );
        return ( current_facing + 6 ) % 6;
    }
}