#ifndef solosnake_layout_hpp
#define solosnake_layout_hpp

#include <memory>
#include <vector>
#include "solosnake/dimension.hpp"
#include "solosnake/gui.hpp"
#include "solosnake/iwidget.hpp"
#include "solosnake/iwidgetcollection.hpp"
#include "solosnake/rectangle_hittest.hpp"

namespace solosnake
{
    //! Maintains a list of the widgets and their locations and is used to
    //! check to see which widget is where onscreen and if the cursor is over it etc.
    //! Could also be responsible for tabbing between controls etc.
    //! Created on heap because it adds itself to widgets.
    class layout : public std::enable_shared_from_this<layout>
    {
        struct HeapOnly
        {
        };

    public:
        layout( iwidgetcollection&&, const HeapOnly& );

        static std::shared_ptr<layout> make_shared( iwidgetcollection&& );

        //! Sets a gui in the layout. When set the gui will receive
        //! notifications of certain key widget events.
        void set_gui( gui* );

        //! Called only by widget.
        void widget_changed_location( iwidget& );

        //! Called only by widget.
        void widget_changed_enabled( iwidget& );

        iwidget* highest_widget_under( int x, int y ) const;

        void widgets_under( 
            int x, 
            int y, 
            std::vector<iwidget*>& under ) const;

        void widgets_under( 
            int x, 
            int y, 
            std::vector<iwidget*>& under,
            std::vector<iwidget*>& notUnder ) const;

        iwidget* find_widget( const char* );

        const iwidgetcollection& widgets() const
        {
            return widgets_;
        }

        iwidgetcollection& widgets()
        {
            return widgets_;
        }

        void screen_was_resized( const dimension2d<unsigned int>& );

        void handle_application_focus_change();

    private:

        layout( const layout& );
        layout& operator=( const layout& );

    private:

        gui*                        gui_;
        rectangle_hittest<iwidget*> hittester_;
        iwidgetcollection           widgets_;
    };
}

#endif
