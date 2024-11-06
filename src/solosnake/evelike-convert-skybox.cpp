#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include "solosnake/evelike-convert-skybox.hpp"
#include "solosnake/evelike-show-model.hpp"
#include "solosnake/skybox.hpp"

namespace solosnake::evelike
{
    int convert_skybox(const std::string_view& skybox,
                       const std::string_view& xp,
                       const std::string_view& xn,
                       const std::string_view& yp,
                       const std::string_view& yn,
                       const std::string_view& zp,
                       const std::string_view& zn,
                       bool debugging,
                       bool show)
    {
        int result = EXIT_SUCCESS;

        try
        {
            std::array<std::string_view, 6> images{ xp, xn, yp, yn, zp, zn };
            solosnake::save_skybox(skybox, images);

            if(show)
            {
                result = show_model("", skybox, debugging, 0u);
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << "\n";
            result = EXIT_FAILURE;
        }

        return result;
    }
}