#ifndef blue_radians_hpp
#define blue_radians_hpp

namespace blue
{
    class Radians
    {
    public:

        Radians() = default;

        explicit Radians( const float r ) noexcept;

        float cos() const noexcept;

        float sin() const noexcept;

        float tan() const noexcept;

        bool operator==( Radians rhs ) const noexcept;

        bool operator!=( Radians rhs ) const noexcept;

        float value() const noexcept;

        operator float () const noexcept;

    private:

        float value_ = 0.0f;
    };
}

#include "solosnake/blue/blue_radians.inl"
#endif
