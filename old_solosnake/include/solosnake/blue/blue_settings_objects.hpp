#ifndef blue_settings_objects_hpp
#define blue_settings_objects_hpp

#include <memory>
#include "solosnake/filepath.hpp"

// A place for creation of objects created based on user settings.

// Forward declarations in solosnake namespace:
namespace solosnake
{
    class bgr;
    class bgra;
    class ilanguagetext;
    class irand;
    class itextureloader;
    class ixml_widgetfactory;
    class luaguicommandexecutor;
    class persistance;
    class rendering_system;
    class sound_renderer;
    class widgetrenderingstyles;
    class window;
}

namespace blue
{
    // Forward declarations:
    class widgetsfactory;
    class user_settings;
    class Board_state;
    class compiler;
    class deck;
    class drawgame;
    class datapaths;
    class drawboard;
    class gamesfx;
    class gamestate;
    class Game_hex_grid;
    class gamestartupargs;
    class Hex_grid;
    class ipendinginstructions;
    class iboardview;
    class igameview;
    class pendinginstructions;
    class player;

    std::shared_ptr<pendinginstructions> make_pendinginstructions();

    std::shared_ptr<player> make_localplayer();

    std::unique_ptr<solosnake::irand> make_irand(
        unsigned int seed );

    std::shared_ptr<gamestartupargs> make_game_startup_args(
        const std::shared_ptr<user_settings>& );

    std::shared_ptr<gamestate> make_gamestate(
        std::unique_ptr<Board_state>,
        const std::shared_ptr<user_settings>& );

    std::shared_ptr<solosnake::widgetrenderingstyles> make_widgetrendingstyles(
        const std::shared_ptr<solosnake::rendering_system>&,
        const std::shared_ptr<user_settings>&,
        const std::shared_ptr<datapaths>& );

    std::shared_ptr<solosnake::rendering_system> make_rendering_system(
        const std::shared_ptr<solosnake::window>&,
        const std::shared_ptr<user_settings>&,
        const std::shared_ptr<datapaths>& );

    std::shared_ptr<solosnake::sound_renderer> make_sound_system(
        const std::shared_ptr<user_settings>&,
        const std::shared_ptr<datapaths>& );

    std::unique_ptr<Board_state> make_picture_boardstate(
        const std::string&,
        const std::shared_ptr<datapaths>&,
        std::unique_ptr<solosnake::irand>,
        const std::shared_ptr<gamestartupargs>&,
        const std::shared_ptr<user_settings>& usersettings );

    std::unique_ptr<Board_state> make_test_boardstate(
        std::unique_ptr<solosnake::irand>,
        const std::shared_ptr<gamestartupargs>&,
        const std::shared_ptr<user_settings>&,
        const solosnake::filepath& );

    std::unique_ptr<Board_state> make_example_boardstate(
        const std::shared_ptr<datapaths>&,
        std::unique_ptr<solosnake::irand>,
        const std::shared_ptr<gamestartupargs>&,
        const std::shared_ptr<user_settings>& settings,
        const solosnake::filepath& gamefileurl );

    std::unique_ptr<Board_state> make_empty_boardstate(
        std::unique_ptr<solosnake::irand>,
        const std::shared_ptr<gamestartupargs>&,
        const std::shared_ptr<user_settings>& settings,
        const solosnake::filepath& gamefileurl );

    std::unique_ptr<Board_state> make_background_boardstate(
        const std::shared_ptr<datapaths>&,
        std::unique_ptr<solosnake::irand>,
        const std::shared_ptr<gamestartupargs>&,
        const std::shared_ptr<user_settings>& settings,
        const solosnake::filepath& gamefileurl );

    std::unique_ptr<gamesfx> make_gamesfx(
        const std::shared_ptr<solosnake::rendering_system>&,
        const std::shared_ptr<solosnake::sound_renderer>&,
        const solosnake::persistance&,
        const std::shared_ptr<datapaths>& );

    std::shared_ptr<drawgame> make_drawboardstate(
        const std::shared_ptr<gamestate>&,
        const std::shared_ptr<solosnake::rendering_system>&,
        const std::shared_ptr<solosnake::sound_renderer>&,
        const std::shared_ptr<user_settings>&,
        const std::shared_ptr<datapaths>&,
        const solosnake::bgra sunColour,
        const solosnake::bgra tilecolours[3],
        const solosnake::bgr ambient );

    std::shared_ptr<widgetsfactory> make_menu_widgetsfactory(
        const std::shared_ptr<user_settings>& usersettings,
        const std::shared_ptr<solosnake::luaguicommandexecutor>&,
        const std::shared_ptr<solosnake::widgetrenderingstyles>&,
        const std::shared_ptr<solosnake::ilanguagetext>& );

    std::shared_ptr<widgetsfactory> make_gamebackground_widgetsfactory(
        const std::shared_ptr<solosnake::luaguicommandexecutor>&,
        const std::shared_ptr<solosnake::widgetrenderingstyles>&,
        const std::shared_ptr<solosnake::ilanguagetext>&,
        const std::weak_ptr<igameview>&,
        const std::shared_ptr<deck>&,
        const std::shared_ptr<pendinginstructions>&,
        const std::shared_ptr<compiler>&,
        const std::shared_ptr<datapaths>&  );
}

#endif
