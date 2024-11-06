namespace blue
{
    inline Cargohold::Cargohold()
        : contents_()
        , volume_limit_( 0 )
        , locked_( false )
    {
    }

    inline Cargohold::Cargohold( const std::uint16_t volume_limit )
        : contents_()
        , volume_limit_( volume_limit )
        , locked_( false )
    {
    }

    inline std::uint16_t Cargohold::count_of( const std::uint16_t i ) const
    {
        return contents_[i];
    }

    inline std::uint16_t Cargohold::operator[]( const std::uint16_t i ) const
    {
        return contents_[i];
    }

    inline bool Cargohold::is_locked() const
    {
        return locked_;
    }

    inline void Cargohold::set_locked( bool lock )
    {
        locked_ = lock;
    }

    inline std::uint16_t Cargohold::total_volume() const
    {
        return volume_limit_;
    }

    inline std::uint16_t Cargohold::free_volume() const
    {
        return volume_limit_ - contents_.volume();
    }

    inline std::uint16_t Cargohold::used_volume() const
    {
        return total_volume() - free_volume();
    }

    inline bool Cargohold::has_capacity_for( const Tradables& amount ) const
    {
        return amount.volume() <= free_volume();
    }

    inline bool Cargohold::has_capacity_for( const Amount& a ) const
    {
        return a.volume() <= free_volume();
    }

    inline const Tradables& Cargohold::cargo_contents() const
    {
        return contents_;
    }

    inline bool Cargohold::contains_at_least( const Amount& a ) const
    {
        return contents_.can_afford( a );
    }
}
