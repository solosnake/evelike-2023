#ifndef SOLOSNAKE_EVELIKE_UNIVERSE_HPP
#define SOLOSNAKE_EVELIKE_UNIVERSE_HPP

#include <cstdint>
#include <vector>
#include "solosnake/maths.hpp"
#include "solosnake/evelike-solarsystem.hpp"
#include "solosnake/evelike-universe-settings.hpp"

namespace solosnake::evelike
{
    // Forward declarations.
    class Universe_event;

    /// Universe object. A Universe starts at frame zero, meaning that it is
    /// has not been started (never advanced). If the frame number is non-zero
    /// the Universe is considered to be started, and some operations on it are
    /// no longer permitted.
    class Universe
    {
    public:

        Universe() = default;

        explicit Universe(const char* filename);

        /// Saves the Universe to the named file, overwriting it if it exists.
        /// Throws if the file could not be saved to disk.
        void save(const char* filename) const;

        /// Returns the Universe's frame number. This is initially zero.
        std::uint64_t frame_number() const noexcept;

        /// Advances the Universe state `n` times, and optionally accumulates
        /// any events generated into the user supplied vector of events.
        /// The events are appended to the vector in order of occurrence, with
        /// most recent events at the back of the vector (highest index).
        void advance_n_frames(std::uint64_t n,
                              std::vector<Universe_event>* events = nullptr);

        /// Returns true only if this Universe has never been advanced.
        bool is_unstarted() const noexcept;

        /// Adds n random solar systems to the Universe, so long as the Universe
        /// is unstarted. If the Universe has been started this will throw
        /// a std::runtime_error.
        void add_random_solar_systems(std::uint32_t n, std::uint32_t seed);

        /// Returns the centre of the Universe.
        Point3d get_centre() const noexcept;

        /// Returns the radius of the Universe: all the solar-systems (as
        /// spheres) are contained within a sphere of this radius.
        float get_radius() const noexcept;

        /// Returns the number of solar systems in the Universe.
        std::size_t get_solar_system_count() const noexcept;

        /// Returns the centre of the indexed solar system.
        Point3d get_solar_system_centre(unsigned int) const;

        /// Returns the radius of the indexed solar system.
        float get_solar_system_radius(unsigned int) const;

        bool operator == (const Universe&) const noexcept;
        bool operator != (const Universe&) const noexcept;

    private:

        friend class serialize;

        void advance_frame( std::vector<Universe_event>* );

        void recalculate_centre();

    private:

        Universe_settings           m_settings;
        std::uint64_t               m_frame_number      {0u};
        std::vector<SolarSystem>    m_solar_systems;
        Point3d                     m_universe_centre   { 0.0f, 0.0f, 0.0f };
        float                       m_universe_radius   {0.0f};
    };
}

#endif
