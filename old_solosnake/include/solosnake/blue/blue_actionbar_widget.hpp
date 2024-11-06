#ifndef blue_actionbar_widget_hpp
#define blue_actionbar_widget_hpp

#include <memory>
#include <vector>
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/ilua_widget.hpp"
#include "solosnake/quad.hpp"
#include "solosnake/texquad.hpp"
#include "solosnake/texturehandle.hpp"
#include "solosnake/utf8text.hpp"
#include "solosnake/widgetvisualstates.hpp"
#include "solosnake/blue/blue_actionbar.hpp"
#include "solosnake/blue/blue_deck.hpp"
#include "solosnake/blue/blue_iboardview.hpp"

namespace solosnake
{
    class iwidgetrenderer;
    class luaguicommandexecutor;
}

namespace blue
{
    struct actionbar_params
    {
        solosnake::texquad          background_tex_coords_;
        solosnake::texturehandle_t  background_tex_handle_;
        solosnake::texturehandle_t  icons_tex_handle_;
        unsigned int                button_sidelength_; // In pixels.
        unsigned int                border_width_;      // In pixels.
        solosnake::bgra             normal_colour_;
        solosnake::bgra             disabled_colour_;
        solosnake::bgra             highlighted_colour_;
    };

    //! A widget representing the main actionbar, the control through which
    //! a player plays the game.
    class actionbar_widget : public solosnake::ilua_widget
    {
    public:

        LUNAR_CLASS( actionbar_widget );

        actionbar_widget( lua_State* );

        actionbar_widget(
            const std::shared_ptr<deck>& dck,
            const actionbar_params& params,
            const solosnake::widgetparams wparams,
            const solosnake::ilua_widget::functionnames& fnames,
            const std::shared_ptr<solosnake::luaguicommandexecutor>& lce,
            const std::shared_ptr<solosnake::iwidgetrenderer>& style );

    private:

        solosnake::rect get_button_screen_pixel_rect( const unsigned int i ) const;

        void on_cursor_moved( solosnake::screenxy, solosnake::cursorbuttons_state ) override;

        void on_cursor_exit( solosnake::screenxy, solosnake::cursorbuttons_state ) override;

        void on_screen_resized( const solosnake::dimension2d<unsigned int>& windowSize ) override;

        void render( const unsigned long dt ) const override;

    private:

        typedef std::vector<solosnake::deferred_renderer::screen_quad> screen_quads_vector;

        enum ButtonState { NormalButton, DisabledButton, HighlightedButton };

        std::shared_ptr<solosnake::iwidgetrenderer>             style_;
        std::unique_ptr<actionbar>                              actionbar_;
        std::vector<ButtonState>                                button_states_;
        mutable screen_quads_vector                             button_quads_;
        solosnake::texquad                                      background_texcoords_;
        solosnake::texturehandle_t                              background_tex_;
        unsigned int                                            button_sidelength_;             // In pixels.
        unsigned int                                            calculated_button_sidelength_;  // In pixels.
        unsigned int                                            border_width_;                  // In pixels.
        unsigned int                                            calculated_border_width_;       // In pixels.
        solosnake::bgra                                         button_colours_[3];
    };
}

#endif
