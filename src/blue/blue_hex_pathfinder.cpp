#include <algorithm>
#include <cassert>
#include <cstdint>
#include <thread>
#include <future>
#include "solosnake/blue/blue_clockwise.hpp"
#include "solosnake/blue/blue_game_hex_grid.hpp"
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_hex_path_costs.hpp"
#include "solosnake/blue/blue_hex_pathfinder.hpp"
#include "solosnake/blue/blue_instruction.hpp"
#include "solosnake/blue/blue_machine_grid_view.hpp"
#include "solosnake/blue/blue_throw.hpp"
#include "solosnake/blue/blue_unaliased.hpp"

namespace blue
{
#   define BLUE_EXPECTED_SIMULTANEOUS_ASTARS       (4)
#   define BLUE_SEARCHNODE_PARENT_SHIFT            (48)
#   define BLUE_SEARCHNODE_COORD_SHIFT             (32)
#   define BLUE_SEARCHNODE_COORD_TO_PARENT_SHIFT   (16)
#   define BLUE_SEARCHNODE_G_SHIFT                 (16)
#   define BLUE_SEARCHNODE_F_SHIFT                 (0)
#   define BLUE_SEARCH_MAX_G                       (0x003FFFFF) // 4,194,303
#   define BLUE_SEARCH_MAX_F                       (0x000000FF) //       255
#   define BLUE_CONTENT_COST_FACTOR                (4)

    static_assert( Hex_grid::MaxPermittedGridWidth  <= 100, "MaxPermittedGridWidth too large." );
    static_assert( Hex_grid::MaxPermittedGridHeight <= 100, "MaxPermittedGridHeight too large." );

    namespace
    {
        static const uint64_t BLUE_SEARCHNODE_PARENT_FLAGS     = 0xFFFF000000000000;
        static const uint64_t BLUE_SEARCHNODE_COORD_FLAGS      = 0x0000FFFF00000000;
        static const uint64_t BLUE_SEARCHNODE_G_FLAGS          = 0x000000003FFFFF00;
        static const uint64_t BLUE_SEARCHNODE_F_FLAGS          = 0x00000000000000FF;
        static const uint64_t BLUE_SEARCHNODE_IN_CLOSED_FLAG   = 0x0000000040000000;
        static const uint64_t BLUE_SEARCHNODE_IN_OPEN_FLAG     = 0x0000000080000000;

        inline Instruction make_advance_instruction( unsigned int n )
        {
            return Instruction::from_asm( BLUE_ASM_ADVANCE | BLUE_VALUE_ARG0,
                                          static_cast<uint16_t>( n ) );
        }

        Instruction make_turn_instruction( HexFacingName facing_now, HexFacingName facing_wanted )
        {
            uint16_t turn_count = 0;
            uint16_t turn_direction = BLUE_C_CLOCKWISE;

            if( facing_wanted > facing_now )
            {
                turn_count = facing_wanted - facing_now;
                if( turn_count > 3 )
                {
                    turn_count = 6 - turn_count;
                }
                else
                {
                    turn_direction = BLUE_CLOCKWISE;
                }
            }
            else if( facing_now > facing_wanted )
            {
                turn_count = facing_now - facing_wanted;
                if( turn_count > 3 )
                {
                    turn_count = 6 - turn_count;
                    turn_direction = BLUE_CLOCKWISE;
                }
            }

            // arg0 is turn count, arg1 is direction.
            return Instruction::from_asm(
                       BLUE_ASM_TURN | BLUE_VALUE_ARG0 | BLUE_VALUE_ARG1, turn_count, turn_direction );
        }

        /// path[0] is the starting point.
        static void make_code_for_path( const std::vector<Hex_coord>& path,
                                        const HexFacingName initial_facing,
                                        std::vector<Instruction>& path_code )
        {
            assert( path_code.empty() );

            HexFacingName facing = initial_facing;

            // Path[0] is the starting point.
            Hex_coord current = path[0];
            unsigned int advance_count = 0;

            for( unsigned int i = 1; i < path.size(); )
            {
                Hex_coord ahead = Hex_grid::facing_neighbour_coord( current, facing );

                if( ahead == path[i] )
                {
                    // Just move ahead.
                    current = path[i];
                    ++advance_count;
                    ++i;
                }
                else
                {
                    if( advance_count > 0 )
                    {
                        // Store moves before changing direction.
                        path_code.push_back( make_advance_instruction( advance_count ) );
                        advance_count = 0;
                    }

                    // Change direction.
                    HexFacingName facing_wanted = Hex_grid::neighbors_facing( current, path[i] );
                    path_code.push_back( make_turn_instruction( facing, facing_wanted ) );
                    facing = facing_wanted;
                }
            }

            if( advance_count > 0 )
            {
                path_code.push_back( make_advance_instruction( advance_count ) );
            }
        }
    }

    /// Asynchronously performs A* search for a path from A to B.
    class Hex_path_search
    {
    public:

        Hex_path_search()
            : start_()
            , goal_()
            , goal_is_vacant_()
            , finished_()
            , grid_()
            , grid_width_()
            , node_grid_()
            , open_heap_()
            , costs_()
            , starting_facing_( FacingTile0 )
            , search_centre_()
            , search_radius_()
            , path_limit_()
            , path_()
            , instructions_()
            , future_()
        {
        }

        class Node
        {
            uint64_t n_;

        public:

            inline Node( uint64_t n ) : n_( n )
            {
            }

            /// This reflects the cost of moving from the start location to this
            // Node's hex tile. Lower is better.
            /// This is the 'g' score in the A* algorithm.
            inline uint16_t local_cost() const
            {
                return static_cast<uint16_t>( ( n_ & BLUE_SEARCHNODE_G_FLAGS ) >> BLUE_SEARCHNODE_G_SHIFT );
            }

            /// A heuristic cost estimate of proximity of this Node's hex tile to
            // the goal. Lower is better.
            inline uint16_t estimate_to_goal() const
            {
                return static_cast<uint16_t>( ( n_ & BLUE_SEARCHNODE_F_FLAGS ) >> BLUE_SEARCHNODE_F_SHIFT );
            }

            /// This is the 'f' score in the A* algorithm. Lower is better.
            inline unsigned int total_cost() const
            {
                return BLUE_CONTENT_COST_FACTOR * static_cast<unsigned int>( estimate_to_goal() )
                       + static_cast<unsigned int>( local_cost() );
            }

            inline Hex_coord parent_coord() const
            {
                return Hex_coord::from_uint16( static_cast<uint16_t>(
                                                  ( n_ & BLUE_SEARCHNODE_PARENT_FLAGS ) >> BLUE_SEARCHNODE_PARENT_SHIFT ) );
            }

            inline Hex_coord coord() const
            {
                return Hex_coord::from_uint16( static_cast<uint16_t>(
                                                  ( n_ & BLUE_SEARCHNODE_COORD_FLAGS ) >> BLUE_SEARCHNODE_COORD_SHIFT ) );
            }

            inline bool is_in_open() const
            {
                return 0 != ( n_ & BLUE_SEARCHNODE_IN_OPEN_FLAG );
            }

            inline bool is_in_closed() const
            {
                return 0 != ( n_ & BLUE_SEARCHNODE_IN_CLOSED_FLAG );
            }

            inline void add_to_open()
            {
                assert( false == is_in_open() );
                assert( false == is_in_closed() );
                n_ |= BLUE_SEARCHNODE_IN_OPEN_FLAG;
                assert( is_in_open() );
            }

            inline void add_to_closed()
            {
                assert( false == is_in_open() );
                assert( false == is_in_closed() );
                n_ |= BLUE_SEARCHNODE_IN_CLOSED_FLAG;
                assert( is_in_closed() );
            }

            inline void remove_from_open()
            {
                assert( is_in_open() );
                assert( false == is_in_closed() );
                n_ &= ~BLUE_SEARCHNODE_IN_OPEN_FLAG;
                assert( false == is_in_open() );
            }

            inline void remove_from_closed()
            {
                assert( false == is_in_open() );
                assert( is_in_closed() );
                n_ &= ~BLUE_SEARCHNODE_IN_CLOSED_FLAG;
                assert( false == is_in_closed() );
            }

            inline bool operator==( const Node rhs ) const
            {
                return n_ == rhs.n_;
            }

            /// Sort by score: default sort criteria used by heap.
            inline bool operator<( const Node rhs ) const
            {
                return total_cost() > rhs.total_cost();
            }

            inline void set_parent_and_costs( uint64_t g, uint64_t f, const Node* parent )
            {
                assert( g <= BLUE_SEARCH_MAX_G );
                assert( f <= BLUE_SEARCH_MAX_F );

                n_ = ( g << BLUE_SEARCHNODE_G_SHIFT ) | ( f << BLUE_SEARCHNODE_F_SHIFT )
                     | ( n_ & BLUE_SEARCHNODE_COORD_FLAGS ) | ( ( parent->n_ & BLUE_SEARCHNODE_COORD_FLAGS )
                                                                << BLUE_SEARCHNODE_COORD_TO_PARENT_SHIFT );

                assert( parent_coord() == parent->coord() );
                assert( false == is_in_open() );
                assert( false == is_in_closed() );
                assert( local_cost() == g );
                assert( estimate_to_goal() == f );
            }

            void reset( Hex_coord xy )
            {
                n_ = ( static_cast<uint64_t>( Hex_coord::to_uint16( xy ) )
                       << BLUE_SEARCHNODE_COORD_SHIFT );
                assert( coord() == xy );
            }
        };

        inline Node* node_from_coord( Hex_coord xy )
        {
            assert( xy.x < grid_width_ );
            assert( static_cast<std::size_t>( grid_width_ * xy.y + xy.x ) < node_grid_.size() );
            return &node_grid_[grid_width_ * xy.y + xy.x];
        }

        inline const Node* node_from_coord( Hex_coord xy ) const
        {
            return &node_grid_[grid_width_ * xy.y + xy.x];
        }

        inline void reset_node( int8_t x, int8_t y )
        {
            node_grid_[grid_width_ * y + x].reset( Hex_coord::make_coord( x, y ) );
        }

        inline bool has_open_nodes() const
        {
            return false == open_heap_.empty();
        }

        inline uint16_t heuristic( const Node* unaliased n ) const
        {
            return Hex_grid::get_step_distance_between( goal_, n->coord() );
        }

        inline const Node* parent_node( const Node* n ) const
        {
            return node_from_coord( n->parent_coord() );
        }

        inline void add_to_closed( Node* n )
        {
            n->add_to_closed();
        }

        inline void remove_from_closed( Node* n )
        {
            n->remove_from_closed();
        }

        inline Node* pop_best()
        {
            assert( false == open_heap_.empty() );
            pop_heap( open_heap_.begin(), open_heap_.end() );
            Node n = open_heap_.back();
            open_heap_.pop_back();
            Node* pn = node_from_coord( n.coord() );
            assert( n == *pn );
            pn->remove_from_open();
            return pn;
        }

        inline Node* get_neighbour( unsigned int i, const Node* n )
        {
            Hex_coord xy = Hex_grid::facing_neighbour_coord( n->coord(), static_cast<HexFacingName>( i ) );

            if( grid_.is_in_board_bounds( xy ) )
            {
                return is_not_off_board_tile( grid_.contents( xy ) ) ? node_from_coord( xy ) : nullptr;
            }

            return nullptr;
        }

        /// Returns the number of turns needed to move from current to n,
        /// taking into account the previous tile.
        inline unsigned int turns_needed( const Node* unaliased current, const Node* unaliased n ) const
        {
            Hex_coord xy = current->coord();
            int n_facing = static_cast<int>( Hex_grid::neighbors_facing( xy, n->coord() ) );
            int c_facing = static_cast<int>( ( xy == start_ ) ? starting_facing_
                                            : Hex_grid::neighbors_facing( current->parent_coord(), xy ) );
            return std::min<unsigned int>( 3u, static_cast<unsigned int>( abs( n_facing - c_facing ) ) );
        }

        /// Returns zero if the cost is infinite.
        unsigned int cost_of_moving_to( const Node* unaliased current, const Node* unaliased n ) const
        {
            assert( 1 == Hex_grid::get_step_distance_between( current->coord(), n->coord() ) );

            // When content_cost is zero the cost of moving is infinite, indicated by
            // returning zero.
            unsigned int content_cost = costs_.get_path_cost( grid_.contents( n->coord() ) );

            if( content_cost > 0u )
            {
                // We consider turns when on the starting tile to not cost anything.
                unsigned int turns = current->coord() == start_ ? 0u : turns_needed( current, n );
                return content_cost * BLUE_CONTENT_COST_FACTOR + turns;
            }

            return 0u;
        }

        inline void add_to_open( Node* n )
        {
            assert( open_heap_.end() == find_if( open_heap_.begin(),
                                                 open_heap_.end(),
                                                 [ = ]( Node x )
            { return x.coord() == n->coord(); } ) );
            n->add_to_open();
            open_heap_.push_back( *n );
            push_heap( open_heap_.begin(), open_heap_.end() );
        }

        inline void remove_from_open( Node* n )
        {
            Hex_coord xy = n->coord();
            auto i = find_if( open_heap_.begin(),
                              open_heap_.end(),
                              [ = ]( Node x )
            { return x.coord() == xy; } );
            assert( i != open_heap_.end() );
            open_heap_.erase( i );
            make_heap( open_heap_.begin(), open_heap_.end() );
            n->remove_from_open();
        }

        inline bool arrived_at_goal( Hex_coord xy ) const
        {
            if( goal_is_vacant_ )
            {
                return xy == goal_;
            }
            else
            {
                return 1 == Hex_grid::get_step_distance_between( goal_, xy );
            }
        }

        void build_node_path( const Node* final_node )
        {
            const Node* n = final_node;
            do
            {
                // Build path from goal -> start.
                path_.push_back( n->coord() );
                n = parent_node( n );
            }
            while( path_.back() != start_ );

            // Reverse path to get start -> goal.
            reverse( path_.begin(), path_.end() );

            // If requested, clamp the returned path to the path limit plus 1.
            if( path_limit_ > 0 )
            {
                path_.resize( std::min<size_t>( path_.size(), 1 + path_limit_ ) );
            }
        }

        static void astar( Hex_path_search* t )
        {
            t->add_to_open( t->node_from_coord( t->start_ ) );

            while( t->has_open_nodes() )
            {
                Node* unaliased current = t->pop_best();

                if( t->arrived_at_goal( current->coord() ) )
                {
                    t->build_node_path( current );
                    return;
                }

                t->add_to_closed( current );

                unsigned int current_cost = current->local_cost();

                for( unsigned int i = 0; i < 6; ++i )
                {
                    Node* unaliased n = t->get_neighbour( i, current );

                    if( n )
                    {
                        // When zero then the cost is infinitely high : not
                        // traversable.
                        unsigned int cost_of_moving = t->cost_of_moving_to( current, n );

                        if( cost_of_moving > 0
                                && t->search_radius_
                                >= Hex_grid::get_step_distance_between( n->coord(), t->search_centre_ ) )
                        {
                            unsigned int current_path_to_n_cost = current_cost + cost_of_moving;

                            bool not_in_open_or_closed = true;

                            if( n->is_in_open() )
                            {
                                not_in_open_or_closed = current_path_to_n_cost < n->local_cost();
                                if( not_in_open_or_closed )
                                {
                                    t->remove_from_open( n );
                                }
                            }
                            else if( n->is_in_closed() )
                            {
                                not_in_open_or_closed = current_path_to_n_cost < n->local_cost();
                                if( not_in_open_or_closed )
                                {
                                    t->remove_from_closed( n );
                                }
                            }

                            if( not_in_open_or_closed )
                            {
                                n->set_parent_and_costs(
                                    current_path_to_n_cost, t->heuristic( n ), current );
                                t->add_to_open( n );
                            }
                        }
                    }
                }
            }
        }

        static void start_astar( Hex_path_search* t )
        {
            t->path_.clear();
            t->path_.reserve( 1 + t->path_limit_ );
            t->instructions_.clear();
            t->instructions_.reserve( 1 + t->path_limit_ );
            t->open_heap_.clear();

            // Assuming the heap is an expanding circle frontier, ~= 2 Pi r
            size_t expected_size = t->search_radius_ * 6u;
            t->open_heap_.reserve( expected_size );

            t->node_grid_.resize( t->grid_.grid_size(), 0u );

            // Do every time - we will always need to clear the flags etc anyways.
            for( int8_t x = 0; x < t->grid_.grid_width(); ++x )
            {
                for( int8_t y = 0; y < t->grid_.grid_height(); ++y )
                {
                    t->reset_node( x, y );
                }
            }

            astar( t );

            if( false == t->path_.empty() )
            {
                make_code_for_path( t->path_, t->starting_facing_, t->instructions_ );
            }
        }

        void search_for_path_between( const Hex_coord start,
                                      const Hex_coord goal,
                                      const Hex_grid& grid,
                                      const Hex_path_costs& costs,
                                      const HexFacingName facing,
                                      const Hex_coord search_centre,
                                      const uint16_t search_radius,
                                      const uint16_t path_limit )
        {
            start_            = start;
            goal_             = goal;
            goal_is_vacant_   = is_empty_tile( grid.contents( goal ) );
            finished_         = false;
            grid_             = grid;
            grid_width_       = grid.grid_width();
            costs_            = costs;
            starting_facing_  = facing;
            search_centre_    = search_centre;
            search_radius_    = search_radius;
            path_limit_        = path_limit;

            future_ = std::async(
                          std::launch::async,
                          std::packaged_task<void()>( std::bind( Hex_path_search::start_astar, this ) ) );
        }

        const std::vector<Instruction>& get_path_instructions()
        {
            if( false == finished_ )
            {
                future_.wait();
                finished_ = true;
            }

            return instructions_;
        }

        const std::vector<Hex_coord>& get_path()
        {
            if( false == finished_ )
            {
                future_.wait();
                finished_ = true;
            }

            return path_;
        }

    private:

        Hex_coord                   start_;
        Hex_coord                   goal_;
        bool                        goal_is_vacant_;
        bool                        finished_;
        Hex_grid                    grid_;
        int                         grid_width_;
        std::vector<Node>                node_grid_;
        std::vector<Node>                open_heap_;
        Hex_path_costs              costs_;
        HexFacingName               starting_facing_;
        Hex_coord                   search_centre_;
        uint16_t                    search_radius_;
        uint16_t                    path_limit_;
        std::vector<Hex_coord>           path_;
        std::vector<Instruction>         instructions_;
        std::future<void>           future_;
    };

    //-------------------------------------------------------------------------

    Hex_pathfinder::Hex_pathfinder()
    {
        tasks_.reserve( BLUE_EXPECTED_SIMULTANEOUS_ASTARS );
        for( size_t i = 0; i < BLUE_EXPECTED_SIMULTANEOUS_ASTARS; ++i )
        {
            tasks_.emplace_back( std::make_unique<Hex_path_search>() );
            idle_tasks_.push( tasks_.back().get() );
        }
    }

    Hex_pathfinder::~Hex_pathfinder()
    {
    }

    Hex_path_search* Hex_pathfinder::search_for_path_between(
        const Hex_coord a,
        const Hex_coord b,
        const Machine_grid_view grid,
        const Hex_path_costs& costs,
        const HexFacingName facing,
        const Hex_coord search_centre,
        const uint16_t search_radius,
        const uint16_t path_limit )
    {
        if( idle_tasks_.empty() )
        {
            tasks_.emplace_back( std::make_unique<Hex_path_search>() );
            idle_tasks_.push( tasks_.back().get() );
        }

        Hex_path_search* h = idle_tasks_.top();
        idle_tasks_.pop();

        h->search_for_path_between(
            a, b, grid.grid(), costs, facing, search_centre, search_radius, path_limit );

        return h;
    }

    const std::vector<Instruction>& Hex_pathfinder::get_path_instructions( Hex_path_search*& user_handle )
    {
        assert( user_handle );
        idle_tasks_.push( user_handle );
        user_handle = nullptr;
        return idle_tasks_.top()->get_path_instructions();
    }

    const std::vector<Hex_coord>& Hex_pathfinder::get_path( Hex_path_search*& user_handle )
    {
        assert( user_handle );
        idle_tasks_.push( user_handle );
        user_handle = nullptr;
        return idle_tasks_.top()->get_path();
    }

    void Hex_pathfinder::cancel( Hex_path_search*& user_handle )
    {
        get_path_instructions( user_handle );
    }

    bool Hex_pathfinder::operator == (const Hex_pathfinder& rhs) const noexcept
    {
        ss_throw("Not implemented.");
    }

    //-------------------------------------------------------------------------

    void make_path_instructions( const std::vector<Hex_coord>& path,
                                 const HexFacingName initial_facing,
                                 std::vector<Instruction>& code )
    {
        code.clear();

        if( false == path.empty() )
        {
            make_code_for_path( path, initial_facing, code );
        }
    }
}
