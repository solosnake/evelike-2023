#ifndef solosnake_power_of_2_hpp
#define solosnake_power_of_2_hpp

namespace solosnake
{
    //! Class which guarantees its internal values is a power of 2.
    //! Zero is no considered a valid power of 2 and so the minimum
    //! value is 1 (2 to the power of zero).
    class power_of_2
    {
    public:
        explicit power_of_2( unsigned int i );

        unsigned int value() const;
        operator unsigned int() const;

        bool operator==( const power_of_2& rhs ) const;
        bool operator!=( const power_of_2& rhs ) const;
        bool operator<=( const power_of_2& rhs ) const;
        bool operator>=( const power_of_2& rhs ) const;
        bool operator<( const power_of_2& rhs ) const;
        bool operator>( const power_of_2& rhs ) const;

        bool operator==( const unsigned int rhs ) const;
        bool operator!=( const unsigned int rhs ) const;
        bool operator<=( const unsigned int rhs ) const;
        bool operator>=( const unsigned int rhs ) const;
        bool operator<( const unsigned int rhs ) const;
        bool operator>( const unsigned int rhs ) const;

    private:
        unsigned int value_;
    };

    //-------------------------------------------------------------------------/

    inline unsigned int power_of_2::value() const
    {
        return value_;
    }

    inline power_of_2::operator unsigned int() const
    {
        return value_;
    }

    inline bool power_of_2::operator==( const power_of_2& rhs ) const
    {
        return value_ == rhs.value_;
    }

    inline bool power_of_2::operator!=( const power_of_2& rhs ) const
    {
        return value_ != rhs.value_;
    }

    inline bool power_of_2::operator<=( const power_of_2& rhs ) const
    {
        return value_ <= rhs.value_;
    }

    inline bool power_of_2::operator>=( const power_of_2& rhs ) const
    {
        return value_ >= rhs.value_;
    }

    inline bool power_of_2::operator<( const power_of_2& rhs ) const
    {
        return value_ < rhs.value_;
    }

    inline bool power_of_2::operator>( const power_of_2& rhs ) const
    {
        return value_ > rhs.value_;
    }

    inline bool power_of_2::operator==( const unsigned int rhs ) const
    {
        return value_ == rhs;
    }

    inline bool power_of_2::operator!=( const unsigned int rhs ) const
    {
        return value_ != rhs;
    }

    inline bool power_of_2::operator<=( const unsigned int rhs ) const
    {
        return value_ <= rhs;
    }

    inline bool power_of_2::operator>=( const unsigned int rhs ) const
    {
        return value_ >= rhs;
    }

    inline bool power_of_2::operator<( const unsigned int rhs ) const
    {
        return value_ < rhs;
    }

    inline bool power_of_2::operator>( const unsigned int rhs ) const
    {
        return value_ > rhs;
    }
}

#endif
