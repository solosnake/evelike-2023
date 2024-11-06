#include "solosnake/blue/blue_emptygame.hpp"
#include <cassert>
#include <algorithm>
#include <vector>
#include <random>
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/image.hpp"
#include "solosnake/languagetexts.hpp"
#include "solosnake/linesegment.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/make_iimg.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/persistance.hpp"
#include "solosnake/perlin_noise_skybox.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/sound_renderer.hpp"
#include "solosnake/sound_renderer_to_lua.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/vkeys.hpp"
#include "solosnake/external/xml.hpp"
#include "solosnake/blue/blue_board_state.hpp"
#include "solosnake/blue/blue_blueprint.hpp"
#include "solosnake/blue/blue_compiler.hpp"
#include "solosnake/blue/blue_components.hpp"
#include "solosnake/blue/blue_create_boardcamera.hpp"
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_datapathfinder.hpp"
#include "solosnake/blue/blue_drawboard.hpp"
#include "solosnake/blue/blue_drawgame.hpp"
#include "solosnake/blue/blue_game.hpp"
#include "solosnake/blue/blue_gamecommands.hpp"
#include "solosnake/blue/blue_gamestartupargs.hpp"
#include "solosnake/blue/blue_gamestate.hpp"
#include "solosnake/blue/blue_iboardcamera.hpp"
#include "solosnake/blue/blue_iscreenview.hpp"
#include "solosnake/blue/blue_keymapping.hpp"
#include "solosnake/blue/blue_log_events.hpp"
#include "solosnake/blue/blue_load_skybox.hpp"
#include "solosnake/blue/blue_messages.hpp"
#include "solosnake/blue/blue_pickresult.hpp"
#include "solosnake/blue/blue_pendinginstructions.hpp"
#include "solosnake/blue/blue_player.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_settings_objects.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/blue/blue_widgets_factory.hpp"
#include "solosnake/blue/blue_xmlscreenviews.hpp"


#define SS_STRESSBOT_COUNT 0
#define SS_STRESSBOT_SCRIPT "random-rotate-and-fire.txt"
#define SS_STRESS_BIGBOTS 1

using namespace solosnake;
using namespace std;

// Z near is initialised to this, but the one loaded from user settings is used.
#define Z_NEAR_DEFAULT 0.1f

namespace blue
{
    namespace
    {
        // Translates 1 or more messages in the buffer into gameaction(s) and
        // pushes them into the existing array.
        void translate_message_to_gameactions(
            const network::byte* unaliased msg,
            size_t msgSize,
            vector<gameaction>& actions)
        {
            assert(msg);
            assert(msgSize > 0);

            while (msgSize > 0)
            {
                gameaction action(msg);
                assert(action.bytes_size() <= msgSize);
                msg += action.bytes_size();
                msgSize -= action.bytes_size();
                actions.push_back(move(action));
            }
        }

        void translate_gameactions_to_message(
            const vector<gameaction>& actions,
            network::bytebuffer& msg)
        {
            if (!actions.empty())
            {
                msg += static_cast<solosnake::network::byte>(GameActionsMessage);

                for (size_t i = 0; i < actions.size(); ++i)
                {
                    msg += actions[i].to_bytebuffer();
                }
            }
        }

        //! Initial view set on game, replaced by proper controller-view once
        //! the game is loaded.
        class firstview : public iscreenview
        {
        public:
            firstview() : iscreenview("firstview")
            {
            }

        private:
            virtual void render_view() const
            {
            }

            virtual void handle_fullscreen_changed(const solosnake::FullscreenState&)
            {
            }

            virtual void handle_minimised_changed(const solosnake::MinimisedState&)
            {
            }

            virtual void handle_screensize_changed(const solosnake::dimension2d<unsigned int>&)
            {
            }

            virtual solosnake::LoopResult handle_inputs(const iinput_events&)
            {
                return LoopAgain;
            }

            virtual void activate_view()
            {
            }

            virtual void deactivate_view()
            {
            }

            virtual void advance_one_frame()
            {
            }
        };

        //! Initial first state, never actually used.
        class firststate : public solosnake::iscreenstate
        {
            virtual bool is_screen_ended() const
            {
                return false;
            }

            virtual nextscreen get_next_screen() const
            {
                // An empty nextscreen is a valid object, and it will exit the app.
                return nextscreen();
            }
        };

        float board_diagonal(const hexgrid& g)
        {
            // For now lets take the board as a box, as high as its shortest side.
            float boxW = static_cast<float>(g.grid_width());
            float boxL = static_cast<float>(g.grid_height());
            float boxDiagonal = sqrt((boxW * boxW) + (boxL * boxL));
            float boxSpan = sqrt((boxDiagonal * boxDiagonal) + max(boxW, boxL));

            return boxSpan;
        }

        void set_proj_matrix(
            const solosnake::rect& windowRect,
            const float boardScale,
            const float zNear,
            const float zFar,
            const solosnake::radians viewAngle,
            solosnake::matrix4x4_t& proj)
        {
            load_perspective(
                viewAngle,
                float(windowRect.width()) / windowRect.height(),
                2.0f * boardScale * zNear,
                zFar * 2.0f,
                proj);
        }

        solosnake::radians get_default_viewangle()
        {
            return PI_RADIANS * 0.25f;
        }

        //! Make the dummy screen views to use before the game is loaded.
        shared_ptr<xmlscreenviews> make_default_game_screenviews()
        {
            return xmlscreenviews::make_shared(make_shared<firstview>(), make_shared<firststate>());
        }

        //! Exposes board states to lua for inspection etc.
        void expose_gamestate_to_lua(const boardstate&, std::shared_ptr<solosnake::luaguicommandexecutor>)
        {
        }
    }

    emptygame::emptygame(
        const string& xmlScreenFile,
        const std::shared_ptr<blue_user_settings>& userSettings,
        const std::shared_ptr<rendering_system>& r,
        const std::shared_ptr<solosnake::sound_renderer>& s,
        const std::shared_ptr<datapaths>& datapaths,
        const std::shared_ptr<ilanguagetext>& language,
        const std::shared_ptr<luaguicommandexecutor>& lce,
        const game::HeapOnly& h)
        : game( xmlScreenFile, userSettings, r, s, datapaths, language, lce, h )
    {
    }

    emptygame::~emptygame()
    {
    }

    //! Creates the internal model from the args and the paths and user settings,
    //! creates the views, removes the default first view,
    //! and shares the model with the new views.
    bool emptygame::try_load_game(const shared_ptr<gamestartupargs>& args)
    {
        // NOTE: It is important this function is exception safe. It
        //       must either work, or fail without making any changes.

        if (fullyLoaded_)
        {
            ss_err("load_game called more than once on same 'emptygame' object.");
            ss_throw("load_game called more than once on same 'emptygame' object.");
        }

        try
        {
            // Check the paths early as its a likely cause of failure (external data).
            auto xmlPath = datapaths_->get_gui_filepath(xmlScreenFile_);
            auto boardfilePath = datapaths_->get_boards_filepath(userSettings_->value(BLUE_GAME_FILE));

            // Create emptygame components
            auto localPlayer   = make_localplayer();
            auto compilerpaths = std::make_shared<datapathfinder>(datapaths_, &datapaths::get_compilers_filepath);
            auto cc            = std::make_shared<compiler>();
            auto machineparts  = std::make_shared<components>(*datapaths_);
            auto pending       = make_pendinginstructions();
            auto boardstate    = make_empty_boardstate(make_irand(args->random_seed()), args, userSettings_, boardfilePath);
            auto widgetstyles  = make_widgetrendingstyles(userSettings_, renderer_, datapaths_);

            auto widgets = make_blue_game_widgets_factory(
                lce_,
                widgetstyles,
                language_,
                this->shared_from_this(),
                localPlayer,
                pending,
                cc);

            // Load skybox: eventually this string will come from whatever
            // data struct describes the board etc.
            auto sb = renderer_->cache().get_skybox("pp.xml");
            renderer_->renderer().load_skybox(*sb);
            renderer_->renderer().enable_skybox(true);

            // Create array of local players (typically local AI and user).
            vector<shared_ptr<player>> localPlayers;
            localPlayers.push_back(localPlayer);

            auto wndSize = renderer_->get_rendering_window_ptr()->get_window_dimensions();

            // Now we have all the emptygame state ready. Expose it to Lua
            expose_gamestate_to_lua(*boardstate, widgets->get_luaguicommandexecutor());
            expose_soundrenderer_to_lua(sound_renderer_.get(), widgets->get_luaguicommandexecutor()->lua());

            // Now create the views containing the GUIs. The emptygame state is exposed
            // to the Lua which the widgets will have access too.
            auto guiViews = xmlscreenviews::make_shared(xmlPath, wndSize, widgets);

            // Create good viewing matrix.
            const float camballRadius = userSettings_->value(BLUE_GAME_CAMERA_RADIUS).as_float();
            const float camballCeiling = userSettings_->value(BLUE_GAME_CAMERA_CEILING_HEIGHT).as_float();

            const float camballTilt = userSettings_->value(BLUE_GAME_CAMERA_TILT).as_float();
            const float camballRotate = userSettings_->value(BLUE_GAME_CAMERA_ROTATION).as_float();
            const float camballArmlength = userSettings_->value(BLUE_GAME_CAMERA_ARMLENGTH).as_float();
            const float cameraZoomFactor = userSettings_->value(BLUE_GAME_CAMERA_ZOOMFACTOR).as_float();

            camera_ = create_boardcamera(camballArmlength,
                camballTilt,
                camballRotate,
                camballRadius,
                camballCeiling,
                cameraZoomFactor,
                *boardstate->hexboard());

            camera_->get_view_matrix(view_);

            const float diagonal = board_diagonal(boardstate->hexboard()->grid());

            // Must be able to see furthest corner of board.
            const float maxViewDist = diagonal + camera_->max_distance_to_lookat();

            zNear_ = userSettings_->value(BLUE_GAME_Z_NEAR).as_float();
            zFar_ = zNear_ + maxViewDist;

            // Create good perspective matrix (this needs to be corrected for
            // screen size changes).
            set_proj_matrix(renderer_->get_window_rect(),
                1.0f, // Board scale
                zNear_,
                zFar_,
                viewAngle_,
                proj_);

            solosnake::bgra boardTileColours[3] = {
                solosnake::bgra(221, 221, 221, 255),
                solosnake::bgra(187, 187, 187, 255),
                solosnake::bgra(153, 153, 153, 255)
            };

            solosnake::bgr ambientbgr( 50, 50, 50 );

            // Create emptygame and emptygame rendering: note move of boardstate here!
            auto sharedmodel = make_gamestate(move(boardstate), userSettings_);

            auto boardrenderer = make_drawboardstate(
              sharedmodel,
              renderer_,
              sound_renderer_,
              userSettings_,
              datapaths_,
              boardTileColours,
              ambientbgr);

            // All done, safe to assign to members.
            local_players_.swap(localPlayers);
            boardRenderer_ = move(boardrenderer);
            sharedgamestate_.swap(sharedmodel);
            guiViews_.swap(guiViews);
            pendinginstructions_.swap(pending);
            fullyLoaded_ = true;

            // Get shortcut members:
            pboardstate_ = sharedgamestate_->boardgamestate();
            pgamehexgrid_ = pboardstate_->hexboard();

            // Change rendering function pointer:
            renderFunc_ = &game::render_with_loaded_game;

            // WIREFRAME
            // renderer_->renderer().set_debug_rendermode(
            // deferred_renderer::WireFrameRenderMode );

            return true;
        }
        catch(const SS_EXCEPTION_TYPE& e)
        {
            ss_err(e.what());
            assert(!"Exception while trying to load emptygame.");
        }

        return false;
    }

}
