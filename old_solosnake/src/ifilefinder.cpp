#include "solosnake/filepath.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    filepath ifilefinder::get_file( const std::string& filename ) const
    {
        auto file = this->get_filepath( filename );

        if( false == filepath_exists( file ) || 
            false == filepath_is_regular_file( file ) )
        {
            ss_err( "Requested file does not exist : ", file.string() );
            ss_throw( "Requested file does not exist." );
        }

        return file;
    }

    ifilefinder::~ifilefinder()
    {
    }
}
