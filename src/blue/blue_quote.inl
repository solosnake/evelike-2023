#include "solosnake/blue/blue_quote.hpp"
#include <cassert>

namespace blue
{
    inline Quote::Quote( Machine& m, For_sale s ) noexcept
        : seller_( &m )
        , goods_( s )
    {
    }

    inline Quote::operator bool () const noexcept
    {
        return nullptr != seller_;
    }

    inline Machine& Quote::seller() const noexcept
    {
        assert( seller_ );
        return *seller_;
    }

    inline std::uint16_t Quote::price() const noexcept
    {
        return goods_.price;
    }

    inline std::uint16_t Quote::availability() const noexcept
    {
        return goods_.quantity;
    }

    inline bool Quote::operator < ( const Quote& rhs ) const noexcept
    {
        if( seller_ && rhs.seller_ )
        {
            // Both have a seller.
            return ( goods_.price <  rhs.goods_.price )
                || ( goods_.price == rhs.goods_.price && goods_.quantity > rhs.goods_.quantity );
        }
        else
        {
            // Only one has seller: sorting is stable, returns less-than
            // if we have a seller and they dont (less is "better" when sorting
            // prices).
            return seller_ != nullptr;
        }
    }
}