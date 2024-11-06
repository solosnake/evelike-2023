#ifndef blue_machine_template_hpp
#define blue_machine_template_hpp

#include <cstdint>
#include <string>
#include <memory>

#include "solosnake/blue/blue_blueprint.hpp"
#include "solosnake/blue/blue_machine_name.hpp"
#include "solosnake/blue/blue_instructions.hpp"

namespace blue
{
    /// pulse_period: Used in visual effects for pulsing emissives: better
    /// Hardware has faster pulsing effects.
    struct Machine_template
    {
        Machine_name                            machines_name;
        std::shared_ptr<Blueprint>              machines_blueprint;
        std::vector<std::shared_ptr<Blueprint>> known_blueprints;
        Instructions                            boot_code;
        unsigned int                            pulse_period;
        std::uint8_t                            hue_shift;

        bool operator == (const Machine_template&) const noexcept;
    };
}

#endif