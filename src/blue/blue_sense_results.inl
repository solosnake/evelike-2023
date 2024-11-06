#include "solosnake/blue/blue_sense_results.hpp"

namespace blue
{
    inline Hex_coord Sense_results::get_sense_origin() const noexcept
    {
        return sense_origin_;
    }

    inline bool Sense_results::is_empty() const noexcept
    {
        return sensed_contents_.empty();
    }

    inline std::uint16_t Sense_results::result_count() const noexcept
    {
        return static_cast<std::uint16_t>(sensed_contents_.size());
    }

    inline HexSenseResult Sense_results::result_type(const uint16_t n) const
    {
        return sensed_contents_[n];
    }

    inline Hex_coord Sense_results::result_location(const uint16_t n) const
    {
        return sensed_tiles_[n].tile_coordinate;
    }
}
