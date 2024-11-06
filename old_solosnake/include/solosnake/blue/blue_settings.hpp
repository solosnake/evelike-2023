#ifndef blue_settings_hpp
#define blue_settings_hpp

/*
//#include "blue_version_cmake_generated.hpp"
#include "solosnake/blue/blue_fixedpoint.hpp"

// Application definitions:
#define SOLOSNAKE_APP_COMPANY               "solosnake"
#define SOLOSNAKE_DIR_SLASH                 "\\"
#define SOLOSNAKE_APP_OPENGL_REQ            "opengl 3.3"

// User persistance options:
#define UO_FULLSCREEN                       "fullscreen"
#define UO_FULLSCREEN_DEFAULT               "false"
#define UO_FULLSCREEN_COMMENT               "Run in fullscreen, true/false, Default: " UO_FULLSCREEN_DEFAULT
#define UO_WINDOW_WIDTH                     "window-width"
#define UO_WINDOW_WIDTH_COMMENT             "Width of window in pixels. Default: " "1280"
#define UO_WINDOW_HEIGHT                    "window-height"
#define UO_WINDOW_HEIGHT_COMMENT            "Height of window in pixels. Default: " "720"
#define UO_WINDOW_TYPE                      "window-type"
#define UO_WINDOW_TYPE_COMMENT              "Graphical window type, opengl, Default: " SOLOSNAKE_APP_OPENGL_REQ
#define UO_WINDOW_QUALITY                   "window-quality"
#define UO_WINDOW_QUALITY_HIGHEST           "Highest"
#define UO_WINDOW_QUALITY_HIGH              "High"
#define UO_WINDOW_QUALITY_MEDIUM            "Medium"
#define UO_WINDOW_QUALITY_LOW               "Low"
#define UO_WINDOW_QUALITY_DEFAULT           UO_WINDOW_QUALITY_HIGH
#define UO_WINDOW_QUALITY_COMMENT           "Quality level of renderer. Default: " UO_WINDOW_QUALITY_DEFAULT ", possible values are: " UO_WINDOW_QUALITY_LOW " " UO_WINDOW_QUALITY_MEDIUM " " UO_WINDOW_QUALITY_HIGH " " UO_WINDOW_QUALITY_HIGHEST
#define UO_SOUND_MAX_SOUNDS                 "sounds-max"
#define UO_SOUND_MAX_SOUNDS_VALUE           "64"
#define UO_SOUND_MAX_SOUNDS_COMMENT         "Maximum number of sounds that can be played at once, Default: " UO_SOUND_MAX_SOUNDS_VALUE
#define UO_SOUND_MODEL                      "sounds-model"
#define UO_SOUND_MODEL_LINEAR               "Linear"
#define UO_SOUND_MODEL_INVERSE              "Inverse"
#define UO_SOUND_MODEL_EXPONENTIAL          "Exponential"
#define UO_SOUND_MODEL_DEFAULT              UO_SOUND_MODEL_LINEAR
#define UO_SOUND_MODEL_COMMENT              "How falloff is modelled. Default: " UO_SOUND_MODEL_DEFAULT ", possible values are: " UO_SOUND_MODEL_LINEAR " " UO_SOUND_MODEL_INVERSE " " UO_SOUND_MODEL_EXPONENTIAL
#define UO_DATA_ROOT_DIRS                   "data-dirs"
#define UO_DATA_ROOT_DIRS_COMMENT           "Semi-colon separated list of directories to search for application data in."
#define UO_DATA_ROOT_SEPERATOR              ";"
#define UO_USER_SAVE_DIR                    "user-save-dir"
#define UO_USER_SAVE_DIR_COMMENT            "Root path to user's save files."
#define UO_CLEAN_RUN                        "clean-run"
#define UO_CLEAN_RUN_DEFAULT                "false"
#define UO_CLEAN_RUN_COMMENT                "Records if the application ran successfully last run. true/false. Default: " UO_CLEAN_RUN_DEFAULT

// Application persistance options:
#define AO_USER_SAVE_PATH                   "user-save-path"
#define AO_USER_SAVE_PATH_COMMENT           "Directory user save data is saved to. (Users setting file is an exception to this)."
#define AO_APP_PATH                         "app-path"
#define AO_APP_PATH_COMMENT                 "Application install location (default initial data directory)."
#define AO_USER_SETTINGS_FILENAME           "user-settings-filename"
#define AO_USER_SETTINGS_FILENAME_VALUE     "blue-user-settings.xml"
#define AO_USER_SETTINGS_FILENAME_COMMENT   "User's settings file."
#define AO_SAFESTART_HEIGHT                 "safestart-height"
#define AO_SAFESTART_HEIGHT_VALUE           "600"
#define AO_SAFESTART_HEIGHT_COMMENT         "Default safe window starting height. Default: " AO_SAFESTART_HEIGHT_VALUE
#define AO_SAFESTART_WIDTH                  "safestart-width"
#define AO_SAFESTART_WIDTH_VALUE            "800"
#define AO_SAFESTART_WIDTH_COMMENT          "Default safe window starting width. Default: " AO_SAFESTART_WIDTH_VALUE
#define AO_APP_TITLE                        "app-title"
#define AO_APP_TITLE_COMMENT                "Application title."
#define AO_SAFESTART_WINDOW_TYPE            "safestart-window-type"
#define AO_SAFESTART_WINDOW_TYPE_COMMENT    "Default safe graphical window type. opengl. Default: " SOLOSNAKE_APP_OPENGL_REQ
#define AO_BUILD_VERSION                    "build-version"
#define AO_BUILD_VERSION_COMMENT            "Build version."
#define AO_FIRST_SCREEN                     "first-screen"
#define AO_FIRST_SCREEN_VALUE               "title.xml"
#define AO_FIRST_SCREEN_COMMENT             "The first screen shown by the application. Default: " AO_FIRST_SCREEN_VALUE

// Game options entry names:
#define BLUE_HOST_IP                        "host-ip"
#define BLUE_HOST_IP_DEFAULT                "127.0.0.1"
#define BLUE_HOST_IP_COMMENT                "The host ip address."
#define BLUE_HOST_PORT                      "host-port"
#define BLUE_DEFAULT_HOST_PORT              "1213"
#define BLUE_HOST_PORT_COMMENT              "The host's port number. Default: " BLUE_DEFAULT_HOST_PORT
#define BLUE_NUM_PLAYERS                    "n-players"
#define BLUE_DEFAULT_NUM_PLAYERS            "1"
#define BLUE_NUM_PLAYERS_COMMENT            "The number of players. Default: " BLUE_DEFAULT_NUM_PLAYERS
#define BLUE_GUI_LANGUAGE                   "gui-language-file"
#define BLUE_DEFAULT_GUI_LANGUAGE           "en.xml"  // Empty, no translation takes place.
#define BLUE_GUI_LANGUAGE_COMMENT           "The language settings used to match GUI text. "\
                                            "When no language file is specified no translation takes place."

// There is intentionally no default game file.
#define BLUE_GAME_FILE                      "game-file"
#define BLUE_GAME_FILE_DEFAULT              ""
#define BLUE_GAME_FILE_COMMENT              "The game file to load and play."

// Game settings
#define BLUE_GAMEFILE_COMMENT                       "The name of the game file to load."
#define BLUE_GAME_RANDOMSEED                        "game-srand"
#define BLUE_GAME_RANDOMSEED_DEFAULT                "1234"
#define BLUE_GAME_RANDOMSEED_COMMENT                "The unsigned integer used to initialise the game's random seed."
#define BLUE_GAME_CAMERA_TILT                       "game-camera-tilt"
#define BLUE_GAME_CAMERA_TILT_DEFAULT               "0.78"
#define BLUE_GAME_CAMERA_TILT_COMMENT               "The tilt of the camera in radians. Default: " BLUE_GAME_CAMERA_TILT_DEFAULT
#define BLUE_GAME_CAMERA_ROTATION                   "game-camera-rotation"
#define BLUE_GAME_CAMERA_ROTATION_DEFAULT           "0.0"
#define BLUE_GAME_CAMERA_ROTATION_COMMENT           "The rotation of the camera in radians. Default: " BLUE_GAME_CAMERA_ROTATION_DEFAULT
#define BLUE_GAME_CAMERA_ARMLENGTH                  "game-camera-armlength"
#define BLUE_GAME_CAMERA_ARMLENGTH_DEFAULT          "6.0"
#define BLUE_GAME_CAMERA_ARMLENGTH_COMMENT          "The distance of camera from lookat point. Default: " BLUE_GAME_CAMERA_ARMLENGTH_DEFAULT
#define BLUE_GAME_CAMERA_ZOOMFACTOR                 "game-camera-zoomfactor"
#define BLUE_GAME_CAMERA_ZOOMFACTOR_DEFAULT         "1.0"
#define BLUE_GAME_CAMERA_ZOOMFACTOR_COMMENT         "Factor applied to camera zooming. Default: " BLUE_GAME_CAMERA_ZOOMFACTOR_DEFAULT
#define BLUE_GAME_CAMERA_RADIUS                     "game-camera-radius"
#define BLUE_GAME_CAMERA_RADIUS_DEFAULT             "1.0"
#define BLUE_GAME_CAMERA_RADIUS_COMMENT             "Camera ball radius. Default: " BLUE_GAME_CAMERA_RADIUS_DEFAULT
#define BLUE_GAME_CAMERA_CEILING_HEIGHT             "game-camera-ceiling-height"
#define BLUE_GAME_CAMERA_CEILING_DEFAULT            "10.0"
#define BLUE_GAME_CAMERA_CEILING_COMMENT            "How high above the board the camera can go. Default: " BLUE_GAME_CAMERA_CEILING_HEIGHT
#define BLUE_GAME_TILESHRINKFACTOR                  "game-tile-shrink-factor"
#define BLUE_GAME_TILESHRINKFACTOR_DEFAULT          "1.0"//"0.975"
#define BLUE_GAME_TILESHRINKFACTOR_COMMENT          "Scales the hexagon tile up or down (creates the spaces). Can be between 0.0 and 1.0. Default: " BLUE_GAME_TILESHRINKFACTOR_DEFAULT
#define BLUE_GAME_TILETEXTURE                       "game-tile-texture"
#define BLUE_GAME_TILETEXTURE_DEFAULT               "tile.bmp"
#define BLUE_GAME_TILETEXTURE_COMMENT               "The texture used to render the board tiles."
#define BLUE_GAME_TILETEXTURE_X                     "game-tile-texture-x"
#define BLUE_GAME_TILETEXTURE_X_DEFAULT             "0.5"
#define BLUE_GAME_TILETEXTURE_X_COMMENT             "Centre coordinate x for tile texture."
#define BLUE_GAME_TILETEXTURE_Y                     "game-tile-texture-y"
#define BLUE_GAME_TILETEXTURE_Y_DEFAULT             "0.5"
#define BLUE_GAME_TILETEXTURE_Y_COMMENT             "Centre coordinate y for tile texture."
#define BLUE_GAME_TILETEXTURE_XW                    "game-tile-texture-xw"
#define BLUE_GAME_TILETEXTURE_XW_DEFAULT            "1.0"
#define BLUE_GAME_TILETEXTURE_XW_COMMENT            "Texture x width (0.0 to 1.0)."
#define BLUE_GAME_TILETEXTURE_YH                    "game-tile-texture-xw"
#define BLUE_GAME_TILETEXTURE_YH_DEFAULT            "1.0"
#define BLUE_GAME_TILETEXTURE_YH_COMMENT            "Texture y height (0.0 to 1.0)."
#define BLUE_GAME_TILEALPHA                         "game-tile-alpha"
#define BLUE_GAME_TILEALPHA_DEFAULT                 "1.00"
#define BLUE_GAME_TILEALPHA_COMMENT                 "How opaque the board is. Default: " BLUE_GAME_TILEALPHA_DEFAULT

#define BLUE_GAME_MAP_TILESHRINKFACTOR              "map-tile-shrink-factor"
#define BLUE_GAME_MAP_TILESHRINKFACTOR_DEFAULT      "1.0"
#define BLUE_GAME_MAP_TILESHRINKFACTOR_COMMENT      "Scales the map hexagon tile up or down (creates the spaces). Can be between 0.0 and 1.0. Default: " BLUE_GAME_MAP_TILESHRINKFACTOR_DEFAULT
#define BLUE_GAME_MAP_TILETEXTURE                   "map-tile-texture"
#define BLUE_GAME_MAP_TILETEXTURE_DEFAULT           "maptile.bmp"
#define BLUE_GAME_MAP_TILETEXTURE_COMMENT           "The texture used to render the map board tiles."
#define BLUE_GAME_MAP_TILETEXTURE_X                 "map-tile-texture-x"
#define BLUE_GAME_MAP_TILETEXTURE_X_DEFAULT         "0.5"
#define BLUE_GAME_MAP_TILETEXTURE_X_COMMENT         "Centre coordinate x for map tile texture."
#define BLUE_GAME_MAP_TILETEXTURE_Y                 "map-tile-texture-y"
#define BLUE_GAME_MAP_TILETEXTURE_Y_DEFAULT         "0.5"
#define BLUE_GAME_MAP_TILETEXTURE_Y_COMMENT         "Centre coordinate y for map tile texture."
#define BLUE_GAME_MAP_TILETEXTURE_XW                "map-tile-texture-xw"
#define BLUE_GAME_MAP_TILETEXTURE_XW_DEFAULT        "1.0"
#define BLUE_GAME_MAP_TILETEXTURE_XW_COMMENT        "Map texture x width (0.0 to 1.0)."
#define BLUE_GAME_MAP_TILETEXTURE_YH                "map-tile-texture-xw"
#define BLUE_GAME_MAP_TILETEXTURE_YH_DEFAULT        "1.0"
#define BLUE_GAME_MAP_TILETEXTURE_YH_COMMENT        "Map texture y height (0.0 to 1.0)."
#define BLUE_GAME_MAP_TILEALPHA                     "map-tile-alpha"
#define BLUE_GAME_MAP_TILEALPHA_DEFAULT             "1.00"
#define BLUE_GAME_MAP_TILEALPHA_COMMENT             "How opaque the map board is. Default: " BLUE_GAME_MAP_TILEALPHA

//#define BLUE_GAME_PROFILE_NAME                      "profilename"
//#define BLUE_GAME_PROFILE_NAME_DEFAULT              ""
//#define BLUE_GAME_PROFILE_NAME_COMMENT              "Name of currently active profile."

#define BLUE_GAME_BOARDFILENAME                     "boardfilename"
#define BLUE_GAME_BOARDFILENAME_DEFAULT             ""
#define BLUE_GAME_BOARDFILENAME_COMMENT             "Temporary value used to communicate the board file's name."

#define BLUE_GAME_WIDGET_STYLEFILE                  "stylefile"
#define BLUE_GAME_WIDGET_STYLEFILE_DEFAULT          "bluestyle.xml"
#define BLUE_GAME_WIDGET_STYLEFILE_COMMENT          "Name of XML style file to use as default wiget style. Default: " BLUE_GAME_WIDGET_STYLEFILE_DEFAULT
#define BLUE_GAME_VIEWANGLE                         "game-viewangle"
#define BLUE_GAME_VIEWANGLE_DEFAULT                 "45"
#define BLUE_GAME_VIEWANGLE_COMMENT                 "Viewing angle in degrees"
#define BLUE_GAME_Z_NEAR                            "game-z-near"
#define BLUE_GAME_Z_NEAR_DEFAULT                    "0.1"
#define BLUE_GAME_Z_NEAR_COMMENT                    "Z near value for game view. Default: " BLUE_GAME_Z_NEAR_DEFAULT
#define BLUE_GAME_SKYBOXROTATEX                     "skybox-rotation-rate-x"
#define BLUE_GAME_SKYBOXROTATEX_COMMENT             "Rotation rate of skybox in x axis, radians per second. Default: " BLUE_GAME_SKYBOXROTATEX_DEFAULT
#define BLUE_GAME_SKYBOXROTATEX_DEFAULT             "0.00071"
#define BLUE_GAME_SKYBOXROTATEY                     "skybox-rotation-rate-y"
#define BLUE_GAME_SKYBOXROTATEY_COMMENT             "Rotation rate of skybox in y axis, radians per second. Default: " BLUE_GAME_SKYBOXROTATEY_DEFAULT
#define BLUE_GAME_SKYBOXROTATEY_DEFAULT             "0.00093"
#define BLUE_GAME_PANGUITE_MODEL                    "panguite-model"
#define BLUE_GAME_PANGUITE_MODEL_DEFAULT            "Panguitemodel.lua"
#define BLUE_GAME_PANGUITE_COMMENT                  "Model used for Panguite asteroids. Default: " BLUE_GAME_PANGUITE_MODEL_DEFAULT
#define BLUE_GAME_KAMACITE_MODEL                    "kamacite-model"
#define BLUE_GAME_KAMACITE_MODEL_DEFAULT            "Kamacitemodel.lua"
#define BLUE_GAME_KAMACITE_COMMENT                  "Model used for Kamacite asteroids. Default: " BLUE_GAME_KAMACITE_MODEL_DEFAULT
#define BLUE_GAME_ATAXITE_MODEL                     "ataxite-model"
#define BLUE_GAME_ATAXITE_MODEL_DEFAULT             "Ataxitemodel.lua"
#define BLUE_GAME_ATAXITE_COMMENT                   "Model used for Ataxite asteroids. Default: " BLUE_GAME_ATAXITE_MODEL_DEFAULT
#define BLUE_GAME_CHONDRITE_MODEL                   "chondrite-model"
#define BLUE_GAME_CHONDRITE_MODEL_DEFAULT           "Chondritemodel.lua"
#define BLUE_GAME_CHONDRITE_COMMENT                 "Model used for Chondrite asteroids. Default: " BLUE_GAME_CHONDRITE_MODEL_DEFAULT
#define BLUE_GAME_SENSEFX_HEIGHT                    "fxsense-height"
#define BLUE_GAME_SENSEFX_HEIGHT_COMMENT            "Height of sense sfx plane on board. Default :" BLUE_GAME_SENSEFX_HEIGHT_DEFAULT
#define BLUE_GAME_SENSEFX_HEIGHT_DEFAULT            "0.15"
#define BLUE_GAME_SENSEFX_WAV                       "fxsense-wav"
#define BLUE_GAME_SENSEFX_WAV_COMMENT               "Sound file played when sensing. Default :" BLUE_GAME_SENSEFX_WAV_DEFAULT
#define BLUE_GAME_SENSEFX_WAV_DEFAULT               "sub_bass_whoosh.wav"
#define BLUE_GAME_SENSEFX_RGBA_TEX                  "fxsense-rgba-tex"
#define BLUE_GAME_SENSEFX_RGBA_TEX_COMMENT          "Texture for sense sfx rgba data. Default: " BLUE_GAME_SENSEFX_RGBA_TEX_DEFAULT
#define BLUE_GAME_SENSEFX_RGBA_TEX_DEFAULT          "sfx-sense-rgba.bmp"
#define BLUE_GAME_SENSEFX_WARP_TEX                  "fxsense-warp-tex"
#define BLUE_GAME_SENSEFX_WARP_TEX_COMMENT          "Texture for sense sfx warp data. Default: " BLUE_GAME_SENSEFX_WARP_TEX_DEFAULT
#define BLUE_GAME_SENSEFX_WARP_TEX_DEFAULT          "sfx-sense-warp.bmp"
#define BLUE_GAME_EXPLODE_HEIGHT                    "explosion-height"
#define BLUE_GAME_EXPLODE_HEIGHT_COMMENT            "Height of explosion sfx plane on board. Default :" BLUE_GAME_EXPLODE_HEIGHT_DEFAULT
#define BLUE_GAME_EXPLODE_HEIGHT_DEFAULT            "0.25"
#define BLUE_GAME_EXPLODE_WAV                       "explosion-wav"
#define BLUE_GAME_EXPLODE_WAV_COMMENT               "Sound file played when machine explodes. Default :" BLUE_GAME_EXPLODE_WAV_DEFAULT
#define BLUE_GAME_EXPLODE_WAV_DEFAULT               "explosion.wav"
#define BLUE_GAME_EXPLODE_BACK_RGBA_TEX             "explode-rgba-tex"
#define BLUE_GAME_EXPLODE_BACK_RGBA_TEX_COMMENT     "Texture for explosion sfx backdrop rgba data. Default: " BLUE_GAME_EXPLODE_BACK_RGBA_TEX_DEFAULT
#define BLUE_GAME_EXPLODE_BACK_RGBA_TEX_DEFAULT     "sfx-explosion-back-rgba.bmp"
#define BLUE_GAME_EXPLODE_BACK_WARP_TEX             "explode-warp-tex"
#define BLUE_GAME_EXPLODE_BACK_WARP_TEX_COMMENT     "Texture for explosion sfx backdrop warp data. Default: " BLUE_GAME_EXPLODE_BACK_WARP_TEX_DEFAULT
#define BLUE_GAME_EXPLODE_BACK_WARP_TEX_DEFAULT     "black.bmp"
#define BLUE_GAME_EXPLODE_RING_RGBA_TEX             "explode-ring-rgba-tex"
#define BLUE_GAME_EXPLODE_RING_RGBA_TEX_COMMENT     "Texture for explosion sfx ring rgba data. Default: " BLUE_GAME_EXPLODE_RING_RGBA_TEX_DEFAULT
#define BLUE_GAME_EXPLODE_RING_RGBA_TEX_DEFAULT     "sfx-explosion-ring-rgba.bmp"
#define BLUE_GAME_EXPLODE_RING_WARP_TEX             "explode-ring-warp-tex"
#define BLUE_GAME_EXPLODE_RING_WARP_TEX_COMMENT     "Texture for explosion sfx ring warp data. Default: " BLUE_GAME_EXPLODE_RING_WARP_TEX_DEFAULT
#define BLUE_GAME_EXPLODE_RING_WARP_TEX_DEFAULT     "black.bmp"
#define BLUE_GAME_SENSEFX_FACTOR                    "sfx-sense-size-factor"
#define BLUE_GAME_SENSEFX_FACTOR_COMMENT            "Factor applied to texture to allow its effect to be aligned: Default " BLUE_GAME_SENSEFX_FACTOR_DEFAULT
#define BLUE_GAME_SENSEFX_FACTOR_DEFAULT            "1.1"
#define BLUE_GAME_PULSE_PERIOD                      "pulse-period"
#define BLUE_GAME_PULSE_PERIOD_COMMENT              "Controls how fast emissives pulse. Default :" BLUE_GAME_PULSE_PERIOD_DEFAULT
#define BLUE_GAME_PULSE_PERIOD_DEFAULT              "125"
#define BLUE_GAME_SELECTED_DECK                      "active-deck"
#define BLUE_GAME_SELECTED_DECK_COMMENT              "The name of the currently active deck. Default :" BLUE_GAME_SELECTED_DECK_DEFAULT
#define BLUE_GAME_SELECTED_DECK_DEFAULT              ""

// Logs and generated files:
#define BLUE_LOGGED_USERSETTINGS                    "run.user.settings.xml"

// Window class name, used to lookup the class at install.
#define BLUE_WINDOW_CLASSNAME                       "blue-window-classx"

*/

#define BLUE_BOARD_Y                                (0.0f)
#endif
