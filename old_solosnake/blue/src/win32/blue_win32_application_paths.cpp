#include <iostream>
#include <vector>
#include <cassert>
#include "solosnake/win32/includewindows.hpp"
#include <shlobj.h>
#include <shlwapi.h>
#include "blue_version_cmake_generated.hpp"
#include "solosnake/filepath.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/unicode_to_utf8.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/blue/blue_settings.hpp"


// CSIDL Name          Typical Path (Windows Vista)        Standard User   Administrator   Access      Description + Example
// ----------------------------------------------------------------------------------------------------------------------------------------------
// CSIDL_PERSONAL      C:\Users\user name\Documents        Read/Write      Read/Write      Per-User    User-specific game files that are
//                                                                                                     read and modified and can be
//                                                                                                     manipulated outside of the game context.
//                                                                                                     Screen shots. Saved game files with a
//                                                                                                     file extension association.
// 
// CSIDL_LOCAL_APPDATA C:\Users\user name\AppData\Local    Read/Write      Read/Write      Per User    User-specific game files that are read
//                                                                                                     and modified and are of use only within
//                                                                                                     the game context.
//                                                                                                     Game cache files. Player configurations.
// 
// CSIDL_COMMON_APPDATA C:\ProgramData                     Read/Write      Read/Write      All Users   Game files that can be created by a user
//                                                                                                     and read by all users.
//                                                                                                     Write access is granted only to the creator
//                                                                                                     of the file (owner).
// 
// CSIDL_PROGRAM_FILES  C:\Program Files                   Read only       Read/Write       All Users  Static game files written by the game’s
//                      C:\Program Files (x86)                                                         installer that are read by all users.
//                                                                                                     Game assets, such as materials


namespace
{
    enum WindowUACFilePaths
    {
        PerUserPersonalFiles,
        PerUserAppFiles,
        PerUserRoamingAppFiles,
        PerUserGameSaveFiles,
        PerUserSharedFiles,
        AllUsersAppFiles
    };

    void append_and_ensure_dir_exists( TCHAR* strPath, WCHAR const * const append )
    {
        PathAppend( strPath, TEXT( "solosnake\\" ) TEXT( BLUE_APP_MAJOR_MINOR_PATCH_VERSIONED_NAME ) );

        if( FALSE == PathFileExists( strPath ) )
        {
            if( ERROR_SUCCESS != ::SHCreateDirectoryEx( nullptr, strPath, nullptr ) )
            {
                ss_throw( "Unable to create application directory for "
                          "solosnake\\" BLUE_APP_MAJOR_MINOR_PATCH_VERSIONED_NAME );
            }
        }

        if( append != nullptr )
        {
            PathAppend( strPath, append );
        }

        PathAddBackslash( strPath );
    }

    std::string get_windows_path( const WindowUACFilePaths uacFilePath, WCHAR const * const append )
    {
        WCHAR strPath[MAX_PATH] = { 0 };

        // NOTE, As of Windows Vista, these values (CSIDL) have been replaced 
        // by KNOWNFOLDERID values. 
        int uac = 0;

        switch( uacFilePath )
        {
            case PerUserPersonalFiles:
                uac = CSIDL_PERSONAL;
                break;

            case PerUserRoamingAppFiles:
                uac = CSIDL_APPDATA;
                break;

            case PerUserGameSaveFiles:
            {
#if _WIN32_WINNT >= 0x06000000
                PWSTR sz = NULL;
                if( SUCCEEDED( ::SHGetKnownFolderPath( FOLDERID_SavedGames, 0, NULL, &sz ) ) )
                {
                    if( sz )
                    {
                        wcscpy_s( strPath, MAX_PATH, sz );
                        ::CoTaskMemFree( sz );
                        append_and_ensure_dir_exists( strPath, append );
                        return unicode_to_utf8( std::wstring( strPath ) );
                    }
                    else
                    {
                        ss_err( "Could not find Windows known folder path for "
                                "'PerUserGameSaveFiles'." );
                    }
                }
                else
                {
                    ss_wrn( "Could not find Windows known folder path for "
                            "'PerUserGameSaveFiles'." );
                }
#else
                // Just use roaming app data.
                uac = CSIDL_APPDATA;
#endif
            }
            break;

            case PerUserAppFiles:
                uac = CSIDL_LOCAL_APPDATA;
                break;

            case PerUserSharedFiles:
                uac = CSIDL_COMMON_APPDATA;
                break;

            case AllUsersAppFiles:
                uac = CSIDL_PROGRAM_FILES;
                break;

            default:
              ss_throw( "Unknown WindowUACFilePaths enum." );
              break;
        };

        if( SUCCEEDED( ::SHGetFolderPath( nullptr, uac, nullptr, SHGFP_TYPE_CURRENT, strPath ) ) )
        {
            append_and_ensure_dir_exists( strPath, append );
        }
        else
        {
            ss_wrn( "Could not find requested Windows known folder path." );
        }

        return solosnake::unicode_to_utf8( std::wstring( strPath ) );
    }
}

namespace blue
{
    //! Returns the application path. This will end in a backslash
    //! e.g. C:\Program Files\solosnake\blue\ <<
    std::string get_game_data_dir()
    {
        TCHAR strPath[MAX_PATH] = { 0 };

        if( 0 == ::GetModuleFileName( nullptr, strPath, MAX_PATH ) )
        {
            ss_throw( "::GetModuleFileName in get_application_data_path failed." );
        }

        if( (FALSE == ::PathRemoveFileSpec( strPath )) || (nullptr == ::PathAddBackslash( strPath )) )
        {
            ss_throw( "get_application_data_path failed." );
        }

        assert( std::wstring( strPath ).size() > 0 );
        assert( std::wstring( strPath ).back() == '\\' );

        return solosnake::unicode_to_utf8( std::wstring( strPath ) );
    }

    //! Returns user save game file path on Vista etc if available, else            //
    //! returns the user account file settings root location,                       //
    //! e.g. C:\Users\solosnake\AppData\Roaming\solosnake\blue\                     //
    //! else finally gives up and returns the app path (which is guaranteed         //
    //! to not fail).                                                               //
    std::string get_userdata_path()
    {
        std::string path = get_windows_path( PerUserGameSaveFiles, nullptr );

        if( path.empty() )
        {
            path = get_windows_path( PerUserRoamingAppFiles, nullptr );

            if( path.empty() )
            {
                path = get_game_data_dir();
            }
        }

        if( path.empty() )
        {
            ss_throw( "Unable to find user save data path." );
        }

        return path;
    }

    std::string get_user_settings_filepath()
    {
        return get_userdata_path() + AO_USER_SETTINGS_FILENAME_VALUE;
    }
}
