#ifndef blue_cargohold_hpp
#define blue_cargohold_hpp

#include <cstdint>
#include "solosnake/blue/blue_tradables.hpp"

namespace blue
{
    /// A Cargohold of a Machine, it can carry Tradables up to its volume limit.
    /// The Cargohold has two states - locked and unlocked - however these
    /// have no effect on the operations. A Cargohold is unlocked by default.
    class Cargohold
    {
    public:

        Cargohold();

        explicit Cargohold( const std::uint16_t volume_limit );

        bool is_locked() const;

        void set_locked( bool );

        std::uint16_t total_volume() const;

        std::uint16_t free_volume() const;

        std::uint16_t used_volume() const;

        bool has_capacity_for( const Tradables& amount ) const;

        bool has_capacity_for( const Amount& amount ) const;

        bool contains_at_least( const Amount& ) const;

        bool try_add_cargo( const Amount& );

        void create_cargo( const Amount& );

        void destroy_cargo( const Amount& );

        void transfer_cargo_to( const Amount& a, Cargohold& other );

        const Tradables& cargo_contents() const;

        std::uint16_t count_of( const std::uint16_t i ) const;

        std::uint16_t operator[]( const std::uint16_t i ) const;

    private:
        Tradables       contents_;
        std::uint16_t   volume_limit_;
        bool            locked_;
    };
}

#include "solosnake/blue/blue_cargohold.inl"
#endif
