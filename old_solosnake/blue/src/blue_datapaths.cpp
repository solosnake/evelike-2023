#include <algorithm>
#include <string>
#include <vector>
#include "solosnake/filepath.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_settings.hpp"

using namespace std;

// Token used to move upwards on Windows, XBox, Linux and Apple.
#define BLUE_DIRECTORY_UP_TOKEN ".."

namespace blue
{
    namespace
    {
        std::string trim_whitespace( const std::string& str )
        {
            std::string trimmed;

            const auto whitespaces = " \t\n\r";

            // Trailing whitespace
            const auto endpos = str.find_last_not_of( whitespaces );

            if( string::npos != endpos )
            {
                trimmed = str.substr( 0, endpos + 1 );
            }

            // Leading whitespace
            const auto startpos = trimmed.find_first_not_of( whitespaces );

            if( string::npos != startpos )
            {
                trimmed = trimmed.substr( startpos );
            }

            return trimmed;
        }


        // Game folder / folders / directories / directory's layout names:
        const string board_folder_name          = "boards";
        const string chassis_folder_name        = "chassis";
        const string fonts_folder_name          = "fonts";
        const string gui_folder_name            = "gui";
        const string hardpoints_folder_name     = "hardpoints";
        const string model_folder_name          = "models";
        const string meshes_folder_name         = "meshes";
        const string scripts_folder_name        = "scripts";
        const string skyboxes_folder_name       = "skyboxes";
        const string suns_folder_name           = "suns";
        const string softpoints_folder_name     = "softpoints";
        const string sound_folder_name          = "sounds";
        const string styles_folder_name         = "styles";
        const string texture_folder_name        = "textures";
        const string thrusters_folder_name      = "thrusters";
        const string translations_folder_name   = "translations";

        // This is not a folder title.
        const string manifest_file_name         = "manifest.xml";

        //! Returns the array of paths which lead to this folder, or throws
        //! if this folder name is not known.
        const vector<solosnake::filepath>&
        get_folders_paths( const string& folder,
                           const map<string, vector<solosnake::filepath>>& paths )
        {
            auto lookups = paths.find( folder );

            if( lookups == paths.end() )
            {
                ss_err( "Paths data set does not contain an entry for data type : ", folder );
                ss_throw( "Paths data set does not contain an entry for this data type." );
            }

            return lookups->second;
        }

        //! Returns an array of all the "manifest.xml" files it finds in the folders paths.
        //! Throws if no manifests are found.
        vector<solosnake::filepath>
        get_manifests( const string& foldername,
                       const vector<solosnake::filepath>& paths )
        {
            vector<solosnake::filepath> manifests;
            manifests.reserve( paths.size() );

            for( size_t i = 0; i < paths.size(); ++i )
            {
                auto filepath = paths[0] / manifest_file_name;

                if( std::filesystem::exists( filepath ) &&
                        std::filesystem::is_regular_file( filepath ) )
                {
                    manifests.push_back( move( filepath ) );
                }
            }

            if( manifests.empty() )
            {
                ss_err( "No ", manifest_file_name, " found for ", foldername );
                ss_throw( "Manifest files are missing." );
            }

            return manifests;
        }

        //! Checks to see if any of the directories contain the requested file.
        //! Forbids the user from searching upwards by throwing if the string
        //! contains any directory 'exit' tokens, e.g. '..' on windows etc.
        solosnake::filepath lookup( const string& filename,
                                    const vector<solosnake::filepath>& paths )
        {
            if( filename.empty() )
            {
                ss_throw( "Cannot lookup empty filename." );
            }

            if( string::npos != filename.find( BLUE_DIRECTORY_UP_TOKEN ) )
            {
                ss_err( "File name '",
                        filename,
                        "' contains '" BLUE_DIRECTORY_UP_TOKEN "' and "
                        "appears to be trying to access non-game directory files." );

                ss_throw( "Attempt made to access file outside of game folders." );
            }

            for( size_t i = 0; i < paths.size(); ++i )
            {
                auto filepath = paths[0] / filename;

                if( std::filesystem::exists( filepath ) && std::filesystem::is_regular_file( filepath ) )
                {
                    return filepath;
                }
            }

            ss_err( "Cannot find file : ", filename );
            ss_throw( "Unable to find requested file in listed data directories." );
        }

        //! Finds the array of folders associated with this data type and passes
        // this on for further lookup.
        solosnake::filepath lookup( const string& filename,
                                    const string& key,
                                    const map<string, vector<solosnake::filepath>>& paths )
        {
            return lookup( filename, get_folders_paths( key, paths ) );
        }

        vector<string> split_to_paths( const string& roots )
        {
            vector<string> paths;

            if( !roots.empty() )
            {
                string::size_type head = 0;
                string::size_type tail = roots.find_first_of( UO_DATA_ROOT_SEPERATOR );

                do
                {
                    // Do a little cautionary tidying up of the input: trim leading
                    // and trailing whitespace.
                    auto path = trim_whitespace( roots.substr( head, tail - head ) );

                    if( !path.empty() )
                    {
                        paths.push_back( move( path ) );
                    }

                    head = tail + 1;
                    tail = roots.find_first_of( UO_DATA_ROOT_SEPERATOR, head );
                }
                while( tail != string::npos );
            }

            return paths;
        }

        bool try_add_dir_path( const string& folder_name,
                               const solosnake::filepath& rootDir,
                               map<string, vector<solosnake::filepath>>& path_lookups )
        {
            bool added = false;

            auto folderPath = rootDir / folder_name;

            if( std::filesystem::is_directory( folderPath ) && std::filesystem::exists( folderPath ) )
            {
                const auto it = find( path_lookups[folder_name].cbegin(),
                                      path_lookups[folder_name].cend(),
                                      folderPath );

                if( it == path_lookups[folder_name].cend() )
                {
                    path_lookups[folder_name].push_back( move( folderPath ) );
                }

                // Strictly we didn't add it, but return that it
                // does contain the expected folder.
                added = true;
            }
            else
            {
                ss_wrn( folderPath, " is not a valid data folder." );
            }

            return added;
        }
    }

    // Here we setup the standardized directory layout.
    // <root>/gui
    // <root>/models
    // <root>/textures
    // At least one must be set or this ctor will throw.
    datapaths::datapaths( const std::string& dirs )
    {
        auto rootPaths = split_to_paths( dirs );

        if( rootPaths.empty() )
        {
            ss_throw( "Unable to parse directories string. "
                      "Expected a " UO_DATA_ROOT_SEPERATOR " separated list of directories." );
        }

        // We now have a set of root directories. Each of these is a base directory
        // to and each should contain a mirror layout of the directories.
        for_each(
            rootPaths.cbegin(),
            rootPaths.cend(),
        [&]( const string & rootPath ) { try_add_new_data_root( rootPath ); } );

        if( path_lookups_.empty() )
        {
            ss_throw( "No data directories were set. Is the '" UO_DATA_ROOT_DIRS
                      "' entry invalid in the user's settings file?" );
        }
    }

    //! Treats @a rootPath as a root directory for a set of data paths and adds the
    //! sets of asset paths to the lookups. Logs to wrn is any of the paths could not
    //! be found.
    bool datapaths::try_add_new_data_root( const string& rootPath )
    {
        solosnake::filepath rootDir( rootPath );

        if( solosnake::filepath_exists( rootDir ) )
        {
            if( solosnake::filepath_is_directory( rootDir ) )
            {
                try_add_dir_path( board_folder_name,        rootDir, path_lookups_ );
                try_add_dir_path( chassis_folder_name,      rootDir, path_lookups_ );
                try_add_dir_path( fonts_folder_name,        rootDir, path_lookups_ );
                try_add_dir_path( gui_folder_name,          rootDir, path_lookups_ );
                try_add_dir_path( hardpoints_folder_name,   rootDir, path_lookups_ );
                try_add_dir_path( meshes_folder_name,       rootDir, path_lookups_ );
                try_add_dir_path( model_folder_name,        rootDir, path_lookups_ );
                try_add_dir_path( scripts_folder_name,      rootDir, path_lookups_ );
                try_add_dir_path( skyboxes_folder_name,     rootDir, path_lookups_ );
                try_add_dir_path( suns_folder_name,         rootDir, path_lookups_ );
                try_add_dir_path( softpoints_folder_name,   rootDir, path_lookups_ );
                try_add_dir_path( styles_folder_name,       rootDir, path_lookups_ );
                try_add_dir_path( sound_folder_name,        rootDir, path_lookups_ );
                try_add_dir_path( texture_folder_name,      rootDir, path_lookups_ );
                try_add_dir_path( thrusters_folder_name,    rootDir, path_lookups_ );
                try_add_dir_path( translations_folder_name, rootDir, path_lookups_ );

                return true;
            }
            else
            {
                ss_wrn( UO_DATA_ROOT_DIRS " specifies '", rootPath, "', but it is not a directory." );
            }
        }
        else
        {
            ss_wrn( "Data directory '", rootPath, "' could not be found." );
        }

        return false;
    }

    solosnake::filepath datapaths::get_boards_filepath( const string& filename ) const
    {
        return lookup( filename, board_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_chassis_filepath( const string& filename ) const
    {
        return lookup( filename, chassis_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_fonts_filepath( const string& filename ) const
    {
        return lookup( filename, fonts_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_gui_filepath( const string& filename ) const
    {
        return lookup( filename, gui_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_hardpoints_filepath( const string& filename ) const
    {
        return lookup( filename, hardpoints_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_meshes_filepath( const string& filename ) const
    {
        return lookup( filename, meshes_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_models_filepath( const string& filename ) const
    {
        return lookup( filename, model_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_scripts_filepath( const string& filename ) const
    {
        return lookup( filename, scripts_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_suns_filepath( const string& filename ) const
    {
        return lookup( filename, suns_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_skyboxes_filepath( const string& filename ) const
    {
        return lookup( filename, skyboxes_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_styles_filepath( const string& filename ) const
    {
        return lookup( filename, styles_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_softpoints_filepath( const string& filename ) const
    {
        return lookup( filename, softpoints_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_sounds_filepath( const string& filename ) const
    {
        return lookup( filename, sound_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_textures_filepath( const string& filename ) const
    {
        return lookup( filename, texture_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_thrusters_filepath( const string& filename ) const
    {
        return lookup( filename, thrusters_folder_name, path_lookups_ );
    }

    solosnake::filepath datapaths::get_translations_filepath( const string& filename ) const
    {
        return lookup( filename, translations_folder_name, path_lookups_ );
    }

    //! Returns the paths to all the manifest files found in the available
    //! directories.
    vector<solosnake::filepath> datapaths::get_chassis_manifests() const
    {
        return get_manifests( chassis_folder_name,
                              get_folders_paths( chassis_folder_name, path_lookups_ ) );
    }

    vector<solosnake::filepath> datapaths::get_hardpoints_manifests() const
    {
        return get_manifests( hardpoints_folder_name,
                              get_folders_paths( hardpoints_folder_name, path_lookups_ ) );
    }

    vector<solosnake::filepath> datapaths::get_softpoints_manifests() const
    {
        return get_manifests( softpoints_folder_name,
                              get_folders_paths( softpoints_folder_name, path_lookups_ ) );
    }

    vector<solosnake::filepath> datapaths::get_thrusters_manifests() const
    {
        return get_manifests( thrusters_folder_name,
                              get_folders_paths( thrusters_folder_name, path_lookups_ ) );
    }

    vector<solosnake::filepath> datapaths::get_translations_manifests() const
    {
        return get_manifests( translations_folder_name,
                              get_folders_paths( translations_folder_name, path_lookups_ ) );
    }
}
