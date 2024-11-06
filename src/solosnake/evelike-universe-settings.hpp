#ifndef SOLOSNAKE_EVELIKE_UNIVERSE_SETTINGS_HPP
#define SOLOSNAKE_EVELIKE_UNIVERSE_SETTINGS_HPP

namespace solosnake::evelike
{
    /// We permit Universes to have different restrictions. This class
    /// contains these restrictions.
    class Universe_settings
    {
    public:

        Universe_settings() = default;

        bool operator == (const Universe_settings&) const noexcept;
        bool operator != (const Universe_settings&) const noexcept;

    private:

        friend class serialize;

    private:

    };
}

#endif