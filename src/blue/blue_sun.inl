namespace blue
{
    inline Hex_coord Sun::board_xy() const
    {
        return xy_;
    }

    inline SunStrength Sun::sun_strength() const
    {
        return static_cast<SunStrength>( strength_ );
    }

    inline SunType Sun::sun_type() const
    {
        return type_;
    }

    inline bool Sun::operator== ( const Sun& rhs ) const
    {
        return ( xy_ == rhs.xy_ ) && ( strength_ == rhs.strength_ ) && ( type_ == rhs.type_ );
    }

    inline bool Sun::operator!= ( const Sun& rhs ) const
    {
        return !( *this == rhs );
    }
}
