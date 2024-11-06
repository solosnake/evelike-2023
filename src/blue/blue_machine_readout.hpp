#ifndef blue_machinereadouts_hpp
#define blue_machinereadouts_hpp

#include <cstdint>
#include <string_view>

namespace blue
{
    /// Enums used to query a Machine for values about its internal states.
    /// The internal state is usually returned as a value between 0.0 and 1.0
    /// although some states may be above 1.0.
    enum Machine_readout : std::uint16_t
    {
        ReadoutOne,
        ReadoutCPUBenchmark,
        Pulse0,
        Pulse1,
        ReadoutAdvanceSpeedLvl,
        ReadoutTurnSpeedLvl,
        ReadoutCapacitorLvl,
        ReadoutSensingLvl,
        ReadoutBroadcastingLvl,
        ReadoutTransmittingLvl,
        ReadoutBuildingLvl
    };

    /// The number of types of readout.
    constexpr std::uint32_t BLUE_MACHINE_READOUTS_COUNT{11u};

    /// Converts n to the name of a Machine readout.
    /// Throws if n is not a valid Machine_readout value.
    std::string_view machine_readout_to_string(const std::uint16_t n);

    /// Converts the name of the Machine_readout to a Machine_readout value.
    /// Throws if the text is not a valid Machine_readout.
    Machine_readout string_to_readout( const std::string_view& txt );

}

#endif
