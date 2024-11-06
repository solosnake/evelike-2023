#include "solosnake/blue/blue_componentorientation.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    std::string_view to_string(ComponentOrientation c)
    {
        switch(c)
        {
            case Rotated0:
                return std::string_view("Rotated0");

            case Rotated90:
                return std::string_view("Rotated90");

            case Rotated180:
                return std::string_view("Rotated180");

            case Rotated270:
                return std::string_view("Rotated270");

            default:
                ss_throw("Unknown ComponentOrientation value.");
        }
    }

    ComponentOrientation component_orientation_from_string(const std::string_view& text)
    {
        if( text == std::string_view("Rotated0") )
        {
            return Rotated0;
        }

        if( text == std::string_view("Rotated90") )
        {
            return Rotated90;
        }

        if( text == std::string_view("Rotated180") )
        {
            return Rotated180;
        }

        if( text == std::string_view("Rotated270") )
        {
            return Rotated270;
        }

        ss_throw("Invalid ComponentOrientation text.");
    }
}
