#ifndef blue_componentorientation_hpp
#define blue_componentorientation_hpp

#include <cstdint>
#include <string_view>

namespace blue
{
    enum ComponentOrientation : std::int8_t
    {
        Rotated0,
        Rotated90,
        Rotated180,
        Rotated270
    };

    std::string_view to_string(ComponentOrientation);

    ComponentOrientation component_orientation_from_string(const std::string_view&);
}

#endif
