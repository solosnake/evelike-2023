#ifndef blue_to_from_json_hpp
#define blue_to_from_json_hpp

#include <cstdint>
#include <memory>
#include "solosnake/nlohmann/json.hpp"

namespace blue
{
    // Classes with default ctors:

    class Bgra;
    void to_json(nlohmann::json&, const Bgra&);
    void from_json(const nlohmann::json&, Bgra&);

    class Hex_grid;
    void to_json(nlohmann::json&, const Hex_grid&);
    void from_json(const nlohmann::json&, Hex_grid&);

    class Game_hex_grid;
    void to_json(nlohmann::json&, const Game_hex_grid&);
    void from_json(const nlohmann::json&, Game_hex_grid&);

    class Machine_name;
    void to_json(nlohmann::json&, const Machine_name&);
    void from_json(const nlohmann::json&, Machine_name&);

    struct Machine_template;
    void to_json(nlohmann::json&, const Machine_template&);
    void from_json(const nlohmann::json&, Machine_template&);

    class Machine;
    void from_json(const nlohmann::json&, std::unique_ptr<Machine>&);

    class Chassis;
    void to_json(nlohmann::json&, const Chassis&);
    void from_json(const nlohmann::json&, Chassis&);

    struct Emissive_channels;
    void to_json(nlohmann::json&, const Emissive_channels&);
    void from_json(const nlohmann::json&, Emissive_channels&);

    struct AttachPoint;
    void to_json(nlohmann::json&, const AttachPoint&);
    void from_json(const nlohmann::json&, AttachPoint&);

    struct Thruster_attachpoint;
    void to_json(nlohmann::json&, const Thruster_attachpoint&);
    void from_json(const nlohmann::json&, Thruster_attachpoint&);

    enum ComponentCategory : std::int32_t;
    void to_json(nlohmann::json&, const ComponentCategory&);
    void from_json(const nlohmann::json&, ComponentCategory&);

    enum ComponentOrientation : std::int8_t;
    void to_json(nlohmann::json&, const ComponentOrientation&);
    void from_json(const nlohmann::json&, ComponentOrientation&);

    struct Hardpoint_shot;
    void to_json(nlohmann::json&, const Hardpoint_shot&);
    void from_json(const nlohmann::json&, Hardpoint_shot&);

    class Hardware_modifier;
    void to_json(nlohmann::json&, const Hardware_modifier&);
    void from_json(const nlohmann::json&, Hardware_modifier&);

    class Hardware_factor;
    void to_json(nlohmann::json&, const Hardware_factor&);
    void from_json(const nlohmann::json&, Hardware_factor&);

    class Tradables;
    void to_json(nlohmann::json&, const Tradables&);
    void from_json(const nlohmann::json&, Tradables&);

    class Amount;
    void to_json(nlohmann::json&, const Amount&);
    void from_json(const nlohmann::json&, Amount&);

    class Instructions;
    void to_json(nlohmann::json&, const Instructions&);
    void from_json(const nlohmann::json&, Instructions&);

    // Classes without default ctors:

    class Board_state;
    void to_json(nlohmann::json&, const Board_state&);
    Board_state boardstate_from_json(const nlohmann::json&);

    class Hardpoint_shooter;
    void to_json(nlohmann::json&, const Hardpoint_shooter&);
    Hardpoint_shooter hardpointshooter_from_json(const nlohmann::json&);

    class Blueprint;
    void to_json(nlohmann::json&, const Blueprint&);
    Blueprint blueprint_from_json(const nlohmann::json&);

    class Component;
    void to_json(nlohmann::json&, const Component&);
    Component component_from_json(const nlohmann::json&);

    class Softpoint;
    void to_json(nlohmann::json&, const Softpoint&);
    Softpoint softpoint_from_json(const nlohmann::json&);

    class Hardpoint;
    void to_json(nlohmann::json&, const Hardpoint&);
    Hardpoint hardpoint_from_json(const nlohmann::json&);

    class Oriented_hardpoint;
    void to_json(nlohmann::json&, const Oriented_hardpoint&);
    Oriented_hardpoint orientedhardpoint_from_json(const nlohmann::json&);

    class Thruster;
    void to_json(nlohmann::json&, const Thruster&);
    Thruster thruster_from_json(const nlohmann::json&);
}

namespace nlohmann
{
    template <>
    struct adl_serializer<blue::Board_state>
    {
        static blue::Board_state from_json(const json& j);

        static void to_json(json& j, const blue::Board_state& b)
        {
            blue::to_json(j, b);
        }
    };

    template <>
    struct adl_serializer<blue::Hardpoint_shooter>
    {
        static blue::Hardpoint_shooter from_json(const json& j);

        static void to_json(json& j, const blue::Hardpoint_shooter& b)
        {
            blue::to_json(j, b);
        }
    };

    template <>
    struct adl_serializer<blue::Blueprint>
    {
        static blue::Blueprint from_json(const json& j);

        static void to_json(json& j, const blue::Blueprint& b)
        {
            blue::to_json(j, b);
        }
    };

    template <>
    struct adl_serializer<blue::Component>
    {
        static blue::Component from_json(const json& j);

        static void to_json(json& j, const blue::Component& b)
        {
            blue::to_json(j, b);
        }
    };

    template <>
    struct adl_serializer<blue::Softpoint>
    {
        static blue::Softpoint from_json(const json& j);

        static void to_json(json& j, const blue::Softpoint& b)
        {
            blue::to_json(j, b);
        }
    };

    template <>
    struct adl_serializer<blue::Hardpoint>
    {
        static blue::Hardpoint from_json(const json& j);

        static void to_json(json& j, const blue::Hardpoint& b)
        {
            blue::to_json(j, b);
        }
    };

    template <>
    struct adl_serializer<blue::Oriented_hardpoint>
    {
        static blue::Oriented_hardpoint from_json(const json& j);

        static void to_json(json& j, const blue::Oriented_hardpoint& b)
        {
            blue::to_json(j, b);
        }
    };

    template <>
    struct adl_serializer<blue::Thruster>
    {
        static blue::Thruster from_json(const json& j);

        static void to_json(json& j, const blue::Thruster& b)
        {
            blue::to_json(j, b);
        }
    };
}

#endif
