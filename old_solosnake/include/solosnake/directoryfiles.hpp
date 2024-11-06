#ifndef solosnake_directoryfiles_hpp
#define solosnake_directoryfiles_hpp

#include <string>
#include <vector>
#include "solosnake/filepath.hpp"

namespace solosnake
{
    //! Object that returns the files in a given directory.
    class directoryfiles
    {
    public:

        explicit directoryfiles( const filepath& );

        //! Returns the full path name of the directory this object refers to.
        const filepath&       directory_name() const;

        //! Returns the full path names of all the files in the directory this object refers to.
        std::vector<filepath> get_files() const;
        
        //! Returns the full path names of all the files in the directory this object refers to
        //! whose full path name matches the provided regular expression.
        std::vector<filepath> get_files_matching( const std::string& regexpression );

    private:

        filepath    dir_;
    };


    //! Returns an vector of the files in the named directory, else throws.
    std::vector<filepath> get_files_in_directory( const filepath& );

}

#endif
