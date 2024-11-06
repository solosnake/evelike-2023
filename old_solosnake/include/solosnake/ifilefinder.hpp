#ifndef solosnake_ifilefinder_hpp
#define solosnake_ifilefinder_hpp

#include <string>
#include "solosnake/filepath.hpp"

namespace solosnake
{
    //! Interface for searching for a named file in a set of data directories.
    //! Throws if the file does not exist.
    class ifilefinder
    {
    public:

        //! Returns a valid URL or throws.
        filepath get_file( const std::string& filename ) const;

        virtual ~ifilefinder();

    private:

        //! Private implementation of get_file.
        virtual filepath get_filepath( const std::string& filename ) const = 0;
    };
}

#endif
