#include "solosnake/blue/blue_machine_template.hpp"

namespace blue
{
    namespace
    {
        bool different_contents(const std::shared_ptr<Blueprint>& lhs,
                                const std::shared_ptr<Blueprint>& rhs) noexcept
        {
            return *lhs != *rhs;
        }

        bool same_contents(const std::vector<std::shared_ptr<Blueprint>>& lhs,
                           const std::vector<std::shared_ptr<Blueprint>>& rhs) noexcept
        {
            if( lhs.size() == rhs.size())
            {
                for(size_t i=0u; i < lhs.size(); ++i)
                {
                    if(different_contents(lhs[i], rhs[i]))
                    {
                        return false;
                    }
                }

                return true;
            }
            else
            {
                return false;
            }
        }
    }

    bool Machine_template::operator == (const Machine_template& rhs) const noexcept
    {
        return machines_name == rhs.machines_name
            && pulse_period == rhs.pulse_period
            && hue_shift == rhs.hue_shift
            && boot_code == rhs.boot_code
            && same_contents(known_blueprints, rhs.known_blueprints)
            && (*machines_blueprint) == (*rhs.machines_blueprint)
            ;
    }
}