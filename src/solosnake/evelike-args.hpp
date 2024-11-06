#ifndef SOLOSNAKE_EVELIKE_ARGS_HPP
#define SOLOSNAKE_EVELIKE_ARGS_HPP

#include <optional>
#include <string>

namespace solosnake::evelike
{
    /// A structure representing the interpretation of the
    /// command line arguments.
    struct Args
    {
        std::string     file_names[7];
        std::uint64_t   max_runtime_ms{ 0u };
        bool            show_model { false };
        bool            show_skybox { false };
        bool            convert_model {false };
        bool            convert_skybox { false };
        bool            debugging {false};
        bool            show_help{false};
    };

    /// Returns the help message for this application, using the given name.
    std::string get_help(const std::string& app_name);

    /// Returns the Args as an interpretation of the command line. Returns
    /// an empty optional if the command line is considered invalid.
    std::optional<Args> get_args(int argc, char** argv);

    /// Returns the application name based on the first command line argument.
    std::string get_app_name(int argc, char** argv);
}

#endif