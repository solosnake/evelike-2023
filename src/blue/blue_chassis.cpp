#include "solosnake/blue/blue_chassis.hpp"
#include "solosnake/blue/blue_machine_readout.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    Chassis::Chassis()
        : chassis_name_()
        , channels_()
    {
        // NOP: Ctor used only by JSON.
    }

    Chassis::Chassis(const std::string_view& name,
                     const Emissive_channels& channels)
        : chassis_name_(name)
        , channels_(channels)
    {
    }

    const std::string& Chassis::name() const
    {
        return chassis_name_;
    }

    const Emissive_channels& Chassis::channels() const
    {
        return channels_;
    }

    bool Chassis::operator == (const Chassis& rhs) const noexcept
    {
        return chassis_name_ == rhs.chassis_name_ && channels_ == rhs.channels_;
    }

    bool Chassis::operator != (const Chassis& rhs) const noexcept
    {
        return chassis_name_ != rhs.chassis_name_ || channels_ != rhs.channels_;
    }

}
