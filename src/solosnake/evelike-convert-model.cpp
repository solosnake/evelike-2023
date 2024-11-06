#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include "solosnake/evelike-convert-model.hpp"
#include "solosnake/evelike-show-model.hpp"
#include "solosnake/gltf-to-mesh.hpp"

namespace solosnake::evelike
{
    int convert_model(const std::string_view& src,
                      const std::string_view& dest,
                      const bool debugging,
                      const bool show)
    {
        int result = EXIT_SUCCESS;

        try
        {
            convert_glTF_to_mesh(src, dest);

            if(show)
            {
                result = show_model(dest, "", debugging, 0u);
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
