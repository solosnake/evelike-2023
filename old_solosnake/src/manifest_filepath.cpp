#include "solosnake/manifest_filepath.hpp"

namespace solosnake
{
    filepath manifest_filepath( const filepath& manifestUrl, const std::string& manifestEntry )
    {
        return filepath_parent_path( manifestUrl ) / filepath( manifestEntry );
    }
}