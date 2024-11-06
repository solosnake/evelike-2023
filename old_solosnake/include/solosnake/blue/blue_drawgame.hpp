#ifndef blue_drawboardstate_hpp
#define blue_drawboardstate_hpp

#include <memory>
#include "solosnake/bgra.hpp"
#include "solosnake/deferred_renderer_fwd.hpp"
#include "solosnake/modelscene.hpp"
#include "solosnake/persistance.hpp"
#include "solosnake/rendering_system_fwd.hpp"
#include "solosnake/rect.hpp"
#include "solosnake/unaliased.hpp"
#include "solosnake/blue/blue_boardevent.hpp"
#include "solosnake/blue/blue_board_state.hpp"
#include "solosnake/blue/blue_boardobserver.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_suncache.hpp"
#include "solosnake/blue/blue_tradables.hpp"

namespace solosnake
{
    class sound_renderer;
}

namespace blue
{
    class Asteroid;
    class Blueprint;
    class user_settings;
    class datapaths;
    class drawboard;
    class gamestate;
    class drawasteroid;
    class drawmachine;
    class drawsun;
    class gamesfx;
    class Machine;
    class Machine_event;
    class Sun;
    class user_settings;
    struct Machine_exploded;

    //! A renderer capable of displaying the game board and its contents. This
    //! manages a parallel model of the board, which is concerned with displaying
    //! the board to the user.
    //!
    //! This does not only draw the game, it is also responsible for other
    //! gui feedbacks such as sound and vibration from game events.
    //!
    //! This handles loading of models etc and is shared amongst the views of the
    //! game. The intention is to free the views from having to manage the
    //! rendering of the common aspects of the board.
    //!
    class drawgame : public boardobserver, public std::enable_shared_from_this<drawgame>
    {
        struct HeapOnly {};

    public:

        //! Should not be called - only public for use by make_shared.
        drawgame(
            const user_settings&,
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::ifilefinder>& sunfinder,
            const HeapOnly& );

        ~drawgame();

        static std::shared_ptr<drawgame> make_shared_drawgame(
            const user_settings&,
            const std::shared_ptr<gamestate>&,
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>&,
            const std::shared_ptr<datapaths>&,
            const solosnake::bgra sunColour,
            const solosnake::bgra tileColours[3],
            const solosnake::bgr ambient );

        solosnake::deferred_renderer* renderer();

        const solosnake::deferred_renderer* renderer() const;

        std::shared_ptr<solosnake::rendering_system> get_rendering_system() const;

        solosnake::rect get_window_rect() const;

        void set_view_matrix( const float* unaliased v );

        void set_proj_matrix( const float* unaliased p, const float zNear );

        void start_scene();

        void draw_boardstate(
            const bool advanceAnimations,
            const unsigned int millisecondsSinceLastFrame,
            const float frameFraction );

        void end_scene();

        void swap_buffers();

    private:

        drawgame( const drawgame& );

        drawgame& operator=( const drawgame& );

        void on_event( const boardevent&, const boardeventcontext& ) override;

        void handle_board_machineevent( const Machine_event& );

        void handle_board_secondaryevent( const Secondary_event& );

        void cache_blueprint_models( const std::shared_ptr<Blueprint>& );

        void cache_asteroid_model( const OreTypes ore );

        void add_machine_instance_to_board( const Machine& );

        void add_asteroid_instance_to_board( const Asteroid& );

        void add_sun_instance_to_board( const Sun& );

        void update_machine_instance_position( const Machine_event& );

        void handle_machine_weapon_fired_and_hit_machine_event( const Secondary_event& );

        void handle_machine_weapon_fired_and_hit_asteroid_event( const Secondary_event& );

        void handle_machine_weapon_fired_and_missed_event( const Secondary_event& );

        void handle_machine_capdrain_fired_and_hit_machine_event( const Secondary_event& );

        void handle_machine_capdrain_fired_and_hit_asteroid_event( const Secondary_event& );

        void handle_machine_capdrain_fired_and_missed_event( const Secondary_event& );

        void handle_machine_capxfer_fired_and_hit_machine_event( const Secondary_event& );

        void handle_machine_capxfer_fired_and_hit_asteroid_event( const Secondary_event& );

        void handle_machine_capxfer_fired_and_missed_event(  const Secondary_event& );

        void handle_machine_repper_fired_and_hit_machine_event(  const Secondary_event& );

        void handle_machine_repper_fired_and_hit_asteroid_event( const Secondary_event& );

        void handle_machine_repper_fired_and_missed_event(  const Secondary_event& );

        void handle_machine_miner_fired_and_hit_machine_event(  const Secondary_event& );

        void handle_machine_miner_fired_and_hit_asteroid_event(  const Secondary_event& );

        void handle_machine_miner_fired_and_missed_event(  const Secondary_event& );

        void handle_asteroid_depleted_event( const Secondary_event& );

        void handle_cash_transferred_event( const Secondary_event& );

        void add_sensing_effect( const Machine_event& );

        void draw_asteroid( drawasteroid&, const bool advanceAnimations, const unsigned int dtMs );

        void draw_machine( drawmachine&, const bool advanceAnimations, const float frameFraction );

        void draw_sun( drawsun&, const bool advanceAnimations, const unsigned int dtMs );

        void sort_machines();

        void remove_machine( const Machine* );

        void add_machine_explosion( const Machine_exploded& );

        drawmachine* get_drawmachine( const Machine* );

        size_t get_drawmachine_index( const Machine* ) const;

        void log_error( const Machine_event& );

        void log_assertion( const Machine_event& );

        void print_text( const Machine_event& );

        void print_instruction( const Machine_event& );

        void rotate_skybox( unsigned int );

    private:

        std::vector<std::weak_ptr<Blueprint>>           bp_models_loaded_;
        std::weak_ptr<gamestate>                        gamestate_;
        std::unique_ptr<drawboard>                      drawboard_;
        std::unique_ptr<gamesfx>                        sfx_;
        std::shared_ptr<solosnake::rendering_system>    rendering_;
        std::vector<std::unique_ptr<drawmachine>>       drawable_machines_;
        std::vector<std::unique_ptr<drawasteroid>>      drawable_asteroids_;
        std::vector<std::unique_ptr<drawsun>>           drawable_suns_;
        scene_sun_types                                 scene_sun_types_;
        solosnake::modelscene                           scene_;
        solosnake::deferred_renderer*                   prenderer_;
        solosnake::radians                              skyboxRotations_[2];
        float                                           skyboxRotationRates_[2];
        std::string                                     asteroid_filenames_[4];
        float                                           distance_between_two_tiles_;
        float                                           ambient_rgb_[3];
    };

    //////////////////////////////////////////////////////////////////////////


    inline solosnake::deferred_renderer* drawgame::renderer()
    {
        return prenderer_;
    }

    inline const solosnake::deferred_renderer* drawgame::renderer() const
    {
        return prenderer_;
    }

    inline std::shared_ptr<solosnake::rendering_system> drawgame::get_rendering_system() const
    {
        return rendering_;
    }
}

#endif
