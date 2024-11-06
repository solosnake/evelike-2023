#include "solosnake/testing/testing.hpp"
#include "solosnake/directoryfiles.hpp"

TEST( directoryfiles, get_files_in_directory )
{
    solosnake::filepath url( "." );
    auto files = solosnake::get_files_in_directory( "." );
    EXPECT_TRUE( files.size() > 0u );
}

TEST( directoryfiles, get_files )
{
    solosnake::filepath url( "." );
    auto allfiles = solosnake::get_files_in_directory( "." );

    solosnake::directoryfiles foo( url );

    EXPECT_TRUE( foo.get_files() == allfiles );
}

TEST( directoryfiles, get_files_matching )
{
    solosnake::filepath url( "." );
    auto allfiles = solosnake::get_files_in_directory( "." );

    solosnake::directoryfiles foo( url );

    auto txtfiles = foo.get_files_matching( ".*\\.txt" );

    EXPECT_TRUE( txtfiles.size() <= allfiles.size() );
}