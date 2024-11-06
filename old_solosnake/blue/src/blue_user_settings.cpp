#include <algorithm>
#include <sstream>
#include "solosnake/filepath.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_get_buildstamp.hpp"
#include "solosnake/blue/blue_settings.hpp"

using namespace solosnake;

namespace blue
{
    namespace
    {
        void check_dir_exists( const user_settings& data, const std::string& key )
        {
            solosnake::filepath dir( data.value( key ).str() );

            if( !( solosnake::filepath_exists( dir ) && solosnake::filepath_is_directory( dir ) ) )
            {
                ss_err( "The value for '",
                        key,
                        "' given (",
                        data.value( key ).str(),
                        ") is not a valid directory." );

                ss_throw( "Directory not found. Please correct this value in XML or "
                          "reinstall " BLUE_APP_MAJOR_MINOR_PATCH_VERSIONED_NAME );
            }
        }

        bool is_good_directory( const std::string& dirName )
        {
            const solosnake::filepath dir( dirName );
            return solosnake::filepath_exists( dir ) && solosnake::filepath_is_directory( dir );
        }

        bool file_exists( const std::string& file )
        {
            const solosnake::filepath url( file );
            if( solosnake::filepath_exists( url ) && solosnake::filepath_is_regular_file( url ) )
            {
                return true;
            }

            return false;
        }
    }

    //! Initialises the user's settings. Settings not present will be set to the default values.
    //! Existing settings are not changed.
    void user_settings::add_settings()
    {        
        set_if_not_present( AO_APP_TITLE,              BLUE_APP_MAJOR_MINOR_PATCH_VERSIONED_NAME, AO_APP_TITLE_COMMENT );
        
        set_if_not_present( AO_SAFESTART_HEIGHT,       AO_SAFESTART_HEIGHT_VALUE,          AO_SAFESTART_HEIGHT_COMMENT );
        set_if_not_present( AO_SAFESTART_WIDTH,        AO_SAFESTART_WIDTH_VALUE,           AO_SAFESTART_WIDTH_COMMENT );
        set_if_not_present( AO_FIRST_SCREEN,           AO_FIRST_SCREEN_VALUE,              AO_FIRST_SCREEN_COMMENT );
        set_if_not_present( AO_SAFESTART_WINDOW_TYPE,  SOLOSNAKE_APP_OPENGL_REQ,           AO_SAFESTART_WINDOW_TYPE_COMMENT );
        set_if_not_present( AO_FIRST_SCREEN,           AO_FIRST_SCREEN_VALUE,              AO_FIRST_SCREEN_COMMENT );
        set_if_not_present( AO_USER_SETTINGS_FILENAME, AO_USER_SETTINGS_FILENAME_VALUE,    AO_USER_SETTINGS_FILENAME_COMMENT );
        
        set_if_not_present( UO_FULLSCREEN,          UO_FULLSCREEN_DEFAULT,      UO_FULLSCREEN_COMMENT );
        set_if_not_present( UO_WINDOW_TYPE,         SOLOSNAKE_APP_OPENGL_REQ,   UO_WINDOW_TYPE_COMMENT );
        set_if_not_present( UO_CLEAN_RUN,           UO_CLEAN_RUN_DEFAULT,       UO_CLEAN_RUN_COMMENT );
        set_if_not_present( UO_WINDOW_QUALITY,      UO_WINDOW_QUALITY_DEFAULT,  UO_WINDOW_QUALITY_COMMENT );
        set_if_not_present( UO_SOUND_MAX_SOUNDS,    UO_SOUND_MAX_SOUNDS_VALUE,  UO_SOUND_MAX_SOUNDS_COMMENT );
        set_if_not_present( UO_SOUND_MODEL,         UO_SOUND_MODEL_DEFAULT,     UO_SOUND_MODEL_COMMENT );        
        set_if_not_present( UO_WINDOW_HEIGHT,       AO_SAFESTART_HEIGHT_VALUE,  UO_WINDOW_HEIGHT_COMMENT );
        set_if_not_present( UO_WINDOW_WIDTH,        AO_SAFESTART_WIDTH_VALUE,   UO_WINDOW_WIDTH_COMMENT );
        
        set_if_not_present( BLUE_HOST_IP,           BLUE_HOST_IP_DEFAULT,       BLUE_HOST_IP_COMMENT );
        set_if_not_present( BLUE_HOST_PORT,         BLUE_DEFAULT_HOST_PORT,     BLUE_HOST_PORT_COMMENT );
        set_if_not_present( BLUE_NUM_PLAYERS,       BLUE_DEFAULT_NUM_PLAYERS,   BLUE_NUM_PLAYERS_COMMENT );
        set_if_not_present( BLUE_GUI_LANGUAGE,      BLUE_DEFAULT_GUI_LANGUAGE,  BLUE_GUI_LANGUAGE_COMMENT );
        
        set_if_not_present( BLUE_GAME_ATAXITE_MODEL,         BLUE_GAME_ATAXITE_MODEL_DEFAULT,       BLUE_GAME_ATAXITE_COMMENT );
        set_if_not_present( BLUE_GAME_CAMERA_ARMLENGTH,      BLUE_GAME_CAMERA_ARMLENGTH_DEFAULT,    BLUE_GAME_CAMERA_ARMLENGTH_COMMENT );
        set_if_not_present( BLUE_GAME_CAMERA_CEILING_HEIGHT, BLUE_GAME_CAMERA_CEILING_DEFAULT,      BLUE_GAME_CAMERA_CEILING_COMMENT );
        set_if_not_present( BLUE_GAME_CAMERA_ROTATION,       BLUE_GAME_CAMERA_ROTATION_DEFAULT,     BLUE_GAME_CAMERA_ROTATION_COMMENT );
        set_if_not_present( BLUE_GAME_CAMERA_RADIUS,         BLUE_GAME_CAMERA_RADIUS_DEFAULT,       BLUE_GAME_CAMERA_RADIUS_COMMENT );
        set_if_not_present( BLUE_GAME_CAMERA_TILT,           BLUE_GAME_CAMERA_TILT_DEFAULT,         BLUE_GAME_CAMERA_TILT_COMMENT );
        set_if_not_present( BLUE_GAME_CAMERA_ZOOMFACTOR,     BLUE_GAME_CAMERA_ZOOMFACTOR_DEFAULT,   BLUE_GAME_CAMERA_ZOOMFACTOR_COMMENT );
        set_if_not_present( BLUE_GAME_CHONDRITE_MODEL,       BLUE_GAME_CHONDRITE_MODEL_DEFAULT,     BLUE_GAME_CHONDRITE_COMMENT );
        set_if_not_present( BLUE_GAME_EXPLODE_BACK_RGBA_TEX, BLUE_GAME_EXPLODE_BACK_RGBA_TEX_DEFAULT, BLUE_GAME_EXPLODE_BACK_RGBA_TEX_COMMENT );
        set_if_not_present( BLUE_GAME_EXPLODE_BACK_WARP_TEX, BLUE_GAME_EXPLODE_BACK_WARP_TEX_DEFAULT, BLUE_GAME_EXPLODE_BACK_WARP_TEX_COMMENT );
        set_if_not_present( BLUE_GAME_EXPLODE_HEIGHT,        BLUE_GAME_EXPLODE_HEIGHT_DEFAULT,      BLUE_GAME_EXPLODE_HEIGHT_COMMENT );
        set_if_not_present( BLUE_GAME_EXPLODE_RING_RGBA_TEX, BLUE_GAME_EXPLODE_RING_RGBA_TEX_DEFAULT, BLUE_GAME_EXPLODE_RING_RGBA_TEX_COMMENT );
        set_if_not_present( BLUE_GAME_EXPLODE_RING_WARP_TEX, BLUE_GAME_EXPLODE_RING_WARP_TEX_DEFAULT, BLUE_GAME_EXPLODE_RING_WARP_TEX_COMMENT );
        set_if_not_present( BLUE_GAME_EXPLODE_WAV,           BLUE_GAME_EXPLODE_WAV_DEFAULT,         BLUE_GAME_EXPLODE_WAV_COMMENT );
        set_if_not_present( BLUE_GAME_FILE,                  BLUE_GAME_FILE_DEFAULT,                BLUE_GAME_FILE_COMMENT );
        set_if_not_present( BLUE_GAME_KAMACITE_MODEL,        BLUE_GAME_KAMACITE_MODEL_DEFAULT,      BLUE_GAME_KAMACITE_COMMENT );
        set_if_not_present( BLUE_GAME_PANGUITE_MODEL,        BLUE_GAME_PANGUITE_MODEL_DEFAULT,      BLUE_GAME_PANGUITE_COMMENT );
        set_if_not_present( BLUE_GAME_PULSE_PERIOD,          BLUE_GAME_PULSE_PERIOD_DEFAULT,        BLUE_GAME_PULSE_PERIOD_COMMENT );
        set_if_not_present( BLUE_GAME_RANDOMSEED,            BLUE_GAME_RANDOMSEED_DEFAULT,          BLUE_GAME_RANDOMSEED_COMMENT );
        set_if_not_present( BLUE_GAME_SELECTED_DECK,         BLUE_GAME_SELECTED_DECK_DEFAULT,       BLUE_GAME_SELECTED_DECK_COMMENT );
        set_if_not_present( BLUE_GAME_SENSEFX_RGBA_TEX,      BLUE_GAME_SENSEFX_RGBA_TEX_DEFAULT,    BLUE_GAME_SENSEFX_RGBA_TEX_COMMENT );
        set_if_not_present( BLUE_GAME_SENSEFX_WARP_TEX,      BLUE_GAME_SENSEFX_WARP_TEX_DEFAULT,    BLUE_GAME_SENSEFX_WARP_TEX_COMMENT );
        set_if_not_present( BLUE_GAME_SENSEFX_FACTOR,        BLUE_GAME_SENSEFX_FACTOR_DEFAULT,      BLUE_GAME_SENSEFX_FACTOR_COMMENT );
        set_if_not_present( BLUE_GAME_SENSEFX_HEIGHT,        BLUE_GAME_SENSEFX_HEIGHT_DEFAULT,      BLUE_GAME_SENSEFX_HEIGHT_COMMENT );
        set_if_not_present( BLUE_GAME_SENSEFX_WAV,           BLUE_GAME_SENSEFX_WAV_DEFAULT,         BLUE_GAME_SENSEFX_WAV_COMMENT );
        set_if_not_present( BLUE_GAME_SKYBOXROTATEX,         BLUE_GAME_SKYBOXROTATEX_DEFAULT,       BLUE_GAME_SKYBOXROTATEX_COMMENT );
        set_if_not_present( BLUE_GAME_SKYBOXROTATEY,         BLUE_GAME_SKYBOXROTATEY_DEFAULT,       BLUE_GAME_SKYBOXROTATEY_COMMENT );
        
        set_if_not_present( BLUE_GAME_TILEALPHA,             BLUE_GAME_TILEALPHA_DEFAULT,           BLUE_GAME_TILEALPHA_COMMENT );
        set_if_not_present( BLUE_GAME_TILESHRINKFACTOR,      BLUE_GAME_TILESHRINKFACTOR_DEFAULT,    BLUE_GAME_TILESHRINKFACTOR_COMMENT );
        set_if_not_present( BLUE_GAME_TILETEXTURE,           BLUE_GAME_TILETEXTURE_DEFAULT,         BLUE_GAME_TILETEXTURE_COMMENT );
        set_if_not_present( BLUE_GAME_TILETEXTURE_X,         BLUE_GAME_TILETEXTURE_X_DEFAULT,       BLUE_GAME_TILETEXTURE_X_COMMENT );
        set_if_not_present( BLUE_GAME_TILETEXTURE_XW,        BLUE_GAME_TILETEXTURE_XW_DEFAULT,      BLUE_GAME_TILETEXTURE_XW_COMMENT );
        set_if_not_present( BLUE_GAME_TILETEXTURE_Y,         BLUE_GAME_TILETEXTURE_Y_DEFAULT,       BLUE_GAME_TILETEXTURE_Y_COMMENT );
        set_if_not_present( BLUE_GAME_TILETEXTURE_Y,         BLUE_GAME_TILETEXTURE_YH_DEFAULT,      BLUE_GAME_TILETEXTURE_YH_COMMENT );

        set_if_not_present( BLUE_GAME_MAP_TILEALPHA,         BLUE_GAME_MAP_TILEALPHA_DEFAULT,        BLUE_GAME_MAP_TILEALPHA_COMMENT );
        set_if_not_present( BLUE_GAME_MAP_TILESHRINKFACTOR,  BLUE_GAME_MAP_TILESHRINKFACTOR_DEFAULT, BLUE_GAME_MAP_TILESHRINKFACTOR_COMMENT );
        set_if_not_present( BLUE_GAME_MAP_TILETEXTURE,       BLUE_GAME_MAP_TILETEXTURE_DEFAULT,      BLUE_GAME_MAP_TILETEXTURE_COMMENT );
        set_if_not_present( BLUE_GAME_MAP_TILETEXTURE_X,     BLUE_GAME_MAP_TILETEXTURE_X_DEFAULT,    BLUE_GAME_MAP_TILETEXTURE_X_COMMENT );
        set_if_not_present( BLUE_GAME_MAP_TILETEXTURE_XW,    BLUE_GAME_MAP_TILETEXTURE_XW_DEFAULT,   BLUE_GAME_MAP_TILETEXTURE_XW_COMMENT );
        set_if_not_present( BLUE_GAME_MAP_TILETEXTURE_Y,     BLUE_GAME_MAP_TILETEXTURE_Y_DEFAULT,    BLUE_GAME_MAP_TILETEXTURE_Y_COMMENT );
        set_if_not_present( BLUE_GAME_MAP_TILETEXTURE_Y,     BLUE_GAME_MAP_TILETEXTURE_YH_DEFAULT,   BLUE_GAME_MAP_TILETEXTURE_YH_COMMENT );

        set_if_not_present( BLUE_GAME_VIEWANGLE,             BLUE_GAME_VIEWANGLE_DEFAULT,           BLUE_GAME_VIEWANGLE_COMMENT );
        set_if_not_present( BLUE_GAME_WIDGET_STYLEFILE,      BLUE_GAME_WIDGET_STYLEFILE_DEFAULT,    BLUE_GAME_WIDGET_STYLEFILE_COMMENT );
        set_if_not_present( BLUE_GAME_Z_NEAR,                BLUE_GAME_Z_NEAR_DEFAULT,              BLUE_GAME_Z_NEAR_COMMENT );   

        set_if_not_present( BLUE_GAME_BOARDFILENAME,         BLUE_GAME_BOARDFILENAME_DEFAULT,       BLUE_GAME_BOARDFILENAME_COMMENT );   
        //set_if_not_present( BLUE_GAME_PROFILE_NAME,          BLUE_GAME_PROFILE_NAME_DEFAULT,        BLUE_GAME_PROFILE_NAME_COMMENT );
    }

    //! @param settingsFile   The location of the file to load which contains
    //!                       the users settings. These are all the settings
    //!                       the app will use, including data directories.
    //! @param userSaveDir    In the event that no save directory is specified
    //!                       in the user settings, this is the directory
    //!                       that will be used and recorded.
    //! @param gameDataDirs   In the event that no data directory is specified
    //!                       in the user settings, this is the data directory
    //!                       which will be used and recorded in the user
    //!                       settings file.
    user_settings::user_settings( const std::string& settingsFile,
                                  const std::string& userSaveDir,
                                  const std::string& gameDataDirs )
        : src_filename_( settingsFile )
    {
        add_settings();
        set( AO_BUILD_VERSION, get_buildstamp(), AO_BUILD_VERSION_COMMENT );

        if( settingsFile.empty() )
        {
            ss_err( "User settings file name is empty." );
            ss_throw( "User settings file name is empty." );
        }
        
        if( userSaveDir.empty() )
        {
            ss_err( "User save directory name is empty." );
            ss_throw( "User save directory name is empty." );
        }
        
        if( gameDataDirs.empty() )
        {
            ss_err( "Game data directory name is empty." );
            ss_throw( "Game data directory name is empty." );
        }
        
        // Check if it exists: if it does NOT, write the default file, and
        // continue as if that were the original file.
        if( false == file_exists( settingsFile ) )
        {
            // Create and save defaults:
            reset_to_safe_defaults();
            save();
        }

        if( load_from_xml( settingsFile.c_str() ) )
        {
            // We add the settings again because we may have loaded an existing file
            // which contained some missing settings.
            add_settings();

            // If they are not present in the settings, set the user
            // save location and data directory.
            set_if_not_present( UO_USER_SAVE_DIR,  userSaveDir,  UO_USER_SAVE_DIR_COMMENT );
            set_if_not_present( UO_DATA_ROOT_DIRS, gameDataDirs, UO_DATA_ROOT_DIRS_COMMENT );

            // Read them back and see if they are safe to use:
            const std::string dataDirs = value( UO_DATA_ROOT_DIRS );
            const std::string userDir  = value( UO_USER_SAVE_DIR );

            // See can we make a datapaths with the dirs given: throws
            // if unable to use dirs.
            try
            {
                const datapaths tempPaths( dataDirs );
            }
            catch( ... )
            {
                ss_err( "The value for '" UO_DATA_ROOT_DIRS "' given (",
                        dataDirs,
                        ") is not valid." );

                ss_throw( "Invalid " UO_DATA_ROOT_DIRS " found. Please "
                          "correct this value in XML or "
                          "reinstall " BLUE_APP_MAJOR_MINOR_PATCH_VERSIONED_NAME );
            }

            if( !is_good_directory( userDir ) )
            {
                ss_err( "The value for '" UO_USER_SAVE_DIR "' given (",
                        userDir,
                        ") is not a valid directory." );

                ss_throw( "Invalid " UO_USER_SAVE_DIR " found. Please "
                          "correct this value in XML or "
                          "reinstall " BLUE_APP_MAJOR_MINOR_PATCH_VERSIONED_NAME );
            }


            if( !is_boolean( UO_FULLSCREEN ) )
            {
                ss_throw( UO_FULLSCREEN " value not recognised. Accepted values are "
                          "'true' or 'false'." );
            }

            if( ( ! is_boolean( UO_CLEAN_RUN ) ) || ( ! this->value( UO_CLEAN_RUN ).is_true() ) )
            {
                // Last run did not shut down cleanly. Use safe defaults.
                reset_to_safe_defaults();
            }

            const std::string recordRunSettings = std::string( value( UO_USER_SAVE_DIR ) )
                                                  + BLUE_LOGGED_USERSETTINGS;
            save_to_xml( recordRunSettings.c_str() );

            if( g_ssLogLvl > SS_LOG_WRN )
            {
                std::ostringstream os;
                print_to( os );
                ss_log( "\nUser settings for this run will be :\n", os.str() );
            }
        }
        else
        {
            ss_err( "Unable to load '",
                    settingsFile,
                    "', (the file containing per-user configuration settings)." );
            ss_throw( "Unable to load user's settings." );
        }
    }

    user_settings::~user_settings()
    {
    }

    // Responsible for setting up the 'safe' defaults for a user settings file.
    void user_settings::reset_to_safe_defaults()
    {
        set( UO_FULLSCREEN,             UO_FULLSCREEN_DEFAULT,              UO_FULLSCREEN_COMMENT );
        set( UO_WINDOW_TYPE,            SOLOSNAKE_APP_OPENGL_REQ,           UO_WINDOW_TYPE_COMMENT );
        set( UO_CLEAN_RUN,              UO_CLEAN_RUN_DEFAULT,               UO_CLEAN_RUN_COMMENT );
        set( UO_WINDOW_HEIGHT,          AO_SAFESTART_HEIGHT_VALUE,          UO_WINDOW_HEIGHT_COMMENT );
        set( UO_WINDOW_WIDTH,           AO_SAFESTART_WIDTH_VALUE,           UO_WINDOW_WIDTH_COMMENT );
        set( AO_SAFESTART_HEIGHT,       AO_SAFESTART_HEIGHT_VALUE,          AO_SAFESTART_HEIGHT_COMMENT );
        set( AO_SAFESTART_WIDTH,        AO_SAFESTART_WIDTH_VALUE,           AO_SAFESTART_WIDTH_COMMENT );
        set( AO_FIRST_SCREEN,           AO_FIRST_SCREEN_VALUE,              AO_FIRST_SCREEN_COMMENT );
        set( AO_SAFESTART_WINDOW_TYPE,  SOLOSNAKE_APP_OPENGL_REQ,           AO_SAFESTART_WINDOW_TYPE_COMMENT );
        set( AO_FIRST_SCREEN,           AO_FIRST_SCREEN_VALUE,              AO_FIRST_SCREEN_COMMENT );
        set( AO_USER_SETTINGS_FILENAME, AO_USER_SETTINGS_FILENAME_VALUE,    AO_USER_SETTINGS_FILENAME_COMMENT );
        set( AO_APP_TITLE,              BLUE_APP_MAJOR_MINOR_PATCH_VERSIONED_NAME, AO_APP_TITLE_COMMENT );
    }

    void user_settings::save() const
    {
        if(src_filename_.empty())
        {
            ss_err( "Use settings file name is empty." );
            ss_throw( "Error saving user settings file." );
        }
        
        if( !save_to_xml( src_filename_.c_str() ) )
        {
            ss_err( "Error writing ", src_filename_ );
            ss_throw( "Error writing user settings file." );
        }
    }

    const std::string user_settings::data_dirs() const
    {
        return value( UO_DATA_ROOT_DIRS );
    }

    void user_settings::print_to( std::ostream& os ) const
    {
        std::for_each( this->values().cbegin(),
                       this->values().cend(),
                       [&]( const persistance::keysvalues_t::value_type & i )
        { os << i.first << "\t=\t" << i.second << '\n'; } );
    }
}
