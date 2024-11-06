#ifndef blue_mapedit_widget_hpp
#define blue_mapedit_widget_hpp

#include "solosnake/ilua_widget.hpp"
#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/persistance.hpp"
#include "solosnake/rect.hpp"
#include "solosnake/blue/blue_mapedit.hpp"
#include "solosnake/blue/blue_mapeditbrushtype.hpp"
#include "solosnake/blue/blue_gameviewing_widget.hpp"

namespace blue
{
    class drawboard;
    class iboardview;

    //! Widget for displaying a map editing widget.
    class mapedit_widget : public gameviewing_widget
    {
    public:

        LUNAR_CLASS( mapedit_widget );

        explicit mapedit_widget(
            lua_State* );

        mapedit_widget(
            const solosnake::widgetparams&,
            const solosnake::persistance& settings,
            const std::shared_ptr<mapedit>& sharedmap,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::iwidgetrenderer>&,
            const ilua_widget::functionnames& );

        ~mapedit_widget();

    private:

        enum MapIntaractionMode
        {
            MovementMode,
            EditingMapMode
        };

        static const unsigned int ModeCount = 2u;

        //! Member function pointers that are used depending on what state (mode) the
        //! widget is in.
        struct mode_handler
        {
            void ( mapedit_widget::* clicked_on_board )( const Hex_coord& );
            void ( mapedit_widget::* key_pressed )( unsigned short, unsigned int );
            void ( mapedit_widget::* key_released )( unsigned short, unsigned int );
            void ( mapedit_widget::* cursor_moved )( solosnake::screenxy, solosnake::cursorbuttons_state );
            void ( mapedit_widget::* cursor_enter )( solosnake::screenxy, solosnake::cursorbuttons_state );
            void ( mapedit_widget::* wheelmoved_inside )( solosnake::screenxy, float );
            void ( mapedit_widget::* reset )( );
        };

        class boarddrawer;

        void render( const unsigned long ) const override;

        int  get_screen_quad( lua_State* );

        int  set_brush_type( lua_State* );

        int  make_brush_bigger( lua_State* );

        int  make_brush_smaller( lua_State* );

        void set_mode( const MapIntaractionMode );

        void change_map( std::unique_ptr<solosnake::command<mapedittarget>> );

        void set_highlighted( const Hex_coord& );

        void set_highlighted( const std::vector<Hex_coord>& );

        mode_handler& handler();

        void on_clicked_on_board( const Hex_coord& xy ) override;

        void on_key_pressed( unsigned short k, unsigned int modifierFlags ) override;

        void on_key_released( unsigned short k, unsigned int modifierFlags ) override;

        void on_cursor_moved( solosnake::screenxy xy, solosnake::cursorbuttons_state s ) override;

        void on_cursor_enter( solosnake::screenxy xy, solosnake::cursorbuttons_state s ) override;

        void on_wheelmoved_inside( solosnake::screenxy xy, float ) override;

        void on_reset() override;

        void movemode_clicked_on_board( const Hex_coord& xy );

        void movemode_key_pressed( unsigned short k, unsigned int modifierFlags );

        void movemode_key_released( unsigned short k, unsigned int modifierFlags );

        void movemode_cursor_moved( solosnake::screenxy xy, solosnake::cursorbuttons_state s );

        void movemode_cursor_enter( solosnake::screenxy xy, solosnake::cursorbuttons_state s );

        void movemode_wheelmoved_inside( solosnake::screenxy xy, float );

        void movemode_reset();

        void editmode_clicked_on_board( const Hex_coord& xy );

        void editmode_key_pressed( unsigned short k, unsigned int modifierFlags );

        void editmode_key_released( unsigned short k, unsigned int modifierFlags );

        void editmode_cursor_moved( solosnake::screenxy xy, solosnake::cursorbuttons_state s );

        void editmode_cursor_enter( solosnake::screenxy xy, solosnake::cursorbuttons_state s );

        void editmode_wheelmoved_inside( solosnake::screenxy xy, float );

        void editmode_reset();

    private:

        std::shared_ptr<solosnake::iwidgetrenderer> renderer_;
        std::unique_ptr<boarddrawer>                drawboard_;
        std::shared_ptr<mapedit>                    board_;
        mode_handler                                modes_[ ModeCount ];
        MapIntaractionMode                          current_mode_;
        unsigned short                              key_undo_;
        unsigned short                              key_redo_;
        unsigned short                              key_change_mode_;
    };
}

#endif
