#include <cstdint>
#include <iostream>
#include <filesystem>
#include <limits>
#include <set>
#include <string>
#include <sstream>
#include <vector>

#include "solosnake/evelike-args.hpp"

namespace solosnake::evelike
{
    namespace
    {
        struct Flags_and_args
        {
            std::vector<char>        flags;
            std::vector<std::string> args;
        };

        bool is_flag(const std::string& arg) noexcept
        {
            return not arg.empty() and '-' == arg[0];
        }

        bool not_a_number(const char x) noexcept
        {
            return x != '0' and
                   x != '1' and
                   x != '2' and
                   x != '3' and
                   x != '4' and
                   x != '5' and
                   x != '6' and
                   x != '7' and
                   x != '8' and
                   x != '9'
                   ;
        }

        bool contains(const std::vector<char>& args, const char text)
        {
            for(const auto& a : args)
            {
                if( a == text )
                {
                    return true;
                }
            }
            return false;
        }

        std::vector<char> split_flags(const std::string& flags)
        {
            std::vector<char> letters;
            for(auto i=1u; i<flags.length(); ++i)
            {
                letters.push_back(flags[i]);
            }
            return letters;
        }

        std::vector<std::string> get_vector_of_command_line_args(const int argc, char** argv)
        {
            std::vector<std::string> args;
            args.reserve(argc);
            for(int i=1; i<argc; ++i)
            {
                args.emplace_back(argv[i]);
            }
            return args;
        }

        std::optional<Flags_and_args> get_flags_and_args(const int argc, char** argv)
        {
            std::optional<Flags_and_args> result;

            auto command_line  = get_vector_of_command_line_args(argc, argv);
            std::vector<char> flags;
            std::vector<std::string> args;

            // Process flags, then arguments.
            bool processing_flags{true};
            bool good_command_line{true};

            for(auto& a: command_line)
            {
                if(good_command_line)
                {
                    if( is_flag(a) )
                    {
                        if(processing_flags)
                        {
                            const auto flag_letters = split_flags(a);
                            for(const char letter : flag_letters)
                            {
                                if(contains(flags, letter) and not_a_number(letter))
                                {
                                    // Repetition of flag.
                                    good_command_line = false;
                                }
                                else
                                {
                                    flags.push_back(letter);
                                }
                            }
                        }
                        else
                        {
                            // Flag encountered after argument.
                            good_command_line = false;
                        }
                    }
                    else
                    {
                        processing_flags = false;
                        args.push_back(std::move(a));
                    }
                }
            }

            if(good_command_line)
            {
                Flags_and_args fa;
                fa.args.swap(args);
                fa.flags.swap(flags);
                result.emplace(std::move(fa));
            }

            return result;
        }

        std::optional<Args> convert_flags(const std::vector<char>& flags)
        {
            Args a;

            bool bad = false;
            std::string time_limit;
            bool parse_time_limit = false;

            for(char flag : flags)
            {
                if(bad)
                {
                    break;
                }

                if(parse_time_limit)
                {
                    if(flag >= '0' and flag <= '9')
                    {
                        time_limit.push_back(flag);
                    }
                    else
                    {
                        parse_time_limit = false;
                    }
                }

                if(not parse_time_limit)
                {
                    switch (flag)
                    {
                    case 'b':
                        a.show_skybox = true;
                        break;

                    case 'B':
                        a.convert_skybox = true;
                        break;

                    case 'd':
                        a.debugging = true;
                        break;

                    case 'm':
                        a.show_model = true;
                        break;

                    case 'M':
                        a.convert_model = true;
                        break;

                    case 't':
                        parse_time_limit = true;
                        break;

                    default:
                        //  Unknown flag.
                        return std::optional<Args>();
                    }
                }
            }

            // Check if we have a time limit.
            if(not bad and not time_limit.empty())
            {
                try
                {
                    // Check that the number of seconds expressed as milliseconds
                    // is not too large to express in uint64.
                    constexpr std::uint64_t upper{ std::numeric_limits<std::uint64_t>::max() / 1000u };
                    // This can throw.
                    const std::uint64_t s{std::stoul(time_limit)};
                    if(s > upper)
                    {
                        bad = true;
                    }
                    else
                    {
                        a.max_runtime_ms = std::uint64_t{1000u} * s;
                    }
                }
                catch(...)
                {
                    bad = true;
                }
            }

            if(bad)
            {
                return std::optional<Args>();
            }
            else
            {
                return a;
            }
        }

        std::vector<std::filesystem::path> convert_args_to_paths(const std::vector<std::string>& args)
        {
            std::vector<std::filesystem::path> paths;
            for(const auto& a : args)
            {
                paths.push_back( std::filesystem::absolute(std::filesystem::path(a)) );
            }
            return paths;
        }
    }

    std::optional<Args> get_args(const int argc, char** argv)
    {
        std::optional<Args> result;

        if(argc > 0)
        {
            // Special case: '--help':
            if(argc == 2 and (std::string(argv[1]) == "-h" or
                              std::string(argv[1]) == "--help"))
            {
                Args a;
                a.show_help = true;
                result = a;
            }
            else
            {
                const auto app_name = std::filesystem::path(argv[0]).filename().string();
                const auto flags_and_args = get_flags_and_args(argc, argv);
                if(flags_and_args.has_value())
                {
                    auto flags     = convert_flags(flags_and_args->flags);
                    auto file_args = convert_args_to_paths(flags_and_args->args);

                    if(flags.has_value() and not file_args.empty())
                    {
                        if(flags->convert_model)
                        {
                            // Convert requires two file arguments.
                            if(2u == file_args.size())
                            {
                                result.swap(flags);
                                result->file_names[0] = file_args[0].string();
                                result->file_names[1] = file_args[1].string();
                            }
                        }
                        else
                        {
                            if(flags->show_model)
                            {
                                if(1u == file_args.size())
                                {
                                    result.swap(flags);
                                    result->file_names[0] = file_args[0].string();
                                }
                                else if(2u == file_args.size())
                                {
                                    result.swap(flags);
                                    result->file_names[0] = file_args[0].string();
                                    result->file_names[1] = file_args[1].string();
                                }
                            }
                            else
                            {
                                if(flags->convert_skybox)
                                {
                                    // Requires 7 arguments
                                    if(7u == file_args.size())
                                    {
                                        result.swap(flags);
                                        result->file_names[0] = file_args[0].string();
                                        result->file_names[1] = file_args[1].string();
                                        result->file_names[2] = file_args[2].string();
                                        result->file_names[3] = file_args[3].string();
                                        result->file_names[4] = file_args[4].string();
                                        result->file_names[5] = file_args[5].string();
                                        result->file_names[6] = file_args[6].string();
                                    }
                                }
                                else
                                {
                                    // All other supported configurations take 1 file arg.
                                    if(1u == file_args.size())
                                    {
                                        result.swap(flags);
                                        result->file_names[0] = file_args[0].string();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        return result;
    }

    std::string get_app_name(int argc, char** argv)
    {
        return (argc > 0) ? std::filesystem::path(argv[0]).filename().string() : std::string{};
    }

    std::string get_help(const std::string& app_name)
    {
        std::stringstream ss;
        ss << app_name << " can be run in following modes:\n";
        ss << "\n";
        ss << app_name << " -h, --help\n";
        ss << "      Shows this help message and exits\n";
        ss << "\n";
        ss << app_name << " [flags] <file>\n";
        ss << "      Opens and runs the universe file <file>\n";
        ss << "\n";
        ss << app_name << " [flags] -b <file>\n";
        ss << "      Show skybox in <file>\n";
        ss << "\n";
        ss << app_name << " [flags] -B <sky> <+x> <-x> <+y> <-y> <+z> <-z>\n";
        ss << "      Creates a skybox file <sky> from the 6 image files <+x>..<-z>.\n";
        ss << "      The image order matches the OpenGL CUBE_MAP face orders.\n";
        ss << "      Can be combined with '-b' to show saved skybox.\n";
        ss << "\n";
        ss << app_name << " [flags] -m <file> [<sky>]\n";
        ss << "      Show model in <file> [with optional skybox <sky>]\n";
        ss << "\n";
        ss << app_name << " [flags] -M <file-a> <file-b>\n";
        ss << "      Convert glTF <file-a> to internal format and save as <file-b>\n";
        ss << "      Can be combined with '-s' to show saved model.\n";
        ss << "\n";
        ss << "Flags:\n";
        ss << " -d   Enable debug output and debug shaders\n";
        ss << " -tN  Limit runtime to N seconds\n";
        ss << "\n";
        ss << "Examples:\n";
        ss << "      " << app_name << " MyUniverseFile\n";
        ss << "      " << app_name << " -d MyUniverseFile\n";
        ss << "      " << app_name << " -t60d MyUniverseFile\n";
        ss << "      " << app_name << " -t60 -d MyUniverseFile\n";
        ss << "      " << app_name << " -M car.gltf car.elm\n";
        ss << "      " << app_name << " -Mm car.gltf car.elm\n";
        ss << "      " << app_name << " -dm car.elm\n";
        ss << "\n";
        ss << app_name << " Copyright (c) 2023 Daire Stockdale\n";
        ss << "\n";
        return ss.str();
    }
}