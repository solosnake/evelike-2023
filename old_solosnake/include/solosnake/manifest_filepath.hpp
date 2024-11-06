#ifndef solosnake_manifest_filepath_hpp
#define solosnake_manifest_filepath_hpp

#include "solosnake/filepath.hpp"

namespace solosnake
{
    //! When the contents listing of a manifest is a list of files co-located with the manifest
    //! file, this function is a convenience function for returning the file-path to that 
    //! contents.
    //! @param  manifesturl     The file-path to the manifest xml file.
    //! @param  manifestentry   The name of a file assumed to be located in the same directory
    //!                         as the manifest xml file.
    filepath manifest_filepath( const filepath& manifestUrl, const std::string& manifestEntry );
}

#endif 
