#include <limits>
#include <random>
#include "solosnake/evelike-json.hpp"
#include "solosnake/evelike-universe.hpp"
#include "solosnake/evelike-universe-event.hpp"

namespace solosnake::evelike
{
    namespace
    {
        SolarSystem generate_random_solar_system( std::mt19937& random )
        {
            std::uniform_int_distribution<> random_coord(-1000, 1000);
            float x = static_cast<float>(random_coord(random));
            float y = static_cast<float>(random_coord(random));
            float z = static_cast<float>(random_coord(random));
            SolarSystem s;
            s.translate_centre( Point3d{x,y,z} );
            return s;
        }
    }

    Universe::Universe(const char* filename)
    {
        *this = load_from_json(filename);
    }

    void Universe::save(const char* filename) const
    {
        save_to_json(*this, filename);
    }

    bool Universe::is_unstarted() const noexcept
    {
        return 0u == m_frame_number;
    }

    void Universe::add_random_solar_systems(const std::uint32_t n,
                                            const std::uint32_t seed)
    {
        if(n > 0u)
        {
            constexpr std::uint64_t mx{ std::numeric_limits<std::uint16_t>::max() };

            // See if we could safely add n to the current count. We do not
            // permit more than 65535 solar systems due to index restrictions
            // when drawing etc.
            if( (m_solar_systems.size() + n) <= mx )
            {
                // Allocated expected memory.
                m_solar_systems.reserve( m_solar_systems.size() + n );

                // Prepare random number generator.
                std::mt19937 gen;
                gen.seed(seed);

                for(unsigned i = 0u; i < n; ++i)
                {
                    auto ss = generate_random_solar_system(gen);

                    if(not m_solar_systems.empty())
                    {
                        unsigned int modify_index = 0u;

                        if(m_solar_systems.size() > 1u)
                        {
                            // Distribution is inclusive of range ends.
                            auto upper_index = static_cast<int>(m_solar_systems.size() - 1u);
                            std::uniform_int_distribution<> random_index(0, upper_index);
                            modify_index = random_index(gen);
                        }

                        // Attempt to make random new system sister system to
                        // the one at `modify_index`:
                    }

                    m_solar_systems.push_back( std::move(ss) );
                }
            }
            else
            {
                throw std::runtime_error("Too many solar systems.");
            }

            assert( m_solar_systems.size() <= 65535u );

            recalculate_centre();
        }
    }

    Point3d Universe::get_centre() const noexcept
    {
        return m_universe_centre;
    }

    float Universe::get_radius() const noexcept
    {
        return m_universe_radius;
    }

    std::size_t Universe::get_solar_system_count() const noexcept
    {
        return m_solar_systems.size();
    }

    Point3d Universe::get_solar_system_centre(const unsigned int i) const
    {
        return m_solar_systems.at(i).get_centre();
    }

    /// Returns the radius of the indexed solar system.
    float Universe::get_solar_system_radius(unsigned int i) const
    {
        return m_solar_systems.at(i).get_radius();
    }

    void Universe::recalculate_centre()
    {
        Point3d mins { std::numeric_limits<float>::max(),
                       std::numeric_limits<float>::max(),
                       std::numeric_limits<float>::max() };
        Point3d maxs { std::numeric_limits<float>::lowest(),
                       std::numeric_limits<float>::lowest(),
                       std::numeric_limits<float>::lowest() };

        // Get min and max of this solar system.
        for(const auto& s: m_solar_systems)
        {
            auto xyz = s.get_centre();
            auto r   = s.get_radius();
            mins.x   = std::min<float>( xyz.x - r, mins.x );
            mins.y   = std::min<float>( xyz.y - r, mins.y );
            mins.z   = std::min<float>( xyz.z - r, mins.z );
            maxs.x   = std::max<float>( xyz.x + r, maxs.x );
            maxs.y   = std::max<float>( xyz.y + r, maxs.y );
            maxs.z   = std::max<float>( xyz.z + r, maxs.z );
        }

        const auto rx = maxs.x - mins.x;
        const auto ry = maxs.y - mins.y;
        const auto rz = maxs.z - mins.z;

        // Calculate centre of the new cloud of solar systems.
        const Point3d centre{ 0.5f * rx + mins.x,
                              0.5f * ry + mins.y,
                              0.5f * rz + mins.z };

        // Find universe radius that encloses all the solar systems.
        auto universe_radius = 0.0f;
        for(const auto& s: m_solar_systems)
        {
            auto d = s.get_radius() + distance(centre, s.get_centre());
            universe_radius = universe_radius < d ? d : universe_radius;
        }

        // Store results.
        m_universe_centre = centre;
        m_universe_radius = universe_radius;
    }

    void Universe::advance_n_frames( const std::uint64_t n, std::vector<Universe_event>* e)
    {
        if(e)
        {
            e->clear();
        }

        if( m_frame_number > (std::numeric_limits<std::uint64_t>::max() - n) )
        {
            // We cant advance to this as it will overflow uint64.
            throw std::runtime_error("Advance would overflow Universe frame counter.");
        }

        for(std::uint64_t i=0u; i < n; ++i)
        {
            advance_frame(e);
        }
    }

    void Universe::advance_frame( std::vector<Universe_event>* )
    {
        ++m_frame_number;
    }

    std::uint64_t Universe::frame_number() const noexcept
    {
        return m_frame_number;
    }

    bool Universe::operator == (const Universe& rhs) const noexcept
    {
        return m_solar_systems == rhs.m_solar_systems;
    }

    bool Universe::operator != (const Universe& rhs) const noexcept
    {
        return not (*this == rhs);
    }
}
