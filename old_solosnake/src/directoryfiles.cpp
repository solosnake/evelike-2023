#include "solosnake/directoryfiles.hpp"
#include "solosnake/throw.hpp"
#include <filesystem>
#include <regex>
#include <algorithm>

namespace solosnake
{
    directoryfiles::directoryfiles( const filepath& url ) : dir_( std::filesystem::canonical(url) )
    {
        if( !( filepath_exists( dir_ ) && filepath_is_directory( dir_ ) ) )
        {
            ss_throw( "Directory provided to directoryfiles not found." );
        }
    }

    const filepath& directoryfiles::directory_name() const
    {
        return dir_;
    }

    std::vector<filepath> directoryfiles::get_files() const
    {
        return solosnake::get_files_in_directory( directory_name() );
    }

    std::vector<filepath> directoryfiles::get_files_matching( const std::string& regexpression )
    {
        const std::regex rex( regexpression );

        auto files = get_files();

        auto end = std::remove_if( files.begin(), 
                                   files.end(), 
                                   [&](const filepath& url)
                                   { 
                                       return ! std::regex_match( url.string(), rex );
                                   } );
        
        files.erase( end, files.end() );

        return files;
    }

    std::vector<filepath> get_files_in_directory( const filepath& url )
    {
        std::vector<filepath> files;

        std::filesystem::directory_iterator dir_itr( url );
        std::filesystem::directory_iterator end_iter;

        for( ; dir_itr != end_iter; ++dir_itr )
        {
            if(std::filesystem::is_regular_file( dir_itr->status() ) )
            {
                files.push_back(std::filesystem::canonical( dir_itr->path() ) );
            }
        }

        return files;
    }
}
