#ifndef blue_boardrenderer_hpp
#define blue_boardrenderer_hpp

#include <memory>
#include "solosnake/deferred_renderer_fwd.hpp"
#include "solosnake/rendering_system_fwd.hpp"
#include "solosnake/modelscene.hpp"
#include "solosnake/persistance.hpp"
#include "solosnake/rect.hpp"
#include "solosnake/unaliased.hpp"
#include "solosnake/modelinstance.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_event.hpp"

namespace solosnake
{
    class persistance;
}

namespace blue
{
    class Blueprint;
    class datapaths;
    class drawboard;
    class gamestate;

    //! A renderer capable of displaying the game board and its contents. This
    //! manages a parallel model of the board, which is concerned with displaying
    //! the board to the user.
    //! This handles loading of models etc and is shared amongst the views of the
    //! game. The intention is to free the views from having to manage the
    //! rendering of the common aspects of the board.
    class boardstaterenderer : public std::enable_shared_from_this<boardstaterenderer>
    {
        struct HeapOnly
        {
        };

    public:
        boardstaterenderer( std::shared_ptr<drawboard>,
                            std::shared_ptr<solosnake::rendering_system>,
                            std::shared_ptr<datapaths> paths,
                            const HeapOnly& );

        ~boardstaterenderer();

        static std::shared_ptr<boardstaterenderer> make_shared( std::shared_ptr<gamestate>,
                std::shared_ptr<drawboard>,
                std::shared_ptr
                <solosnake::rendering_system>,
                std::shared_ptr<datapaths> paths );

        solosnake::deferred_renderer* renderer()
        {
            return prenderer_;
        }

        const solosnake::deferred_renderer* renderer() const
        {
            return prenderer_;
        }

        std::shared_ptr<solosnake::rendering_system> get_rendering_system() const
        {
            return rendering_;
        }

        float board_tilewidth() const;

        float board_y() const;

        solosnake::rect get_window_rect() const;

        void set_view_matrix( const float* unaliased v );

        void set_proj_matrix( const float* unaliased p, const float zNear );

        void start_scene();

        void render_boardstate();

        void end_scene();

        void swap_buffers();

    private:
        boardstaterenderer( const boardstaterenderer& );

        boardstaterenderer& operator=( const boardstaterenderer& );

        void on_event( const event& );

        void cache_blueprint_models( std::shared_ptr<Blueprint> );

        void add_machine_instance_to_board( const Machine& );

        void update_machine_instance_position( const event::positionalinfoupdate& );

    private:
        std::vector<std::weak_ptr<Blueprint>> bp_models_loaded_;
        std::weak_ptr<gamestate> gamestate_;
        std::shared_ptr<drawboard> drawboard_;
        std::shared_ptr<solosnake::rendering_system> rendering_;
        std::vector<solosnake::modelinstance> machineInstances_;
        solosnake::modelscene scene_;
        std::vector<const Machine*> machines_;
        solosnake::deferred_renderer* prenderer_;
    };

    typedef std::shared_ptr<boardstaterenderer> boardstaterenderer_ptr;
}

#endif
