#ifndef SOLOSNAKE_EVELIKE_SOLARSYSTEM_HPP
#define SOLOSNAKE_EVELIKE_SOLARSYSTEM_HPP

#include <array>
#include <cstdint>
#include <vector>
#include "solosnake/maths.hpp"

namespace solosnake::evelike
{
    enum class RingType : std::uint8_t
    {
        Empty, Sun, Planet, LaGrangePoint, AsteroidBelt
    };

    enum PlanetType : std::uint8_t
    {
        Rocky, GasGiant
    };

    enum SunType : std::uint8_t
    {
        MainSequence, RedGiant, WhiteDwarf

    };

    enum LaGrangePointType : std::uint8_t
    {
        EmptyLaGrangePoint, StationPoint, GatePoint
    };

    enum AsteroidType : std::uint8_t
    {
    };

    enum CelestialSize : std::uint8_t
    {
        Small, Medium, Large
    };

    struct Planet
    {
        PlanetType          planet_type;
        CelestialSize       planet_size;
    };

    struct Sun
    {
        SunType             sun_type;
        CelestialSize       sun_size;
    };

    struct LaGrangePoint
    {
        LaGrangePointType   la_grange_type;
        std::uint8_t        index;
    };

    struct AsteroidBelt
    {
        AsteroidType        asteroid_type;
        std::uint8_t        index;
    };

    union RingContent
    {
        Sun                 sun;
        Planet              planet;
        LaGrangePoint       la_grange_point;
        AsteroidBelt        asteroids;
    };

    struct Ring
    {
        RingType            ring_type { RingType::Sun };
        RingContent         contents;
        std::uint16_t       frames_for_one_degree { 0u };
    };

    struct Station
    {
    };

    struct StarGate
    {
        std::uint16_t   other_star_system { 0u };
    };

    bool operator==(const Sun&,             const Sun&) noexcept;
    bool operator==(const Planet&,          const Planet&) noexcept;
    bool operator==(const LaGrangePoint&,   const LaGrangePoint&) noexcept;
    bool operator==(const AsteroidBelt&,    const AsteroidBelt&) noexcept;
    bool operator==(const Ring&,            const Ring&) noexcept;
    bool operator==(const Station&,         const Station&) noexcept;
    bool operator==(const StarGate&,        const StarGate&) noexcept;

    /// The solar systems centre point and radius are expressed in the same units.
    class SolarSystem
    {
    public:
        SolarSystem() = default;

        Point3d         get_centre() const noexcept;

        void            translate_centre(const Point3d&) noexcept;

        std::size_t     get_rings_count() const noexcept;

        float           get_radius() const noexcept;

        const Ring*     get_ring(std::size_t n) const noexcept;

        void            add_sun(const Sun&, std::uint16_t rate) noexcept;

        void            add_planet(const Planet&, std::uint16_t rate) noexcept;

        void            add_empty_la_grange_point(std::uint16_t rate) noexcept;

        void            add_asteroid_belt(const AsteroidType&, std::uint16_t rate) noexcept;

        bool operator == (const SolarSystem&) const noexcept;
        bool operator != (const SolarSystem&) const noexcept;

    private:
        friend class serialize;

        void validate();

    private:

        Point3d                 m_centre { 0.0f, 0.0f, 0.0f };
        std::vector<Ring>       m_rings;
        std::vector<StarGate>   m_stargates;
        std::vector<Station>    m_stations;
    };
}
#endif