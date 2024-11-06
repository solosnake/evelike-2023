#ifndef solosnake_filepath_hpp
#define solosnake_filepath_hpp

#include <string>
#include <filesystem>

namespace solosnake
{
    typedef std::filesystem::path       filepath;

    inline bool filepath_is_regular_file( const filepath& url )
    {
        return std::filesystem::is_regular_file( url );
    }

    inline bool filepath_is_directory( const filepath& url )
    {
        return std::filesystem::is_directory( url );
    }

    inline bool filepath_exists( const filepath& url )
    {
        return std::filesystem::exists( url );
    }

    //! Returns "baa" for "/foo/bar/baa.txt".
    inline std::string filepath_file_stem( const filepath& url )
    {
        return url.stem().string();
    }

    //! Returns "/foo/bar" for "/foo/bar/baa.txt".
    inline filepath filepath_parent_path( const filepath& url )
    {
        return url.parent_path();
    }

    //! Returns ".txt" for "/foo/bar/baa.txt".
    inline std::string filepath_file_extension( const filepath& url )
    {
        return url.extension().string();
    }

    //! Returns "baa.txt" for "/foo/bar/baa.txt".
    inline std::string filepath_file_name( const filepath& url )
    {
        return url.filename().string();
    }
}

#endif
