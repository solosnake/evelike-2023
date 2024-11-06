#ifndef blue_emissive_channels_hpp
#define blue_emissive_channels_hpp

#include <cstdint>
#include "solosnake/blue/blue_bgra.hpp"

namespace blue
{
    /// Default channels are One and with white colours.
    struct Emissive_channels
    {
        Emissive_channels();

        /// There are two emissive channels in the model textures. These
        /// are 8 bit grey channels which are coloured by these colours here.
        /// At present the alpha channel has no input to the final colours.
        Bgra emissive_colours[2];

        /// Each colour is multiplied by two floats to modulate it. The values
        /// of these floats is determined by the MachineReadouts which
        /// correspond to these values. The valid ranges of these values is the
        /// MachineReadouts enums.
        /// emissive_readouts[0][0] and [0][1] are applied to channel 0,
        /// and [1][0] and [1][1] are applied to channel 1.
        std::uint8_t emissive_readouts[2][2];

        bool operator == (const Emissive_channels& rhs) const noexcept;
        bool operator != (const Emissive_channels& rhs) const noexcept;
    };
}

#endif
