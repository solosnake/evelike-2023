#ifndef SOLOSNAKE_EVELIKE_JSON_HPP
#define SOLOSNAKE_EVELIKE_JSON_HPP

#include "solosnake/nlohmann/json_fwd.hpp"

namespace solosnake::evelike
{
    class SolarSystem;
    void to_json(nlohmann::json&, const SolarSystem&);
    void from_json(const nlohmann::json&, SolarSystem&);

    class Universe;
    void to_json(nlohmann::json&, const Universe&);
    void from_json(const nlohmann::json&, Universe&);
    void save_to_json(const Universe&, const char*);
    Universe load_from_json(const char*);

    class Universe_settings;
    void to_json(nlohmann::json&, const Universe_settings&);
    void from_json(const nlohmann::json&, Universe_settings&);

}
#endif
