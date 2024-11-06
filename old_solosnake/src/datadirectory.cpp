#include "solosnake/filepath.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/throw.hpp"

using namespace std;

namespace solosnake
{
    datadirectory::datadirectory( const string& directoryName ) : dir_( directoryName )
    {
    }

    filepath datadirectory::get_filepath( const string& filename ) const
    {
        return dir_ / filename;
    }

    datadirectories::datadirectories( const vector<string>& dirs ) : dirs_( dirs )
    {
        if( dirs.empty() )
        {
            ss_throw( "datadirectories was created with empty directory series." );
        }
    }

    filepath datadirectories::get_filepath( const string& filename ) const
    {
        for( size_t i = 0; i < dirs_.size(); ++i )
        {
            const filepath url( dirs_[i] );
            filepath_is_directory( url );
            const auto possibleFile = url / filename;
            if( filepath_exists( possibleFile ) && filepath_is_regular_file( possibleFile ) )
            {
                return possibleFile;
            }
        }

        return filepath();
    }
}
