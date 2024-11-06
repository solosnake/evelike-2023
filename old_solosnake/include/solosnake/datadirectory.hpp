#ifndef solosnake_datadirectory_hpp
#define solosnake_datadirectory_hpp

#include <string>
#include <vector>
#include "solosnake/filepath.hpp"
#include "solosnake/ifilefinder.hpp"

namespace solosnake
{
    //! Class for representing a single data directory to retrieve files from.
    //! e.g. if you have a resource type (textures) in a directory (App\MyTextures\)
    //! and you want a search for a file (wall.tex) to return files from this
    //! directory
    //! (App\MyTextures\wall.tex).
    class datadirectory : public ifilefinder
    {
    public:

        explicit datadirectory( const std::string& directoryName );

    private:

        filepath get_filepath( const std::string& ) const override;

    private:

        filepath dir_;
    };

    //! Searches in several directories for the requested file, in the order
    //! they are specified in.
    class datadirectories : public ifilefinder
    {
    public:

        explicit datadirectories( const std::vector<std::string>& directories );

    private:

        filepath get_filepath( const std::string& filename ) const override;

    private:

        std::vector<std::string> dirs_;
    };
}

#endif
