#include <memory>
#include "solosnake/blue/blue_board_state.hpp"
#include "solosnake/blue/blue_circle.hpp"
#include "solosnake/blue/blue_explosion_damages.hpp"
#include "solosnake/blue/blue_hex_pathfinder.hpp"
#include "solosnake/blue/blue_intersection2d.hpp"
#include "solosnake/blue/blue_instructions.hpp"
#include "solosnake/blue/blue_line2d.hpp"
#include "solosnake/blue/blue_line_circle_intersection.hpp"
#include "solosnake/blue/blue_line_hit.hpp"
#include "solosnake/blue/blue_logging.hpp"
#include "solosnake/blue/blue_machine.hpp"
#include "solosnake/blue/blue_quote.hpp"
#include "solosnake/blue/blue_throw.hpp"
#include "solosnake/blue/blue_unreachable.hpp"

namespace blue
{
    namespace
    {
        Line2d line2d_between( const Hex_coord A, const Hex_coord B )
        {
            float a[2];
            float b[2];
            Hex_grid::calculate_xy( A, a );
            Hex_grid::calculate_xy( B, b );
            return Line2d( a[0], a[1], b[0], b[1] );
        }
    }

    Board_state::Board_state( unsigned int random_seed,
                              Game_hex_grid&& grid,
                              std::vector<Asteroid>&& asteroids,
                              std::vector<Sun>&& suns )
        : Board_state( Good_rand(random_seed),
                       std::move(grid),
                       std::move(asteroids),
                       std::move(suns) )
    {
        // NOP - forwarding ctor.
    }

    Board_state::Board_state( Good_rand&& randoms,
                              Game_hex_grid&& grid,
                              std::vector<Asteroid>&& asteroids,
                              std::vector<Sun>&& suns )
        : frame_number_( 0u )
        , trade_requests_()
        , machine_events_()
        , secondary_events_()
        , delayed_events_()
        , collisions_()
        , asteroids_( std::move( asteroids ) )
        , suns_( move( suns ) )
        , tile_buffer_()
        , grid_( std::move( grid ) )
        , teams_()
        , randoms_( std::move( randoms ) )
        , pathfinder_( std::make_unique<Hex_pathfinder>() )
    {
        trade_requests_.reserve( 16 );
        machine_events_.reserve( 128 );
        secondary_events_.reserve( 128 );
        tile_buffer_.reserve( 16 );

        sort( asteroids_.begin(),
              asteroids_.end(),
              [ = ]( const Asteroid & lhs, const Asteroid & rhs )
              { return lhs.board_xy() < rhs.board_xy(); } );

        for( size_t i = 0; i < asteroids_.size(); ++i )
        {
            if( asteroids_[i].is_on_board() )
            {
                // We can place all asteroids on grid, even zero volumed
                // asteroids, as they will immediately be removed at the
                // start of the frame.
                grid_.place_asteroid_on_tile( asteroids_[i].board_xy() );
            }
        }

        sort( suns_.begin(),
              suns_.end(),
              [ = ]( const Sun & lhs, const Sun & rhs )
              { return lhs.board_xy() < rhs.board_xy(); } );

        for( size_t i = 0; i < suns_.size(); ++i )
        {
            grid_.place_sun_on_tile( suns_[i].sun_strength(), suns_[i].board_xy() );
        }
    }

    Board_state::~Board_state()
    {
    }

    bool Board_state::operator == (const Board_state& rhs) const noexcept
    {
        auto same_priority_queues = [](const std::priority_queue<Delayed_machine_event>& ,
                                       const std::priority_queue<Delayed_machine_event>& ) noexcept -> bool
        {
            return false;
        };

       return (frame_number_     == rhs.frame_number_)
           && (trade_requests_   == rhs.trade_requests_)
           && (machine_events_   == rhs.machine_events_)
           && (secondary_events_ == rhs.secondary_events_)
           && (collisions_       == rhs.collisions_)
           && (asteroids_        == rhs.asteroids_)
           && (suns_             == rhs.suns_)
           && (tile_buffer_      == rhs.tile_buffer_)
           && (grid_             == rhs.grid_)
           && (teams_            == rhs.teams_)
           && (randoms_          == rhs.randoms_)
           && (*pathfinder_      == *rhs.pathfinder_)
           && same_priority_queues(delayed_events_, rhs.delayed_events_)
           ;
    }

    Asteroid* Board_state::asteroid_at( Hex_coord xy )
    {
        auto i = lower_bound( asteroids_.begin(),
                              asteroids_.end(),
                              xy,
                              [ = ]( const Asteroid & a, Hex_coord axy )
                              { return a.board_xy() < axy; } );

        return ( ( i == asteroids_.cend() ) || ( i->board_xy() != xy ) ) ? nullptr : &*i;
    }

    /// Returns a pointer to an asteroid at xy else nullptr.
    const Asteroid* Board_state::asteroid_at_coord( Hex_coord xy ) const
    {
        auto i = lower_bound( asteroids_.cbegin(),
                              asteroids_.cend(),
                              xy,
                              [ = ]( const Asteroid & a, Hex_coord axy )
                              { return a.board_xy() < axy; } );

        return ( ( i == asteroids_.cend() ) || ( i->board_xy() != xy ) ) ? nullptr : &*i;
    }

    Sun* Board_state::sun_at( Hex_coord xy )
    {
        auto i = lower_bound( suns_.begin(),
                              suns_.end(),
                              xy,
                              [ = ]( const Sun & s, Hex_coord axy )
                              { return s.board_xy() < axy; } );

        return ( ( i == suns_.end() ) || ( i->board_xy() != xy ) ) ? nullptr : &*i;
    }

    Machine* Board_state::machine_at( Hex_coord xy ) const
    {
        Machine* m = grid_.machine_at( xy );
        return m;
    }

    float Board_state::machine_radius_at( Hex_coord xy ) const
    {
        const Machine* m = machine_at_coord( xy );
        assert( m );
        return m->bounding_radius();
    }

    float Board_state::asteroid_radius_at( Hex_coord xy ) const
    {
        const Asteroid* a = asteroid_at_coord( xy );
        assert( a );
        return a->bounding_radius();
    }

    const Machine* Board_state::machine_at_coord( Hex_coord xy ) const
    {
        return machine_at( xy );
    }

    //-------------------------------------------------------------------------
    //                       HARDPOINT FIRING EVENTS
    // Each method is very similar but with slight differences. These are the
    // core of the application in many ways.
    //-------------------------------------------------------------------------

    void Board_state::weapon_fired( const Machine_event& e, const Line_hit hit )
    {
        if( hit.hit )
        {
            Shot_fired hits_shot = Shot_fired::make_shot_that_hit( e.event().fired_, hit.xy );

            if( is_bot_on_tile( hit.content ) )
            {
                const uint16_t r = Hex_grid::get_step_distance_between( e.event().fired_.firerXY_, hit.xy );
                const float falloff  = e.machine()->get_indexed_hardpoint_falloff_at_range( e.event().fired_.shot_.hardpoint_index_, r );
                const Damages dmg    = e.event().fired_.shot_.shot_effect_.weapon.damage * falloff;
                Machine* hit_machine = machine_at( hit.xy );
                assert( hit_machine );

                hit_machine->inc_watched_refcount();

                delayed_events_.push( Delayed_machine_event( frame_number_, Machine_event::machine_received_damage( *hit_machine, dmg ) ) );
                secondary_events_.push_back( Secondary_event::make_shotfiredandhitmachine( hits_shot ) );
            }
            else if( is_asteroid_on_tile( hit.content ) )
            {
                secondary_events_.push_back( Secondary_event::make_shotfiredandhitasteroid( hits_shot ) );
            }
            else
            {
                assert( ! "Unknown content hit" );
            }
        }
        else
        {
            secondary_events_.push_back( Secondary_event::make_shotfiredandmissed( Shot_fired::make_shot_that_missed( e.event().fired_ ) ) );
        }
    }

    void Board_state::cap_drain_fired( const Machine_event& e, const Line_hit hit )
    {
        if( hit.hit )
        {
            Shot_fired hits_shot = Shot_fired::make_shot_that_hit( e.event().fired_, hit.xy );

            if( is_bot_on_tile( hit.content ) )
            {
                const uint16_t r = Hex_grid::get_step_distance_between( e.event().fired_.firerXY_, hit.xy );
                const float falloff = e.machine()->get_indexed_hardpoint_falloff_at_range( e.event().fired_.shot_.hardpoint_index_, r );

                // Calculate potential drain.
                const auto drained_shot = e.event().fired_.shot_.shot_effect_.cap_drain.cap_drained * falloff;
                // Note that drained is clamped and made negative here.
                const int drained =  -static_cast<int>( std::min<float>( std::numeric_limits<uint16_t>::max(), drained_shot) );
                Machine* hit_machine = machine_at( hit.xy );

                assert( hit_machine );
                assert( drained <= 0 );

                if( drained < 0 )
                {
                    hit_machine->inc_watched_refcount();
                    delayed_events_.push( Delayed_machine_event(
                                             frame_number_, Machine_event::machine_cap_was_altered( *hit_machine, drained ) ) );
                }

                secondary_events_.push_back( Secondary_event::make_capdrainfiredandhitmachine( hits_shot ) );
            }
            else if( is_asteroid_on_tile( hit.content ) )
            {
                secondary_events_.push_back( Secondary_event::make_capdrainfiredandhitasteroid( hits_shot ) );
            }
            else
            {
                assert( ! "Unknown content hit" );
            }
        }
        else
        {
            secondary_events_.push_back( Secondary_event::make_capdrainfiredandmissed(
                                            Shot_fired::make_shot_that_missed( e.event().fired_ ) ) );
        }
    }

    void Board_state::cap_xfer_fired( const Machine_event& e, const Line_hit hit )
    {
        if( hit.hit )
        {
            Shot_fired hits_shot = Shot_fired::make_shot_that_hit( e.event().fired_, hit.xy );

            if( is_bot_on_tile( hit.content ) )
            {
                const uint16_t r = Hex_grid::get_step_distance_between( e.event().fired_.firerXY_, hit.xy );
                const float falloff = e.machine()->get_indexed_hardpoint_falloff_at_range( e.event().fired_.shot_.hardpoint_index_, r );

                // Calculate and clamp the transfer.
                const auto shot_transfer = e.event().fired_.shot_.shot_effect_.cap_xfer.cap_transferred * falloff;
                const int transfer = static_cast<int>( std::min<float>( std::numeric_limits<uint16_t>::max(), shot_transfer ) );
                assert( transfer >= 0 );

                Machine* hit_machine = machine_at( hit.xy );
                assert( hit_machine );

                if( transfer > 0 )
                {
                    hit_machine->inc_watched_refcount();
                    delayed_events_.push( Delayed_machine_event(
                                             frame_number_,
                                             Machine_event::machine_cap_was_altered( *hit_machine, transfer ) ) );
                }

                secondary_events_.push_back( Secondary_event::make_capxferfiredandhitmachine( hits_shot ) );
            }
            else if( is_asteroid_on_tile( hit.content ) )
            {
                secondary_events_.push_back( Secondary_event::make_capxferfiredandhitasteroid( hits_shot ) );
            }
            else
            {
                assert( ! "Unknown content hit" );
            }
        }
        else
        {
            secondary_events_.push_back( Secondary_event::make_capxferfiredandmissed(
                                            Shot_fired::make_shot_that_missed( e.event().fired_ ) ) );
        }
    }

    void Board_state::armour_repairer_fired( const Machine_event& e, const Line_hit hit )
    {
        if( hit.hit )
        {
            Shot_fired hits_shot = Shot_fired::make_shot_that_hit( e.event().fired_, hit.xy );

            if( is_bot_on_tile( hit.content ) )
            {
                const uint16_t r = Hex_grid::get_step_distance_between( e.event().fired_.firerXY_, hit.xy );
                // Calculate repaired.
                const float falloff = e.machine()->get_indexed_hardpoint_falloff_at_range( e.event().fired_.shot_.hardpoint_index_, r );
                const auto repairing_shot = e.event().fired_.shot_.shot_effect_.repairs.armour_repaired * falloff;
                // Clamp to max permitted.
                const int repaired = static_cast<int>( std::min<float>( std::numeric_limits<uint16_t>::max(), repairing_shot ) );
                assert( repaired >= 0 );

                Machine* hit_machine = machine_at( hit.xy );
                assert( hit_machine );

                if( repaired > 0 )
                {
                    hit_machine->inc_watched_refcount();
                    delayed_events_.push( Delayed_machine_event(
                                             frame_number_,
                                             Machine_event::machine_received_armour_rep( *hit_machine, repaired ) ) );
                }

                secondary_events_.push_back( Secondary_event::make_armourrepfiredandhitmachine( hits_shot ) );
            }
            else if( is_asteroid_on_tile( hit.content ) )
            {
                secondary_events_.push_back(
                    Secondary_event::make_armourrepfiredandhitasteroid( hits_shot ) );
            }
            else
            {
                assert( ! "Unknown content hit" );
            }
        }
        else
        {
            secondary_events_.push_back( Secondary_event::make_armourrepfiredandmissed(
                                            Shot_fired::make_shot_that_missed( e.event().fired_ ) ) );
        }
    }

    void Board_state::miner_fired( const Machine_event& e, const Line_hit hit )
    {
        if( hit.hit )
        {
            Shot_fired hits_shot = Shot_fired::make_shot_that_hit( e.event().fired_, hit.xy );

            if( is_asteroid_on_tile( hit.content ) )
            {
                // The asteroid might have been mined out already during
                // this frame.
                Asteroid* a = asteroid_at( hit.xy );
                assert( a );

                if( a->volume() > 0 )
                {
                    const uint16_t r = Hex_grid::get_step_distance_between( e.event().fired_.firerXY_, hit.xy );
                    const float falloff = e.machine()->get_indexed_hardpoint_falloff_at_range( e.event().fired_.shot_.hardpoint_index_, r );

                    const int space_free   = static_cast<int>( e.machine()->cargo().free_volume() );
                    const float shot_mined = e.event().fired_.shot_.shot_effect_.mining.volume_mined * falloff;
                    const int max_mined    = static_cast<int>( std::min<float>( std::numeric_limits<uint16_t>::max(), shot_mined ) );
                    const int mined_volume = max_mined > space_free ? space_free : max_mined;
                    assert( mined_volume >= 0 );

                    if( mined_volume > 0 )
                    {
                        Amount ore = a->try_extract_ore_volume( static_cast<uint16_t>( mined_volume ) );
                        assert( ore.volume() <= mined_volume );

                        if( false == e.machine()->try_add_cargo( ore ) )
                        {
                            assert( ! "Ore was not added to machine cargo." );
                        }

                        if( a->volume() == 0 )
                        {
                            // Volume became zero. The asteroid will be removed
                            // at the start of the next frame.
                            secondary_events_.push_back( Secondary_event::make_asteroid_depleted() );
                        }
                    }

                    secondary_events_.push_back( Secondary_event::make_miningfiredandhitasteroid( hits_shot ) );
                }
                else
                {
                    // Empty asteroid.
                }
            }
            else if( is_bot_on_tile( hit.content ) )
            {
                secondary_events_.push_back( Secondary_event::make_miningfiredandhitmachine( hits_shot ) );
            }
            else
            {
                assert( ! "Unknown content hit" );
            }
        }
        else
        {
            secondary_events_.push_back(
                Secondary_event::make_miningfiredandmissed(
                    Shot_fired::make_shot_that_missed( e.event().fired_ ) ) );
        }
    }

    Line_hit Board_state::find_first_hit_by_line( Hex_coord from, Hex_coord target, uint16_t max_range ) const
    {
        // We may want to allow the Board_state the ability to cache the results
        // of these calls - there will often be an Machine_event per Hardpoint.
        grid_.contents_under_line( from, target, max_range, collisions_ );

        if( false == collisions_.empty() )
        {
            const Line2d line_to_target = line2d_between( from, target );

            for( size_t i = 0; i < collisions_.size(); ++i )
            {
                Hex_coord coord = collisions_[i];
                HexContentFlags content = grid_.contents( coord );

                assert( is_bot_on_tile( content ) || is_asteroid_on_tile( content ) );

                float r = 0.0f;

                if( is_bot_on_tile( content ) )
                {
                    r = machine_radius_at( coord );
                }
                else if( is_asteroid_on_tile( content ) )
                {
                    r = asteroid_radius_at( coord );
                }
                else
                {
                    // Error
                }

                Point2d centre;
                Hex_grid::calculate_xy( coord, centre.xy );

                const Intersection2d cuts = line_circle_intersection( line_to_target, Circle( centre, r ) );

                if( cuts )
                {
                    return Line_hit( content, coord );
                }
            }
        }

        return Line_hit();
    }

    //-------------------------------------------------------------------------
    //                           EVENT HANDLERS
    // Events that can postpone by inserting back into delayed must themselves
    // handle being delayed and the possibility that the machine is no longer
    // alive and on the board, and also must decrement the Machine_event ref.
    // count of the machine.
    //
    //-------------------------------------------------------------------------

    void Board_state::handle_fired( const Machine_event& e )
    {
        ss_dbg( "Machine ", e.machine()->name(), " fired." );

        // See if shot reached intended target, or if it hit something in
        // between.
        const Line_hit hit = find_first_hit_by_line( e.event().fired_.firerXY_,
                                             e.event().fired_.targetXY_,
                                             e.event().fired_.shot_.max_range_ );

#ifndef NDEBUG
        if( hit.hit )
        {
            ss_log( "Machine hit." );
        }
        else
        {
            ss_log( "Machine missed." );
        }
#endif

        switch( e.event().fired_.shot_.hardpoint_type_ )
        {
            case WeaponHardpoint:
                weapon_fired( e, hit );
                break;

            case CapDrainHardpoint:
                cap_drain_fired( e, hit );
                break;

            case CapXFerHardpoint:
                cap_xfer_fired( e, hit );
                break;

            case ArmourRepairerHardpoint:
                armour_repairer_fired( e, hit );
                break;

            case MinerHardpoint:
                miner_fired( e, hit );
                break;

            default:
                ss_unreachable;
        }
    }

    void Board_state::handle_broadcast( const Machine_event& b )
    {
        assert( b.machine()->is_alive() );

        // Return only the bots within the range. tile_buffer_ is cleared in this
        // call.
        grid_.get_sorted_circle_contents( b.event().broadcasted_.broadcastFromXY_,
                                          b.event().broadcasted_.broadcastRadius_,
                                          tile_buffer_,
                                          bot_tile_value() );

        // Always pop the code, even if we have no one to send it to.
        auto code = b.machine()->pop_broadcast( b.event().broadcasted_.broadcastId_ );

        // Send the code to every tile which contains a bot (this may include
        // the original sender).
        for( size_t i = 0; i < tile_buffer_.size(); ++i )
        {
            assert( is_not_off_board_tile( tile_buffer_[i].tile_contents ) );
            assert( is_bot_on_tile( tile_buffer_[i].tile_contents ) );
            send_instructions_to_board( tile_buffer_[i].tile_coordinate, code );
        }
    }

    void Board_state::handle_transmission( const Machine_event& t )
    {
        assert( t.machine()->is_alive() );
        send_instructions_to_board( t.event().transmitted_.transmitToXY_,
                                    t.machine()->pop_transmission( t.event().transmitted_.transmissionId_ ) );
    }

    void Board_state::handle_machine_received_damage( const Machine_event& e )
    {
        e.machine()->apply_damage( e.event().damage_ );
    }

    void Board_state::handle_machine_received_repairs( const Machine_event& e )
    {
        e.machine()->apply_armour_repairs( e.event().armour_repairs_ );
    }

    void Board_state::handle_machine_cap_altered( const Machine_event& e )
    {
        e.machine()->apply_cap_alteration( e.event().cap_delta_ );
    }

    void Board_state::handle_cargo_transfer( const Machine_event& e )
    {
        Machine* receives = machine_at( e.event().cargo_transferred_.giveTo );
        Machine* gives    = machine_at( e.event().cargo_transferred_.takeFrom );

        if( receives &&
                gives &&
                receives->is_alive() &&
                gives->is_alive() &&
                ( e.event().cargo_transferred_.tradableType < BLUE_TRADABLE_TYPES_COUNT ) )
        {
            TradableTypes t = static_cast<TradableTypes>( e.event().cargo_transferred_.tradableType );
            gives->try_transfer_cargo_to( *receives, t, e.event().cargo_transferred_.units );
        }
    }

    void Board_state::handle_machine_cash_transfer( const Machine_event& e )
    {
        Machine* receives = machine_at( e.event().cash_transfer_.giveTo );
        uint16_t amount   = e.event().cash_transfer_.amount;

        if( receives && amount > 0u && e.machine()->try_transfer_credits_to( *receives, amount ) )
        {
            // An actual non-zero transfer took place.
            secondary_events_.push_back(
                Secondary_event::make_cash_transferred( e.machine()->board_xy(),
                                                       receives->board_xy(),
                                                       amount ) );
        }
    }

    void Board_state::handle_completed_build( const Machine_event& e )
    {
        assert( false == grid_.is_tile_empty_and_unreserved( e.event().build_completed_.buildSite ) );

        auto newMachine = e.machine()->build_machine( e.event().build_completed_.blueprintIndex,
                                                      e.event().build_completed_.codeIndex );

        grid_.unreserve_empty_tile( e.event().build_completed_.buildSite );

        add_machine_to_board( e.event().build_completed_.buildSite,
                              e.machine()->facing_direction(),
                              move( newMachine ) );
    }

    void Board_state::handle_initiated_self_destruct( const Machine_event& sd )
    {
        if( sd.machine()->is_alive() )
        {
            if( 0 == sd.event().self_destructed_.countdown_ )
            {
                // Blow up machine now.
                sd.machine()->kill();
                explode_machine( *sd.machine() );
            }
            else
            {
                // Blow up later.
                sd.machine()->inc_watched_refcount();
                delayed_events_.push(
                    Delayed_machine_event( frame_number_ + sd.event().self_destructed_.countdown_,
                                         Machine_event::machine_initiated_self_destruct( *sd.machine(), 0 ) ) );
            }
        }
    }

    void Board_state::handle_event( const Machine_event& e )
    {
        switch( e.event_id() )
        {
            case EMachineAddedToBoard:
            case EMachinePositionalInfoUpdate:
            case EMachineChangeTile:
            case EMachineSensed:
            case EMachineExploded:
            case EMachineError:
            case EMachineAsserted:
            case EMachinePrintText:
            case EMachinePrintInstruction:
            case EMachineStartedBuild:
                break;

            case EMachineFired:
                handle_fired( e );
                break;

            case EMachineBroadcasted:
                handle_broadcast( e );
                break;

            case EMachineTransmitted:
                handle_transmission( e );
                break;

            case EMachineInitiatedSelfDestruct:
                handle_initiated_self_destruct( e );
                break;

            case EMachineCompletedBuild:
                handle_completed_build( e );
                break;

            case EMachineTransferredCargo:
                handle_cargo_transfer( e );
                break;

            case EMachineReceivedDamage:
                handle_machine_received_damage( e );
                break;

            case EMachineReceivedRepairs:
                handle_machine_received_repairs( e );
                break;

            case EMachineCapAltered:
                handle_machine_cap_altered( e );
                break;

            case EMachineTransferCash:
                handle_machine_cash_transfer( e );
                break;

#ifndef NDEBUG
            default:
                assert( ! "Unhandled Machine_event type" );
                break;
#else
            default:
                ss_unreachable;
#endif
        }
    }

    //-------------------------------------------------------------------------

    void Board_state::explode_machine( Machine& m )
    {
        assert( false == m.is_alive() );

        const Machine_exploded boom = m.get_explosion();
        const Damages explosiveDmg  = get_explosion_damages( boom );
        const Hex_coord epicentre   = m.board_xy();

        grid_.get_sorted_circle_contents(
            epicentre, boom.explosionRange, tile_buffer_, bot_tile_value() );

        // Damage all the bots in range of explosion:
        for( size_t i = 1; i < tile_buffer_.size(); ++i )
        {
            const Hex_coord xy = tile_buffer_[i].tile_coordinate;
            const unsigned int distToExplosion = Hex_grid::get_step_distance_between( xy, epicentre );
            // Damage falls off as distance squared.
            const float falloff = 1.0f / static_cast<float>( distToExplosion * distToExplosion );
            auto nearby = machine_at( xy );
            assert( nearby );
            machine_events_.push_back( Machine_event::machine_received_damage( *nearby, explosiveDmg * falloff ) );
        }

        // Signal a machine explosion:
        machine_events_.push_back( Machine_event::machine_exploded( m ) );
    }

    bool Board_state::grid_and_machines_are_consistent() const
    {
        // Check for double occupancy of same tile.
        for( size_t i = 0u; i < grid_.machines_count(); ++i )
        {
            Hex_coord xy = grid_[i]->board_xy();

            for( size_t j = i; j < grid_.machines_count(); ++j )
            {
                if( j != i )
                {
                    if( xy == grid_[j]->board_xy() )
                    {
                        ss_err( "Double occupancy" );
                        return false;
                    }
                }
            }
        }

        // Check machines agree with grid.
        for( size_t i = 0u; i < grid_.machines_count(); ++i )
        {
            Hex_coord xy = grid_[i]->board_xy();

            if( grid_[i]->is_alive() )
            {
                if( false == is_bot_on_tile( grid_.contents( xy ) ) )
                {
                    ss_err( "Alive bot not on his tile" );
                    return false;
                }

                if( is_bot_on_tile( grid_.contents( xy ) ) )
                {
                    const Machine* m = machine_at_coord( xy );

                    if( m != grid_[i] )
                    {
                        ss_err( "Bot on tile is not correct bot" );
                        return false;
                    }
                }
            }
            else
            {
                // Machine at this xy must be another machine, this one
                // is dead.
                if( is_bot_on_tile( grid_.contents( xy ) ) )
                {
                    const Machine* m = machine_at_coord( xy );

                    if( m == grid_[i] )
                    {
                        ss_err( "Dead Bot is still on his tile" );
                        return false;
                    }
                }
            }
        }

        // Check grid agrees with machines.
        for( unsigned int h = 0; h < grid_.grid_height(); ++h )
        {
            for( unsigned int w = 0; w < grid_.grid_width(); ++w )
            {
                Hex_coord xy = Hex_coord::make_coord( w, h );

                if( is_bot_on_tile( grid_.contents( xy ) ) )
                {
                    if( grid_.machine_at( xy ) == nullptr )
                    {
                        ss_err( "Bot in grid but not at that coord in machines" );
                        return false;
                    }

                    if( grid_.machine_at( xy )->board_xy() != xy )
                    {
                        ss_err( "Bot at xy in machines has different board coord" );
                        return false;
                    }

                    if( false == grid_.machine_at( xy )->is_alive() )
                    {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    void Board_state::advance_one_frame(Frame_events& events)
    {
        grid_.cull_dead_machines();

        if( false == grid_and_machines_are_consistent() )
        {
            ss_err( "INCONSISTENT" );
        }

        // Remove all empty asteroids from board.
        for( size_t i = 0u; i < asteroids_.size(); ++i )
        {
            if( asteroids_[i].is_on_board() && asteroids_[i].volume() == 0 )
            {
                asteroids_[i].set_as_removed_from_board();
                grid_.remove_asteroid_from_tile( asteroids_[i].board_xy() );
                ss_log( "REMOVING ASTEROID" );
            }
        }

        const std::vector<Machine*>& living_machines = grid_.find_living_machines();
        const size_t living_count = living_machines.size();

        //
        // MOVEMENT & DEAD MACHINE REMOVAL PHASE
        // Resolve movement on all living machines and record which machines
        // are alive. Machines which are not alive have their indices recorded
        // and are removed.
        //
        for( size_t i = 0u; i < living_count; ++i )
        {
            living_machines[i]->resolve_movement( grid_.as_movement_grid_view() );
        }

        assert( grid_and_machines_are_consistent() );

        //
        // TICK PHASE
        // Tick all machines. This will result in events being processed
        // and delayed events being recorded, which may store the address
        // and state of the machines for future Machine_event processing.
        //
        for( size_t i = 0u; i < living_count; ++i )
        {
            auto m = living_machines[i];

            if( m->resolve_combat( randoms_ ) )
            {
                m->advance_one_frame();
            }
            else
            {
                explode_machine( *m );
            }
        }

        // Sort trade requests so that the order of the trades is dependent
        // on the trades and not on relative board locations.
        sort( trade_requests_.begin(), trade_requests_.end() );

        // Process trade requests - these might generate events.
        for( size_t i = 0u; i < trade_requests_.size(); ++i )
        {
            handle_trade_request( trade_requests_[i] );
        }

        // Process all events received this frame. These events
        // can include explosions etc which can cause machines to die.
        for( size_t i = 0u; i < machine_events_.size(); ++i )
        {
            handle_event( machine_events_[i] );
        }

        // Process delayed events.
        while( ( false == delayed_events_.empty() )
                && ( delayed_events_.top().timestamp_ == frame_number_ ) )
        {
            auto de = delayed_events_.top();
            delayed_events_.pop();

            assert( false == de.event_.machine()->is_not_being_watched() );
            de.event_.machine()->dec_watched_refcount();

            // The machine referenced here may already be dead.
            handle_event( de.event_ );
        }

        // Tell the user what happened this frame.
        events.machine_events   = machine_events_;
        events.secondary_events = secondary_events_;

        assert( grid_and_machines_are_consistent() );

        trade_requests_.clear();
        machine_events_.clear();
        secondary_events_.clear();

        assert( grid_and_machines_are_consistent() );

        ++frame_number_;
    }

    void Board_state::handle_trade_transaction( const Trade_request& r )
    {
        unsigned int quote_count = 0;
        Quote quotes[6];
        Hex_coord buyer_xy = r.customer_->board_xy();

        // Find all quotes at right price
        for( size_t i = 0; i < 6; ++i )
        {
            Hex_coord n = Hex_grid::facing_neighbour_coord( buyer_xy, static_cast<HexFacingName>( i ) );

            if( grid_.is_on_board( n ) && is_bot_on_tile( grid_.contents( n ) ) )
            {
                Quote q = machine_at( n )->get_trade_quote( r );
                if( q )
                {
                    quotes[quote_count++] = q;
                }
            }
        }

        std::sort( quotes, quotes + quote_count );

        uint16_t units_sold = 0;

        for( size_t i = 0; i < quote_count; ++i )
        {
            if( units_sold < r.quantity_ )
            {
                units_sold += quotes[i].seller().sell_goods_to(
                                 *r.customer_, static_cast<TradableTypes>( r.commodity_ ), r.quantity_ - units_sold );
            }
        }

        r.customer_->trade_reply( units_sold );
    }

    void Board_state::handle_trade_query( const Trade_request& r )
    {
        assert( r.customer_ );

        const Hex_coord buyer_xy = r.customer_->board_xy();

        // Find best quote.
        Quote best_quote;

        // Look only at the 6 neighbours. We can only buy from
        // machines beside us.
        for( size_t i = 0; i < 6; ++i )
        {
            const Hex_coord n = Hex_grid::facing_neighbour_coord( buyer_xy, static_cast<HexFacingName>( i ) );

            if( grid_.is_on_board( n ) && is_bot_on_tile( grid_.contents( n ) ) )
            {
                const Quote q = machine_at( n )->get_trade_quote( r );
                if( q < best_quote )
                {
                    best_quote = q;
                }
            }
        }

        if( best_quote )
        {
            switch( r.request_type_ )
            {
                case QueryPrice:
                    r.customer_->trade_reply( best_quote.price() );
                    break;

                case QueryAvailability:
                    r.customer_->trade_reply( best_quote.availability() );
                    break;

                default:
                    ss_unreachable;
                    break;
            }
        }
        else
        {
            r.customer_->trade_reply( 0u );
        }
    }

    void Board_state::handle_trade_request( const Trade_request& r )
    {
        if( r.request_type_ == Buying )
        {
            handle_trade_transaction( r );
        }
        else
        {
            handle_trade_query( r );
        }
    }

    void Board_state::add_machine_to_board( Hex_coord pos,
                                            HexFacingName facing,
                                            std::unique_ptr<Machine> new_machine )
    {
        assert( new_machine );

        if( grid_.is_tile_empty_and_unreserved( pos ) )
        {
            Machine* m = new_machine.get();

            new_machine->setup_externals( machine_events_, trade_requests_, *this, *pathfinder_, randoms_ );

            grid_.add_machine_to_board( move( new_machine ), pos, facing );

            assert( m->board_xy() == pos );

            machine_events_.push_back( Machine_event::machine_added_to_board( *m ) );
        }
        else
        {
            ss_throw( "Trying to add a machine to a reserved tile." );
        }
    }

    //-------------------------------------------------------------------------
    //                          ACTIONS
    //-------------------------------------------------------------------------

    void Board_state::send_instructions_to_board( Hex_coord xy,
                                                  const std::shared_ptr<const Instructions>& code )
    {
        if( Machine* m = machine_at( xy ) )
        {
            // Note: the machine's xy can differ from xy when it has moved this
            // frame. The Board_state returns the machine on xy at the start of
            // the frame.
            m->receive_broadcast( code );
        }
    }
}
