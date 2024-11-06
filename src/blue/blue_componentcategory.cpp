#include "solosnake/blue/blue_componentcategory.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    std::string_view  to_string(ComponentCategory c)
    {
        switch(c)
        {
            case OffensiveComponent:
                return "OffensiveComponent";

            case DefensiveComponent:
                return "DefensiveComponent";

            case IndustrialComponent:
                return "IndustrialComponent";

            case PropulsionComponent:
                return "PropulsionComponent";

            case DualUseComponent:
                return "DualUseComponent";

            default:
                ss_throw("Invalid ComponentCategory enum");
        }
    }

    ComponentCategory component_category_from_string(const std::string_view& s)
    {
        if( s == "OffensiveComponent" )
        {
            return OffensiveComponent;
        }

        if( s == "DefensiveComponent" )
        {
            return DefensiveComponent;
        }

        if( s == "IndustrialComponent" )
        {
            return IndustrialComponent;
        }

        if( s == "PropulsionComponent" )
        {
            return PropulsionComponent;
        }

        if( s == "DualUseComponent" )
        {
            return DualUseComponent;
        }

        ss_throw("Not a ComponentCategory string.");
    }
}
