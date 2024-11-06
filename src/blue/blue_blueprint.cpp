#include <algorithm>
#include <cassert>
#include <numeric>
#include "solosnake/blue/blue_blueprint.hpp"
#include "solosnake/blue/blue_hardware.hpp"
#include "solosnake/blue/blue_throw.hpp"
#include "solosnake/blue/blue_unaliased.hpp"

namespace blue
{
    namespace
    {
        static constexpr char AttachSquareOccupied  = 1;
        static constexpr char AttachSquareVisited   = 2;
        static constexpr auto GRID_DIMENSION        = 10;
        static constexpr auto GRID_SIZE             = (GRID_DIMENSION* GRID_DIMENSION);

        static constexpr Fixed_angle orientations[] =
        {
            0,
            BLUE_INT32_HALF_PI,
            BLUE_INT32_PI,
            BLUE_INT32_THREE_QUARTERS_PI
        };

        bool occupy_component_at( const unsigned short locationNumber, char* const unaliased grid )
        {
            if( ( 0 == grid[locationNumber] )      && ( 0 == grid[locationNumber +  1] ) &&
                    ( 0 == grid[locationNumber + 10] ) && ( 0 == grid[locationNumber + 11] ) )
            {
                grid[locationNumber]      = AttachSquareOccupied;
                grid[locationNumber +  1] = AttachSquareOccupied;
                grid[locationNumber + 10] = AttachSquareOccupied;
                grid[locationNumber + 11] = AttachSquareOccupied;

                return true;
            }

            return false;
        }
    }


    /// Returns true if this arrangement of modules can be used to create
    /// a blueprint.
    Result Blueprint::can_form_valid_blueprint( const Chassis& ,
                                                const std::map<Thruster_attachpoint, Thruster>& thrusters,
                                                const std::map<AttachPoint, Softpoint>& softpoints,
                                                const std::map<AttachPoint, Oriented_hardpoint>& hardpoints )
    {
        // We require at least one Softpoint to create a valid Machine.
        if( softpoints.empty() )
        {
            return Result("Softpoints were empty.");
        }

        // Cannot have more than max components:
        if( ( thrusters.size() + softpoints.size() + hardpoints.size() ) > MaxComponents )
        {
            return Result("Too many components.");
        }

        // There are 10 x 10 squares for the components to occupy. 4 of them
        // are unreachable due to constraints imposed by the attach points:
        // 0, 9, 90 and 99
        char squares[GRID_SIZE] = { 0 };

        // Write each Component into the grid, returning false if any
        // Component could not be written (meaning two or more components
        // overlap).

        for( auto i = thrusters.cbegin(); i != thrusters.cend(); ++i )
        {
            if( false == occupy_component_at( static_cast<unsigned short>(i->first), squares ) )
            {
                ss_dbg( "thruster ",
                        static_cast<unsigned short>(i->first),
                        " ",
                        i->second.details().component_name(),
                        " conflicts with previous Component" );
                return Result("A thruster conflicts with another Component.");
            }
        }

        for( auto i = hardpoints.cbegin(); i != hardpoints.cend(); ++i )
        {
            if( false == occupy_component_at( static_cast<unsigned short>(i->first), squares ) )
            {
                ss_dbg( "Hardpoint ",
                        static_cast<unsigned short>(i->first),
                        " ",
                        i->second.oriented_hardpoint().details().component_name(),
                        " conflicts with previous Component" );
                return Result("A Hardpoint conflicts with another Component.");
            }
        }

        for( auto i = softpoints.cbegin(); i != softpoints.cend(); ++i )
        {
            if( false == occupy_component_at( static_cast<unsigned short>(i->first), squares ) )
            {
                ss_dbg( "Softpoint ",
                        static_cast<unsigned short>(i->first),
                        " ",
                        i->second.details().component_name(),
                        " conflicts with previous Component" );
                return Result("A Softpoint conflicts with another Component.");
            }
        }

        // Check that the layout has no 'islands' in it. This is a graph theory
        // kind of analysis - we need to see that all squares are reachable
        // from all other squares. If there are two squares which are not
        // connected to each other then this means the Machine layout has 2+
        // disjoint sections and this is illegal.

        // There are no islands if, starting from any square, we can visit all
        // the other squares.
        const size_t expectedSquaresCount = 4 * ( hardpoints.size() + softpoints.size() + thrusters.size() );
        assert( std::accumulate( squares, squares + GRID_SIZE, 0 ) == static_cast<char>( expectedSquaresCount ) );

        // If there are no islands, the size of this vector will be equal to
        // the expected square count.
        char visitedSquares[GRID_SIZE];
        visitedSquares[0] = static_cast<char>( softpoints.cbegin()->first.location_number() );
        unsigned int visitedSquaresIndex = 1;
        unsigned int currentSquareIndex = 0;

        // Search ends when we reach the end of the visited array after stopping
        // adding new squares to visit.
        const char neighbourOffsets[4] = { -GRID_DIMENSION, -1, +1, +GRID_DIMENSION };
        while( currentSquareIndex != visitedSquaresIndex )
        {
            // Visit each of the current square's four neighbours.
            for( unsigned int i = 0; i < 4; ++i )
            {
                const char neighbourSquare = visitedSquares[currentSquareIndex] + neighbourOffsets[i];
                if( neighbourSquare >= 0 && neighbourSquare < GRID_SIZE )
                {
                    if( squares[static_cast<size_t>( neighbourSquare )] == AttachSquareOccupied )
                    {
                        assert( visitedSquaresIndex < GRID_SIZE );
                        squares[static_cast<size_t>( neighbourSquare )] = AttachSquareVisited;
                        visitedSquares[visitedSquaresIndex++] = neighbourSquare;
                    }
                }
            }

            ++currentSquareIndex;
        }

        // If we visited the number we expected, the layout has no islands
        // and is a valid build.
        if( visitedSquaresIndex == 0u )
        {
            return Result("Error in blueprint.");
        }

        if ( (visitedSquaresIndex - 1u ) != expectedSquaresCount )
        {
            return Result("Layout would contain islands.");
        }

        return true;
    }

    /// Constructs a blueprint if and only if these form a valid blueprint.
    Blueprint::Blueprint( const Chassis& chassis,
                          const std::map<Thruster_attachpoint, Thruster>& thrusters,
                          const std::map<AttachPoint, Softpoint>& softpoints,
                          const std::map<AttachPoint, Oriented_hardpoint>& hardpoints )
        : chassis_( chassis )
        , hardpoints_()
        , softpoints_()
        , thrusters_()
        , build_cost_()
        , build_complexity_( 0.0f )
        , hardware_()
    {
        const auto good_bp = can_form_valid_blueprint( chassis, thrusters, softpoints, hardpoints );

        if( good_bp )
        {
            Amount total_build_cost;

            thrusters_.reserve( thrusters.size() );
            hardpoints_.reserve( hardpoints.size() );
            softpoints_.reserve( softpoints.size() );

            for( auto i = softpoints.cbegin(); i != softpoints.cend(); ++i )
            {
                softpoints_.push_back( Attached_softpoint( i->second, i->first ) );
                total_build_cost    += softpoints_.back().first.details().build_cost();
                build_complexity_   += softpoints_.back().first.details().build_complexity();
            }

            for( auto i = hardpoints.cbegin(); i != hardpoints.cend(); ++i )
            {
                hardpoints_.push_back( Attached_hardpoint( i->second, i->first ) );
                total_build_cost    += hardpoints_.back().first.oriented_hardpoint().details().build_cost();
                build_complexity_   += hardpoints_.back().first.oriented_hardpoint().details().build_complexity();
            }

            for( auto i = thrusters.cbegin(); i != thrusters.cend(); ++i )
            {
                thrusters_.push_back( Attached_thruster( i->second, i->first ) );
                total_build_cost    += thrusters_.back().first.details().build_cost();
                build_complexity_   += thrusters_.back().first.details().build_complexity();
            }

            build_cost_ = total_build_cost;
            hardware_   = calculate_hardware();
        }
        else
        {
            ss_throw( good_bp.error().data() );
        }
    }

    std::map<Thruster_attachpoint, Thruster>  Blueprint::make_thrustpoints_map() const
    {
        std::map<Thruster_attachpoint, Thruster> thrusters;

        for(const auto& t : thrusters_)
        {
            thrusters.insert({t.second, t.first});
        }

        return thrusters;
    }

    std::map<AttachPoint, Softpoint> Blueprint::make_softpoints_map() const
    {
        std::map<AttachPoint, Softpoint> softpoints;

        for(const auto& t : softpoints_)
        {
            softpoints.insert({t.second, t.first});
        }

        return softpoints;
    }

    std::map<AttachPoint, Oriented_hardpoint> Blueprint::make_oriented_hardpoints_map() const
    {
        std::map<AttachPoint, Oriented_hardpoint> oriented_hardpoints;

        for(const auto& t : hardpoints_)
        {
            oriented_hardpoints.insert({t.second, t.first});
        }

        return oriented_hardpoints;
    }

    /// Returns the sum of the changes of the Softpoint hardware modifiers.
    Hardware_modifier Blueprint::cumulative_modifiers() const
    {
        Hardware_modifier result;

        for( size_t i = 0; i < softpoints_.size(); ++i )
        {
            result += softpoints_[i].first.modifier();
        }

        return result;
    }

    Hardware Blueprint::calculate_hardware() const
    {
        // Go over all units that compose this blueprint and find the weight
        // and hit-points totals.
        int32_t hardpoint_hit_points = 0;
        int32_t hardpoint_total_mass = 0;
        for( size_t i = 0; i < hardpoints_.size(); ++i )
        {
            hardpoint_hit_points += hardpoints_[i].first.oriented_hardpoint().details().hull_hitpoints();
            hardpoint_total_mass += hardpoints_[i].first.oriented_hardpoint().details().mass();
        }

        int32_t softpoint_hit_points = 0;
        int32_t softpoint_total_mass = 0;
        for( size_t i = 0; i < softpoints_.size(); ++i )
        {
            softpoint_hit_points += softpoints_[i].first.details().hull_hitpoints();
            softpoint_total_mass += softpoints_[i].first.details().mass();
        }

        int32_t thruster_hit_points = 0;
        int32_t thruster_total_mass = 0;
        int32_t thruster_power = 0;
        for( size_t i = 0; i < thrusters_.size(); ++i )
        {
            thruster_hit_points += thrusters_[i].first.details().hull_hitpoints();
            thruster_total_mass += thrusters_[i].first.details().mass();
            thruster_power += thrusters_[i].first.power();
        }

        const int32_t hit_points = hardpoint_hit_points + softpoint_hit_points + thruster_hit_points;
        const int32_t mass = hardpoint_total_mass + softpoint_total_mass + thruster_total_mass;

        const float mass_factor = 1.0f;
        const auto total_mass = ceil( mass * mass_factor );
        if( total_mass >= std::numeric_limits<int32_t>::max() )
        {
            ss_throw( "Blueprint total mass out of range of int32" );
        }

        const float toughness_factor = 1.0f;
        const auto total_hull_hit_points = ceil( hit_points * toughness_factor );
        if( total_hull_hit_points >= std::numeric_limits<int32_t>::max() )
        {
            ss_throw( "Blueprint total hull hit_points out of range of int32" );
        }

        const Hardware hw( BLUE_DEFAULT_CYCLES_PER_INSTRUCTION,
                           BLUE_DEFAULT_MOBILITY,
                           BLUE_DEFAULT_MAX_SPEED,
                           BLUE_DEFAULT_MAX_TURN_SPEED,
                           BLUE_DEFAULT_MAX_CAPACITOR,
                           thruster_power,
                           static_cast<int32_t>( total_mass ),
                           static_cast<uint32_t>( total_hull_hit_points ) );

        Hardware modified_hw = hw * cumulative_modifiers();

        if( thrusters_.empty() )
        {
            modified_hw.remove_thruster_traits();
        }

        return modified_hw;
    }

    Fixed_angle Blueprint::hardpoint_angle( size_t n ) const
    {
        return orientations[hardpoints_[n].first.hardpoint_orientation()];
    }

    bool Blueprint::operator == ( const Blueprint& rhs ) const noexcept
    {
        return (chassis_          == rhs.chassis_)
            && (hardpoints_       == rhs.hardpoints_)
            && (softpoints_       == rhs.softpoints_)
            && (thrusters_        == rhs.thrusters_)
            && (build_cost_       == rhs.build_cost_)
            && (build_complexity_ == rhs.build_complexity_)
            && (hardware_         == rhs.hardware_)
            ;
    }

    bool Blueprint::operator != ( const Blueprint& rhs ) const noexcept
    {
        return ! ((*this) == rhs);
    }
}
