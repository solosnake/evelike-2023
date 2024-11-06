#ifndef solosnake_rectangle_hittest_hpp
#define solosnake_rectangle_hittest_hpp

#include <algorithm>
#include <cstdint>
#include <vector>
#include "solosnake/rect.hpp"

namespace solosnake
{
    //! Class for determining quickly if a point is inside a 2D
    //! rectangle.
    template <typename T> class rectangle_hittest
    {
    public:
        typedef T handle_t;

        void add_rect( const Rect& r, const handle_t& );

        void update_rect( const handle_t& id, const Rect& r );

        void remove_rect( const handle_t& id );

        void get_rects_under( int x, int y, std::vector<T>& under ) const;

        void get_rects_under( int x, int y, std::vector<T>& under, std::vector<T>& notUnder ) const;

    private:

        //! "Handle" rect.
        struct hrect
        {
            hrect( const Rect& r, handle_t id )
                : left_( r.left() ), right_( r.right() ), top_( r.top() ), bottom_( r.bottom() ), id_( id )
            {
                // These are the rectangle pre-conditions.
                assert( left_ <= right_ );
                assert( top_ <= bottom_ );
            }

            std::int32_t left_;
            std::int32_t right_;
            std::int32_t top_;
            std::int32_t bottom_;
            handle_t id_;
        };

    private:

        std::vector<hrect> lhs_to_rhs_sorted_rect_;
    };

    //-------------------------------------------------------------------------/

    //! Adds a new rect to the list of rects, and returns a handle to that rect.
    template <typename T> inline void rectangle_hittest<T>::add_rect( const Rect& r, const handle_t& id )
    {
        assert( r.left() <= r.right() );
        assert( r.top() <= r.bottom() );

        lhs_to_rhs_sorted_rect_.push_back( hrect( r, id ) );

        std::sort( lhs_to_rhs_sorted_rect_.begin(),
                   lhs_to_rhs_sorted_rect_.end(),
                   [&]( const hrect & lhs, const hrect & rhs )
        { return lhs.left_ < rhs.left_; } );
    }

    template <typename T>
    inline void rectangle_hittest<T>::update_rect( const handle_t& id, const Rect& r )
    {
        auto pos = std::remove_if( lhs_to_rhs_sorted_rect_.begin(),
                                   lhs_to_rhs_sorted_rect_.end(),
                                   [id]( const hrect & h )
        { return h.id_ == id; } );

        // std::remove does not change the length, only overwrites. Thus we
        // still have the required entry in the vector. Overwrite and re-sort.
        if( pos != lhs_to_rhs_sorted_rect_.end() )
        {
            *( pos ) = hrect( r, id );

            std::sort( lhs_to_rhs_sorted_rect_.begin(),
                       lhs_to_rhs_sorted_rect_.end(),
                       [&]( const hrect & lhs, const hrect & rhs )
            { return lhs.left_ < rhs.left_; } );
        }
    }

    template <typename T> inline void rectangle_hittest<T>::remove_rect( const handle_t& id )
    {
        lhs_to_rhs_sorted_rect_.erase( std::remove_if( lhs_to_rhs_sorted_rect_.begin(),
                                       lhs_to_rhs_sorted_rect_.end(),
                                       [id]( const hrect & h )
        { return h.id_ == id; } ), // Closure matches hrect with same id.
        lhs_to_rhs_sorted_rect_.end() );
    }

    //! Returns the rects which are on or inside this point.
    template <typename T>
    inline void rectangle_hittest<T>::get_rects_under( int x, int y, std::vector<T>& under ) const
    {
        under.clear();

        if( !lhs_to_rhs_sorted_rect_.empty() )
        {
            const auto e = lhs_to_rhs_sorted_rect_.end();
            auto i = lhs_to_rhs_sorted_rect_.begin();

            bool inside_left = i != e;

            while( inside_left && i != e )
            {
                inside_left = x >= i->left_;

                // Note inversion of y wrt top/bottom, due to top < bottom
                // in screen coordinates (Microsoft Windows style).
                if( inside_left && x < i->right_ && y >= i->top_ && y < i->bottom_ )
                {
                    under.push_back( i->id_ );
                }

                ++i;
            }
        }
    }

    //! Returns the rects which are on or inside this point.
    template <typename T>
    inline void rectangle_hittest<T>::get_rects_under(
        int x,
        int y,
        std::vector<T>& under,
        std::vector<T>& notUnder ) const
    {
        under.clear();
        notUnder.clear();

        if( !lhs_to_rhs_sorted_rect_.empty() )
        {
            const auto e = lhs_to_rhs_sorted_rect_.cend();
            auto i = lhs_to_rhs_sorted_rect_.cbegin();

            bool inside_left = i != e;

            while( inside_left && i != e )
            {
                inside_left = x >= i->left_;

                // Note inversion of y wrt top/bottom, due to top < bottom
                // in screen coordinates (Microsoft Windows style).
                if( inside_left && x <= i->right_ && y >= i->top_ && y <= i->bottom_ )
                {
                    under.push_back( i->id_ );
                }
                else
                {
                    notUnder.push_back( i->id_ );
                }

                ++i;
            }

            while( i != e )
            {
                // These were not tested and so presumably not under.
                notUnder.push_back( i->id_ );
                ++i;
            }
        }
    }
}

#endif
