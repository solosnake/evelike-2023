#include "solosnake/blue/blue_backgroundgame.hpp"
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

#define SS_SKYBOXNAME "yello.xml"
#define SS_BOARD_BMP "background.bmp"
#define SS_HUESHIFT 47

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
            load_perspective(viewAngle,
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

        // ******************** TEMP CODE ONLY ***************************

        //! Returns an array of gridDim x gridDim board positions, with
        //! position[0] always being (0,0).
        vector<pair<unsigned char, unsigned char>> make_random_board_positions(size_t gridDim)
        {
            vector<pair<unsigned char, unsigned char>> positions(gridDim);
            positions.reserve(gridDim * gridDim);

            if (gridDim > 0)
            {
                // Always place first bot on 0,0
                positions.push_back(make_pair<unsigned char, unsigned char>(0, 0));

                for (unsigned int x = 1; x < gridDim; ++x)
                {
                    for (unsigned int y = 1; y < gridDim; ++y)
                    {
                        positions.push_back(pair<unsigned char, unsigned char>(x, y));
                    }
                }

                // Randomly shuffle all positions except position #0.
                if (positions.size() > 1)
                {
                    // Use a standardised and repeatable rand so as not to get
                    // differences on Linux.
                    minstd_rand0 generator(0xB33F);
                    auto shuffler = [&](size_t n)
                    { return ((generator()) % n); };
                    random_shuffle(positions.begin() + 1, positions.end(), shuffler);
                }
            }

            return positions;
        }

        void fill_cargo_with_building_materials(Machine& m)
        {
            amount oneUnitOfEach[BLUE_TRADABLE_TYPES_COUNT];
            for (size_t i = 0; i < BLUE_TRADABLE_TYPES_COUNT; ++i)
            {
                oneUnitOfEach[i][i] = 1;
            }

            bool hasSpace = m.cargo().free_volume() > 0;

            while (hasSpace)
            {
                // For now only add the building blocks.
                for (size_t i = Metals; i <= Alkalis; ++i)
                {
                    hasSpace = m.try_add_cargo(oneUnitOfEach[i]);
                }
            }
        }

        class testmachinemaker
        {
        public:
            static const unsigned short APs[19];
            static const unsigned short TAPs[4];

            testmachinemaker(
                std::shared_ptr<datapaths> paths,
                shared_ptr<compiler> cc,
                shared_ptr<components> machineparts,
                unsigned int defaultPeriod)
                : datapaths_(paths)
                , cc_(cc)
                , machineparts_(machineparts)
                , ap_(APs, APs + sizeof(APs) / sizeof(unsigned short))
                , tap_(TAPs, TAPs + sizeof(TAPs) / sizeof(unsigned short))
                , next_ap_index_(0)
                , next_tap_index_(0)
                , defaultPeriod_(defaultPeriod)
            {
            }

            thrusterattachpoint next_thrusterattachpoint()
            {
                return tap_.at(next_tap_index_++);
            }

            AttachPoint next_attachpoint()
            {
                return ap_.at(next_ap_index_++);
            }

            unique_ptr<Machine> make_machine(uint8_t hue_shift, const char* name, const char* scriptname, ...)
            {
                next_ap_index_ = next_tap_index_ = 0;
                typedef map<AttachPoint, blueprint::orientedhardpoint> hpmap_t;
                hpmap_t hardpoints;
                map<thrusterattachpoint, Thruster> thrusters;
                map<AttachPoint, Softpoint> softpoints;
                va_list args;
                va_start(args, scriptname);
                for (const char* a = va_arg(args, const char*); a; a = va_arg(args, const char*))
                {
                    if (auto t = machineparts_->get_hardpoint(a))
                    {
                        blueprint::orientedhardpoint hp(blueprint::Rotated270, *t);
                        hardpoints.insert(hpmap_t::value_type(next_attachpoint(), hp));
                    }
                    else if (auto t = machineparts_->get_softpoint(a))
                    {
                        softpoints.insert(
                            map<AttachPoint, Softpoint>::value_type(next_attachpoint(), *t));
                    }
                    else if (auto t = machineparts_->get_thruster(a))
                    {
                        thrusters.insert(
                            map<thrusterattachpoint, Thruster>::value_type(next_thrusterattachpoint(), *t));
                    }
                    else
                    {
                        ss_err("Unknown Component name ", a);
                        ss_throw("Unknown Component name");
                    }
                }
                va_end(args);

                std::shared_ptr<blueprint> bpo = make_shared<blueprint>(thrusters, softpoints, hardpoints);
                vector<std::shared_ptr<blueprint>> knownBPOs;
                auto testScriptURL = datapaths_->get_scripts_filepath(scriptname);
                instructions compiled = cc_->compile_file(testScriptURL);

                return Machine::create_machine(
                    name, hue_shift, bpo, knownBPOs, defaultPeriod_, move(compiled));
            }

        private:
            std::shared_ptr<datapaths> datapaths_;
            shared_ptr<compiler> cc_;
            shared_ptr<components> machineparts_;
            vector<unsigned short> ap_;
            vector<unsigned short> tap_;
            size_t next_ap_index_;
            size_t next_tap_index_;
            unsigned int defaultPeriod_;
        };

        const unsigned short testmachinemaker::APs[]
        = { 64, 62, 66, 44, 42, 46, 68, 60, 48, 40, 24, 22, 26, 20, 28, 3, 5, 1, 7 };

        const unsigned short testmachinemaker::TAPs[4] = { 83, 85, 81, 87 };    }

    backgroundgame::backgroundgame(
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

    backgroundgame::~backgroundgame()
    {
    }

    //! Creates the internal model from the args and the paths and user settings,
    //! creates the views, removes the default first view,
    //! and shares the model with the new views.
    bool backgroundgame::try_load_game(const shared_ptr<gamestartupargs>& args)
    {
        // NOTE: It is important this function is exception safe. It
        //       must either work, or fail without making any changes.

        if (fullyLoaded_)
        {
            ss_err("load_game called more than once on same 'backgroundgame' object.");
            ss_throw("load_game called more than once on same 'backgroundgame' object.");
        }

        try
        {
            // Check the paths early as its a likely cause of failure (external data).
            auto xmlPath = datapaths_->get_gui_filepath(xmlScreenFile_);
            auto boardfilePath = datapaths_->get_boards_filepath(userSettings_->value(BLUE_GAME_FILE));

            // Create backgroundgame components
            auto localPlayer   = make_localplayer();
            auto compilerpaths = std::make_shared<datapathfinder>(datapaths_, &datapaths::get_compilers_filepath);
            auto cc            = std::make_shared<compiler>();
            auto machineparts  = std::make_shared<components>(*datapaths_);
            auto pending       = make_pendinginstructions();
            auto boardstate    = make_picture_boardstate(SS_BOARD_BMP, datapaths_, make_irand(args->random_seed()), args, userSettings_);
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
            auto sb = renderer_->cache().get_skybox(SS_SKYBOXNAME);
            renderer_->renderer().load_skybox(*sb);
            renderer_->renderer().enable_skybox(true);

            // Create array of local players (typically local AI and user).
            vector<shared_ptr<player>> localPlayers;
            localPlayers.push_back(localPlayer);

            auto wndSize = renderer_->get_rendering_window_ptr()->get_window_dimensions();

            int botcounter = 0;

            unsigned int defaultPeriod = userSettings_->value(BLUE_GAME_PULSE_PERIOD).as_uint();

            auto machinemaker = std::make_shared<testmachinemaker>(datapaths_, cc, machineparts, defaultPeriod);

            auto addkillbot
                = [&](const char* scriptname, uint8_t hue_shift, Hex_coord xy, HexFacingName f, const unsigned int botId)
            {
                char name[3] = { 'a', 'a', 0 };
                name[1] += (botId % (1 + 'z' - 'a'));
                name[0] += ((botId / ('z' - 'a')) % (1 + 'z' - 'a'));

                auto bot = machinemaker->make_machine(
                    hue_shift,
                    name,
                    scriptname,
                    "laser",
                    "laser",
                    "laser",
                    "thruster1",
                    "thruster1",
                    "thruster1",
                    "thruster1",
                    "cpu",
                    "capacitor",
                    "capacitor",
                    "comms",
                    "electronics",
                    "generic01",
                    "generic02",
                    "generic03",
                    "radar",
                    "radardome",
                    "reactor",
                    nullptr);

                fill_cargo_with_building_materials(*bot);
                boardstate->add_machine_to_board(xy, f, move(bot));
                ++botcounter;
            };

            // Machines etc are added now, AFTER creation. This ensures
            // all interested parties get all information.

            // TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE

            // Add central test bot:
            const Hex_coord centreXY = {
                static_cast<uint8_t>(boardstate->grid_width() / 2u),
                static_cast<uint8_t>(boardstate->grid_height() / 2u) };

            const Hex_coord nearbyXY2 = hexgrid::calculate_neighbour_coord(centreXY, FacingTile1, 1);

            addkillbot( "nop.txt", SS_HUESHIFT, nearbyXY2, FacingTile5, 0);

            // TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE TEST CODE

            // Now we have all the backgroundgame state ready. Expose it to Lua
            expose_gamestate_to_lua(*boardstate, widgets->get_luaguicommandexecutor());
            expose_soundrenderer_to_lua(sound_renderer_.get(), widgets->get_luaguicommandexecutor()->lua());

            // Now create the views containing the GUIs. The backgroundgame state is exposed
            // to the Lua which the widgets will have access too.
            auto guiViews = xmlscreenviews::make_shared(xmlPath, wndSize, widgets);

            // Create good viewing matrix.
            const float camballRadius = userSettings_->value(BLUE_GAME_CAMERA_RADIUS).as_float();
            const float camballCeiling = userSettings_->value(BLUE_GAME_CAMERA_CEILING_HEIGHT).as_float();

            const float camballTilt = userSettings_->value(BLUE_GAME_CAMERA_TILT).as_float();
            const float camballRotate = userSettings_->value(BLUE_GAME_CAMERA_ROTATION).as_float();
            const float camballArmlength = userSettings_->value(BLUE_GAME_CAMERA_ARMLENGTH).as_float();
            const float cameraZoomFactor = userSettings_->value(BLUE_GAME_CAMERA_ZOOMFACTOR).as_float();

            camera_ = create_boardcamera(
                camballArmlength,
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
            set_proj_matrix(
                renderer_->get_window_rect(),
                1.0f, // Board scale
                zNear_,
                zFar_,
                viewAngle_,
                proj_);

            solosnake::bgra boardTileColours[3] = {
                solosnake::bgra(  9, 129, 178, 200),
                solosnake::bgra(  9,  72, 178, 200),
                solosnake::bgra(  9, 179, 178, 200)
            };

            solosnake::bgr ambientbgr( 50, 50, 50 );

            // Create emptygame and emptygame rendering: note move of boardstate here!
            auto sharedmodel = make_gamestate(move(boardstate), userSettings_);
            auto boardrenderer = make_drawboardstate(sharedmodel, renderer_, sound_renderer_, userSettings_, datapaths_, boardTileColours, ambientbgr);

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

            ss_log(botcounter, " bots on board.");

            return true;
        }
        catch(const SS_EXCEPTION_TYPE& e)
        {
            ss_err(e.what());
            assert(!"Exception while trying to load backgroundgame.");
        }

        return false;
    }



}
