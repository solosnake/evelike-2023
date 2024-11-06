#include <limits>
#include "solosnake/evelike-solarsystem.hpp"

namespace solosnake::evelike
{
    namespace
    {
        bool same_contents(const RingContent& lhs,
                           const RingContent& rhs,
                           const RingType ring_type)
        {
            switch(ring_type)
            {
                case RingType::Empty:
                    return true;

                case RingType::Sun:
                    return lhs.sun == rhs.sun;

                case RingType::Planet:
                    return lhs.planet == rhs.planet;

                case RingType::LaGrangePoint:
                    return lhs.la_grange_point == rhs.la_grange_point;

                case RingType::AsteroidBelt:
                    return lhs.asteroids == rhs.asteroids;

                default:
                    break;
            }

            return false;
        }
    }

    bool operator==(const Ring& lhs, const Ring& rhs) noexcept
    {
        return  lhs.frames_for_one_degree == rhs.frames_for_one_degree and
                lhs.ring_type == rhs.ring_type and
                same_contents(lhs.contents, rhs.contents, lhs.ring_type)
                ;
    }

    bool operator==(const Sun& lhs, const Sun& rhs) noexcept
    {
        return  lhs.sun_size == rhs.sun_size and
                lhs.sun_type == rhs.sun_type
                ;
    }

    bool operator==(const Planet& lhs, const Planet& rhs) noexcept
    {
        return  lhs.planet_size == rhs.planet_size and
                lhs.planet_type == rhs.planet_type
                ;
    }

    bool operator==(const LaGrangePoint& lhs, const LaGrangePoint& rhs) noexcept
    {
        return  lhs.index          == rhs.index and
                lhs.la_grange_type == rhs.la_grange_type
                ;
    }

    bool operator==(const AsteroidBelt& lhs, const AsteroidBelt& rhs) noexcept
    {
        return  lhs.index   == rhs.index and
                lhs.asteroid_type == rhs.asteroid_type
                ;
    }

    bool operator==(const Station&, const Station&) noexcept
    {
        return true;
    }

    bool operator==(const StarGate& lhs, const StarGate& rhs) noexcept
    {
        return lhs.other_star_system == rhs.other_star_system;
    }

    bool SolarSystem::operator == (const SolarSystem& rhs) const noexcept
    {
        return m_centre     == rhs.m_centre and
               m_rings      == rhs.m_rings and
               m_stargates  == rhs.m_stargates and
               m_stations   == rhs.m_stations
               ;
    }

    bool SolarSystem::operator != (const SolarSystem& rhs) const noexcept
    {
        return not (*this == rhs);
    }

    Point3d SolarSystem::get_centre() const noexcept
    {
        return m_centre;
    }

    float SolarSystem::get_radius() const noexcept
    {
        return 1.0f + static_cast<float>(m_rings.size());
    }

    std::size_t SolarSystem::get_rings_count() const noexcept
    {
        return m_rings.size();
    }

    const Ring* SolarSystem::get_ring(std::size_t n) const noexcept
    {
        return n < m_rings.size() ? &m_rings[n] : nullptr;
    }

    void SolarSystem::add_sun(const Sun& sun, std::uint16_t rate) noexcept
    {
        m_rings.push_back( Ring{ RingType::Sun, sun, rate } );
    }

    void SolarSystem::add_planet(const Planet& planet, std::uint16_t rate) noexcept
    {
        Ring r{};
        r.ring_type = RingType::Planet;
        r.contents.planet = planet;
        r.frames_for_one_degree = rate;
        m_rings.push_back( r );
    }

    void SolarSystem::add_empty_la_grange_point(std::uint16_t rate) noexcept
    {
        Ring r{};
        r.ring_type = RingType::LaGrangePoint;
        r.contents.la_grange_point.index = 0;
        r.contents.la_grange_point.la_grange_type = LaGrangePointType::EmptyLaGrangePoint;
        r.frames_for_one_degree = rate;
        m_rings.push_back( r );
    }

    void SolarSystem::add_asteroid_belt(const AsteroidType& a, std::uint16_t rate) noexcept
    {
        Ring r{};
        r.ring_type = RingType::AsteroidBelt;
        r.contents.asteroids.asteroid_type = a;
        r.contents.asteroids.index = 0;
        r.frames_for_one_degree = rate;
        m_rings.push_back( r );
    }

    void SolarSystem::translate_centre(const Point3d& t) noexcept
    {
        m_centre = m_centre + t;
    }
}