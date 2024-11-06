#ifndef blue_quote_hpp
#define blue_quote_hpp

#include <cstdint>
#include <cstdlib>
#include "solosnake/blue/blue_for_sale.hpp"
#include "solosnake/blue/blue_trade_request.hpp"

namespace blue
{
    class Machine;

    /// A Quote is an offer to fulfill a trade or trade query.
    class Quote
    {
    public:

        Quote() = default;

        Quote( Machine& m, For_sale s ) noexcept;

        operator bool () const noexcept;

        Machine& seller() const noexcept;

        std::uint16_t price() const noexcept;

        std::uint16_t availability() const noexcept;

        /// Sorts first by price, than by quantity.
        bool operator < ( const Quote& rhs ) const noexcept;

    private:

        Machine*        seller_ = nullptr;
        For_sale        goods_  = {};
    };

}

#include "solosnake/blue/blue_quote.inl"
#endif
