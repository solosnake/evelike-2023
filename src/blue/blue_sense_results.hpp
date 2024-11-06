#ifndef blue_senseresults_hpp
#define blue_senseresults_hpp

#include <cstdint>
#include <vector>
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_hexcontentflags.hpp"
#include "solosnake/blue/blue_hexsenseresult.hpp"

namespace blue
{
    class Machine_grid_view;

    /// Captures the snapshot sense results of a machines scan.
    class Sense_results
    {
    public:

        Sense_results();

        void            reserve( const size_t n );

        bool            is_empty() const noexcept;

        Hex_coord       get_sense_origin() const noexcept;

        void            sense_surroundings( const Machine_grid_view,
                                            const Hex_coord origin,
                                            const std::uint16_t range );

        std::uint16_t   result_count() const noexcept;

        Hex_coord       result_location( const std::uint16_t n ) const;

        HexSenseResult  result_type( const std::uint16_t n ) const;

    private:
        std::vector<Hex_grid::Hex_tile> sensed_tiles_;
        std::vector<HexSenseResult>     sensed_contents_;
        Hex_coord                       sense_origin_;
    };
}

#include "solosnake/blue/blue_sense_results.inl"

#endif
