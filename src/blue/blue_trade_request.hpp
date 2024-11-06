#ifndef blue_traderequest_hpp
#define blue_traderequest_hpp

#include <cstdint>
#include <cassert>

namespace blue
{
    class Machine;

    /// Buying has lowest value so that when sorting all purchases are done
    /// before price checks, and so price checks etc should reflect the market.
    enum TradeRequestType
    {
        Buying,
        QueryPrice,
        QueryAvailability
    };

    /// Created by a machine interested in trading (or just querying).
    struct Trade_request
    {
        Machine*        customer_;
        std::uint8_t    request_type_;  // Buying or querying.
        std::uint8_t    commodity_;     // Type of item machine has interest in.
        std::uint16_t   price_limit_;   // How much buyer is willing to pay, per unit.
        std::uint16_t   quantity_;      // Amount of tradable_ the machine has interest in.
    };

    bool operator == (const Trade_request& lhs, const Trade_request& rhs) noexcept;

    /// Sort by request type, then commodity, then price, then quantity.
    inline bool operator<( const Trade_request& lhs, const Trade_request& rhs ) noexcept
    {
        assert( lhs.customer_ && rhs.customer_ );
        return ( lhs.request_type_ < rhs.request_type_ )
               || ( lhs.request_type_ == rhs.request_type_ && lhs.commodity_ < rhs.commodity_ )
               || ( lhs.request_type_ == rhs.request_type_ && lhs.commodity_ == rhs.commodity_
                    && lhs.price_limit_ > rhs.price_limit_ )
               || ( lhs.request_type_ == rhs.request_type_ && lhs.commodity_ == rhs.commodity_
                    && lhs.price_limit_ == rhs.price_limit_ && lhs.quantity_ > rhs.quantity_ );
    }
}

#endif