#include <cstdlib>
#include <iostream>
#include <memory>
#include "solosnake/evelike-args.hpp"
#include "solosnake/evelike-main.hpp"
#include "solosnake/evelike-game.hpp"
#include "solosnake/evelike-show-model.hpp"
#include "solosnake/evelike-convert-model.hpp"
#include "solosnake/evelike-convert-skybox.hpp"
#include "solosnake/evelike-run-universe.hpp"

namespace solosnake::evelike
{
    int evelike_main(int argc, char** argv)
    {
        auto args = get_args(argc, argv);

        if(args.has_value())
        {
            if(args->show_help)
            {
                std::cout << get_help(EveLike_game::AppName);
                return EXIT_SUCCESS;
            }

            if(args->convert_model)
            {
                return convert_model(args->file_names[0],
                                     args->file_names[1],
                                     args->debugging,
                                     args->show_model);
            }

            if(args->convert_skybox)
            {
                return convert_skybox(args->file_names[0],
                                      args->file_names[1],
                                      args->file_names[2],
                                      args->file_names[3],
                                      args->file_names[4],
                                      args->file_names[5],
                                      args->file_names[6],
                                      args->debugging,
                                      args->show_skybox);
            }

            if(args->show_skybox)
            {
                return show_model("", args->file_names[0], args->debugging, 0u);
            }

            if(args->show_model)
            {
                return show_model(args->file_names[0], args->file_names[1], args->debugging, 0u);
            }

            return run_universe(args->file_names[0],
                                args->max_runtime_ms,
                                args->debugging);
        }
        else
        {
            if(argc > 1)
            {
                std::cerr << "Invalid " << EveLike_game::AppName << " command line arguments: '";
                for(auto i=1; i < argc; ++i)
                {
                    std::cerr << argv[i] << " ";
                }
                std::cerr << "'\n";
            }

            std::cerr << get_help(EveLike_game::AppName);
        }

        return EXIT_FAILURE;
    }
}