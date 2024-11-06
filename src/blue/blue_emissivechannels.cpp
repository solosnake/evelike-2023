#include "solosnake/blue/blue_emissive_channels.hpp"
#include "solosnake/blue/blue_machine_readout.hpp"

namespace blue
{
    Emissive_channels::Emissive_channels()
    {
        emissive_colours[0] = emissive_colours[1] = Bgra( 255u, 255u, 255u, 255u );
        emissive_readouts[0][0] = emissive_readouts[0][1] = ReadoutOne;
        emissive_readouts[1][0] = emissive_readouts[1][1] = ReadoutOne;
    }

    bool Emissive_channels::operator == (const Emissive_channels& rhs) const noexcept
    {
        return  emissive_colours[0] == rhs.emissive_colours[0]
            &&  emissive_colours[1] == rhs.emissive_colours[1]
            &&  emissive_readouts[0][0] == rhs.emissive_readouts[0][0]
            &&  emissive_readouts[0][1] == rhs.emissive_readouts[0][1]
            &&  emissive_readouts[1][0] == rhs.emissive_readouts[1][0]
            &&  emissive_readouts[1][1] == rhs.emissive_readouts[1][1]
            ;
    }

    bool Emissive_channels::operator != (const Emissive_channels& rhs) const noexcept
    {
        return ! ((*this) == rhs);
    }
}
