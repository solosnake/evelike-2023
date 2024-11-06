#ifndef blue_hardware_factor_hpp
#define blue_hardware_factor_hpp

namespace blue
{
    /// Guarantees that the factor float is always >= 0.0f
    class Hardware_factor
    {
    public:

        Hardware_factor() = default;

        Hardware_factor( float f );

        operator float() const noexcept;

        Hardware_factor& operator*=( const Hardware_factor& rhs ) noexcept;

        bool operator == ( const Hardware_factor& rhs ) const noexcept;

        bool operator != ( const Hardware_factor& rhs ) const noexcept;

    private:

        float value_{1.0f};
    };
}

#include "solosnake/blue/blue_hardware_factor.inl"
#endif
