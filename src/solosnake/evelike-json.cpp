#include <fstream>
#include <stdexcept>
#include <sstream>
#include "solosnake/nlohmann/json.hpp"
#include "solosnake/evelike-json.hpp"
#include "solosnake/evelike-universe.hpp"
#include "solosnake/evelike-solarsystem.hpp"

#define EVELIKE_JSON_SERIALIZE_CALLS(NAME)\
            void to_json(nlohmann::json& j, const NAME& x)   { serialize::to_json(j, x); } \
            void from_json(const nlohmann::json& j, NAME& x) { serialize::from_json(j, x); }

#define EVELIKE_JSON_SERIALIZE_DECL(NAME)\
        static void to_json(nlohmann::json& j, const NAME& x); \
        static void from_json(const nlohmann::json&, NAME&)

namespace solosnake
{
    void to_json(nlohmann::json& j, const Point3d& x)
    {
        j = nlohmann::json{ {"x", x.x}, {"y", x.y}, {"z", x.z} };
    }

    void from_json(const nlohmann::json& j, Point3d& x)
    {
        Point3d p;
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
        j.at("z").get_to(p.z);
        x = p;
    }
}

namespace solosnake::evelike
{
    //--------------------------------------------------------------------------

    class serialize
    {
    public:
        EVELIKE_JSON_SERIALIZE_DECL( SolarSystem );
        EVELIKE_JSON_SERIALIZE_DECL( Universe );
        EVELIKE_JSON_SERIALIZE_DECL( Universe_settings );
        EVELIKE_JSON_SERIALIZE_DECL( Point3d );
        EVELIKE_JSON_SERIALIZE_DECL( Sun );
        EVELIKE_JSON_SERIALIZE_DECL( Planet );
        EVELIKE_JSON_SERIALIZE_DECL( LaGrangePoint );
        EVELIKE_JSON_SERIALIZE_DECL( AsteroidBelt );
        EVELIKE_JSON_SERIALIZE_DECL( Ring );
        EVELIKE_JSON_SERIALIZE_DECL( Station );
        EVELIKE_JSON_SERIALIZE_DECL( StarGate );
    };

    EVELIKE_JSON_SERIALIZE_CALLS( SolarSystem )
    EVELIKE_JSON_SERIALIZE_CALLS( Universe )
    EVELIKE_JSON_SERIALIZE_CALLS( Universe_settings )
    EVELIKE_JSON_SERIALIZE_CALLS( Sun )
    EVELIKE_JSON_SERIALIZE_CALLS( Planet )
    EVELIKE_JSON_SERIALIZE_CALLS( LaGrangePoint )
    EVELIKE_JSON_SERIALIZE_CALLS( AsteroidBelt )
    EVELIKE_JSON_SERIALIZE_CALLS( Ring )
    EVELIKE_JSON_SERIALIZE_CALLS( Station )
    EVELIKE_JSON_SERIALIZE_CALLS( StarGate )

    //--------------------------------------------------------------------------

    void save_to_json(const Universe& u, const char* filename)
    {
        if(filename)
        {
            std::ofstream file(filename);
            const nlohmann::json j = u;
#ifndef NDEBUG
            file << j.dump(4);
#else
            file << j;
#endif
        }
    }

    Universe load_from_json(const char* filename)
    {
        std::ifstream file(filename);
        if (file)
        {
            auto j = nlohmann::json::parse(file);
            return j.get<Universe>();
        }
        else
        {
            std::stringstream ss;
            ss << "Failed to open Universe JSON: '" << filename << "'.";
            throw std::runtime_error(ss.str());
        }
    }

    //--------------------------------------------------------------------------

    void serialize::to_json(nlohmann::json& j, const Sun& x)
    {
        j = nlohmann::json{
            {"size",        static_cast<std::uint8_t>(x.sun_size)},
            {"sun-type",    static_cast<std::uint8_t>(x.sun_type)}
        };
    }

    void serialize::from_json(const nlohmann::json& j, Sun& x)
    {
        Sun s;
        j.at("size").get_to(s.sun_size);
        j.at("sun-type").get_to(s.sun_type);
        x = s;
    }

    void serialize::to_json(nlohmann::json& j, const Planet& x)
    {
        j = nlohmann::json{
            {"size",        static_cast<std::uint8_t>(x.planet_size)},
            {"planet-type", static_cast<std::uint8_t>(x.planet_type)}
        };
    }

    void serialize::from_json(const nlohmann::json& j, Planet& x)
    {
        Planet s;
        j.at("size").get_to(s.planet_size);
        j.at("planet-type").get_to(s.planet_type);
        x = s;
    }

    void serialize::to_json(nlohmann::json& j, const LaGrangePoint& x)
    {
        j = nlohmann::json{
            {"lgp-type",    static_cast<std::uint8_t>(x.la_grange_type)},
            {"index",       static_cast<std::uint8_t>(x.index)}
        };
    }

    void serialize::from_json(const nlohmann::json& j, LaGrangePoint& x)
    {
        LaGrangePoint s;
        j.at("lgp-type").get_to(s.la_grange_type);
        j.at("index").get_to(s.index);
        x = s;
    }

    void serialize::to_json(nlohmann::json& j, const AsteroidBelt& x)
    {
        j = nlohmann::json{
            {"asteroid-type",   static_cast<std::uint8_t>(x.asteroid_type)},
            {"index",           static_cast<std::uint8_t>(x.index)}
        };
    }

    void serialize::from_json(const nlohmann::json& j, AsteroidBelt& x)
    {
        AsteroidBelt s;
        j.at("asteroid-type").get_to(s.asteroid_type);
        j.at("index").get_to(s.index);
        x = s;
    }

    void serialize::to_json(nlohmann::json& j, const Ring& x)
    {
        j = nlohmann::json{
            {"ring-type",   static_cast<std::uint8_t>(x.ring_type)},
            {"one-degree",  static_cast<std::uint8_t>(x.frames_for_one_degree)}
        };

        switch(x.ring_type)
        {
            case RingType::Empty:
                j["contents"] = nullptr;
                break;

            case RingType::Sun:
                j["contents"] = x.contents.sun;
                break;

            case RingType::Planet:
                j["contents"] = x.contents.planet;
                break;

            case RingType::LaGrangePoint:
                j["contents"] = x.contents.la_grange_point;
                break;

            case RingType::AsteroidBelt:
                j["contents"] = x.contents.asteroids;
                break;

            default:
                break;
        }
    }

    void serialize::from_json(const nlohmann::json& j, Ring& x)
    {
        Ring s;
        j.at("ring-type").get_to(s.ring_type);
        j.at("one-degree").get_to(s.frames_for_one_degree);

        switch(s.ring_type)
        {
            case RingType::Empty:
                break;

            case RingType::Sun:
                j.at("contents").get_to(s.contents.sun);
                break;

            case RingType::Planet:
                j.at("contents").get_to(s.contents.planet);
                break;

            case RingType::LaGrangePoint:
                j.at("contents").get_to(s.contents.la_grange_point);
                break;

            case RingType::AsteroidBelt:
                j.at("contents").get_to(s.contents.asteroids);
                break;

            default:
                throw std::runtime_error("Bad Ring type.");
        }

        x = s;
    }

    void serialize::to_json(nlohmann::json& j, const Station& )
    {
        j = nlohmann::json{
        };
    }

    void serialize::from_json(const nlohmann::json&, Station& x)
    {
        Station s;
        x = s;
    }

    void serialize::to_json(nlohmann::json& j, const StarGate& x)
    {
        j = nlohmann::json{
            {"other-star-system", x.other_star_system }
        };
    }

    void serialize::from_json(const nlohmann::json& j, StarGate& x)
    {
        StarGate s;
        j.at("other-star-system").get_to(s.other_star_system);
        x = s;
    }

    void serialize::to_json(nlohmann::json& j, const SolarSystem& x)
    {
        j = nlohmann::json{
            {"centre",      x.m_centre},
           // {"rings",       x.m_rings},
            {"stargates",   x.m_stargates},
            {"stations",    x.m_stations},
        };
    }

    void serialize::from_json(const nlohmann::json& j, SolarSystem& x)
    {
        SolarSystem s;
        j.at("centre").get_to(s.m_centre);
        //j.at("rings").get_to(s.m_rings);
        j.at("stargates").get_to(s.m_stargates);
        j.at("stations").get_to(s.m_stations);
       // x.validate();
        std::swap(s,x);
    }

    void serialize::to_json(nlohmann::json& j, const Universe& x)
    {
        j = nlohmann::json{
            {"settings",      x.m_settings},
            {"frame-number",  x.m_frame_number},
            {"solar-systems", x.m_solar_systems}
        };
    }

    void serialize::from_json(const nlohmann::json& j, Universe& x)
    {
        Universe u;
        j.at("settings").get_to(u.m_settings);
        j.at("frame-number").get_to(u.m_frame_number);
        j.at("solar-systems").get_to(u.m_solar_systems);
        u.recalculate_centre();
        std::swap(u,x);
    }

    void serialize::to_json(nlohmann::json& j, const Universe_settings& )
    {
        j = nlohmann::json{
        };
    }

    void serialize::from_json(const nlohmann::json& , Universe_settings& x)
    {
        Universe_settings u;
        std::swap(u,x);
    }
}